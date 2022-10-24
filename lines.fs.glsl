#version 430
out vec4 fragColor;
uniform vec4 u_color = vec4(0, 0, 0, 1);
void main()
{
    fragColor = u_color;
}