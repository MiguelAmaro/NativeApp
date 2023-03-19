
precision mediump float;
uniform vec2  UWinRes;

varying vec4 Color;
varying vec2 UV;
varying vec2 Dim;

vec2 SdBox(vec2 uv, vec2 wh, float r, float t)
{
  wh = clamp(wh, vec2(0.0), vec2(1.0));
  vec2 wh1 = wh-r-t*2.0f;
  vec2 c    = abs(uv) - wh1; //axis-aligned corner
  float d   = length(max(c, vec2(0)))-r + min(max(c.x, c.y), 0.0f);
  float BoxFill    = smoothstep(0.01f,0.0f, d);
  float BoxOutline = smoothstep(0.01f,0.0f, abs(d)-t);
  return vec2(BoxFill, BoxOutline);
}
float RectSDF(vec2 p, vec2 b, float r)
{
  vec2 d = abs(p) - b + vec2(r);
  return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - r;   
}
void main()
{
  vec2 st = (2.0f*gl_FragCoord.xy-UWinRes.xy)/UWinRes.y;
  float Circle = smoothstep(0.0f,0.01f, length(UV)-0.5);
  //vec2(1.0)
#if 0
  vec2 Box = SdBox(UV, vec2(1.0), 0.8f, 0.01);
  vec3 OutColor = Color.xyz*Box.x+Box.y*vec3(1.0f,0.2f,0.7f);
  gl_FragColor = vec4(OutColor, Color.w*Box.x*0.8+Box.y*0.2);
#else
  vec2 pos = (UV*0.5+0.5)*Dim;
  float bt = 4.0f; //borderThickness
  float r  = 30.0f; //borderThickness
  float d = RectSDF(pos-Dim/2.0, 
                    Dim/2.0f - bt/2.0 - 1.0, 
                    r); //dist
  float b = smoothstep(-1.0, 1.0, abs(d) - bt / 2.0); //blend amount
  vec4 OutColor = (d<0.0)?Color:vec4(0.0);//+Box.y*vec3(1.0f,0.2f,0.7f);
  vec4 BorderColor = Color+0.4;
  vec4 Final = mix(BorderColor, OutColor, b);
  gl_FragColor = Final;
#endif
}