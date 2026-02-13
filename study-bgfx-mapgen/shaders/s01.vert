
$input a_position
$input a_texcoord0
$input a_texcoord1
$output v_uv0
$output v_uv1

#include <bgfx_shader.sh>
void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	v_uv0 = a_texcoord0;
	v_uv1 = a_texcoord1;
}
