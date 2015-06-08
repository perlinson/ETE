/*******************************************************************************
  Headerfile for abstract Firewire-API running on multiple platforms.
  c. kuehnel, intek, 12.10.2001
*******************************************************************************/

#ifndef FCTLMAIN_H
#define FCTLMAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <haldefs.h>
#include <basetype.h>
#include <1394base.h>
#include <fctlargs.h>

/*******************************************************************************
  Windows specific declarations.
*******************************************************************************/

#ifdef _WIN64
 #define FCTLUSESTDCALL 1
#endif

#ifdef WIN32

/* See what link convention to use */
#ifdef FCTLUSESTDCALL
 #define LINKCONVENTION __stdcall
#else
 #define LINKCONVENTION __cdecl
#endif

/* Compile the library and export our functions */
#ifdef FCTLEXPORT
 #define TSPEC __declspec(dllexport) LINKCONVENTION
#endif

/* Windows Kernel Mode */
#ifdef _NTDDK_
 #define TSPEC
#endif

/* Link library into program code */
#ifdef FCTLLINK
 #define TSPEC
#endif

/* Import functions from dynamic link library */
#ifndef TSPEC
 #define TSPEC __declspec(dllimport) LINKCONVENTION
#endif

/*******************************************************************************
  Export function specific for Windows-DLL
*******************************************************************************/

typedef struct
{
  UINT32        Low;
  UINT32        High;
}FCTLLICENSE;

UINT32 TSPEC FCTLInstallDriver(char *pDirectory);
UINT32 TSPEC FCTLUninstallDriver();
UINT32 TSPEC FCTLTest(CARDHANDLE hCard,SYSHDR *pHdr,FCTLSYNC *pSync);
UINT32 TSPEC FCTLSetLicense(CARDHANDLE hCard,UINT32 Cnt,FCTLLICENSE *pLicense);
UINT32 TSPEC FCTLLicenseRequest(CARDHANDLE hCard,FCTLLICENSEREQUEST *pArg);

UINT32  TSPEC FCTLSetParameter(CARDHANDLE hCard,FCTLPARAMETER *pArg);
UINT32  TSPEC FCTLGetParameter(CARDHANDLE hCard,FCTLPARAMETER *pArg);

/* Internal only */
UINT32 FCTLRegisterCallback(CARDHANDLE hCard,void (*pFct)(void*,UINT32,void*),
                            void*);

#else /* of WIN32 */

#define TSPEC

#endif

/*******************************************************************************
  Exported functions.
*******************************************************************************/

/* Configure PCI-Bus (Set card descriptor) */
UINT32  TSPEC FCTLPciConfig(FCTLCARDDESC *pCardDesc);

/* Get card count */
UINT32  TSPEC FCTLGetCardCount(UINT32 *pCardCnt);

/* Get system ID of card */
UINT32  TSPEC FCTLGetCardSystemId(UINT32 CardNr,UINT32 *pId);

/* Get HAL- and FCTL-Version */
UINT32  TSPEC FCTLGetVersion(CARDHANDLE hCard,UINT32 *pVersion);

/* Open card and return handle */
UINT32  TSPEC FCTLOpenCard(UINT8 CardNr,CARDHANDLE *phCard);

/* Close card and free handle */
UINT32  TSPEC FCTLCloseCard(CARDHANDLE hCard);

/* Open chip/module for asynchronous communication */
UINT32  TSPEC FCTLOpenBus(CARDHANDLE hCard,FCTLOPENBUS *pArg);

/* Close all communication */
UINT32  TSPEC FCTLCloseBus(CARDHANDLE hCard);

/* Perform a busreset */
UINT32  TSPEC FCTLBusReset(CARDHANDLE hCard);

/* Get info about members on the bus after busreset */
UINT32  TSPEC FCTLGetBusInfo(CARDHANDLE hCard,FCTLGETBUSINFO *pArg);

/* Write quadlet to device */
UINT32  TSPEC FCTLWriteQuad(CARDHANDLE hCard,FCTLWRITEQUAD *pArg,FCTLSYNC *pSync);

/* Read quadlet from device */
UINT32  TSPEC FCTLReadQuad(CARDHANDLE hCard,FCTLREADQUAD *pArg,FCTLSYNC *pSync);

/* Write block to device */
UINT32  TSPEC FCTLWriteBlk(CARDHANDLE hCard,FCTLWRITEBLK *pArg,FCTLSYNC *pSync);

/* Read block from device */
UINT32  TSPEC FCTLReadBlk(CARDHANDLE hCard,FCTLREADBLK *pArg,FCTLSYNC *pSync);

/* Wait until Sync is signaled (also informational with Timeout=0) */
UINT32  TSPEC FCTLWaitSync(CARDHANDLE hCard,FCTLSYNC *pSync,UINT32 TimeoutInMs);

/* Cancel action started with a UserSync */
UINT32  TSPEC FCTLCancelJob(CARDHANDLE hCard,FCTLSYNC *pSync);

