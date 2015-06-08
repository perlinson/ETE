/*******************************************************************************
 Headerfile for operating system functions for FireBus.
 intek, C. Kuehnel, 5.12.2002
*******************************************************************************/

#ifndef OSUTIL_H
#define OSUTIL_H

/* Include ostypes first */
#include <ostypes.h>

/* Include basetypes */
#include <basetype.h>

#define OSINFTIME 0xFFFFFFFF

/* Little helper macros */
#define HIUINT16(n)             ((UINT16)((n)>>16))
#define LOUINT16(n)             ((UINT16)((n)&0xFFFF))
#define MAKEUINT32(hi,lo)       (((UINT32)(hi)<<16)|(lo))

#ifndef max
 #define max(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef min
 #define min(a,b) (((a)<(b))?(a):(b))
#endif

#ifdef __cplusplus
extern "C" {
#endif

void  OSDelay(UINT32 TimeInMs);

POSSEMAPHORE OSSemaphoreCreate(UINT32 State);
void         OSSemaphoreDelete(POSSEMAPHORE pSemaphore);
UINT8        OSSemaphorePend(POSSEMAPHORE pSemaphore,UINT32 TimeoutInMs);
void         OSSemaphorePost(POSSEMAPHORE pSemaphore);

#ifdef __cplusplus
}
#endif

#endif
