/*
  Copyright (C) 2004, 2005 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify,
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#if defined(MUPARSER_DLL) && defined(_WIN32)

#include "muParserDLL.h"
#include "muParser.h"
#include "muParserError.h"


#define MU_PARSER_TRY  \
        try            \
        {

#define MU_PARSER_CATCH          \
        }                        \
        catch(exception_type &e) \
        {                        \
          g_bError = true;       \
          g_ParserError = e;     \
          if (g_pErrHandler)     \
            g_pErrHandler();     \
        }                        \
        catch(...)               \
        {                        \
          g_bError = true;       \
          g_ParserError = exception_type(mu::ecINTERNAL_ERROR); \
          if (g_pErrHandler)     \
            g_pErrHandler();     \
        }

//---------------------------------------------------------------------------
typedef mu::ParserBase::exception_type exception_type;
typedef mu::ParserBase* parser_type;

//---------------------------------------------------------------------------
//
//
//  unexported variables
//
//
//---------------------------------------------------------------------------

/** \brief The last exception that was caught.
*/
exception_type g_ParserError;
errhandler_type g_pErrHandler;

//---------------------------------------------------------------------------
/** \brief Flags indicating an error occured.
*/
bool g_bError;

//---------------------------------------------------------------------------
//
//
//  unexported functions
//
//
//---------------------------------------------------------------------------

parser_type GetPtr(parser_handle a_hParser)
{
  return static_cast<parser_type>(a_hParser);
}

//---------------------------------------------------------------------------
/** \brief DLL entry point.
*/
BOOL APIENTRY DllMain( HANDLE /*hModule*/, DWORD  ul_reason_for_call, LPVOID /*lpReserved*/ )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
        g_pErrHandler = 0;
        g_bError = false;
        break;

  case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
    		break;
	}

  return TRUE;
}

//---------------------------------------------------------------------------
//
//
//  exported functions
//
//
//---------------------------------------------------------------------------

MU_PARSER_API void mupSetErrorHandler(errhandler_type a_pHandler)
{
  g_pErrHandler = a_pHandler;
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupSetVarFactory(parser_handle a_hParser, facfun_type a_pFactory)
{
  parser_type p(GetPtr(a_hParser));
  p->SetVarFactory(a_pFactory);
}

//---------------------------------------------------------------------------
/** \brief Create a new Parser instance and return its handle.
*/
MU_PARSER_API parser_handle mupInit()
{
  return (void*)(new mu::Parser());
}

//---------------------------------------------------------------------------
/** \brief Release the parser instance related with a parser handle.
*/
MU_PARSER_API void mupRelease(parser_handle a_hParser)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    delete p;
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
/** \brief Evaluate the expression.
*/
MU_PARSER_API double mupEval(parser_handle a_hParser)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    return p->Eval();
  MU_PARSER_CATCH

  return 0;
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupSetExpr(parser_handle a_hParser, const char *a_szExpr)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->SetExpr(a_szExpr);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupRemoveVar(parser_handle a_hParser, const char *a_szName)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->RemoveVar( std::string(a_szName) );
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
/** \brief Release all parser variables.
    \param a_hParser Handle to the parser instance.
*/
MU_PARSER_API void mupClearVar(parser_handle a_hParser)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->ClearVar();
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
/** \brief Release all parser variables.
    \param a_hParser Handle to the parser instance.
*/
MU_PARSER_API void mupClearConst(parser_handle a_hParser)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->ClearConst();
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
/** \brief Clear all user defined operators.
    \param a_hParser Handle to the parser instance.
*/
MU_PARSER_API void mupClearOprt(parser_handle a_hParser)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->ClearOprt();
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineFun1(parser_handle a_hParser, const char *a_szName, fun_type1 a_pFun, bool a_bAllowOpt)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineFun2(parser_handle a_hParser, const char *a_szName, fun_type2 a_pFun, bool a_bAllowOpt)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineFun3(parser_handle a_hParser, const char *a_szName, fun_type3 a_pFun, bool a_bAllowOpt)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineFun4(parser_handle a_hParser, const char *a_szName, fun_type4 a_pFun, bool a_bAllowOpt)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineFun5(parser_handle a_hParser, const char *a_szName, fun_type5 a_pFun, bool a_bAllowOpt)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineStrFun(parser_handle a_hParser, const char *a_szName, strfun_type1 a_pFun, bool a_bAllowOpt)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineMultFun(parser_handle a_hParser, const char *a_szName, multfun_type a_pFun, bool a_bAllowOpt)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefineFun(a_szName, a_pFun, a_bAllowOpt);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineOprt(parser_handle a_hParser, const char *a_szName, fun_type2 a_pFun, int a_iPri, bool a_bAllowOpt)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefineOprt(a_szName, a_pFun, a_iPri, a_bAllowOpt);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineVar(parser_handle a_hParser, const char *a_szName, double *a_pVar)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefineVar(a_szName, a_pVar);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineConst(parser_handle a_hParser, const char *a_szName, double a_fVal)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefineConst(a_szName, a_fVal);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineStrConst(parser_handle a_hParser, const char *a_szName, const char *a_szVal)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefineStrConst(a_szName, a_szVal);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API const char* mupGetExpr(parser_handle a_hParser)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    return p->GetExpr().c_str();
  MU_PARSER_CATCH

  return "";
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefinePostfixOprt(parser_handle a_hParser,
                                        const char *a_szName,
                                        fun_type1 a_pOprt,
                                        bool a_bAllowOpt)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefinePostfixOprt(a_szName, a_pOprt, a_bAllowOpt);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineInfixOprt(parser_handle a_hParser,
                                      const char *a_szName,
                                      fun_type1 a_pOprt,
                                      bool a_bAllowOpt)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->DefineInfixOprt(a_szName, a_pOprt, a_bAllowOpt);
  MU_PARSER_CATCH
}

