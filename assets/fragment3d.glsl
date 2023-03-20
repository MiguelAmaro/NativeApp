precision mediump float;

uniform vec2  UWinRes;
varying vec4 Color;
varying vec2 UV;
varying float Height;

void main()
{
  vec2 st = (2.0f*gl_FragCoord.xy-UWinRes.xy)/UWinRes.y;
  vec4 Snow  = vec4(0.092,0.89,0.9, 1.0);
  vec4 Grass = vec4(0.03,0.1,0.12,1.0);
  gl_FragColor = mix(Grass, Snow, Height*Height)*0.0+Color*0.6;
}