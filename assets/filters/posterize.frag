uniform sampler2D texture;
varying vec2 vTexCoord;
void main() {
    vec4 color = texture2D(texture, vTexCoord);
    float levels = 4.0;
    color.rgb = floor(color.rgb * levels) / levels;
    gl_FragColor = color;
}
