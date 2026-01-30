#version 330 core 
//uniform mat4 worldviewproj_matrix;
uniform mat4 projection;
in vec3 vertex;
in vec2 uv0;//
in vec2 uv1;//

out float v_e;
out vec2 v_xy;
out vec2 v_uv;
void main() {
	vec2 a_xy = vertex.xy;
	vec2 a_em = vec2(vertex.z, 0.0);
	float v_z = max(0.0, a_em.x);
	vec2 xy_clamped = clamp(a_xy, vec2(0, 0), vec2(1000, 1000));
	vec4 pos = projection * vec4(xy_clamped, v_z, 1);	
	v_uv = a_xy * 0.001 ;
	v_xy = (1.0 + pos.xy) * 0.5;
	gl_Position = pos;
}