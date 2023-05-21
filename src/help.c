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

static string manual_ZH_CN[] = { "使用教程",
                                 "一、编辑操作",
                                 "- 按下键盘即可输入",
                                 "- 使用鼠标拖拽可选中一段文字，<Ctrl-C> 复制 <Ctrl-V> 粘贴 <Ctrl-X> 剪切 <Ctrl-Z> 撤销 <Ctrl-Y> 重做",
                                 "- 按住 <Ctrl> 并使用鼠标滚轮可以调整字体大小"
                                 "- 使用 <Tab> 进行缩进（请注意，<Tab> 将会被替换为空格）",
                                 "- 按下 <Enter> 可以进行自动补全（如果有提示框），也可以通过鼠标选择其他的补全项",
                                 "二、文件操作",
                                 "- 按下 <Ctrl-N> 新建文件",
                                 "- 按下 <Ctrl-S> 保存文件",
                                 "- 按下 <Ctrl-M> 可以切换不同文件",
                                 "三、个性化设置",
                                 "- 按下 <Ctrl-P> 可以切换暗色 / 亮色主题",
                                 " ",
                                 "- 按下 <Ctrl-H> 退出本界面"
                               };

static string about_EN_US[] = {"Lyther Version 1.3",
                               "Built by TDM-GCC 4.9.2 64-bit Release",
                               "Press <Ctrl+B> to exit."
                              };

static string about_ZH_CN[] = {"轻羽 Version 1.3",
                               "由 TDM-GCC 4.9.2 64-bit Release 构建",
                               "按 <Ctrl+B> 退出"
                              };

void drawHelper(double x, double y, double w, double h)
{
    if (!isHelperActivated) return;
    SetPenColor(getBackgroundColor());
    drawRectangle(x, y, w, h, 1);
    SetFont("微软雅黑");
    drawPassage(x + w / 10, y + h - 0.8, manual_ZH_CN, sizeof(manual_ZH_CN) / sizeof(string));
    SetFont("Consolas");
    drawPassage(x + w / 10, y + h - 4.2, manual_EN_US, sizeof(manual_EN_US) / sizeof(string));
}

void drawAbout(double x, double y, double w, double h)
{
    if (!isAboutActivated) return;
    SetPenColor(getBackgroundColor());
    drawRectangle(x, y, w, h, 1);
    SetFont("微软雅黑");
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
