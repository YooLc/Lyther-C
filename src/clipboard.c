#include <windows.h>
#include <winuser.h>

#include "string.h"
#include "clipboard.h"
#include "codeparser.h"
#include "textarea.h"
#include "undoredo.h"

static void mySetClipBoard(char *str)
{
    char *dst;
    HGLOBAL hglbCopy;
    // Open the clipboard, and empty it.
    if (!OpenClipboard(NULL)) return;
    EmptyClipboard();
    // Allocate memory
    hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (strlen(str) + 1) * sizeof(char));
    if (hglbCopy == NULL) {
        CloseClipboard();
        return;
    }
    // Lock the global memory and assign it to dst
    dst = GlobalLock(hglbCopy);
    // Copy str to dst
    memcpy(dst, str, (strlen(str) + 1) * sizeof(char));
    GlobalUnlock(hglbCopy);
    // Place the handle on the clipboard.
    SetClipboardData(CF_TEXT, hglbCopy);
    CloseClipboard();
}

static void myGetClipBoard(char *dst)
{
    if (!IsClipboardFormatAvailable(CF_TEXT) || !OpenClipboard(NULL))
        dst[0] = '\0';
    HGLOBAL hglb;
    char *src;
    hglb = GetClipboardData(CF_TEXT);
    if (hglb != NULL) {
        src = GlobalLock(hglb);
        if (src == NULL) dst[0] = '\0';
        else strcpy(dst, src);
        CloseClipboard();
    } else dst[0] = '\0';
}

void Copy(EditorForm *form)
{
    PosRC posl = form->selectLeftPos, posr = form->selectRightPos;
    if (posl.r > posr.r || (posl.r == posr.r && posl.c > posr.c)) {
        PosRC tmp = posl;
        posl = posr;
        posr = tmp;
    }
    char *dst = getString(form->passage, posl.r, posl.c + 1, posr.r, posr.c);
    mySetClipBoard(dst);
    form->inSelectionMode = false;
    free(dst);
}

void Cut(EditorForm *form)
{
    PosRC posl = form->selectLeftPos, posr = form->selectRightPos;
    if (posl.r > posr.r || (posl.r == posr.r && posl.c > posr.c)) {
        PosRC tmp = posl;
        posl = posr;
        posr = tmp;
    }
    char *dst = getString(form->passage, posl.r, posl.c + 1, posr.r, posr.c);
    mySetClipBoard(dst);
    deleteString(form->passage, posl.r, posl.c + 1, posr.r, posr.c);
    addTrace(form->urStack, DELE, posl.r, posl.c + 1, posr.r, posr.c, dst);
    form->inSelectionMode = false;
    form->caretPos = form->realCaretPos = posl;
    free(dst);
}

void Paste(EditorForm *form)
{
    char *dst = (char *)malloc(sizeof(char) * MAX_LINE_SIZE * 40);
    PosRC newRC;
    myGetClipBoard(dst);
    if (dst[0] == '\0') return;
    newRC = addString(form->passage, dst, form->realCaretPos.r, form->realCaretPos.c + 1);
    addTrace(form->urStack, ADD, form->realCaretPos.r, form->realCaretPos.c + 1, newRC.r,
             newRC.c, dst);
    form->caretPos = form->realCaretPos = newRC;
    free(dst);
}
