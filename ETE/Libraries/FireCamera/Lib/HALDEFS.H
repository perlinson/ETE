/*******************************************************************************
  Basedefinitions for HAL (Hardware Abstraction Layer).
  c. kuehnel, intek, 10.10.2001
*******************************************************************************/

#ifndef HALDEFS_H
#define HALDEFS_H

#include <errstat.h>

#define MAXCARDS                16              /* Maximum cards we support */
#define MAXISORXDMAS             8              /* Max. ISO RX DMAs we support */

/* Device types */
#define HWTYPE_LYNX             0
#define HWTYPE_OHCI             1
#define HWTYPE_GPLYNX           3

/* Phy Types */
#define TPHY_OLD                0
#define TPHY_A                  1
#define TPHY_B                  2

#define ISONRASY                0xFF            /* Isonr. for asy. stream */

/* Mask for accessing groups of flags (legacy) */
#define COMMERRMSK      HALEVT_ERROR
#define LYNXERRMSK      HALEVT_ERROR
#define PCIERRMSK       HALEVT_ERROR
#define HLTERRMSK       HALEVT_ERROR

/* Eventflags */
#define HALEVT_ISORX0           0x00000001      /* ISO-Channel 0 RX ready */
#define HALEVT_ISORX1           0x00000002      /* ISO-Channel 1 RX ready */
#define HALEVT_ISORX2           0x00000004      /* ISO-Channel 2 RX ready */
#define HALEVT_ISORX3           0x00000008      /* ISO-Channel 3 RX ready */
#define HALEVT_ISORX4           0x00000010      /* ISO-Channel 4 RX ready */
#define HALEVT_ISORX5           0x00000020      /* ISO-Channel 5 RX ready */
#define HALEVT_ISORX6           0x00000040      /* ISO-Channel 6 RX ready */
#define HALEVT_ISORX7           0x00000080      /* ISO-Channel 7 RX ready */
#define HALEVT_BUSRESETEND      0x00000100      /* Busreset occured */
#define HALEVT_BUSRESETBEGIN    0x00000200      /* Busreset occured */
#define HALEVT_ERROR            0x00000400      /* Sumflag error */
#define HALEVT_EXTACCESS        0x00000800      /* External access */
#define HALEVT_CANCELLED        0x00001000      /* Wait cancelled */
#define HALEVT_CSRCOMMAND       0x00002000      /* CSR command available */
#define HALEVT_SHUTDOWN         0x00004000      /* System Shutdown */
#define HALEVT_WAKEUP           0x00008000      /* After system wakeup */
#define HALEVT_SNOOP            0x80000000      /* Snoop mode */

/* System internal events */
#define SYSEVT_ASYRX            0x00000001      /* Asynch. RX ready */
#define SYSEVT_ASYTX            0x00000002      /* Asynch. TX ready */
#define SYSEVT_ISOTX            0x00000004      /* Global ISO TX event */
#define SYSEVT_ISORX            0x00000008      /* Global ISO RX event */
#define SYSEVT_ISOFRMSTART      0x00000010      /* Frame start */

/* CSR commands */
#define CSRCMD_LINKOFFON        1               /* Link off on */
#define CSRCMD_LINKOFFOFF       2               /* Link off */
#define CSRCMD_DREQON           3               /* Disable request */
#define CSRCMD_DREQOFF          4               /* Enable request */
#define CSRCMD_BUSIDCHANGED     5               /* Bus ID changed */
#define CSRCMD_RESET            6               /* Perform reset */

/* Helpful masks */
#define HALEVTMSK_ERROR HALEVT_ERROR
#define HALEVTMSK_ISORX (HALEVT_ISORX0|HALEVT_ISORX1|HALEVT_ISORX2|HALEVT_ISORX3|\
                         HALEVT_ISORX4|HALEVT_ISORX5|HALEVT_ISORX6|HALEVT_ISORX7)
#define HALERFMSK_ISORXHLT (HALERF_RXHLTISO0|HALERF_RXHLTISO1|HALERF_RXHLTISO2|HALERF_RXHLTISO3|\
                          HALERF_RXHLTISO4|HALERF_RXHLTISO5|HALERF_RXHLTISO6|HALERF_RXHLTISO7)

/* This is the mask for autoclearing flags on read (WaitEvent) */
#define HALEVTAUTOCLR (HALEVT_BUSRESETBEGIN|HALEVT_BUSRESETEND|HALEVT_ERROR|\
                       HALEVT_CANCELLED|HALEVT_SHUTDOWN)

/* Special flags */
#define FCTLSPEC_WAITWRITERES     0x00000001    /* Wait for write response */
#define FCTLSPEC_USERFRAMES       0x00000002    /* User frame buffers */
#define FCTLSPEC_FRMSTARTEVT      0x00000004    /* Create frame start events */
#define FCTLSPEC_RESIZEALL        0x00000008    /* Always resize all frames */
#define FCTLSPEC_USEISODMAMODE    0x00000010    /* Use ISO Dma mode */
#define FCTLSPEC_DETAILEDASYPKT   0x01000000    /* Detailed packet spec. */
#define FCTLSPEC_CARDRESET        0x40000000    /* Card reset (internal) */
#define FCTLSPEC_SNOOP            0x80000000    /* Snoop mode (internal) */

/* ISO security checking flags */
#define FCTLIF_CHECKSYNC          0x01          /* Check for double sync field */
#define FCTLIF_CHECKCYCLE         0x02          /* Check for wrong cycle */
#define FCTLIF_CHECKLENGTH        0x04          /* Check each packet length */
#define FCTLIF_FRMSTARTEX         0x80          /* Frame start flag EX */

#endif
