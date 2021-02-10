#version 460 core
#define prec double
#define precvec dvec2

precision highp float;
const int max_i = 500;
uniform vec3 colourmap[max_i + 1];


out vec4 FragColor;

in vec3 pos;

uniform prec scale;
uniform precvec offset;

float QqhToRgb(float q1, float q2, float hue)
{
	if (hue > 360) hue -= 360;
	else if (hue < 0) hue += 360;

	if (hue < 60) return q1 + (q2 - q1) * hue / 60;
	if (hue < 180) return q2;
	if (hue < 240) return q1 + (q2 - q1) * (240 - hue) / 60;
	return q1;
}

vec3 HlsToRgb(float h, float l, float s)
{
	float p2;
	if (l <= 0.5) p2 = l * (1 + s);
	else p2 = l + s - l * s;

	float p1 = 2 * l - p2;
	float float_r, float_g, float_b;
	if (s == 0)
	{
		float_r = l;
		float_g = l;
		float_b = l;
	}
	else
	{
		float_r = QqhToRgb(p1, p2, h + 120);
		float_g = QqhToRgb(p1, p2, h);
		float_b = QqhToRgb(p1, p2, h - 120);
	}

	// Convert RGB to the 0 to 255 range.

	return vec3(float_r, float_g, float_b);
}

void main() {
    prec x0 = pos.x;
    prec y0 = pos.y ;
	
    x0 = (x0 / scale + 1);
	x0 *= 1.75;
	x0 += offset.x - 2.5;
	y0 /= scale;
	y0 += offset.y;

    prec x;
    prec y;
    int i = 0;
    prec tempx;
	prec x2;
	prec y2;

    while(x2 + y2 < 4 && i < max_i)
    {
        y = 2*x*y + y0;
        x = x2 - y2 + x0;
		x2 = x*x;
		y2 = y*y;
        i++;
    }

    FragColor = vec4(/*HlsToRgb(270 *i /max_i, (i == max_i)? 0 :.5, .5)*/ colourmap[i], 1);
    //FragColor = vec4(0., 1., 0., 1.);
}
