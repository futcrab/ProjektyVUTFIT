// Hair simulation PGPa project 24/25
// Peter Durica (xduric05) VUT FIT
// Hair simulation inspired by https://github.com/rcedermalm/Realtime-Rendering-of-Fur

#ifdef COMPUTE_SHADER

layout (local_size_x = 1, local_size_y = 1) in;

// Hair data textures
layout(rgba16f, binding = 0) uniform readonly image2D hair_rest_pos;
layout(rgba16f, binding = 1) uniform readonly image2D hair_last_pos;
layout(rgba16f, binding = 2) uniform readonly image2D hair_curr_pos;
layout(rgba16f, binding = 3) uniform writeonly image2D hair_sim_pos;

uniform mat4 model = mat4(1.f);

uniform float dt;
uniform vec4 gravity = vec4(0.f, -9.81f, 0.f, 0.f);
uniform float hairStiffness = 0.08f;
uniform float hairDrag = 0.9f;

uniform float hairSegmentLength = 0.05f;
const int nOfHairSegments = 12; // If changed, also change in furShader.glsl (Geom) and in main code
uniform int constraintIterations = 2;

uniform float windMagnitude;
uniform vec4 windDirection;
uniform bool windEnabled = false;

uniform float localConstraint = 0.005f;

// Enforces the edge constraint between two hair segments (p1 and p2)
// This ensures the distance between the segments remains close to the desired segment length
void fullfillEdgeConstraint(inout vec4 p1, inout vec4 p2, int index){
    vec4 n = normalize(p1 - p2); // Direction vector between the two points
    float deltaD = length(p1 - p2) - hairSegmentLength; // Distance between the two points
    vec4 dP1, dP2;
    if(index == 0){
        dP1 = vec4(0.f); // First point is fixed
        dP2 = deltaD * n; // Second point is moved
    } else {
        dP1 = -0.5 * deltaD * n; // Else each point is moved by half of the distance
        dP2 =  0.5 * deltaD * n;
    }

    // Update the positions
    p1 = p1 + dP1;
    p2 = p2 + dP2;
}

// Enforces the local constraint for a hair segment (p1) relative to its rest position (p1_rest)
// Ensures the segment remains near its original position
void fullfillLocalConstraint(inout vec4 p0, inout vec4 p1, in vec4 p1_rest, in int index){
    vec4 d = p1_rest - p1; // Direction vector between the two points
    float local_constraint_strength = localConstraint; // Uniform strength constraint
    vec4 dP0, dP1;
        if(index == 0){ // If first point is fixed, only move the second point
            dP0 = vec4(0.f);
            dP1 = local_constraint_strength * d;
        } else { // else move both points by half of the distance
            dP0 = -0.5 * local_constraint_strength * d;
            dP1 =  0.5 * local_constraint_strength * d;
        }

    // Update the positions
    p0 = p0 + dP0;
    p1 = p1 + dP1;
}

void main(){
    // Initialize arrays and fill them with your designated hair strand
    ivec2 texCoords[nOfHairSegments];
    vec4 rest_pos[nOfHairSegments];
    vec4 sim_pos[nOfHairSegments];
    vec4 curr_vel[nOfHairSegments];
    vec4 curr_pos;
    vec4 last_pos;

    for(int i = 0; i < nOfHairSegments; i++){
        texCoords[i] = ivec2(gl_GlobalInvocationID.xy) + ivec2(i, 0); // Invocation ID + hair segment index
        rest_pos[i] = model * imageLoad(hair_rest_pos, texCoords[i]);
        last_pos = imageLoad(hair_last_pos, texCoords[i]);
        curr_pos = imageLoad(hair_curr_pos, texCoords[i]);
        
        sim_pos[i] = curr_pos;
        curr_vel[i] = curr_pos - last_pos;
    }

    // Integration and global constraints
    // Calculate effect of gravity for each hair segment
    float max_c_strength = hairStiffness;
    float constraint_strength = max_c_strength;
    for(int i = 0; i < nOfHairSegments; i++){
        sim_pos[i] += curr_vel[i] * hairDrag + gravity * pow(dt, 2);

        sim_pos[i] += constraint_strength * (rest_pos[i] - sim_pos[i]);
        constraint_strength = constraint_strength - (max_c_strength / nOfHairSegments);
    }

    // Local constraints
    // Decide how much the hair should be affected by rest position
    int numberOfIterations = constraintIterations;
    for(int iter = 0; iter < numberOfIterations; iter++){
        for(int i = 1; i < nOfHairSegments - 1; i++){
            fullfillLocalConstraint(sim_pos[i - 1], sim_pos[i], rest_pos[i], i);
        }
    }
    

    // Edge constraints
    // Decide how much the hair should be affected by the previous hair segment
    for(int iter = 0; iter < numberOfIterations; iter++){
        for(int i = 0; i < nOfHairSegments - 1; i++){
            fullfillEdgeConstraint(sim_pos[i], sim_pos[i + 1], i);
        }
    }


    // Wind calculation
    if(windEnabled){
        // Create a wind direction vectors to help simulate swirling wind
        vec4 c1 = normalize(vec4(0.f, 1.f, 0.f, 0.f));
        vec4 c2 = normalize(vec4(cross(vec3(c1), vec3(windDirection)), 0.f));

        // Create 4 wind variations to simulate turbulence
        vec4 w1 = windDirection + 0.2 * c1 + 0.2 * c2;
        vec4 w2 = windDirection + 0.2 * c1 - 0.2 * c2;
        vec4 w3 = windDirection - 0.2 * c1 + 0.2 * c2;
        vec4 w4 = windDirection - 0.2 * c1 - 0.2 * c2;

        for (int i = 1; i < nOfHairSegments; i++){
            float a = (texCoords[i].x) % 20 / 20.f; // Generate weight a for blending the wind directions
            vec4 wind = a * w1 + (1 - a) * w2 + a * w3 + (1 - a) * w4; // Interpolate between the wind directions

            vec4 v = sim_pos[i-1] - sim_pos[i]; // Vector between current and previous hair segment

            vec4 f = vec4(cross(cross(vec3(v), vec3(wind)), vec3(v)), 0.f); // Wind force acting perpendicular to the hair segment
            vec4 dP = pow(dt, 2) * f; // Displacement of the hair segment due to wind
            
            // Add wind force to the hair segment
            sim_pos[i] += windMagnitude * dP;
        }
    }
    

    // Update positions
    for(int i = 0; i < nOfHairSegments; i++){
        imageStore(hair_sim_pos, texCoords[i], sim_pos[i]);
    }
}

#endif

