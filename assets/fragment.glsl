
precision mediump float;
uniform vec2  UWinRes;

varying vec4 Color;
varying vec2 UV;

vec2 SdBox(vec2 uv, float r, float t)
{
  vec2 wh = vec2(1.0f-r-t*2.0f);
  vec2 c    = abs(uv) - wh; //axis-aligned corner
  float d   = length(max(c, vec2(0)))-r + min(max(c.x, c.y), 0.0f);
  float BoxFill    = smoothstep(0.01f,0.0f, d);
  float BoxOutline = smoothstep(0.01f,0.0f, abs(d)-t);
  return vec2(BoxFill, BoxOutline);
}
void main()
{
  vec2 st = (2.0f*gl_FragCoord.xy-UWinRes.xy)/UWinRes.y;
  float Circle = smoothstep(0.0f,0.01f, length(UV)-0.5);
  
  vec2 Box = SdBox(UV, 0.8f, 0.01);
  vec3 OutColor = Color.xyz*Box.x+Box.y*vec3(1.0f,0.2f,0.7f);
  gl_FragColor = vec4(OutColor, Box.x*0.2+Box.y);
}