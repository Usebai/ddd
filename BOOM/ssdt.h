#ifndef _SSDT_H
#define _SSDT_H

#include "_global.h"


class SSDT
{
public:
    static PVOID GetFunctionAddress(const char* apiname);
};

#endif