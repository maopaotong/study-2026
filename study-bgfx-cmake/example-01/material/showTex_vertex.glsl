#version 330 core

uniform mat4 projection;

in vec3 vertex;
in vec2 uv0;
in vec2 uv1;
out vec2 v_uv;

void main() {
	vec4 pos = projection * vec4(vertex.xy, 0, 1.0);
	pos.y = -pos.y;
	gl_Position = pos;
	v_uv = vertex.xy * 0.001;	
	//v_uv.y = 1.0 - v_uv.y;//flip y
	v_uv += 1.0 / 64;//offset to avoid edge artifacts
}
