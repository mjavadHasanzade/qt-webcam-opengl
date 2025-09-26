uniform sampler2D texture;
varying vec2 vTexCoord;
void main() {
    vec4 color = texture2D(texture, vTexCoord);
    color.r += 0.2;
    gl_FragColor = color;
}
