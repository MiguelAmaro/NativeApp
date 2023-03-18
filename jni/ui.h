#ifndef UI_H
#define UI_H

#define UI_NULL_ELEMENT_ID (U32Max)
typedef struct ui_elm ui_elm;
struct ui_elm
{
  u32 Id;
  r2f Rect;
  v4f Color;
};
#define UI_STACKS_MAX_COUNT (256)
typedef struct ui_stacks ui_stacks;
struct ui_stacks
{
  v4f Color[UI_STACKS_MAX_COUNT];
  v2f Offset[UI_STACKS_MAX_COUNT];
};
#define UI_ELEMENT_MAX_COUNT (64)
typedef struct ui_state ui_state;
struct ui_state
{
  ui_elm Elements[UI_ELEMENT_MAX_COUNT];
  ui_elm *NextSlot;
  ui_elm *OnePastLastSlot;
  u32 ElementCount;
  u32 SelectedId;
};
inline b32 ElementIsBeforeLastPushed(ui_state *State, ui_elm *elm_ptr) {return (elm_ptr < State->NextSlot);}
inline b32 ElementStorageEmpty(ui_state *State) {return (State->NextSlot == State->Elements);}
inline b32 ElementStorageFull (ui_state *State) {return (State->NextSlot == State->OnePastLastSlot);}
inline u32 ElementStorageCount (ui_state *State) {return (State->ElementCount);}
inline u32 ElementIsSelected (ui_state *State, ui_elm *Element) {return (State->SelectedId == Element->Id);}
inline u32 ElementNoneSelected (ui_state *State)                {return (State->SelectedId == UI_NULL_ELEMENT_ID);}
inline u32 ElementSelect      (ui_state *State, ui_elm *Element) {return (State->SelectedId = Element->Id);}

void UIStateInit(ui_state *State)
{
  State->ElementCount = 0;
  State->NextSlot    = (State->Elements);
  State->OnePastLastSlot = (State->Elements + UI_ELEMENT_MAX_COUNT);
  State->SelectedId = UI_NULL_ELEMENT_ID;
  return;
}
void UIStateElementPush(ui_state *State, ui_elm Element)
{
  if(ElementStorageFull(State)) return;
  ui_elm *Slot = State->NextSlot++;
  *Slot = Element;
  State->ElementCount++;
  return;
}
void UIStateElementPop(ui_state *State)
{
  if(ElementStorageEmpty(State)) return;
  ui_elm *Slot = --State->NextSlot;
  ui_elm ZeroedElement = {0};
  *Slot = ZeroedElement;
  State->ElementCount--;
  return;
}
ui_elm UIElementInit(r2f Rect, v4f Color, u32 Id)
{
  assert(Id != UI_NULL_ELEMENT_ID);
  ui_elm Element = { 
    .Rect = Rect, 
    .Color = Color,
    .Id = Id,
  };
  return Element;
}
#endif //UI_H
