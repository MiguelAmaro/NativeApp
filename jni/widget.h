#ifndef WIDGET_H
#define WIDGET_H

typedef struct ui_user_sig ui_user_sig;
struct ui_user_sig
{
  b32 IsTouched;
  b32 JustPressed;
  b32 IsPressed;
  b32 IsSelected;
};
ui_user_sig UIDoButton(ui_elm *Element)
{
  ui_user_sig Result = {
    .IsTouched   = IsInRect(Element->Rect, GlobalTouchPos),
    .IsSelected  = ElementIsSelected(&GlobalUIState, Element),
    .JustPressed = GlobalJustPressed,
    .IsPressed   = GlobalIsPressed,
  };
  Element->Color = ((Element->Id==ELMPUSH_BTN_ELM_ID)?V4f(0.0f, 0.8f, 0.8f, 0.8f):
                    (Element->Id==ELMPOP_BTN_ELM_ID )?V4f(0.8f, 0.0f, 0.0f, 0.8f):
                    V4f(0.08f, 0.08f, 0.08f, 0.8f));
  
  
  if(Result.IsTouched && Result.IsPressed)
  {
    Element->Color = ((Element->Id==ELMPUSH_BTN_ELM_ID)?V4f(0.0f, 1.0f, 1.0f, 1.0f):
                      (Element->Id==ELMPOP_BTN_ELM_ID )?V4f(1.0f, 0.0f, 0.0f, 1.0f):
                      V4f(0.2f, 0.2f, 0.2f, 1.0f));
  }
  if(Element->Flags & UI_Flag_Selectable)
  {
    if(Result.IsSelected && Result.IsTouched)
    {
      UIStateZListPutElementAtTop(&GlobalUIState, Element);
    }
    if(Result.IsSelected && !Result.IsPressed)
    {
      Element->Color = V4f(1.0f, 1.0f, 1.0f, 0.4f);
    }
    if(Result.IsSelected)
    {
      if(!Result.JustPressed && Result.IsPressed)
      {
        v2f HalfDim = V2f((Element->Rect.max.x-Element->Rect.min.x)*0.5f,
                          (Element->Rect.max.y-Element->Rect.min.y)*0.5f);
        
        Element->Rect.min.x = GlobalTouchPos.x-HalfDim.x;
        Element->Rect.min.y = GlobalTouchPos.y-HalfDim.y;
        Element->Rect.max.x = GlobalTouchPos.x+HalfDim.x;
        Element->Rect.max.y = GlobalTouchPos.y+HalfDim.y;
      }
    }
    if(Result.JustPressed && Result.IsTouched && 
       (ElementNoneSelected(&GlobalUIState) || !Result.IsSelected))
    {
      ElementSelect(&GlobalUIState, Element);
    }
  }
  else {}
  return Result;
}

#endif //WIDGET_H
