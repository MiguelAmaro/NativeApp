//A: Attributes
//U: Uniforms

attribute vec2 APosition;
attribute vec3 AColor;
varying vec4 Color;

uniform float URect[4];
uniform vec2  UWinRes;
uniform vec4  UColor;

void main()
{
  Color = UColor;
  //gl_VetexID;
  vec2 Min = vec2(URect[0], URect[1]);
  vec2 Max = vec2(URect[2], URect[3]);
  vec2 HalfDim   = vec2((Max.x-Min.x)*0.5f,
                        (Max.y-Min.y)*0.5f);
  
  
  vec2 PosCenter    = Min+HalfDim;
  PosCenter.y = (UWinRes.y-PosCenter.y);
  vec2 NDCHalfDim   = 2.0*HalfDim.xy/UWinRes.xy;
  vec2 NDCPosCenter = 2.0f*(PosCenter.xy/UWinRes.xy)-1.0f;
  gl_Position = vec4(APosition*NDCHalfDim+NDCPosCenter, 0.0, 1.0);
}
