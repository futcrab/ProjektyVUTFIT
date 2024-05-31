# version 430

const int MAXPOINTS = 1500000;

uniform int nOfPoints;
uniform int offset;
uniform int NPointsInObject;

uniform float HMintensity = 100.0f;
uniform float HMdistance = 1.0f;

in vec3 v_color;
in vec3 FragPos;

uniform int f_switcher = 1;

float distance_p = 0.0;
float color_cor = 0.0;

const vec3 white_c = vec3(1.0, 1.0, 1.0);
const vec3 black_c = vec3(0.0, 0.0, 0.0);
const vec3 blue_c = vec3(0.0, 0.0, 1.0);
const vec3 red_c = vec3(1.0, 0.0, 0.0);
const vec3 green_c = vec3(0.0, 1.0, 0.0);

layout (binding=0, std430) buffer positionBuffer {
        float position[MAXPOINTS];
} position;

layout (binding=1, std430) buffer objectPointsBuffer {
        int objectPoints[MAXPOINTS];
} objectPoints;

out vec4 out_color;

void main()
{
    if (f_switcher == 1){
        out_color = vec4(v_color, 1.0f);
    }
    else{
      vec3 HMpoint;
      for(int i = offset; i < offset + NPointsInObject; i++){
        HMpoint[0] = position.position[objectPoints.objectPoints[i]*3];
        HMpoint[1] = position.position[objectPoints.objectPoints[i]*3+1];
        HMpoint[2] = position.position[objectPoints.objectPoints[i]*3+2];
        
        distance_p = distance(FragPos, HMpoint) / HMdistance;
        if (distance_p <= 1.0f){
        color_cor = color_cor + ((1.0f-distance_p)/HMintensity);
        }
      }
      vec3 point_c;
      if (color_cor < 1.0f){
        point_c = vec3(1.0f - color_cor, 1.0f - color_cor, 1.0f);
      }else if (color_cor < 2.0f){
        color_cor = color_cor - 1.0f;
        point_c = normalize(vec3(0.0f, 0.0f + color_cor, 1.0f - color_cor));
      }else{
        color_cor = color_cor - 2.0f;
        point_c = normalize(vec3(0.0f + color_cor, 1.0f - color_cor, 0.0f));
      }
      out_color = vec4(point_c, 1.0f);
    }
}