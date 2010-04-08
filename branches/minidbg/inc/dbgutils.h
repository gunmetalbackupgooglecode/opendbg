#ifndef _DBGUTILS_H__
#define _DBGUTILS_H__

// TODO: Platform IFDEF is needed

void print_error(const char * str)
{
    OutputDebugString(str);
    OutputDebugString("\n");
}


#endif //_DBGUTILS_H__
