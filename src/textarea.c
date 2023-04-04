#include <stdio.h>
#include "extgraph.h"
#include "textarea.h"
#include "codeparser.h"
#include "graphics.h"
#include "imgui.h"

void DrawTextArea(Passage *passage) {
    double winWidth = GetWindowWidth();
    double winHeight = GetWindowHeight();
    
    double fontHeight = GetFontHeight();
    printf("font height: %.2f\n", fontHeight);
    
    int idx = 1;
 	Listptr nowLineNode = kthNode(&(passage->passList), 1);
 	while(nowLineNode != NULL){
 		Line* l = nowLineNode->datptr;
 		Listptr nowWordNode = kthNode(&(l->lineList), 1);
 		textbox(GenUIID(idx), 0, winHeight - fontHeight * idx, winWidth, fontHeight, (nowWordNode->datptr), sizeof(nowWordNode->datptr));
 		nowLineNode = nowLineNode->next;
 		printf("%d %.2f\n", idx, winHeight - fontHeight * idx);
 		idx++;
 	}
    return;
}
