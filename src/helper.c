#include <stdio.h>
#include <stdbool.h>

#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "style.h"
#include "helper.h"

static bool isActivated = false;
static string manual_EN_US[] = { "Mannual",
"Using this editor is simply like using other code editors, such as Dev-C++, VsCode or notepad.",
"Just type what you want to type, then you are on your way.",
"Lots of useful features are implemented in this editor, such as Code Highlighting and Auto Complete.",
"Press keyboard to type. Press <Ctrl+H> to exit."
};

static string manual_ZH_CN[] = { "使用教程",
"这款代码编辑器和其他的的编辑器没什么两样，",
"按照使用 Dev-C++, VsCode 或者记事本的方法即可",
"此外，我们还实现了很多实用的功能，比如代码高亮和自动补全",
"用键盘敲字吧~ 按 <Ctrl+H> 退出"
};

void drawHelper(double x, double y, double w, double h)
{
    if (!isActivated) return;
    SetPenColor(getBackgroundColor());
    drawRectangle(x, y, w, h, 1);
    printManual(x, y + h - 1, manual_EN_US, sizeof(manual_ZH_CN) / sizeof(string));
    SetFont("微软雅黑");
    printManual(x, y + h - 3, manual_ZH_CN, sizeof(manual_ZH_CN) / sizeof(string));
    SetFont("Consolas");
}

static void printManual(double x, double y, string manual[], int n)
{
    int i;
    double fontHeight = GetFontHeight(), fontAscent = GetFontAscent();
    double currentY = y - fontHeight;
    SetPenColor("White");
    for (i = 0; i < n; i++) {
        MovePen(x, currentY + fontAscent);
        DrawTextString(manual[i]);
        currentY -= fontHeight;
    }
}

void closeHelper()
{
    isActivated = false;
}

void activateHelper()
{
    isActivated = true;
}

bool isHelperActivated()
{
    return isActivated;
}
