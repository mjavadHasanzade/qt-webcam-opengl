uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec2 uv = vTexCoord;
    vec4 c = texture2D(texture, uv);
    // Lomo: strong vignette, boosted saturation
    vec2 p = uv - 0.5;
    float d = dot(p,p);
    float vig = smoothstep(0.9, 0.3, d);
    float sat = 1.25;
    float l = dot(c.rgb, vec3(0.299,0.587,0.114));
    vec3 cs = mix(vec3(l), c.rgb, sat);
    vec3 res = cs * vig;
    gl_FragColor = vec4(clamp(res,0.0,1.0), 1.0);
}

