uniform sampler2D texture;
uniform float time;
varying vec2 vTexCoord;

float hash(vec2 p){
    p = vec2(dot(p, vec2(127.1,311.7)), dot(p, vec2(269.5,183.3)));
    return fract(sin(p.x+p.y)*43758.5453123);
}

void main(){
    vec2 uv = vTexCoord;
    vec3 c = texture2D(texture, uv).rgb;
    // animate grain over time for visibility
    float n = hash(uv * 720.0 + vec2(time * 37.0, time * 19.0));
    vec3 grain = vec3(n - 0.5) * 0.16;
    gl_FragColor = vec4(clamp(c + grain, 0.0, 1.0), 1.0);
}

