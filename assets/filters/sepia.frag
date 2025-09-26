uniform sampler2D texture;
varying vec2 vTexCoord;
void main() {
    vec4 color = texture2D(texture, vTexCoord);
    float r = dot(color.rgb, vec3(0.393, 0.769, 0.189));
    float g = dot(color.rgb, vec3(0.349, 0.686, 0.168));
    float b = dot(color.rgb, vec3(0.272, 0.534, 0.131));
    gl_FragColor = vec4(r, g, b, 1.0);
}
