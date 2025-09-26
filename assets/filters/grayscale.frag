uniform sampler2D texture;
varying vec2 vTexCoord;
void main() {
    vec4 color = texture2D(texture, vTexCoord);
    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    gl_FragColor = vec4(gray, gray, gray, 1.0);
}
