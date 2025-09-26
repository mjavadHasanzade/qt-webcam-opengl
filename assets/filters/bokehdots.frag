uniform sampler2D texture;
uniform float time;
varying vec2 vTexCoord;

float circle(vec2 uv, vec2 c, float r){
    return smoothstep(r, r-0.01, length(uv-c));
}

void main(){
    vec2 uv = vTexCoord;
    vec3 base = texture2D(texture, uv).rgb;
    // moving dots
    vec2 c1 = vec2(fract(time*0.07), 0.3);
    vec2 c2 = vec2(0.6, fract(time*0.05));
    vec2 c3 = vec2(fract(time*0.09), fract(time*0.11));
    float d1 = circle(uv, c1, 0.06);
    float d2 = circle(uv, c2, 0.08);
    float d3 = circle(uv, c3, 0.05);
    vec3 overlay = vec3(1.0, 0.9, 0.8) * d1 + vec3(0.8, 0.9, 1.0) * d2 + vec3(1.0) * d3;
    vec3 color = clamp(base + overlay * 0.25, 0.0, 1.0);
    gl_FragColor = vec4(color, 1.0);
}

