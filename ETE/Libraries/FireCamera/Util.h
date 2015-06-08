////////////////////////////////////////////////////////////////////////////////
// Software for FirePackage (c) Evaluation.
// intek - c. kuehnel - 2007
// This software may be used and distributed freely. 
// Please leave a hint to the original supplier intek within your source code.
////////////////////////////////////////////////////////////////////////////////

#ifndef UTIL_H
#define UTIL_H

#include <basetype.h>

#define HEXTOBIN(n)     ((n)>='A'?(n)-('A')+10:(n)-'0')

#undef MAKEULONG
#define MAKEULONG(a,b,c,d) (((ULONG)(a)<<24)|((ULONG)(b)<<16)|\
                           ((ULONG)(c)<<8)|(ULONG)(d))

#undef MAKEUWORD
#define MAKEUWORD(a,b)     (((UWORD)(a)<<8)|(UWORD)(b))

UINT8  GetLong(char *pBuf,SINT32 *plValue);
UINT32 ScanHex(char* pStr,UINT8 *pOut,UINT32 MaxSize);

#endif
