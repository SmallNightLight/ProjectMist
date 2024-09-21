
#version 330 core
in vec3 fragColor;
out vec4 color;

void main()
{
    color = vec4(fragColor, 1.0); // Set the output color
}

/*
#version 300 es

precision highp float;

in vec2 v_vel;

out vec4 o_fragColor;

void main()
{
    vec3 v_color = vec3(min(v_vel.y, 0.8f), max(v_vel.x, 0.5f), 0);
    o_fragColor = vec4(v_color, 1.0f);
}*/
