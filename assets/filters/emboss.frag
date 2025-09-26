uniform sampler2D texture;
varying vec2 vTexCoord;
void main() {
    float offset = 1.0/300.0;
    vec3 c1 = texture2D(texture, vTexCoord + vec2(-offset, -offset)).rgb;
    vec3 c2 = texture2D(texture, vTexCoord + vec2( offset,  offset)).rgb;
    float gray = dot(c1 - c2, vec3(0.299, 0.587, 0.114)) + 0.5;
    gl_FragColor = vec4(gray, gray, gray, 1.0);
}
