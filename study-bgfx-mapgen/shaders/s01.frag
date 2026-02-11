$input v_texcoord0

uniform sampler2D s_colorMap;

#include <bgfx_shader.sh>

void main()
{
	gl_FragColor = texture2D(s_colorMap, v_texcoord0);
}
