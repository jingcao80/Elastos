precision mediump float;
varying vec2 vTextureCoord;
varying vec4 ambient;
varying vec4 diffuse;
varying vec4 specular;
uniform sampler2D sTexture;
void main()
{
    vec4 finalColor = texture2D(sTexture, vTextureCoord);
    finalColor.a=(finalColor.r+finalColor.g+finalColor.b)/3.0;
    finalColor=finalColor*ambient+finalColor*specular+finalColor*diffuse;
    gl_FragColor = finalColor;
}