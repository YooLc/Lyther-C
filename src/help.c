#include <stdio.h>
#include <stdbool.h>

#include "help.h"
#include "imgui.h"
#include "style.h"
#include "graphics.h"
#include "extgraph.h"
#include "textarea.h"

static bool isHelperActivated = false, isAboutActivated = false;
static string manual_EN_US[] = { "Mannual",
"- Press keys to type",
"- You can select a range of text by dragging.",
"- Hit <Ctrl> and scroll can zoom the codes",
"- Use <Tab> to indent (Please note: <tab> will be replaced by spaces)",
"- Hit <Enter> to auto complete (if shown), you can also use mouse to choose",
"- Hit <Ctrl-H> to exit this page"
};

static string manual_ZH_CN[] = { "ʹ�ý̳�",
"- ���¼��̼�������",
"- ʹ�������ק��ѡ��һ�����֣�<Ctrl-C> ���� <Ctrl-V> ճ�� <Ctrl-X> ����",
"- ��ס <Ctrl> ��ʹ�������ֿ��Ե��������С" 
"- ʹ�� <Tab> ������������ע�⣬<Tab> ���ᱻ�滻Ϊ�ո�",
"- ���� <Enter> ���Խ����Զ���ȫ���������ʾ�򣩣�Ҳ����ͨ�����ѡ�������Ĳ�ȫ��",
"- ���� <Ctrl-H> �˳�������"
};

static string about_EN_US[] = {"Lyther Version 1.3",
"Built by TDM-GCC 4.9.2 64-bit Release",
"Press <Ctrl+B> to exit."};

static string about_ZH_CN[] = {"���� Version 1.3",
"�� TDM-GCC 4.9.2 64-bit Release ����",
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
    SetPenColor(getForegroundColor());
    SetStyle(Bold);
    for (i = 0; i < n; i++) {
        MovePen(x, currentY + fontAscent);
        DrawTextString(passage[i]);
        currentY -= fontHeight;
        SetStyle(Normal);
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
