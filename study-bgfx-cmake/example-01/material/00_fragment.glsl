#version 330 core

in float v_e;
in vec2 v_xy;

out vec4 color;

void main() {
    float e = 0.5 * (1.0 + v_e);
    
    if(e >= 0.5) {
        color = vec4(e, 0, 0, 1);
    } else {
        color = vec4(e, 0, 0, 1);
    }
}