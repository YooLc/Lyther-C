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
                                 "I. Edit Operations",
                                 "- Press keys to type",
                                 "- You can select a range of text by dragging.",
                                 "- Hit <Ctrl> and scroll can zoom the codes",
                                 "- Use <Tab> to indent (Please note: <tab> will be replaced by spaces)",
                                 "- Hit <Enter> to auto complete (if shown), you can also use mouse to choose",
                                 "II. File Operations",
                                 "- Hit <Ctrl-N> to create a file",
                                 "- Hit <Ctrl-S> to save current file",
                                 "- Hit <Ctrl-M> to switch between code pages",
                                 "III. Personalization",
                                 "- Hit <Ctrl-P> to change theme (palette)",
                                 " ",
                                 "- Hit <Ctrl-H> to exit this page"
                               };

static string manual_ZH_CN[] = { "ʹ�ý̳�",
                                 "һ���༭����",
                                 "- ���¼��̼�������",
                                 "- ʹ�������ק��ѡ��һ�����֣�<Ctrl-C> ���� <Ctrl-V> ճ�� <Ctrl-X> ���� <Ctrl-Z> ���� <Ctrl-Y> ����",
                                 "- ��ס <Ctrl> ��ʹ�������ֿ��Ե��������С"
                                 "- ʹ�� <Tab> ������������ע�⣬<Tab> ���ᱻ�滻Ϊ�ո�",
                                 "- ���� <Enter> ���Խ����Զ���ȫ���������ʾ�򣩣�Ҳ����ͨ�����ѡ�������Ĳ�ȫ��",
                                 "�����ļ�����",
                                 "- ���� <Ctrl-N> �½��ļ�",
                                 "- ���� <Ctrl-S> �����ļ�",
                                 "- ���� <Ctrl-M> �����л���ͬ�ļ�",
                                 "�������Ի�����",
                                 "- ���� <Ctrl-P> �����л���ɫ / ��ɫ����",
                                 " ",
                                 "- ���� <Ctrl-H> �˳�������"
                               };

static string about_EN_US[] = {"Lyther Version 1.3",
                               "Built by TDM-GCC 4.9.2 64-bit Release",
                               "Press <Ctrl+B> to exit."
                              };

static string about_ZH_CN[] = {"���� Version 1.3",
                               "�� TDM-GCC 4.9.2 64-bit Release ����",
                               "�� <Ctrl+B> �˳�"
                              };

void drawHelper(double x, double y, double w, double h)
{
    if (!isHelperActivated) return;
    SetPenColor(getBackgroundColor());
    drawRectangle(x, y, w, h, 1);
    SetFont("΢���ź�");
    drawPassage(x + w / 10, y + h - 0.8, manual_ZH_CN, sizeof(manual_ZH_CN) / sizeof(string));
    SetFont("Consolas");
    drawPassage(x + w / 10, y + h - 4.2, manual_EN_US, sizeof(manual_EN_US) / sizeof(string));
}

void drawAbout(double x, double y, double w, double h)
{
    if (!isAboutActivated) return;
    SetPenColor(getBackgroundColor());
    drawRectangle(x, y, w, h, 1);
    SetFont("΢���ź�");
    drawPassage(x + w / 10, y + h - 1, about_ZH_CN, sizeof(about_ZH_CN) / sizeof(string));
    SetFont("Consolas");
    drawPassage(x + w / 10, y + h - 3, about_EN_US, sizeof(about_EN_US) / sizeof(string));
}

static void drawPassage(double x, double y, string passage[], int n)
{
    int i;
    double fontHeight = GetFontHeight(), fontAscent = GetFontAscent();
    double currentY = y - fontHeight;
    SetPenColor(getForegroundColor());
    for (i = 0; i < n; i++) {
        if (passage[i][0] != '-') SetStyle(Bold);
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
