//
// MATLAB Compiler: 4.18.1 (R2013a)
// Date: Fri Aug 15 18:05:31 2014
// Arguments: "-B" "macro_default" "-W" "cpplib:psicalcdll" "-T" "link:lib"
// "psicalc" "confocalcalc" 
//

#include <stdio.h>
#define EXPORTING_psicalcdll 1
#include "psicalcdll.h"

static HMCRINSTANCE _mcr_inst = NULL;


#if defined( _MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__LCC__)
#ifdef __LCC__
#undef EXTERN_C
#endif
#include <windows.h>

static char path_to_dll[_MAX_PATH];

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, void *pv)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        if (GetModuleFileName(hInstance, path_to_dll, _MAX_PATH) == 0)
            return FALSE;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
    }
    return TRUE;
}
#endif
#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultPrintHandler(const char *s)
{
  return mclWrite(1 /* stdout */, s, sizeof(char)*strlen(s));
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultErrorHandler(const char *s)
{
  int written = 0;
  size_t len = 0;
  len = strlen(s);
  written = mclWrite(2 /* stderr */, s, sizeof(char)*len);
  if (len > 0 && s[ len-1 ] != '\n')
    written += mclWrite(2 /* stderr */, "\n", sizeof(char));
  return written;
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_psicalcdll_C_API
#define LIB_psicalcdll_C_API /* No special import/export declaration */
#endif

LIB_psicalcdll_C_API 
bool MW_CALL_CONV psicalcdllInitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler)
{
    int bResult = 0;
  if (_mcr_inst != NULL)
    return true;
  if (!mclmcrInitialize())
    return false;
  if (!GetModuleFileName(GetModuleHandle("psicalcdll"), path_to_dll, _MAX_PATH))
    return false;
    {
        mclCtfStream ctfStream = 
            mclGetEmbeddedCtfStream(path_to_dll);
        if (ctfStream) {
            bResult = mclInitializeComponentInstanceEmbedded(   &_mcr_inst,
                                                                error_handler, 
                                                                print_handler,
                                                                ctfStream);
            mclDestroyStream(ctfStream);
        } else {
            bResult = 0;
        }
    }  
    if (!bResult)
    return false;
  return true;
}

LIB_psicalcdll_C_API 
bool MW_CALL_CONV psicalcdllInitialize(void)
{
  return psicalcdllInitializeWithHandlers(mclDefaultErrorHandler, mclDefaultPrintHandler);
}

LIB_psicalcdll_C_API 
void MW_CALL_CONV psicalcdllTerminate(void)
{
  if (_mcr_inst != NULL)
    mclTerminateInstance(&_mcr_inst);
}

LIB_psicalcdll_C_API 
void MW_CALL_CONV psicalcdllPrintStackTrace(void) 
{
  char** stackTrace;
  int stackDepth = mclGetStackTrace(&stackTrace);
  int i;
  for(i=0; i<stackDepth; i++)
  {
    mclWrite(2 /* stderr */, stackTrace[i], sizeof(char)*strlen(stackTrace[i]));
    mclWrite(2 /* stderr */, "\n", sizeof(char)*strlen("\n"));
  }
  mclFreeStackTrace(&stackTrace, stackDepth);
}


LIB_psicalcdll_C_API 
bool MW_CALL_CONV mlxPsicalc(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "psicalc", nlhs, plhs, nrhs, prhs);
}

LIB_psicalcdll_C_API 
bool MW_CALL_CONV mlxConfocalcalc(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "confocalcalc", nlhs, plhs, nrhs, prhs);
}

LIB_psicalcdll_CPP_API 
void MW_CALL_CONV psicalc(int nargout, mwArray& IM_unwrap, const mwArray& path, const 
                          mwArray& imagenumber, const mwArray& avernumber, const mwArray& 
                          im_mask, const mwArray& rsize, const mwArray& csize)
{
  mclcppMlfFeval(_mcr_inst, "psicalc", nargout, 1, 6, &IM_unwrap, &path, &imagenumber, &avernumber, &im_mask, &rsize, &csize);
}

LIB_psicalcdll_CPP_API 
void MW_CALL_CONV confocalcalc(int nargout, mwArray& confocalmodeoutput, mwArray& 
                               normalmodeoutput, const mwArray& path, const mwArray& 
                               imagenumber, const mwArray& rsize, const mwArray& csize)
{
  mclcppMlfFeval(_mcr_inst, "confocalcalc", nargout, 2, 4, &confocalmodeoutput, &normalmodeoutput, &path, &imagenumber, &rsize, &csize);
}

