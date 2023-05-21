#include "menu.h"
#include "locale.h"
#include "extgraph.h"
#include "undoredo.h"
#include "clipboard.h"

void initMenu()
{
    SetPointSize(15);
    menu.activate = 0;
    menu.entryMargin = 0.03;
    menu.entryHeight = GetFontHeight() + 2 * menu.entryMargin + GetFontDescent();
    menu.menuHeight = MENUENTRY * menu.entryHeight;
    menu.menuWidth = MENUWIDTH;
    menu.selected = -1;
    menu.posx = 100;
    menu.posy = 100;
    DefineColor("MenuBackground", (double)223 / 256, (double)230 / 256, (double)233 / 256);
    DefineColor("MenuSelected", (double)178 / 256, (double)190 / 256, (double)195 / 256);
}

static int inMenu(int x, int y)
{
    if (!menu.activate) return 0;

    if (x >= menu.posx + menu.menuWidth * GetXResolution()
        || y >= menu.posy + menu.menuHeight * GetYResolution() || x <= menu.posx || y <= menu.posy) {
        menu.selected = -1;
        return 0;
    }

    int entry = (y - menu.posy) / (menu.entryHeight * GetYResolution());
    menu.selected = entry;
    return 1;
}

void menuGetMouse(EditorForm *form, int x, int y, int button, int event)
{
    inMenu(x, y);

    switch (event) {
    case BUTTON_DOWN:
        if (button == RIGHT_BUTTON) {
            menu.activate = 1;
            menu.posx = x;
            menu.posy = y;
        } else if (button == LEFT_BUTTON && menu.selected != -1) {
            menu.activate = 0;
            PosRC newPos;

            switch (menu.selected) {
            case 0:
                if (form->inSelectionMode) Copy(form);

                break;

            case 1:
                if (form->inSelectionMode) Cut(form);

                break;

            case 2:
                Paste(form);
                break;

            case 3:
                newPos = Undo(form->urStack);

                if (newPos.r != -1) form->caretPos = form->realCaretPos = newPos;

                break;

            case 4:
                newPos = Redo(form->urStack);

                if (newPos.r != -1) form->caretPos = form->realCaretPos = newPos;

                break;
            }

            //printf("%d %d %d %d\n", form->selectLeftPos.r, form->selectLeftPos.c, form->selectRightPos.r, form->selectRightPos.c);
            menu.selected = -1;
        } else
            menu.activate = 0;
    }
}

void displayMenu()
{
    if (!menu.activate) return;

    int i = 0;
    //Draw menu background
    SetPenColor("MenuBackground");
    SetPenSize(1);
    MovePen(ScaleXInches(menu.posx), ScaleYInches(menu.posy));
    StartFilledRegion(1);
    DrawLine(menu.menuWidth, 0);
    DrawLine(0, -menu.menuHeight);
    DrawLine(-menu.menuWidth, 0);
    DrawLine(0, menu.menuHeight);
    EndFilledRegion();
    SetPenColor("Black");
    DrawLine(menu.menuWidth, 0);
    DrawLine(0, -menu.menuHeight);
    DrawLine(-menu.menuWidth, 0);
    DrawLine(0, menu.menuHeight);
    SetPenColor("Black");

    for (i = 0; i < MENUENTRY; i++) {
        MovePen(ScaleXInches(menu.posx),
                ScaleYInches(menu.posy) - (i + 1)*menu.entryHeight + GetFontDescent() + menu.entryMargin);
        DrawTextString(menuEntry[i]);
    }

    //Highlight selected entry
    if (menu.selected == -1) return;

    SetPenColor("MenuSelected");
    SetPenSize(1);
    MovePen(ScaleXInches(menu.posx) + menu.entryMargin,
            ScaleYInches(menu.posy) - menu.selected * menu.entryHeight - menu.entryMargin);
    StartFilledRegion(1);
    DrawLine(menu.menuWidth - menu.entryMargin * 2, 0);
    DrawLine(0, -(menu.entryHeight - menu.entryMargin * 2));
    DrawLine(-menu.menuWidth + menu.entryMargin * 2, 0);
    DrawLine(0, menu.entryHeight - menu.entryMargin * 2);
    EndFilledRegion();
    SetPenColor("White");
    MovePen(ScaleXInches(menu.posx),
            ScaleYInches(menu.posy) - (menu.selected + 1)*menu.entryHeight + GetFontDescent() +
            menu.entryMargin);
    DrawTextString(menuEntry[menu.selected]);
}
