uniform sampler2D texture;
varying vec2 vTexCoord;
void main() {
    float offset = 1.0/300.0;
    vec4 sum = vec4(0.0);
    sum += texture2D(texture, vTexCoord + vec2(-offset, -offset));
    sum += texture2D(texture, vTexCoord + vec2( offset, -offset));
    sum += texture2D(texture, vTexCoord + vec2(-offset,  offset));
    sum += texture2D(texture, vTexCoord + vec2( offset,  offset));
    sum *= 0.25;
    gl_FragColor = sum;
}
