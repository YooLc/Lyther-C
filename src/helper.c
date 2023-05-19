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

static string manual_ZH_CN[] = { "ʹ�ý̳�",
"������༭���������ĵı༭��ûʲô������",
"����ʹ�� Dev-C++, VsCode ���߼��±��ķ�������",
"���⣬���ǻ�ʵ���˺ܶ�ʵ�õĹ��ܣ��������������Զ���ȫ",
"�ü������ְ�~ �� <Ctrl+H> �˳�"
};

void drawHelper(double x, double y, double w, double h)
{
    if (!isActivated) return;
    SetPenColor(getBackgroundColor());
    drawRectangle(x, y, w, h, 1);
    printManual(x, y + h - 1, manual_EN_US, sizeof(manual_ZH_CN) / sizeof(string));
    SetFont("΢���ź�");
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
