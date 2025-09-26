uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec4 c = texture2D(texture, vTexCoord);
    // Rise: warm highlights, faded blacks
    vec3 lifted = c.rgb * 0.96 + vec3(0.04, 0.03, 0.02);
    vec3 res = clamp(lifted, 0.0, 1.0);
    gl_FragColor = vec4(res, 1.0);
}

