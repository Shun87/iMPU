static const char* fragmentShader = STRINGIFY(
precision mediump float;
uniform sampler2D SamplerY;
uniform sampler2D SamplerU;
uniform sampler2D SamplerV;

varying highp vec2 coordinate;

void main()
{
    mediump vec3 yuv;
    lowp vec3 rgb;
    
    yuv.x = texture2D(SamplerY, coordinate).r;
    
    yuv.y = texture2D(SamplerU, coordinate).r - 0.5;
    
    yuv.z = texture2D(SamplerV, coordinate).r - 0.5;
    
    // Using BT.709 which is the standard for HDTV
    rgb = mat3(      1,       1,       1,
               0, -.21482, 2.12798,
               1.28033, -.38059,       0) * yuv;
    
    gl_FragColor = vec4(rgb, 1);
});
