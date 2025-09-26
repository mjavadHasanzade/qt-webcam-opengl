uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec2 uv = vTexCoord;
    vec3 base = texture2D(texture, uv).rgb;
    float w = 0.02; // frame thickness
    float frame = step(uv.x, w) + step(uv.y, w) + step(1.0-w, uv.x) + step(1.0-w, uv.y);
    frame = clamp(frame, 0.0, 1.0);
    vec3 color = mix(base, vec3(1.0), frame);
    gl_FragColor = vec4(color, 1.0);
}

