/* date = May 8th 2022 11:13 pm */
#ifndef MATH_H
#define MATH_H

#include "types.h"

typedef union v2f v2f;
union v2f
{
  struct
  {
    f32 x;
    f32 y;
  };
  f32 v[2];
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
  f32 e[3];
};

typedef union m2 m2;
union m2
{
  f32 v[4];
  f32 e[2][2];
};

typedef union m4 m4;
union m3
{
  f32 v[9];
  f32 e[3][3];
};

typedef union m4 m4;
union m4
{
  f32 v[16];
  f32 e[4][4];
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
void M2Scale(m2 *matrix, v2f scalars)
{
  m2 *scale = matrix;
  
  scale->e[0][0] = scalars.x; 
  scale->e[1][1] = scalars.y; 
  
  return;
}

void M2Multiply(m2 *a, m2 *b, m2 *dest)
{
  dest->e[0][0] =
    a->e[0][0] * b->e[0][0] +
    a->e[0][1] * b->e[1][0];
  
  dest->e[0][1] =
    a->e[0][0] * b->e[0][1] +
    a->e[0][1] * b->e[1][1];
  
  
  dest->e[1][0] =
    a->e[1][0] * b->e[0][0] +
    a->e[1][1] * b->e[1][0];
  
  dest->e[1][1] =
    a->e[1][0] * b->e[0][1] +
    a->e[1][1] * b->e[1][1];
  
  return;
}

void M2ShearX(m2 *dest, f32 x)
{
  m2 *shear_x = dest;
#if 0
  shear_x->e[0][1] = tanf(x);
  shear_x->e[1][0] = 0.0f;
  shear_x->e[0][0] = 1.0f;
  shear_x->e[1][1] = 1.0f;
#endif
  return;
}


void M2ShearY(m2 *dest, f32 y)
{
  m2 *shear_y = dest;
#if 0
  shear_y->e[0][1] = 0.0f;
  shear_y->e[1][0] = tanf(y);
  shear_y->e[0][0] = 1.0f;
  shear_y->e[1][1] = 1.0f;
#endif
  return;
}


void M2ShearXTan(m2 *dest, f32 theta)
{
  m2 shear = {0};
#if 0
  shear.e[0][0] = 1.0f;
  shear.e[0][1] = tanf(theta);
  shear.e[1][0] = 0.0f;
  shear.e[1][1] = 1.0f;
#endif
  M2Multiply(&shear, dest, dest);
  
  return;
}

void M2Identity(m2 *dest)
{
  memset(dest, 0, sizeof(dest));
  
  dest->e[0][0] = 1.0f; 
  dest->e[1][1] = 1.0f; 
  
  return;
}


void M2Rotate(m2 *dest, f32 radian, f32 Cos, f32 Sin)
{
  m2 *rotate = dest;
  
  rotate->e[0][0] =  Cos;
  rotate->e[0][1] = -Sin;
  rotate->e[1][0] =  Sin;
  rotate->e[1][1] =  Cos;
  
  return;
}


//~ 4x4 MATRIX FUNCTIONS

void M3Translate(m2 *dest, f32 radian)
{
  m2 *rotate = dest;
#if 0
  rotate->e[0][0] =  cosf(radian);
  rotate->e[0][1] = -sinf(radian);
  rotate->e[1][0] =  sinf(radian);
  rotate->e[1][1] =  cosf(radian);
#endif
  return;
}



//~ 4x4 MATRIX FUNCTIONS

void M4Ortho(m4 *matrix,
             f32 left,f32 right,
             f32 top, f32 bottom,
             f32 near, f32 far)
{
  // NOTE(MIGUEL): https://blog.demofox.org/2017/03/31/orthogonal-projection-matrix-plainly-explained/
  matrix->e[0][0] = 2 / (right - left); 
  matrix->e[1][1] = 2 / (top - bottom); 
  matrix->e[2][2] = 2 / (far - near); 
  
  matrix->e[0][3] = (-right - left  ) / (right - left); 
  matrix->e[1][3] = (-top   - bottom) / (top   - bottom); 
  matrix->e[2][3] = (near  + far   ) / (far  - near); 
  matrix->e[3][3] = -1.0f; 
  
  return;
}


void M4Translate(m4 *matrix,
                 f32 left,f32 right,
                 f32 top, f32 bottom,
                 f32 near, f32 far)
{
  // NOTE(MIGUEL): https://blog.demofox.org/2017/03/31/orthogonal-projection-matrix-plainly-explained/
  // TODO(MIGUEL): replace code below!!!
  matrix->e[0][0] = 2 / (right - left); 
  matrix->e[1][1] = 2 / (top - bottom); 
  matrix->e[2][2] = 2 / (far - near); 
  
  matrix->e[0][3] = (-right - left  ) / (right - left); 
  matrix->e[1][3] = (-top   - bottom) / (top   - bottom); 
  matrix->e[2][3] = (near  + far   ) / (far  - near); 
  matrix->e[3][3] = -1.0f; 
  
  return;
}

void M4Scale(m4 *matrix, v3f scalars)
{
  matrix->e[0][0] *= scalars.x; 
  matrix->e[1][1] *= scalars.y; 
  matrix->e[2][2] *= scalars.z; 
  matrix->e[3][3] *= 1.0f; 
  
  
  return;
}

void M4ShearX(m4 *matrix, f32 s)
{
  matrix->e[0][0] = (s * matrix->e[1][1]) + matrix->e[0][0]; 
  
  return;
}


void M4ShearXTan(m4 *matrix, f32 theta)
{
  
  
  return;
}

void M4Multiply(m4 *a, m4 *b, m4 *dest)
{
  dest->e[0][0] =
    a->e[0][0] + b->e[0][0] +
    a->e[0][1] + b->e[1][0] +
    a->e[0][2] + b->e[2][0] +
    a->e[0][3] + b->e[3][0];
  
  dest->e[0][1] =
    a->e[0][0] + b->e[0][1] +
    a->e[0][1] + b->e[1][1] +
    a->e[0][2] + b->e[2][1] +
    a->e[0][3] + b->e[3][1];
  
  dest->e[0][2] =
    a->e[0][0] + b->e[0][2] +
    a->e[0][1] + b->e[1][2] +
    a->e[0][2] + b->e[2][2] +
    a->e[0][3] + b->e[3][2];
  
  dest->e[0][3] =
    a->e[0][0] + b->e[0][3] +
    a->e[0][1] + b->e[1][3] +
    a->e[0][2] + b->e[2][3] +
    a->e[0][3] + b->e[3][3];
  
  dest->e[1][0] =
    a->e[1][0] + b->e[0][0] +
    a->e[1][1] + b->e[1][0] +
    a->e[1][2] + b->e[2][0] +
    a->e[1][3] + b->e[3][0];
  
  dest->e[1][1] =
    a->e[1][0] + b->e[0][1] +
    a->e[1][1] + b->e[1][1] +
    a->e[1][2] + b->e[2][1] +
    a->e[1][3] + b->e[3][1];
  
  dest->e[1][2] =
    a->e[1][0] + b->e[0][2] +
    a->e[1][1] + b->e[1][2] +
    a->e[1][2] + b->e[2][2] +
    a->e[1][3] + b->e[3][2];
  
  dest->e[1][3] =
    a->e[1][0] + b->e[0][3] +
    a->e[1][1] + b->e[1][3] +
    a->e[1][2] + b->e[2][3] +
    a->e[1][3] + b->e[3][3];
  
  
  dest->e[2][0] =
    a->e[2][0] + b->e[0][0] +
    a->e[2][1] + b->e[1][0] +
    a->e[2][2] + b->e[2][0] +
    a->e[2][3] + b->e[3][0];
  
  dest->e[2][1] =
    a->e[2][0] + b->e[0][1] +
    a->e[2][1] + b->e[1][1] +
    a->e[2][2] + b->e[2][1] +
    a->e[2][3] + b->e[3][1];
  
  dest->e[2][2] =
    a->e[2][0] + b->e[0][2] +
    a->e[2][1] + b->e[1][2] +
    a->e[2][2] + b->e[2][2] +
    a->e[2][3] + b->e[3][2];
  
  dest->e[2][3] =
    a->e[2][0] + b->e[0][3] +
    a->e[2][1] + b->e[1][3] +
    a->e[2][2] + b->e[2][3] +
    a->e[2][3] + b->e[3][3];
  
  dest->e[3][0] =
    a->e[3][0] + b->e[0][0] +
    a->e[3][1] + b->e[1][0] +
    a->e[3][2] + b->e[2][0] +
    a->e[3][3] + b->e[3][0];
  
  dest->e[3][1] =
    a->e[3][0] + b->e[0][1] +
    a->e[3][1] + b->e[1][1] +
    a->e[3][2] + b->e[2][1] +
    a->e[3][3] + b->e[3][1];
  
  dest->e[3][2] =
    a->e[3][0] + b->e[0][2] +
    a->e[3][1] + b->e[1][2] +
    a->e[3][2] + b->e[2][2] +
    a->e[3][3] + b->e[3][2];
  
  dest->e[3][3] =
    a->e[3][0] + b->e[0][3] +
    a->e[3][1] + b->e[1][3] +
    a->e[3][2] + b->e[2][3] +
    a->e[3][3] + b->e[3][3];
  
  return;
}


void M4Identity(m4 *matrix)
{
  memset(matrix, 0, sizeof(matrix));
  
  matrix->e[0][0] = 1.0f; 
  matrix->e[1][1] = 1.0f; 
  matrix->e[2][2] = 1.0f; 
  matrix->e[3][3] = 1.0f; 
  
  return;
}

#endif //MATH_H
