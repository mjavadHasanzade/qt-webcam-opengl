uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec2 uv = vTexCoord;
    vec3 base = texture2D(texture, uv).rgb;
    vec2 p = uv - 0.5;
    float d = length(p);
    float vign = smoothstep(0.7, 0.3, d);
    vec3 color = base * vign;
    gl_FragColor = vec4(color, 1.0);
}

