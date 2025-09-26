uniform sampler2D texture;
varying vec2 vTexCoord;
void main(){
    vec4 c = texture2D(texture, vTexCoord);
    // Clarendon-like: boost midtones, cool shadows, warm highlights
    vec3 shadows = vec3(0.0, 0.02, 0.06);
    vec3 highlights = vec3(0.06, 0.02, 0.0);
    float l = dot(c.rgb, vec3(0.299,0.587,0.114));
    vec3 tonemap = mix(shadows, highlights, smoothstep(0.2, 0.8, l));
    vec3 mid = c.rgb * 1.08;
    vec3 res = clamp(mid + tonemap, 0.0, 1.0);
    gl_FragColor = vec4(res, 1.0);
}

