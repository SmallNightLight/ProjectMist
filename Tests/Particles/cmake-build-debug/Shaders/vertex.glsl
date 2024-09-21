#version 330 core
layout(location = 1) in vec2 position;
layout(location = 2) in vec2 velocity;

out vec3 fragColor;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);

    float speed = length(velocity) * 0.3f;
    fragColor = hsv2rgb(vec3(speed, 1, 1));
}