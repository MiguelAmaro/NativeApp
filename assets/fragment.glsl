//...

precision mediump float;
varying vec3 Color;

void main()
{
  //gl_FragColor = vec4(vec3(0.0, 1.0, 0.4), 1.0);
  gl_FragColor = vec4(Color, 1.0);
}
