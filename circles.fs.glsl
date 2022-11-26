#version 430
out vec4 fragColor;
uniform vec4 u_color;// = vec4(0.125, 0.25, 1.0, 0.125);
void main()
{
    fragColor = u_color;
}