uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec4 c = texture2D(texture, vTexCoord);
    // Amaro: increased exposure, slight cyan shadows
    vec3 brighter = clamp(c.rgb * 1.1, 0.0, 1.0);
    vec3 tint = vec3(-0.02, 0.0, 0.04);
    vec3 res = clamp(brighter + tint, 0.0, 1.0);
    gl_FragColor = vec4(res, 1.0);
}

