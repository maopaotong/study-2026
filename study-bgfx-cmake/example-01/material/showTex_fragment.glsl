#version 330 core

uniform sampler2D tex_elevation;
uniform sampler2D tex_drape;

in vec2 v_uv;

void main() {
    vec4 c0 = texture2D(tex_elevation, v_uv);
    vec4 c1 = texture2D(tex_drape, v_uv);
    
    gl_FragColor = c1;
    //gl_FragColor = vec4(1,1,1,1);


}//end of main()
