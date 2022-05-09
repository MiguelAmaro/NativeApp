//A: Attributes
//U: Uniforms

attribute vec2 APosition;
attribute vec3 AColor;
varying vec3 Color;

uniform vec2 UPos;
uniform vec2 UDim;
uniform vec2 URes;
uniform mat2 UProj;

void main()
{
#if 1
  vec2 NewPos = UProj * APosition;
  // NOTE(MIGUEL): is temp code. translation not done in shader
  vec2 NormSize = vec2((UDim.x / URes.x), (UDim.y / URes.y));
  NewPos.x *= NormSize.x;
  NewPos.y *= NormSize.y;
  
  vec2 NormPos = vec2((-0.5f + (UPos.x / URes.x)),
                      (0.5f + (-UPos.y / URes.y)));
  
  NewPos.x += NormPos.x;
  NewPos.y += NormPos.y;
  Color = AColor;
  gl_Position = vec4(NewPos, 0.0, 1.0);
#else
  Color = AColor;
  gl_Position = vec4(APosition*0.5, 0.0, 1.0);
#endif
}
