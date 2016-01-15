
uniform mat4 uMVPMatrix;
uniform float uPointSize;
attribute vec3 aPosition;

void main()
{
   gl_Position = uMVPMatrix * vec4(aPosition,1);
   gl_PointSize=uPointSize;
}