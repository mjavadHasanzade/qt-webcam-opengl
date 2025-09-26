uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec2 uv = vTexCoord;
    vec3 base = texture2D(texture, uv).rgb;
    float gridSize = 20.0;
    float line = step(0.98, fract(uv.x * gridSize)) + step(0.98, fract(uv.y * gridSize));
    float grid = clamp(line, 0.0, 1.0);
    vec3 color = mix(base, vec3(1.0), grid * 0.4);
    gl_FragColor = vec4(color, 1.0);
}