// Define character sets for identifiers
//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineNameChars(parser_handle a_hParser, const char *a_szCharset)
{
  parser_type p(GetPtr(a_hParser));
  p->DefineNameChars(a_szCharset);
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineOprtChars(parser_handle a_hParser, const char *a_szCharset)
{
  parser_type p(GetPtr(a_hParser));
  p->DefineOprtChars(a_szCharset);
}

//---------------------------------------------------------------------------
MU_PARSER_API void mupDefineInfixOprtChars(parser_handle a_hParser, const char *a_szCharset)
{
  parser_type p(GetPtr(a_hParser));
  p->DefineInfixOprtChars(a_szCharset);
}

//---------------------------------------------------------------------------
/** \brief Get the number of variables defined in the parser.
    \param a_hParser [in] Must be a valid parser handle.
    \return The number of used variables.
    \sa mupGetExprVar
*/
MU_PARSER_API int mupGetVarNum(parser_handle a_hParser)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    const mu::varmap_type VarMap = p->GetVar();
    return (int)VarMap.size();
  MU_PARSER_CATCH

  return 0; // never reached
}

//---------------------------------------------------------------------------
/** \brief Return a variable that is used in an expression.

    Prior to calling this function call mupGetExprVarNum in order to get the
    number of variables in the expression. If the parameter a_iVar is greater
    than the number of variables both a_szName and a_pVar will be set to zero.
    As a side effect this function will trigger an internal calculation of the
    expression undefined variables will be set to zero during this calculation.
    During the calculation user defined callback functions present in the expression
    will be called, this is unavoidable.

    \param a_hParser [in] A valid parser handle.
    \param a_iVar [in] The index of the variable to return.
    \param a_szName [out] Pointer to the variable name.
    \param a_pVar [out] Pointer to the variable.
    \throw nothrow
*/
MU_PARSER_API void mupGetVar(parser_handle a_hParser, unsigned a_iVar, const char **a_szName, double **a_pVar)
{
  // A static buffer is needed for the name since i cant return the
  // pointer from the map.
  static char  szName[1024];

  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    const mu::varmap_type VarMap = p->GetVar();

    if (a_iVar>=VarMap.size())
    {
     *a_szName = 0;
     *a_pVar = 0;
      return;
    }
    mu::varmap_type::const_iterator item;

    item = VarMap.begin();
    for (unsigned i=0; i<a_iVar; ++i)
      item++;

     strncpy(szName, item->first.c_str(), sizeof(szName));
     szName[sizeof(szName)-1] = 0;

    *a_szName = &szName[0];
    *a_pVar = item->second;
     return;

  MU_PARSER_CATCH

  *a_szName = 0;
  *a_pVar = 0;
}

//---------------------------------------------------------------------------
/** \brief Get the number of variables used in the expression currently set in the parser.
    \param a_hParser [in] Must be a valid parser handle.
    \return The number of used variables.
    \sa mupGetExprVar
*/
MU_PARSER_API int mupGetExprVarNum(parser_handle a_hParser)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    const mu::varmap_type VarMap = p->GetUsedVar();
    return (int)VarMap.size();
  MU_PARSER_CATCH

  return 0; // never reached
}

