#ifndef _CLIPBOARD_H_
#define _CLIPBOARD_H_

/*
	function mySetClipBoard(char *str)
	set <str> to the clipboard
*/
void mySetClipBoard(char *str);

/*
	function myGetClipBoard(char *str)
	store the content in clipboard in the address starting at <dst>
	*if there is nothing or not text-type data in the clipboard,
	 <dst[0]> will be set as the terminal character '\0'  
*/
void myGetClipBoard(char* dst);
#endif
