//...

precision mediump float;
uniform vec2  UWinRes;
varying vec4 Color;

void main()
{
  vec2 st = (2.0f*gl_FragCoord.xy-UWinRes.xy)/UWinRes.y;
  float Circle = smoothstep(0.0f,0.01f, length(st)-0.2);
  vec3 OutColor = Color.xyz+Circle*vec3(1.0f,0.2f,0.7f);
  gl_FragColor = vec4(OutColor, Color.w);
}
