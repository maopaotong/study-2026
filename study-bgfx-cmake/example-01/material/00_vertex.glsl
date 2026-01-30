#version 330 core 
//uniform mat4 worldviewproj_matrix;
uniform mat4 projection;
in vec3 vertex;
in vec2 uv0;
in vec2 uv1;

out float v_e;
out vec2 v_xy;
void main() {

	vec4 pos = projection * vec4(vertex.xy, 0, 1.0);
	//gPos = vec4(vertex.x * 0.001 * 0.25, -vertex.y * 0.001 * 0.5, -0.1, 1);
	v_xy = (1.0 + pos.xy) * 0.5;  
	v_e = vertex.z;
	gl_Position = pos;
}
