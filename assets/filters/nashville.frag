uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec4 c = texture2D(texture, vTexCoord);
    // Nashville-like: pinkish highlights, cyan shadows, slight contrast reduction
    float l = dot(c.rgb, vec3(0.299,0.587,0.114));
    vec3 shadows = vec3(0.0, 0.04, 0.06);
    vec3 highlights = vec3(0.06, 0.02, 0.06);
    vec3 tint = mix(shadows, highlights, smoothstep(0.3, 0.7, l));
    vec3 faded = mix(c.rgb, vec3(l), 0.1);
    vec3 res = clamp(faded + tint, 0.0, 1.0);
    gl_FragColor = vec4(res, 1.0);
}

