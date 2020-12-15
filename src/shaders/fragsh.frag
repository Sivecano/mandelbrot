#version 330 core


out vec4 FragColor;

in vec3 pos;

uniform float scale;

void main() {
float x0 = (pos.x / scale + 1)* 1.75 - 2.5;
    float y0 = pos.y / scale;
    float x;
    float y;
    const int max_i = 10000;
    int i = 0;
    float tempx;

    while(x*x + y*y < 2 && i < max_i)
    {
        tempx = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = tempx;
        i++;
    }

    gl_FragColor = vec4(vec3(1, 1, 1) - vec3(1, 1, 1) * i / max_i, (i != max_i) ? 0 : 1);
    //FragColor = vec4(0., 1., 0., 1.);
}
