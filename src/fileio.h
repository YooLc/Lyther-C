#ifndef _FILE_IO_H_
#define _FILE_IO_H_

#include "textarea.h"

/*
    function newFile(Editor *editor)
    Creat a new C file, named as <Unamed 1>
*/
void newFile(Editor *editor);

/*
    function loadFile(Editor *editor)
    load <filename> into editor, and rewind the <curselect> to this file
*/
void loadFile(Editor *editor);

/*
    function saveAs(Editor *editor)
    Save <curselect> in editor as <fileName>
*/
void saveAs(Editor *editor);

void saveFile(Editor *editor);

#endif
