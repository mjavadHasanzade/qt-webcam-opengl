uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec4 c = texture2D(texture, vTexCoord);
    // X-Pro II-like: strong contrast, vignette hint, cool shadows
    vec2 uv = vTexCoord;
    vec2 centered = uv - 0.5;
    float dist = dot(centered, centered);
    float vignette = smoothstep(0.8, 0.2, dist);
    vec3 cool = c.rgb + vec3(0.0, 0.0, 0.03) - vec3(0.02, 0.0, 0.0);
    vec3 contrast = clamp((cool - 0.5) * 1.2 + 0.5, 0.0, 1.0);
    vec3 res = mix(contrast * vignette, contrast, 0.85);
    gl_FragColor = vec4(res, 1.0);
}

