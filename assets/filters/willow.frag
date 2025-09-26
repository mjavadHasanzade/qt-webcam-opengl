uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec4 c = texture2D(texture, vTexCoord);
    // Willow: monochrome with slight warm tone
    float l = dot(c.rgb, vec3(0.299,0.587,0.114));
    vec3 mono = vec3(l);
    vec3 warm = mono + vec3(0.02, 0.01, 0.0);
    gl_FragColor = vec4(clamp(warm,0.0,1.0), 1.0);
}

