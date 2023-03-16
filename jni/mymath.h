/* date = May 8th 2022 11:13 pm */
#ifndef MATH_H
#define MATH_H
#include <math.h>
#include <cglm/cglm.h>

void MemorySet(u32 Value, void *Src, size_t Size)
{
  u8 *Memory = Src;
  while(Size--) *Memory = Value;
  return;
}

typedef union v2f v2f;
union v2f
{
  struct{ f32 x; f32 y; };
  f32 comp[2];
};
typedef union v3f v3f;
union v3f
{
  struct{ f32 x; f32 y; f32 z;};
  f32 comp[3];
};
typedef union v4f v4f;
union v4f
{
  struct { f32 x; f32 y; f32 z; f32 w; };
  f32 comp[4];
};
typedef union m2f m2f;
union m2f
{
  f32 v[4];
  f32 x[2][2];
};

typedef union m3 m3;
union m3
{
  f32 v[9];
  f32 x[3][3];
};

typedef union m4f m4f;
union m4f
{
  v4f r[4];
  f32 e[16];
  f32 x[4][4];
};
typedef union r2f r2f;
union r2f
{
  struct { v2f min; v2f max; };
  f32 comp[4];
};


//~ VECTOR 2 FUNCTIONS
v2f V2f(f32 x, f32 y)
{
  v2f Result = {0};
  Result.x = x;
  Result.y = y;
  return Result;
}
v2f V2fScale(v2f a, f32 Scalar)
{
  v2f Result = {0};
  Result.x = a.x*Scalar;
  Result.y = a.y*Scalar;
  return Result;
}
v4f V4f(f32 x, f32 y, f32 z, f32 w)
{
  v4f Result = { x,y,z,w };
  return Result;
}
//~ 2x2 MATRIX FUNCTIONS
m2f M2fScale(f32 x, f32 y)
{
  m2f Result = {0};
  Result.x[0][0] = x; 
  Result.x[1][1] = y; 
  return Result;
}

void M2fMultiply(m2f *a, m2f *b, m2f *dest)
{
  dest->x[0][0] =
    a->x[0][0] * b->x[0][0] +
    a->x[0][1] * b->x[1][0];
  
  dest->x[0][1] =
    a->x[0][0] * b->x[0][1] +
    a->x[0][1] * b->x[1][1];
  
  
  dest->x[1][0] =
    a->x[1][0] * b->x[0][0] +
    a->x[1][1] * b->x[1][0];
  
  dest->x[1][1] =
    a->x[1][0] * b->x[0][1] +
    a->x[1][1] * b->x[1][1];
  
  return;
}
void M2fShearX(m2f *dest, f32 x)
{
  m2f *shear_x = dest;
#if 0
  shear_x->e[0][1] = tanf(x);
  shear_x->e[1][0] = 0.0f;
  shear_x->e[0][0] = 1.0f;
  shear_x->e[1][1] = 1.0f;
#endif
  return;
}
void M2ShearY(m2f *dest, f32 y)
{
  m2f *shear_y = dest;
#if 0
  shear_y->e[0][1] = 0.0f;
  shear_y->e[1][0] = tanf(y);
  shear_y->e[0][0] = 1.0f;
  shear_y->e[1][1] = 1.0f;
#endif
  return;
}
void M2fShearXTan(m2f *dest, f32 theta)
{
  m2f shear = {0};
#if 0
  shear.e[0][0] = 1.0f;
  shear.e[0][1] = tanf(theta);
  shear.e[1][0] = 0.0f;
  shear.e[1][1] = 1.0f;
#endif
  M2fMultiply(&shear, dest, dest);
  
  return;
}
m2f M2fIdentity(void)
{
  m2f Result = {0};
  Result.x[0][0] = 1.0f; 
  Result.x[1][1] = 1.0f; 
  return Result;
}
void M2fRotate(m2f *dest, f32 radian, f32 Cos, f32 Sin)
{
  m2f *rotate = dest;
  
  rotate->x[0][0] =  Cos;
  rotate->x[0][1] = -Sin;
  rotate->x[1][0] =  Sin;
  rotate->x[1][1] =  Cos;
  
  return;
}


//~ 4x4 MATRIX FUNCTIONS

void M4Translate(m4f *Dest, v3f Shift)
{
  glm_translate(Dest, Shift.comp);
  return;
}



//~ 4x4 MATRIX FUNCTIONS
void M4Ortho(m4f *Matrix,
             f32 LeftPlane  , f32 RightPlane,
             f32 BottomPlane, f32 TopPlane,
             f32 NearPlane  , f32 FarPlane)
{
  glm_ortho(LeftPlane, RightPlane, BottomPlane, TopPlane, NearPlane, FarPlane, Matrix);
  return;
}
void M4Scale(m4f *matrix, v3f scalars)
{
  matrix->x[0][0] *= scalars.x; 
  matrix->x[1][1] *= scalars.y; 
  matrix->x[2][2] *= scalars.z; 
  matrix->x[3][3] *= 1.0f; 
  
  
  return;
}

void M4ShearX(m4f *matrix, f32 s)
{
  matrix->x[0][0] = (s * matrix->x[1][1]) + matrix->x[0][0]; 
  
  return;
}


void M4ShearXTan(m4f *matrix, f32 theta)
{
  
  
  return;
}

void M4Mul(m4f *a, m4f *b, m4f *dest)
{
  glm_mat4_mul(a, b, dest);
  return;
}

b32 IsInRect(r2f Rect, v2f Touch)
{
  return ((Rect.min.x <= Touch.x && Touch.x <= Rect.max.x) &&
          (Rect.min.y <= Touch.y && Touch.y <= Rect.max.y));
}

void M4Identity(m4f *Matrix)
{
  MemorySet(0, Matrix, sizeof(m4f));
  
  Matrix->x[0][0] = 1.0f; 
  Matrix->x[1][1] = 1.0f; 
  Matrix->x[2][2] = 1.0f; 
  Matrix->x[3][3] = 1.0f; 
  
  return;
}

r2f R2f(v2f min, v2f max)
{
  r2f Result = {min, max};
  return Result;
}
#endif //MATH_H
