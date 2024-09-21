#version 330 core
layout(location = 0) in vec2 aPos; // Position
layout(location = 1) in vec2 aOffset; // Instance position
layout(location = 2) in vec2 aVelocity; // Instance color

out vec3 fragColor;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    gl_Position = vec4(aPos + aOffset, 0.0, 1.0); // Position offset by instance
    gl_PointSize = 10.0; // Set point size

    float speed = length(aVelocity) * 0.3f;
    fragColor = hsv2rgb(vec3(speed, 1, 1));
}




/*#version 300 es

precision highp float;

in vec2 a_pos;
in vec2 a_vel;

out vec2 out_pos;
out vec2 out_vel;

out vec2 v_vel;

//uniform mat4 u_mvp;
uniform float u_deltaTime;
uniform vec2 u_attractorPosition;
uniform float u_damping;
uniform float u_attractorMass;
uniform float u_particleMass;
uniform float u_gravity;
uniform float u_softening;
uniform float u_isAttracting;
uniform float u_isRunning;

void main()
{
    vec2 r = u_attractorPosition - a_pos;
    float rSquared = dot(r, r) + u_softening;
    vec2 force = (u_gravity * u_attractorMass * u_particleMass * normalize(r) / rSquared) * u_isAttracting * u_isRunning;

    vec2 acceleration = force / u_particleMass;
    vec2 position = a_pos + (a_vel * u_deltaTime + 0.5f * acceleration * u_deltaTime * u_deltaTime) * u_isRunning;
    vec2 velocity = a_vel + acceleration * u_deltaTime;

    out_pos = position;

    out_vel = mix(velocity, velocity * u_damping, u_isRunning);

    gl_Position = vec4(position, 0.0, 1.0); //u_mvp * vec4(position, 0.0, 1.0);

    v_vel = velocity;
}*/
