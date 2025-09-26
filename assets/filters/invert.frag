uniform sampler2D texture;
varying vec2 vTexCoord;
void main() {
    vec4 color = texture2D(texture, vTexCoord);
    gl_FragColor = vec4(1.0 - color.rgb, 1.0);
}
