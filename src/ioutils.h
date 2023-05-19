#ifndef _IO_UTILS_H_
#define _IO_UTILS_H_

#include "textarea.h"

void moveCaret(EditorForm *form, CaretAction action, char* curLine, char* preLine);
void deleteLastChar(EditorForm* form);
PosRC pixelToPosRC(EditorForm *form, int px, int py);

void handleMouseEvent(Editor* editor, int x, int y, int button, int event); 
void handleInputEvent(Editor* editor, char ch);
void handleKeyboardEvent(Editor* editor, int key, int event);

#endif
