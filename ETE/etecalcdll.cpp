//
// MATLAB Compiler: 4.18.1 (R2013a)
// Date: Wed Oct 15 14:24:42 2014
// Arguments: "-B" "macro_default" "-W" "cpplib:etecalcdll" "-T" "link:lib"
// "psicalc" "confocalcalc" "confocal3d" "confocalweightcalc" "sectioncalc"
// "confocaleasyver" "vsieasyver" "vsiam" "confocal3DMemory" "datafilter" 
//

#include <stdio.h>
#define EXPORTING_etecalcdll 1
#include "etecalcdll.h"

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
#ifndef LIB_etecalcdll_C_API
#define LIB_etecalcdll_C_API /* No special import/export declaration */
#endif

LIB_etecalcdll_C_API 
bool MW_CALL_CONV etecalcdllInitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler)
{
    int bResult = 0;
  if (_mcr_inst != NULL)
    return true;
  if (!mclmcrInitialize())
    return false;
  if (!GetModuleFileName(GetModuleHandle("etecalcdll"), path_to_dll, _MAX_PATH))
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

LIB_etecalcdll_C_API 
bool MW_CALL_CONV etecalcdllInitialize(void)
{
  return etecalcdllInitializeWithHandlers(mclDefaultErrorHandler, mclDefaultPrintHandler);
}

LIB_etecalcdll_C_API 
void MW_CALL_CONV etecalcdllTerminate(void)
{
  if (_mcr_inst != NULL)
    mclTerminateInstance(&_mcr_inst);
}

LIB_etecalcdll_C_API 
void MW_CALL_CONV etecalcdllPrintStackTrace(void) 
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


LIB_etecalcdll_C_API 
bool MW_CALL_CONV mlxPsicalc(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "psicalc", nlhs, plhs, nrhs, prhs);
}

LIB_etecalcdll_C_API 
bool MW_CALL_CONV mlxConfocalcalc(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "confocalcalc", nlhs, plhs, nrhs, prhs);
}

LIB_etecalcdll_C_API 
bool MW_CALL_CONV mlxConfocal3d(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "confocal3d", nlhs, plhs, nrhs, prhs);
}

LIB_etecalcdll_C_API 
bool MW_CALL_CONV mlxConfocalweightcalc(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                        *prhs[])
{
  return mclFeval(_mcr_inst, "confocalweightcalc", nlhs, plhs, nrhs, prhs);
}

LIB_etecalcdll_C_API 
bool MW_CALL_CONV mlxSectioncalc(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "sectioncalc", nlhs, plhs, nrhs, prhs);
}

LIB_etecalcdll_C_API 
bool MW_CALL_CONV mlxConfocaleasyver(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "confocaleasyver", nlhs, plhs, nrhs, prhs);
}

LIB_etecalcdll_C_API 
bool MW_CALL_CONV mlxVsieasyver(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "vsieasyver", nlhs, plhs, nrhs, prhs);
}

LIB_etecalcdll_C_API 
bool MW_CALL_CONV mlxVsiam(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "vsiam", nlhs, plhs, nrhs, prhs);
}

LIB_etecalcdll_C_API 
bool MW_CALL_CONV mlxConfocal3Dmemory(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                      *prhs[])
{
  return mclFeval(_mcr_inst, "confocal3Dmemory", nlhs, plhs, nrhs, prhs);
}

LIB_etecalcdll_C_API 
bool MW_CALL_CONV mlxDatafilter(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "datafilter", nlhs, plhs, nrhs, prhs);
}

LIB_etecalcdll_CPP_API 
void MW_CALL_CONV psicalc(int nargout, mwArray& IM_unwrap, mwArray& maxvalue, const 
                          mwArray& path, const mwArray& imagenumber, const mwArray& 
                          avernumber, const mwArray& left, const mwArray& right, const 
                          mwArray& rsize, const mwArray& csize)
{
  mclcppMlfFeval(_mcr_inst, "psicalc", nargout, 2, 7, &IM_unwrap, &maxvalue, &path, &imagenumber, &avernumber, &left, &right, &rsize, &csize);
}

LIB_etecalcdll_CPP_API 
void MW_CALL_CONV confocalcalc(int nargout, mwArray& confocalmodeoutput, mwArray& 
                               normalmodeoutput, const mwArray& path, const mwArray& 
                               imagenumber, const mwArray& rsize, const mwArray& csize)
{
  mclcppMlfFeval(_mcr_inst, "confocalcalc", nargout, 2, 4, &confocalmodeoutput, &normalmodeoutput, &path, &imagenumber, &rsize, &csize);
}

