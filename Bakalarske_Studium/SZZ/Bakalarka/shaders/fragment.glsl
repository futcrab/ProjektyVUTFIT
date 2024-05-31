# version 430

uniform int f_switcher = 1;

in vec3 v_color;

out vec4 out_color;

void main()
{
    out_color = vec4(v_color, 1.0f);
}