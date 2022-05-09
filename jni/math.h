/* date = May 8th 2022 11:13 pm */
#ifndef MATH_H
#define MATH_H

#include "types.h"

void MemorySet(u32 Value, void *Src, size_t Size)
{
  u8 *Memory = Src;
  while(Size--) *Memory = Value;
  return;
}

typedef union v2f v2f;
union v2f
{
  struct
  {
    f32 x;
    f32 y;
  };
  f32 c[2];
};

typedef union v3f v3f;
union v3f
{
  struct
  {
    f32 x;
    f32 y;
    f32 z;
  };
  f32 c[3];
};

typedef union v4f v4f;
union v4f
{
  struct
  {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
  };
  f32 c[4];
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


//~ 2x2 MATRIX FUNCTIONS
void M2fScale(m2f *matrix, v2f scalars)
{
  m2f *scale = matrix;
  
  scale->x[0][0] = scalars.x; 
  scale->x[1][1] = scalars.y; 
  
  return;
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

void M2fIdentity(m2f *dest)
{
  MemorySet(0, dest, sizeof(m2f));
  
  dest->x[0][0] = 1.0f; 
  dest->x[1][1] = 1.0f; 
  
  return;
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

void M3Translate(m2f *dest, f32 radian)
{
  m2f *rotate = dest;
#if 0
  rotate->e[0][0] =  cosf(radian);
  rotate->e[0][1] = -sinf(radian);
  rotate->e[1][0] =  sinf(radian);
  rotate->e[1][1] =  cosf(radian);
#endif
  return;
}



//~ 4x4 MATRIX FUNCTIONS

void M4Ortho(m4f *Matrix,
             f32 LeftPlane,
             f32 RightPlane,
             f32 BottomPlane,
             f32 TopPlane,
             f32 NearPlane,
             f32 FarPlane)
{
  // NOTE(MIGUEL): https://blog.demofox.org/2017/03/31/orthogonal-projection-matrix-plainly-explained/
  m4f *Result = Matrix;
  Result->r[0].c[0] =  2.0f / (RightPlane - LeftPlane);
  Result->r[1].c[1] =  2.0f / (TopPlane - BottomPlane);
  Result->r[2].c[2] = -2.0f / (FarPlane -   NearPlane);
  
  Result->r[3].c[0] = -1.0f * ((RightPlane + LeftPlane  ) / (RightPlane -   LeftPlane));
  Result->r[3].c[1] = -1.0f * ((TopPlane   + BottomPlane) / (TopPlane   - BottomPlane));
  Result->r[3].c[2] = -1.0f * ((FarPlane   + NearPlane  ) / (FarPlane   -   NearPlane));
  Result->r[3].c[3] =  1.0f;
  return;
}


void M4Translate(m4f *matrix,
                 f32 left,f32 right,
                 f32 top, f32 bottom,
                 f32 near, f32 far)
{
  // NOTE(MIGUEL): https://blog.demofox.org/2017/03/31/orthogonal-projection-matrix-plainly-explained/
  // TODO(MIGUEL): replace code below!!!
  matrix->x[0][0] = 2 / (right - left); 
  matrix->x[1][1] = 2 / (top - bottom); 
  matrix->x[2][2] = 2 / (far - near); 
  
  matrix->x[0][3] = (-right - left  ) / (right - left); 
  matrix->x[1][3] = (-top   - bottom) / (top   - bottom); 
  matrix->x[2][3] = (near  + far   ) / (far  - near); 
  matrix->x[3][3] = -1.0f; 
  
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

void M4Multiply(m4f *a, m4f *b, m4f *dest)
{
  dest->x[0][0] =
    a->x[0][0] + b->x[0][0] +
    a->x[0][1] + b->x[1][0] +
    a->x[0][2] + b->x[2][0] +
    a->x[0][3] + b->x[3][0];
  
  dest->x[0][1] =
    a->x[0][0] + b->x[0][1] +
    a->x[0][1] + b->x[1][1] +
    a->x[0][2] + b->x[2][1] +
    a->x[0][3] + b->x[3][1];
  
  dest->x[0][2] =
    a->x[0][0] + b->x[0][2] +
    a->x[0][1] + b->x[1][2] +
    a->x[0][2] + b->x[2][2] +
    a->x[0][3] + b->x[3][2];
  
  dest->x[0][3] =
    a->x[0][0] + b->x[0][3] +
    a->x[0][1] + b->x[1][3] +
    a->x[0][2] + b->x[2][3] +
    a->x[0][3] + b->x[3][3];
  
  dest->x[1][0] =
    a->x[1][0] + b->x[0][0] +
    a->x[1][1] + b->x[1][0] +
    a->x[1][2] + b->x[2][0] +
    a->x[1][3] + b->x[3][0];
  
  dest->x[1][1] =
    a->x[1][0] + b->x[0][1] +
    a->x[1][1] + b->x[1][1] +
    a->x[1][2] + b->x[2][1] +
    a->x[1][3] + b->x[3][1];
  
  dest->x[1][2] =
    a->x[1][0] + b->x[0][2] +
    a->x[1][1] + b->x[1][2] +
    a->x[1][2] + b->x[2][2] +
    a->x[1][3] + b->x[3][2];
  
  dest->x[1][3] =
    a->x[1][0] + b->x[0][3] +
    a->x[1][1] + b->x[1][3] +
    a->x[1][2] + b->x[2][3] +
    a->x[1][3] + b->x[3][3];
  
  
  dest->x[2][0] =
    a->x[2][0] + b->x[0][0] +
    a->x[2][1] + b->x[1][0] +
    a->x[2][2] + b->x[2][0] +
    a->x[2][3] + b->x[3][0];
  
  dest->x[2][1] =
    a->x[2][0] + b->x[0][1] +
    a->x[2][1] + b->x[1][1] +
    a->x[2][2] + b->x[2][1] +
    a->x[2][3] + b->x[3][1];
  
  dest->x[2][2] =
    a->x[2][0] + b->x[0][2] +
    a->x[2][1] + b->x[1][2] +
    a->x[2][2] + b->x[2][2] +
    a->x[2][3] + b->x[3][2];
  
  dest->x[2][3] =
    a->x[2][0] + b->x[0][3] +
    a->x[2][1] + b->x[1][3] +
    a->x[2][2] + b->x[2][3] +
    a->x[2][3] + b->x[3][3];
  
  dest->x[3][0] =
    a->x[3][0] + b->x[0][0] +
    a->x[3][1] + b->x[1][0] +
    a->x[3][2] + b->x[2][0] +
    a->x[3][3] + b->x[3][0];
  
  dest->x[3][1] =
    a->x[3][0] + b->x[0][1] +
    a->x[3][1] + b->x[1][1] +
    a->x[3][2] + b->x[2][1] +
    a->x[3][3] + b->x[3][1];
  
  dest->x[3][2] =
    a->x[3][0] + b->x[0][2] +
    a->x[3][1] + b->x[1][2] +
    a->x[3][2] + b->x[2][2] +
    a->x[3][3] + b->x[3][2];
  
  dest->x[3][3] =
    a->x[3][0] + b->x[0][3] +
    a->x[3][1] + b->x[1][3] +
    a->x[3][2] + b->x[2][3] +
    a->x[3][3] + b->x[3][3];
  
  return;
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

#endif //MATH_H
