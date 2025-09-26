uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec3 c = texture2D(texture, vTexCoord).rgb;
    float b = 0.12; // +brightness
    gl_FragColor = vec4(clamp(c + vec3(b), 0.0, 1.0), 1.0);
}

