uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec4 c = texture2D(texture, vTexCoord);
    // Valencia-like: warm tint, slight fade
    vec3 warm = c.rgb * mat3(1.08,0.02,0.0, 0.02,1.04,0.0, 0.0,0.0,0.98);
    vec3 faded = mix(warm, vec3(0.95), 0.08);
    gl_FragColor = vec4(clamp(faded,0.0,1.0), 1.0);
}

