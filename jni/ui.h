#ifndef UI_H
#define UI_H

typedef struct ui ui;
struct ui
{
  r2f Rect;
  b32 IsSelected;
  v4f Color;
};

#define UI_STACKS_MAX_COUNT (64)
typedef struct ui_stacks ui_stacks;
struct ui_stacks
{
  v4f Color[UI_STACKS_MAX_COUNT];
  //v4f Color[UI_STACKS_MAX_COUNT];
};




#endif //UI_H
