#version 450


layout(binding = 3) uniform ShaderUniform {
    float time;
}shaderUniform; 

vec4 begin = vec4(1.0, 1.0, 1.0, 1.0);
vec4 intersection = vec4(1.0, 1.0, 1.0, 1.0);
vec4 end = vec4(0.5, 0.4, 0.5, 1.0);

vec3 resolution = vec3(1920,1080,1.0);

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragCoord;

layout(location = 0) out vec4 outColor;

float random (in vec2 _st) {
    return fract(sin(dot(_st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float noise (in vec2 _st) {
    vec2 i = floor(_st);
    vec2 f = fract(_st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

#define NUM_OCTAVES 5

float fbm ( in vec2 _st) {
    float v = 0.0;
    float a = 0.5;
    vec2 shift = vec2(100.0);
    // Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5),
                    -sin(0.5), cos(0.50));
    for (int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise(_st);
        _st = rot * _st * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}


void main () {

    vec2 xy = fragCoord.xy / (1920/1080)*3;

    vec3 color = vec3(0.0);

    vec2 q = vec2(0.);
    q.x = fbm( xy + 0.00*shaderUniform.time);
    q.y = fbm( xy + vec2(1.0));

    vec2 r = vec2(0.);
    r.x = fbm( xy + 1.0*q + vec2(1.7,9.2)+ 0.15*shaderUniform.time);
    r.y = fbm( xy + 1.0*q + vec2(8.3,2.8)+ 0.126*shaderUniform.time);
    
    float f = fbm(xy+r);

    float h = sin(0.3 * shaderUniform.time * cos(f));
    vec4 col = mix(mix(begin, intersection, xy.y/h), mix(intersection, end, (xy.y - h)/(1.5 - h)), step(h, xy.y));

    outColor = vec4(0.0, col.y * sin(f), col.z * sin(f), 1.0);
}