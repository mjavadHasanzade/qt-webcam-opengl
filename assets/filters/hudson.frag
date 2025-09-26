uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec4 c = texture2D(texture, vTexCoord);
    // Hudson: cool highlights, slight contrast
    vec3 cool = c.rgb + vec3(-0.02, 0.0, 0.06);
    vec3 res = clamp((cool - 0.5) * 1.1 + 0.5, 0.0, 1.0);
    gl_FragColor = vec4(res, 1.0);
}

