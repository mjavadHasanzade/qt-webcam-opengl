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
    kernel[0] =  1.0; kernel[1] =  2.0; kernel[2] =  1.0;
    kernel[3] =  0.0; kernel[4] =  0.0; kernel[5] =  0.0;
    kernel[6] = -1.0; kernel[7] = -2.0; kernel[8] = -1.0;

    float sum = 0.0;
    for(int i = 0; i < 9; i++) {
        vec4 color = texture2D(texture, vTexCoord + offsets[i]);
        float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
        sum += gray * kernel[i];
    }
    sum = abs(sum);
    gl_FragColor = vec4(sum, sum, sum, 1.0);
}