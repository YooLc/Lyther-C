#include <stdio.h>
#include <stdbool.h>

#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "style.h"
#include "help.h"

static bool isHelperActivated = false, isAboutActivated = false;
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

static string about_EN_US[] = {"St. Jiaran Version 1.3",
"Built by TDM-GCC 4.9.2 64-bit Release",
"Author: <Placeholder for git repository>",
"Press <Ctrl+B> to exit."};

static string about_ZH_CN[] = {"֯�Ʊ༭�� Version 1.3",
"�� TDM-GCC 4.9.2 64-bit Release ����",
"���ߣ�<Placeholder for git repository>",
"�� <Ctrl+B> �˳�"};

void drawHelper(double x, double y, double w, double h)
{
    if (!isHelperActivated) return;
    SetPenColor(getBackgroundColor());
    drawRectangle(x, y, w, h, 1);
    drawPassage(x + w / 10, y + h - 1, manual_EN_US, sizeof(manual_ZH_CN) / sizeof(string));
    SetFont("΢���ź�");
    drawPassage(x + w / 10, y + h - 3, manual_ZH_CN, sizeof(manual_ZH_CN) / sizeof(string));
    SetFont("Consolas");
}

void drawAbout(double x, double y, double w, double h)
{
    if (!isAboutActivated) return;
    SetPenColor(getBackgroundColor());
    drawRectangle(x, y, w, h, 1);
    drawPassage(x + w / 10, y + h - 1, about_EN_US, sizeof(about_ZH_CN) / sizeof(string));
    SetFont("΢���ź�");
    drawPassage(x + w / 10, y + h - 3, about_ZH_CN, sizeof(about_ZH_CN) / sizeof(string));
    SetFont("Consolas");
}

static void drawPassage(double x, double y, string passage[], int n)
{
    int i;
    double fontHeight = GetFontHeight(), fontAscent = GetFontAscent();
    double currentY = y - fontHeight;
    SetPenColor("White");
    for (i = 0; i < n; i++) {
        MovePen(x, currentY + fontAscent);
        DrawTextString(passage[i]);
        currentY -= fontHeight;
    }
}

void closeHelper()
{
    isHelperActivated = false;
}

void activateHelper()
{
    isHelperActivated = true;
    isAboutActivated = false;
}

bool helperActivated()
{
    return isHelperActivated;
}

void closeAbout()
{
    isAboutActivated = false;
}

void activateAbout()
{
    isAboutActivated = true;
    isHelperActivated = false;
}

bool aboutActivated()
{
    return isAboutActivated;
}
