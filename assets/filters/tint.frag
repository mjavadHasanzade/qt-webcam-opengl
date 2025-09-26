uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec3 c = texture2D(texture, vTexCoord).rgb;
    vec3 tint = vec3(1.05, 1.0, 0.92);
    gl_FragColor = vec4(clamp(c * tint, 0.0, 1.0), 1.0);
}

