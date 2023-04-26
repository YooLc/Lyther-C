#include <windows.h>
#include <stdlib.h>
#include <commdlg.h>

#include "fileio.h"
#include "textarea.h"
#include "codeparser.h"

void loadFile(Editor *editor){
    OPENFILENAME ofn;
    char openFile[MAX_PATH];
    memset(&ofn,0,sizeof(OPENFILENAME));
    memset(openFile,0,sizeof(char)*MAX_PATH);
    ofn.lStructSize=sizeof(OPENFILENAME);
    ofn.lpstrFilter="(*.c)\0*.c\0";
    ofn.lpstrFile=(LPTSTR)openFile;
    ofn.nMaxFile=MAX_PATH;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags=OFN_FILEMUSTEXIST;
    GetOpenFileName(&ofn);
    //strcat(openFile, ".c");
    printf("LOAD %s\n", openFile);
    //return;
    FILE *fp = fopen(openFile, "r");
    if(fp == NULL){
        puts("FAILED TO OPEN FILE");
        return;
    }
    addCodeToEditor(editor, fp, openFile);    //need to be fixed, malloc char*
    fclose(fp);
}

void saveFile(Editor *editor){
    char saveFile[MAX_PATH];

    if(strcmp(editor->filePath[editor->curSelect], "Unamed 1") == 0){ //If it does not have a name
        //Get file path
        OPENFILENAME ofn;
        
        memset(&ofn,0,sizeof(OPENFILENAME));
        memset(saveFile,0,sizeof(char)*MAX_PATH);
        ofn.lStructSize=sizeof(OPENFILENAME);
        ofn.lpstrFilter="(*.c)\0*.c\0";      //set file name filter
        ofn.lpstrFile=saveFile;              //set file name pointer
        ofn.nMaxFile=MAX_PATH;               //set max file name size
        ofn.lpstrInitialDir = NULL;
        ofn.Flags=OFN_FILEMUSTEXIST;
        
        GetSaveFileName(&ofn);
        //strcat(saveFile,".c");
        editor->filePath[editor->curSelect] = (char* )malloc(sizeof(char) * strlen(saveFile) + 1);
        strcpy(editor->filePath[editor->curSelect], saveFile);
    }else{
        strcpy(saveFile, editor->filePath[editor->curSelect]);
    }
    printf("SAVE %s\n", saveFile);
    FILE *fp = fopen(saveFile, "w");
    if(fp == NULL){
        puts("FAILURE IN SAVE FILE");
        return;
    }

    //Write data in file
    EditorForm *form = editor->forms[editor->curSelect];

    Passage *p = form->passage;
    Listptr nowLineNode = kthNode(&(p->passList), 1);

     while(nowLineNode != NULL){
         Line* l = nowLineNode->datptr;
         Listptr nowWordNode = kthNode(&(l->lineList), 1);
         while(nowWordNode != NULL){
             Token* w = nowWordNode->datptr;
             fprintf(fp, "%s", w->content);
             nowWordNode = nowWordNode->next;
         }
         nowLineNode = nowLineNode->next;
     }
     fclose(fp);
}