/* Wait on event */
UINT32  TSPEC FCTLWaitEvent(CARDHANDLE hCard,UINT32 *pEvent,UINT32 TimeoutInMs);

/* Cancel any wait on events */
UINT32  TSPEC FCTLCancelWaitEvent(CARDHANDLE hCard);

/* Get all errors */
UINT32  TSPEC FCTLGetErrors(CARDHANDLE hCard,UINT32 *pErrors);

/* Map address range */
UINT32  TSPEC FCTLMapAddress(CARDHANDLE hCard,FCTLMAPADDRESS *pArg);

/* Unmap address range */
UINT32  TSPEC FCTLUnmapAddress(CARDHANDLE hCard,FCTLHANDLE MapHandle);

/* Release address range */
UINT32  TSPEC FCTLReleaseAddress(CARDHANDLE hCard,FCTLACCESSINFO *pArg);

/* Get access information */
UINT32  TSPEC FCTLGetAccessInfo(CARDHANDLE hCard,FCTLACCESSINFO *pArg);

/* Lock memory (Windows only) */
UINT32  TSPEC FCTLLockMemory(CARDHANDLE hCard,FCTLLOCKMEMORY *pArg);

/* Unlock memory (Windows only) */
UINT32  TSPEC FCTLUnlockMemory(CARDHANDLE hCard,UINT8 *pLockMem);

/* Start Iso receiption */
UINT32  TSPEC FCTLIsoStartListen(CARDHANDLE hCard,FCTLISOSTARTLISTEN *pArg);

/* Stop Iso receiption */
UINT32  TSPEC FCTLIsoStopListen(CARDHANDLE hCard,UINT32 IsoNr);

/* Inquire IsoBuffer */
UINT32  TSPEC FCTLIsoInquire(CARDHANDLE hCard,UINT32 IsoNr,FCTLFRMINFO* pArg);

/* Clear all Isobuffers */
UINT32  TSPEC FCTLIsoClear(CARDHANDLE hCard,UINT32 IsoNr);

/* Return IsoBuffer */
UINT32  TSPEC FCTLIsoPost(CARDHANDLE hCard,FCTLFRMINFO *pArg);

/* Resize IsoBuffer */
UINT32  TSPEC FCTLIsoResize(CARDHANDLE hCard,FCTLISORESIZE *pArg);

/* Write an isochronous buffer */
UINT32  TSPEC FCTLIsoWrite(CARDHANDLE hCard,FCTLISOWRITE *pArg,FCTLSYNC *pSync);

/* Flush cache to locked memory */
UINT32  TSPEC FCTLFlushCache(CARDHANDLE hCard,UINT8 *pLockMem,UINT32 Length);

/* Invalidate cache for locked memory */
UINT32  TSPEC FCTLInvalidateCache(CARDHANDLE hCard,UINT8 *pLockMem,UINT32 Length);

/* Create FCTLSYNC */
UINT32  TSPEC FCTLCreateSync(CARDHANDLE hCard,FCTLSYNC* *ppSync);

/* Delete FCTLSYNC */
UINT32  TSPEC FCTLDeleteSync(CARDHANDLE hCard,FCTLSYNC *pSync);

/* Lock request */
UINT32  TSPEC FCTLLockRequest(CARDHANDLE hCard,FCTLLOCKREQUEST *pArg,FCTLSYNC *pSync);

/* Get CSR command */
UINT32  TSPEC FCTLGetCsrCommand(CARDHANDLE hCard,UINT32 *pCmd);

/* Get actual isochronous cycle time */
UINT32  TSPEC FCTLGetCycleTime(CARDHANDLE hCard,UINT32 *pCycleTime);

/* Query free isochronous resources */
UINT32  TSPEC FCTLIsoQueryResources(CARDHANDLE hCard,FCTLISORESOURCES *pArg);

/* Allocate isochronous resources */
UINT32  TSPEC FCTLIsoAllocateResources(CARDHANDLE hCard,FCTLISORESOURCES *pArg);

/* Free allocated isochronous resources */
UINT32  TSPEC FCTLIsoFreeResources(CARDHANDLE hCard,FCTLISORESOURCES *pArg);

/* Get maximum speed between devices */
UINT32  TSPEC FCTLGetMaxSpeedBetweenDevices(CARDHANDLE hCard,FCTLMAXSPEED *pArg);

/* Send phy packet */
UINT32  TSPEC FCTLSendPhyPacket(CARDHANDLE hCard,UINT32 PhyPacket);

/* Fast write block for controller only */
UINT32  TSPEC FCTLFastWriteBlk(CARDHANDLE hCard,FCTLWRITEBLK *pArg,
                         FCTLUSERCB *pErrorCB,FCTLUSERCB *pReadyCB,
                         void* Context);

/* Garbage collection for fast write block */
UINT32  TSPEC FCTLFastWriteGarbageCollection(CARDHANDLE hCard);

/* Process Snoop event (stack internal!) */
UINT32  TSPEC FCTLOnSnoop(CARDHANDLE hCard,FCTLSNOOP *pArg);

#ifdef __cplusplus
}
#endif

#endif
