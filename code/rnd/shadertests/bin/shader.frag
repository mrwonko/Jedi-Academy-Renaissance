#version 110

#define NUM_ELEMENTS 2

struct Test
{
	vec3 color;
};

struct TestContainer
{
	Test array[NUM_ELEMENTS];
};

uniform TestContainer UniformStruct;

void main()
{
    vec3 color = vec3(0, 0, 0);
    for(int i = 0; i < NUM_ELEMENTS; ++i)
    {
        color += (1.f/NUM_ELEMENTS) * UniformStruct.array[i].color;
    }
    gl_FragColor = vec4(color, 1);
}
