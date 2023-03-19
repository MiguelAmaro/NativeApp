#ifndef UI_H
#define UI_H

// current element identifiers:
//   - pointers
//   - id (indexes atm)
//   - hashed keys(not impl)

#define UI_NULL_ELEMENT_ID (U32Max)
typedef struct ui_elm ui_elm;
struct ui_elm
{
  u32 Id;
  r2f Rect;
  v4f Color;
  //z list: draw order
  ui_elm *Next;
  ui_elm *Prev;
  //u32 *Text;
  //hierarcy
  //...
};
#define UI_STACKS_MAX_COUNT (256)
typedef struct ui_stacks ui_stacks;
struct ui_stacks
{
  v4f Color [UI_STACKS_MAX_COUNT];
  v2f Offset[UI_STACKS_MAX_COUNT];
};
typedef struct ui_zlist ui_zlist;
struct ui_zlist
{
  ui_elm *Top;
  ui_elm *Bottom;
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
  //draw order not stack dependent
  ui_zlist ZList;
};
ui_state GlobalUIState = {0};
inline b32 ElementIsBeforeLastPushed(ui_state *State, ui_elm *elm_ptr) {return (elm_ptr < State->NextSlot);}
inline b32 ElementStorageEmpty      (ui_state *State) {return (State->NextSlot == State->Elements);}
inline b32 ElementStorageFull       (ui_state *State) {return (State->NextSlot == State->OnePastLastSlot);}
inline u32 ElementStorageCount      (ui_state *State) {return (State->ElementCount);}
inline u32 ElementIsSelected        (ui_state *State, ui_elm *Element) {return (State->SelectedId == Element->Id);}
inline u32 ElementNoneSelected      (ui_state *State)                  {return (State->SelectedId == UI_NULL_ELEMENT_ID);}
inline u32 ElementSelect            (ui_state *State, ui_elm *Element) {return (State->SelectedId = Element->Id);}
inline u32 ElementIsNull            (ui_elm *Element)                  {return (Element->Id == UI_NULL_ELEMENT_ID);}
#include "ui_logs.h"

void UIStateInit(ui_state *State)
{
  State->ElementCount    = 0;
  State->NextSlot        = (State->Elements);
  State->OnePastLastSlot = (State->Elements + UI_ELEMENT_MAX_COUNT);
  State->SelectedId   = UI_NULL_ELEMENT_ID;
  State->ZList.Top    = NULL;
  State->ZList.Bottom = NULL;
  return;
}
ui_elm *UIElementGetById(u32 Id)
{
  ui_elm *Result = (Id!=UI_NULL_ELEMENT_ID)?&GlobalUIState.Elements[Id]:NULL;
  return Result;
}
void UIStateZListPutElementAtTop(ui_state *State, ui_elm *Element)
{
  if(State->ZList.Bottom == Element && Element == State->ZList.Top)
  {//only element
    //do nothing; is already top
  }
  else if(Element == State->ZList.Top)
  {// there is at least one elm behind
    // do nothing; is already top
  }
  else if(Element == State->ZList.Bottom)
  {// there is at least one elm ahead
    ui_elm *LastTop = State->ZList.Top;
    State->ZList.Bottom = Element->Next;
    State->ZList.Top    = Element;
    // new top bi link
    LastTop->Next = Element;
    Element->Prev = LastTop;
    //cleanup
    State->ZList.Bottom->Prev = NULL;
    Element->Next = NULL;
  }
  else
  {// there is at least one elm behind and ahead
    ui_elm *LastTop = State->ZList.Top;
    State->ZList.Top = Element;
    // fix disconect
    Element->Prev->Next = Element->Next;
    Element->Next->Prev = Element->Prev; 
    // new top bi link
    LastTop->Next = Element;
    Element->Prev = LastTop;
    //cleanup
    Element->Next = NULL;
  }
  return;
}
void UIStateZListHandlePushedElement(ui_state *State, ui_elm *Element)
{
  // NOTE(MIGUEL): v This condition is highly dependent on stack mgmt code and may falsly report
  //                 storage as not empty on first elm push.
  if(ElementStorageEmpty(State))
  {
    State->ZList.Top = State->ZList.Bottom = Element;
  }
  else
  {//push to the bottom/front
    ui_elm *LastBottom = State->ZList.Bottom;
    //forward link
    State->ZList.Bottom = Element;
    Element->Next       = LastBottom;
    //back link
    LastBottom->Prev = Element;
    //cleanup
    Element->Prev   = NULL;
  }
  return;
}
void UIStateZListHandlePoppedElement(ui_state *State, ui_elm *Element)
{
  if(State->ZList.Bottom == Element && Element == State->ZList.Top)
  {//only element
    State->ZList.Top    = NULL;
    State->ZList.Bottom = NULL;
  }
  else if(Element == State->ZList.Bottom)
  {// there is at least one elm ahead
    // fix disconnect
    State->ZList.Bottom = Element->Next;
    //cleanup
    State->ZList.Bottom->Prev = NULL;
  }
  else if(Element == State->ZList.Top)
  {// there is at least one elm behind
    //cleanup
    State->ZList.Top->Next = NULL;
  }
  else
  {// there is at least one elm behind and ahead
    // fix disconect
    Element->Prev->Next = Element->Next;
    Element->Next->Prev = Element->Prev; 
  }
  //clean elm
  Element->Next = NULL;
  Element->Prev = NULL;
  return;
}
void UIStateElementPush(ui_state *State, ui_elm Element)
{
  if(ElementStorageFull(State)) return;
  ui_elm *NewElement = State->NextSlot;
  *NewElement = Element;
  UIStateZListHandlePushedElement(State, NewElement); 
  // NOTE(MIGUEL): ^ this call is relying on the next slot pointer
  //                 pointing at the first slot indicating empty to handle linked list operations
  State->ElementCount++;
  State->NextSlot++;
  return;
}
void UIStateElementPop(ui_state *State)
{
  if(ElementStorageEmpty(State)) return;
  ui_elm *TopElement = (State->NextSlot - 1);
  ui_elm ZeroedElement = {0};
  UIStateZListHandlePoppedElement(State, TopElement);
  // NOTE(MIGUEL): ^ this call is relying on the top element not being zeroed
  //                 to handle linked list operations
  *TopElement = ZeroedElement;
  State->ElementCount--;
  State->NextSlot--;
  return;
}
ui_elm UIElementInit(r2f Rect, v4f Color, u32 Id)
{
  // NOTE(MIGUEL): assisgned id could point to somethind out side the table
  assert(Id<UI_ELEMENT_MAX_COUNT);
  assert(Id!=UI_NULL_ELEMENT_ID);
  ui_elm Element = { 
    .Rect = Rect, 
    .Color = Color,
    .Id = Id,
    .Next = NULL,
    .Prev = NULL,
  };
  return Element;
}

#endif //UI_H
