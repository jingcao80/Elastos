precision mediump float;
varying vec2 vTextureCoord;
varying vec4 vAmbient;
varying vec4 vDiffuse;
varying vec4 vSpecular;
uniform sampler2D sTexture;
void main()
{
  vec4 finalColor = texture2D(sTexture, vTextureCoord);
  gl_FragColor = finalColor*vAmbient+finalColor*vSpecular+finalColor*vDiffuse;
}