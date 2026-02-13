
$input v_uv0
$input v_uv1

uniform sampler2D s_colorMap;

#include <bgfx_shader.sh>

void main()
{	
	vec4 color = texture2D(s_colorMap, v_uv0);
	//vec4(1,0,1,1);
	gl_FragColor = color;
}