LIB_etecalcdll_CPP_API 
void MW_CALL_CONV confocal3d(int nargout, mwArray& newmaxvalue, mwArray& newmaxindex, 
                             mwArray& newm1value, mwArray& newn1value, mwArray& 
                             newm2value, mwArray& newn2value, mwArray& newm3value, 
                             mwArray& newn3value, const mwArray& current_section, const 
                             mwArray& current_index, const mwArray& maxvalue, const 
                             mwArray& maxindex, const mwArray& m1value, const mwArray& 
                             n1value, const mwArray& m2value, const mwArray& n2value, 
                             const mwArray& m3value, const mwArray& n3value, const 
                             mwArray& rsize, const mwArray& csize)
{
  mclcppMlfFeval(_mcr_inst, "confocal3d", nargout, 8, 12, &newmaxvalue, &newmaxindex, &newm1value, &newn1value, &newm2value, &newn2value, &newm3value, &newn3value, &current_section, &current_index, &maxvalue, &maxindex, &m1value, &n1value, &m2value, &n2value, &m3value, &n3value, &rsize, &csize);
}

LIB_etecalcdll_CPP_API 
void MW_CALL_CONV confocalweightcalc(int nargout, mwArray& grav, mwArray& extend_image, 
                                     const mwArray& maxvalue, const mwArray& maxindex, 
                                     const mwArray& m1value, const mwArray& n1value, 
                                     const mwArray& m2value, const mwArray& n2value, 
                                     const mwArray& m3value, const mwArray& n3value, 
                                     const mwArray& rsize, const mwArray& csize)
{
  mclcppMlfFeval(_mcr_inst, "confocalweightcalc", nargout, 2, 10, &grav, &extend_image, &maxvalue, &maxindex, &m1value, &n1value, &m2value, &n2value, &m3value, &n3value, &rsize, &csize);
}

LIB_etecalcdll_CPP_API 
void MW_CALL_CONV sectioncalc(int nargout, mwArray& confocalmodeoutput, const mwArray& 
                              im1, const mwArray& im2, const mwArray& im3, const mwArray& 
                              rsize, const mwArray& csize)
{
  mclcppMlfFeval(_mcr_inst, "sectioncalc", nargout, 1, 5, &confocalmodeoutput, &im1, &im2, &im3, &rsize, &csize);
}

LIB_etecalcdll_CPP_API 
void MW_CALL_CONV confocaleasyver(int nargout, mwArray& grav, mwArray& maxvalue, const 
                                  mwArray& path, const mwArray& imagenumber, const 
                                  mwArray& step, const mwArray& left, const mwArray& 
                                  right, const mwArray& rsize, const mwArray& csize)
{
  mclcppMlfFeval(_mcr_inst, "confocaleasyver", nargout, 2, 7, &grav, &maxvalue, &path, &imagenumber, &step, &left, &right, &rsize, &csize);
}

LIB_etecalcdll_CPP_API 
void MW_CALL_CONV vsieasyver(int nargout, mwArray& dmaxvalue, mwArray& dmaxindex, 
                             mwArray& dm1value, mwArray& dn1value, const mwArray& path, 
                             const mwArray& imagenumber, const mwArray& step, const 
                             mwArray& rsize, const mwArray& csize)
{
  mclcppMlfFeval(_mcr_inst, "vsieasyver", nargout, 4, 5, &dmaxvalue, &dmaxindex, &dm1value, &dn1value, &path, &imagenumber, &step, &rsize, &csize);
}

LIB_etecalcdll_CPP_API 
void MW_CALL_CONV vsiam(int nargout, mwArray& value_max, mwArray& idx_max, const mwArray& 
                        path, const mwArray& imagenumber, const mwArray& step, const 
                        mwArray& rsize, const mwArray& csize)
{
  mclcppMlfFeval(_mcr_inst, "vsiam", nargout, 2, 5, &value_max, &idx_max, &path, &imagenumber, &step, &rsize, &csize);
}

LIB_etecalcdll_CPP_API 
void MW_CALL_CONV confocal3Dmemory(int nargout, mwArray& grav, mwArray& maxvalue, const 
                                   mwArray& image_array, const mwArray& step_count, const 
                                   mwArray& step, const mwArray& left, const mwArray& 
                                   right, const mwArray& rsize, const mwArray& csize)
{
  mclcppMlfFeval(_mcr_inst, "confocal3Dmemory", nargout, 2, 7, &grav, &maxvalue, &image_array, &step_count, &step, &left, &right, &rsize, &csize);
}

LIB_etecalcdll_CPP_API 
void MW_CALL_CONV datafilter(int nargout, mwArray& filtered_array, const mwArray& 
                             data_array, const mwArray& rsize, const mwArray& csize, 
                             const mwArray& n)
{
  mclcppMlfFeval(_mcr_inst, "datafilter", nargout, 1, 4, &filtered_array, &data_array, &rsize, &csize, &n);
}

