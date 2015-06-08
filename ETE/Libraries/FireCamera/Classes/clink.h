////////////////////////////////////////////////////////////////////////////////
// Headerfile for cplusplus link utility.
// intek, C. Kuehnel, 5.12.2002
////////////////////////////////////////////////////////////////////////////////

#ifndef CLINK_H
#define CLINK_H

class CLink
{
public:
  CLink         *m_pNext;
  CLink         *m_pPrev;

                CLink();
  virtual      ~CLink();

  CLink*        First();                      // 'this' is base from list
  CLink*        Last();
  CLink*        FirstAndUnlink();
  CLink*        Next(CLink *pActual);         // 'this' is base from list
  CLink*        Prev(CLink *pActual);

  void          Init();
  void          LinkAfter(CLink *pPrev);
  void          LinkBefore(CLink *pNext);
  void          Unlink();
};

#endif