//---------------------------------------------------------------------------
/** \brief Return a variable that is used in an expression.

    Prior to calling this function call mupGetExprVarNum in order to get the
    number of variables in the expression. If the parameter a_iVar is greater
    than the number of variables both a_szName and a_pVar will be set to zero.
    As a side effect this function will trigger an internal calculation of the
    expression undefined variables will be set to zero during this calculation.
    During the calculation user defined callback functions present in the expression
    will be called, this is unavoidable.

    \param a_hParser [in] A valid parser handle.
    \param a_iVar [in] The index of the variable to return.
    \param a_szName [out] Pointer to the variable name.
    \param a_pVar [out] Pointer to the variable.
    \throw nothrow
*/
MU_PARSER_API void mupGetExprVar(parser_handle a_hParser, unsigned a_iVar, const char **a_szName, double **a_pVar)
{
  // A static buffer is needed for the name since i cant return the
  // pointer from the map.
  static char  szName[1024];

  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    const mu::varmap_type VarMap = p->GetUsedVar();

    if (a_iVar>=VarMap.size())
    {
     *a_szName = 0;
     *a_pVar = 0;
      return;
    }
    mu::varmap_type::const_iterator item;

    item = VarMap.begin();
    for (unsigned i=0; i<a_iVar; ++i)
      item++;

     strncpy(szName, item->first.c_str(), sizeof(szName));
     szName[sizeof(szName)-1] = 0;

    *a_szName = &szName[0];
    *a_pVar = item->second;
     return;

  MU_PARSER_CATCH

  *a_szName = 0;
  *a_pVar = 0;
}

//---------------------------------------------------------------------------
/** \brief Return the number of constants defined in a parser. */
MU_PARSER_API int mupGetConstNum(parser_handle a_hParser)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    const mu::valmap_type ValMap = p->GetConst();
    return (int)ValMap.size();
  MU_PARSER_CATCH

  return 0; // never reached
}

//---------------------------------------------------------------------------
/** \brief Retrieve name and value of a single parser constant.
    \param a_hParser [in] a valid parser handle
    \param a_iVar [in] Index of the constant to query
    \param a_pszName [out] pointer to a null terminated string with the constant name
    \param [out] The constant value
*/
MU_PARSER_API void mupGetConst(parser_handle a_hParser, unsigned a_iVar,
                               const char **a_pszName, double &a_fVal)
{
  // A static buffer is needed for the name since i cant return the
  // pointer from the map.
  static char  szName[1024];

  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    const mu::valmap_type ValMap = p->GetConst();

    if (a_iVar>=ValMap.size())
    {
     *a_pszName = 0;
      a_fVal = 0;
      return;
    }

    mu::valmap_type::const_iterator item;
    item = ValMap.begin();
    for (unsigned i=0; i<a_iVar; ++i)
      item++;

    strncpy(szName, item->first.c_str(), sizeof(szName));
    szName[sizeof(szName)-1] = 0;

    *a_pszName = &szName[0];
     a_fVal = item->second;
     return;

  MU_PARSER_CATCH

  *a_pszName = 0;
   a_fVal = 0;
}

//---------------------------------------------------------------------------
/** \brief Add a custom value regognition function.
*/
MU_PARSER_API void mupAddValIdent(parser_handle a_hParser, identfun_type a_pFun)
{
  MU_PARSER_TRY
    parser_type p(GetPtr(a_hParser));
    p->AddValIdent(a_pFun);
  MU_PARSER_CATCH
}

//---------------------------------------------------------------------------
/** \brief Query if an error occured.

    After querying the internal error bit will be reset. So a consecutive call
    will return false.
*/
MU_PARSER_API bool mupError()
{
  bool bError(g_bError);
  g_bError = false;
  return bError;
}

//---------------------------------------------------------------------------
/** \brief Reset the internal error flag.
*/
MU_PARSER_API void mupErrorReset()
{
  g_bError = false;
}

//---------------------------------------------------------------------------
/** \brief Return the message associated with the last error.
*/
MU_PARSER_API const char* mupGetErrorMsg()
{
  return g_ParserError.GetMsg().c_str();
}

//---------------------------------------------------------------------------
/** \brief Return the message associated with the last error.
*/
MU_PARSER_API const char* mupGetErrorToken()
{
  return g_ParserError.GetToken().c_str();
}

//---------------------------------------------------------------------------
/** \brief Return the code associated with the last error.
*/
MU_PARSER_API int mupGetErrorCode()
{
  return g_ParserError.GetCode();
}

//---------------------------------------------------------------------------
/** \brief Return the postion associated with the last error. */
MU_PARSER_API int mupGetErrorPos()
{
  return (int)g_ParserError.GetPos();
}


#endif      // MUPARSER_DLL
