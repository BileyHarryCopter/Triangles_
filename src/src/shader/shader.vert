#version 450

layout(location = 0) out vec3 fragColor;

layout(location = 0) in  vec2  position;

//  Create pre-vertex data for each vertex
vec3 colors[6] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    //  gl_Position - special variable of vec4 which sets position of the current 
    //                vertex in 3D and establish a parametr of normalization
    gl_Position = vec4(position, 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}
