uniform sampler2D texture;
varying vec2 vTexCoord;
void main() {
    float offset = 1.0/300.0;
    vec2 offsets[9];
    offsets[0] = vec2(-offset,  offset);
    offsets[1] = vec2( 0.0f,    offset);
    offsets[2] = vec2( offset,  offset);
    offsets[3] = vec2(-offset,  0.0f);
    offsets[4] = vec2( 0.0f,    0.0f);
    offsets[5] = vec2( offset,  0.0f);
    offsets[6] = vec2(-offset, -offset);
    offsets[7] = vec2( 0.0f,   -offset);
    offsets[8] = vec2( offset, -offset);

    float kernel[9];
    kernel[0] =  0.0; kernel[1] = -1.0; kernel[2] =  0.0;
    kernel[3] = -1.0; kernel[4] =  5.0; kernel[5] = -1.0;
    kernel[6] =  0.0; kernel[7] = -1.0; kernel[8] =  0.0;

    vec3 sum = vec3(0.0);
    for(int i = 0; i < 9; i++) {
        vec3 color = texture2D(texture, vTexCoord + offsets[i]).rgb;
        sum += color * kernel[i];
    }
    gl_FragColor = vec4(sum, 1.0);
}
