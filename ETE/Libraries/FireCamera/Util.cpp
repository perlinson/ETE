////////////////////////////////////////////////////////////////////////////////
// Software for FirePackage (c) Evaluation.
// intek - c. kuehnel - 2007
// This software may be used and distributed freely. 
// Please leave a hint to the original supplier intek within your source code.
////////////////////////////////////////////////////////////////////////////////

#include <util.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

////////////////////////////////////////////////////////////////////////////////
// Scans a string for HEX characters and returns the number of bytes scanned.

UINT32 ScanHex(char* pStr,UINT8 *pOut,UINT32 MaxSize)
{
  UINT32 Cnt=0,Ix=0;
  char  c;

  do
  {
    c=(char)toupper(pStr[Ix]);
    if((c>='0' && c<='9') || (c>='A' && c<='F'))
    {
      if((Ix&1)==0)
      {
        pOut[Cnt]=HEXTOBIN(c);
      }
      else
      {
        pOut[Cnt]=pOut[Cnt]*16+HEXTOBIN(c);
        Cnt++;
      }
      Ix++;
    }
    else
    {
      if(c==0 && ((Ix&1)==0))
       return Cnt;
      else
       return 0;
    }
  }while(Cnt<MaxSize);

  return Cnt;
}

/////////////////////////////////////////////////////////////////////////////
// Return a decimal value from a string (zero terminated).

UINT8 GetLong(char *pBuf,SINT32 *plValue)
{
  SINT32 sign;
  UINT16 i,j;
  SINT32 value;

  // erstmal String holen
  i=(UINT16)strlen(pBuf);

  // Fehler falls keine Eingabe
  if(!i)
   return FALSE;

  if(pBuf[0]=='-')      // Vorzeichen holen
  {
    sign=-1;
    i--;
  }
  else
   sign=1;

  if(!i)                // Fehler falls nur '-' eingegeben
   return FALSE;

  value=0;
  j=(sign==-1)?1:0;     // Index setzen (Vorzeicheneingabe beachten)
  while(i)
  {
    if((pBuf[j]<'0') || (pBuf[j]>'9'))
     return FALSE;
    value=value*10+(pBuf[j++]-'0');
    i--;
  }

  *plValue=value*sign;
 
  return TRUE;
}

