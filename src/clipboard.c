#include <windows.h>
#include <winuser.h>

#include "string.h"
#include "clipboard.h"

void mySetClipBoard(char *str){
	
	char *dst;
	HGLOBAL hglbCopy;
	
	// Open the clipboard, and empty it. 
    if (!OpenClipboard(NULL)) return;
    EmptyClipboard(); 
    
    // Allocate memory
    hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (strlen(str)+1) * sizeof(char)); 
    if (hglbCopy == NULL) { 
        CloseClipboard(); 
        return; 
    } 
    
    // Lock the global memory and assign it to dst
    dst = GlobalLock(hglbCopy);
    
    // Copy str to dst
    memcpy(dst, str, (strlen(str)+1) * sizeof(char));
    
	GlobalUnlock(hglbCopy); 
 
    // Place the handle on the clipboard.
    SetClipboardData(CF_TEXT, hglbCopy); 
    CloseClipboard();
}
