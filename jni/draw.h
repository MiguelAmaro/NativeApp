#ifndef DRAW_H
#define DRAW_H

#define DRAW_BUCKET_MAX_COUNT (256)
// TODO(MIGUEL): add border thickness, roundness, border color
typedef struct quad_attribs quad_attribs;
struct quad_attribs
{r2f Rect; v4f Color;};
typedef struct draw_bucket draw_bucket;
struct draw_bucket
{
  //trasform (projection)
  quad_attribs QuadAttribs [DRAW_BUCKET_MAX_COUNT];
  u32 Count;
  mat4 Model;
  mat4 Projection;
};
void DrawBucketBegin(draw_bucket *Bucket, mat4 Model, mat4 Projection)
{
  // TODO(MIGUEL): zero QuadAttribs array, maybe?
  Bucket->Count = 0;
  if(Model     ) { memcpy(Bucket->Model     , Model     , sizeof(mat4)); }
  if(Projection) { memcpy(Bucket->Projection, Projection, sizeof(mat4)); }
  return;
}
void DrawBucketEnd(draw_bucket *Bucket)
{
  // NOTE(MIGUEL): push bucket to some storage for later for gfx rendering code
}
void DrawBucketPushText(draw_bucket *Bucket)
{
  return;
}
void DrawBucketPushQuad(draw_bucket *Bucket, u32 Count)
{
  //vertex3d *Verts, 
  return;
}
void DrawBucketPushUIElements(draw_bucket *Bucket, ui_elm *Elements, u32 Count)
{
  if(!(Bucket->Count<DRAW_BUCKET_MAX_COUNT)) return;
  for(ui_elm *Current = Elements; Current!=NULL; Current = Current->Next)
  {
    quad_attribs *Attribs = &Bucket->QuadAttribs[Bucket->Count++];
    Attribs->Rect  = Current->Rect;
    Attribs->Color = Current->Color;
    //DrawBucketPushText(draw_bucket *Bucket, Elements.Text);
  }
  return;
}

#endif //DRAW_H
