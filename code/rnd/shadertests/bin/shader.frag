#version 110

uniform sampler2D background;
uniform sampler2D layer1;
uniform sampler2D layer2;

void main()
{
    vec4 color = texture2D(background, gl_TexCoord[0].st); //st = uv
    color += texture2D(layer1, gl_TexCoord[0].st);
    vec4 layer2col = texture2D(layer2, gl_TexCoord[0].st);
    if(layer2col.a >= 0.5)
    {
        color = layer2col;
    }
    gl_FragColor = color;
}
