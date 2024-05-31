# version 430

uniform int m_bands;
uniform int SHLineLength;
uniform int nofVerticesPerSH;
const float shScale = 0.1;
uniform int ncols = 20;
uniform int nrows = 20;
uniform int SHColor = 0;

uniform float scaleFactor = 1.0f;
uniform float HMintensity = 100.0f;
uniform float HMdistance = 1.0f;

uniform int nOfPoints;
uniform int offset;
uniform int NPointsInObject;

const int MAXPOINTS = 1500000;

uniform vec3 objectColor;
uniform vec3 pointColor;

const vec3 white_c = vec3(1.0, 1.0, 1.0);
const vec3 blue_c = vec3(0.0, 0.0, 1.0);
const vec3 red_c = vec3(1.0, 0.0, 0.0);
const vec3 green_c = vec3(0.0, 1.0, 0.0);

float distance_p = 0.0;
float color_cor = 0.0;

vec3 point_c = green_c;

layout (binding=0, std430) buffer positionBuffer {
        float position[MAXPOINTS];
} position;

layout (binding=1, std430) buffer objectPointsBuffer {
        int objectPoints[MAXPOINTS];
} objectPoints;

layout (binding=2, std430) buffer SHBuffer {
        float values[MAXPOINTS];
} SH;

layout (location = 0) in vec3 a_position;

layout (location = 1) in mat4 p_posMatrix;
layout (location = 5) in vec3 p_tangent;
layout (location = 6) in vec3 p_bitangent;
layout (location = 7) in vec3 p_normal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform int switcher;
uniform float size;

out vec3 v_color;

int factorial(int a){
  int r=1;
  for(int i=2;i<=a;++i)
    r*=i;
  return r;
}


float computeNormalization(int l, int m) {
	return sqrt(((2*l+1) * factorial(l-m)) / (4 * radians(180) * factorial(l+m)));
}


float legendreP(int l, int m, float x) {
	/* Evaluate the recurrence in double precision */
	double p_mm = 1;

	if (m > 0) {
		double somx2 = sqrt((1 - x) * (1 + x));
		double fact = 1;
		for (int i=1; i<=m; i++) {
			p_mm *= (-fact) * somx2;
			fact += 2;
		}
	}

	if (l == m)
		return float(p_mm);

	double p_mmp1 = x * (2*m + 1) * p_mm;
	if (l == m+1)
		return float(p_mmp1);

	double p_ll = 0;
	for (int ll=m+2; ll <= l; ++ll) {
		p_ll = ((2*ll-1)*x*p_mmp1 - (ll+m-1) * p_mm) / (ll-m);
		p_mm = p_mmp1;
		p_mmp1 = p_ll;
	}

	return float(p_ll);
}

float coef(uint sh,int l,int m){
  return SH.values[SHLineLength*sh+uint(l*(l+1) + m)];
}

float eval(uint sh,float theta,float phi){
  float result = 0;
  float cosTheta = cos(theta);

  for (int l = 0; l < m_bands; ++l) {
      for (int m = 1; m <= l; ++m) {
          float L = legendreP(l, m, 2.0f * cosTheta - 1) * computeNormalization(l, m);
          result += coef(sh,l, -m) * sqrt(2) * sin((m-1+1)*phi) * L;
          result += coef(sh,l, +m) * sqrt(2) * cos((m-1+1)*phi) * L;
      }

      result += coef(sh,l, 0) * legendreP(l, 0, 2.0f * cosTheta - 1) * computeNormalization(l, 0);
  }
  return result;
}

void hemisphere(uint vID,uint shID,in mat4 vp,in mat4 model,in uint nCols,in uint nRows,in float radius){
  uint nCells = nCols * nRows;
  uint verticesPerCell = 6u;
  uint nVertices = nCells * verticesPerCell;
  uint verticesPerRow = nCols * verticesPerCell;

  uint cellID = vID / verticesPerCell;
  uint verID  = vID % verticesPerCell;

  uint rowID = cellID / nCols;
  uint colID = cellID % nCols;

  uvec2 verOffset[] = uvec2[](
    uvec2(0,0),
    uvec2(1,0),
    uvec2(0,1),
    uvec2(0,1),
    uvec2(1,0),
    uvec2(1,1)
  );

  float zAngleNorm = float(colID + verOffset[verID].y * 1.0f) / float(nCols);
  float yAngleNorm = float(rowID + verOffset[verID].x * 1.0f) / float(nRows);

  float yAngle = radians(yAngleNorm * 90.f);
  float zAngle = radians(zAngleNorm * 360.f);

  vec2 xyCircle = vec2(cos(zAngle),sin(zAngle));
  vec3 hemisphere = vec3(xyCircle*cos(yAngle),sin(yAngle));

  float shR = eval(shID,yAngle,zAngle);
  if (SHColor == 0){
    v_color   = normalize(vec3(inverse(transpose(model))*vec4(hemisphere,1.0)));
  }
  else if (SHColor == 1){
    v_color   = normalize(vec3(0.0 + abs(shR), 1.0 - abs(shR), 0.0));
  }
  
  gl_Position = vp * model * vec4(hemisphere*radius*shR,1.f);
}

void main()
{
  mat4 sceneScale = mat4(1);
  sceneScale[0][0] = sceneScale[1][1] = sceneScale[2][2] = scaleFactor;

  if (switcher == 0){
      gl_Position = projection * view * sceneScale * model * vec4(a_position, 1.0);
      vec3 HMpoint;
      for(int i = offset; i < offset + NPointsInObject; i++){
        HMpoint[0] = position.position[objectPoints.objectPoints[i]*3];
        HMpoint[1] = position.position[objectPoints.objectPoints[i]*3+1];
        HMpoint[2] = position.position[objectPoints.objectPoints[i]*3+2];
        
        distance_p = distance(vec3(model * vec4(a_position, 1.0)), HMpoint) / HMdistance;
        if (distance_p <= 1.0){
        color_cor = color_cor + ((1.0f-distance_p)/HMintensity);
        }
      }

      if (color_cor < 1.0f){
        point_c = vec3(1.0f - color_cor, 1.0f - color_cor, 1.0f);
      }else if (color_cor < 2.0f){
        color_cor = color_cor - 1.0f;
        point_c = normalize(vec3(0.0f, 0.0f + color_cor, 1.0f - color_cor));
      }else{
        color_cor = color_cor - 2.0f;
        point_c = normalize(vec3(0.0f + color_cor, 1.0f - color_cor, 0.0f));
      }
      v_color = point_c;
  }else if(switcher == 2){
      gl_Position = projection * view * sceneScale * model * vec4(a_position, 1.0);
      v_color = objectColor;
  }else if (switcher == 1){
      gl_Position = projection * view * sceneScale * p_posMatrix * vec4(a_position * size, 1.0);
      v_color = pointColor;
  }else if (switcher == 3){
      uint shID = gl_InstanceID;
      uint vID = gl_VertexID;

      vec3 t = normalize(p_tangent), b = normalize(p_bitangent), n = normalize(p_normal);

      mat4 tbn = mat4(1);
      tbn[0].xyz = t;
      tbn[1].xyz = b;
      tbn[2].xyz = n;

      mat4 vp = projection*view;
      mat4 model = sceneScale*p_posMatrix*tbn;
      
      hemisphere(vID,shID,vp,model,ncols,nrows,shScale * size);
  }
}