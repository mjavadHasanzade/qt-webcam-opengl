uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec3 c = texture2D(texture, vTexCoord).rgb;
    float k = 1.2; // contrast factor
    vec3 res = clamp((c - 0.5) * k + 0.5, 0.0, 1.0);
    gl_FragColor = vec4(res, 1.0);
}

