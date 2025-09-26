uniform sampler2D texture;
uniform float time;
varying vec2 vTexCoord;
void main() {
    float wave = sin(20.0 * vTexCoord.y + time * 2.0) * 0.02;
    vec2 uv = vTexCoord + vec2(wave, 0.0);
    gl_FragColor = texture2D(texture, uv);
}
