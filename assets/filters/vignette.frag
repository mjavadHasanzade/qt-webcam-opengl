uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec2 uv = vTexCoord;
    vec3 c = texture2D(texture, uv).rgb;
    vec2 p = uv - 0.5;
    float d = dot(p,p);
    float v = smoothstep(0.8, 0.2, d);
    gl_FragColor = vec4(c * v, 1.0);
}

