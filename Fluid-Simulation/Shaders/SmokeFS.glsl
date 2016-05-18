#version 430 core
 
in vec4 color;

uniform float globalTime;

void main(void) {

    /*if (color.a < 0.5) {
        gl_FragColor = vec4(1.0-color.a, color.a, 0.2, color.a);
    } else {
        gl_FragColor = vec4(color.a, 1.0 - color.a, 0.2, color.a);
    }*/

    //if (color.a > 0.01) {
        gl_FragColor = color; //vec4(0.8, 0.2, 0.3, 1.0);
    //} else {
    //    gl_FragColor = vec4(0.8, 0.2, 0.3, 0.0);
    //}
    
}
