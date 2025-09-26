uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec4 c = texture2D(texture, vTexCoord);
    // Gingham-like: desaturate, soft contrast, warm lift
    float l = dot(c.rgb, vec3(0.299,0.587,0.114));
    vec3 gray = vec3(l);
    vec3 desat = mix(c.rgb, gray, 0.25);
    vec3 lifted = desat * 0.95 + vec3(0.03, 0.02, 0.0);
    gl_FragColor = vec4(clamp(lifted,0.0,1.0), 1.0);
}

