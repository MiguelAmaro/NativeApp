//A: Attributes
//U: Uniforms

attribute vec3 APosition;
attribute vec2 AUV;

uniform vec2  UWinRes;
uniform float UTime;
uniform mat4  UModel;
uniform mat4  UProjection;

varying vec4 Color;
varying vec2 UV;
varying float Height;


vec2 hash( vec2 p ) // replace this by something better
{
	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
  const float K1 = 0.366025404; // (sqrt(3)-1)/2;
  const float K2 = 0.211324865; // (3-sqrt(3))/6;
  
	vec2  i = floor( p + (p.x+p.y)*K1 );
  vec2  a = p - i + (i.x+i.y)*K2;
  float m = step(a.y,a.x); 
  vec2  o = vec2(m,1.0-m);
  vec2  b = a - o + K2;
	vec2  c = a - 1.0 + 2.0*K2;
  vec3  h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	vec3  n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
  return dot( n, vec3(70.0) );
}
void main()
{
  vec3 pos = APosition;
  vec2 uv  = AUV;
  UV = uv;
  Color = vec4(normalize(pos.xyz), 1.0);
  pos.y = noise(pos.xz*100.0+UTime*0.3)*3.0;
  Height = (pos.y*0.5)+(3.0f*0.5);
  gl_Position = (UModel*vec4(pos, 1.0)); //UProjection
}
