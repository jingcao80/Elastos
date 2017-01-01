
#ifndef _TLBINTER_H_
#define _TLBINTER_H_

#define RPC_NO_WINDOWS_H
#define _RPC_H
#define __RPCNDR_H__
#define _WTYPES_H
#define _OBJBASE_H

typedef wchar_t OLECHAR;
typedef OLECHAR *LPOLESTR;
#ifndef _MSVC
interface ICreateTypeInfo : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetGuid(
        /* [in] */ REMuid guid) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetTypeFlags(
        /* [in] */ UINT uTypeFlags) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDocString(
        /* [in] */ LPOLESTR pStrDoc) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetHelpContext(
        /* [in] */ DWORD dwHelpContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetVersion(
        /* [in] */ WORD wMajorVerNum,
        /* [in] */ WORD wMinorVerNum) = 0;

    virtual HRESULT STDMETHODCALLTYPE AddRefTypeInfo(
        /* [in] */ ITypeInfo  *pTInfo,
        /* [in] */ HREFTYPE  *phRefType) = 0;

    virtual HRESULT STDMETHODCALLTYPE AddFuncDesc(
        /* [in] */ UINT index,
        /* [in] */ FUNCDESC  *pFuncDesc) = 0;

    virtual HRESULT STDMETHODCALLTYPE AddImplType(
        /* [in] */ UINT index,
        /* [in] */ HREFTYPE hRefType) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetImplTypeFlags(
        /* [in] */ UINT index,
        /* [in] */ INT implTypeFlags) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetAlignment(
        /* [in] */ WORD cbAlignment) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetSchema(
        /* [in] */ LPOLESTR pStrSchema) = 0;

    virtual HRESULT STDMETHODCALLTYPE AddVarDesc(
        /* [in] */ UINT index,
        /* [in] */ VARDESC  *pVarDesc) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFuncAndParamNames(
        /* [in] */ UINT index,
        /* [in][size_is][in] */ LPOLESTR  *rgszNames,
        /* [in] */ UINT cNames) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetVarName(
        /* [in] */ UINT index,
        /* [in] */ LPOLESTR szName) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetTypeDescAlias(
        /* [in] */ TYPEDESC  *pTDescAlias) = 0;

    virtual HRESULT STDMETHODCALLTYPE DefineFuncAsDllEntry(
        /* [in] */ UINT index,
        /* [in] */ LPOLESTR szDllName,
        /* [in] */ LPOLESTR szProcName) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFuncDocString(
        /* [in] */ UINT index,
        /* [in] */ LPOLESTR szDocString) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetVarDocString(
        /* [in] */ UINT index,
        /* [in] */ LPOLESTR szDocString) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFuncHelpContext(
        /* [in] */ UINT index,
        /* [in] */ DWORD dwHelpContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetVarHelpContext(
        /* [in] */ UINT index,
        /* [in] */ DWORD dwHelpContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetMops(
        /* [in] */ UINT index,
        /* [in] */ BSTR bstrMops) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetTypeIdldesc(
        /* [in] */ IDLDESC  *pIdlDesc) = 0;

    virtual HRESULT STDMETHODCALLTYPE LayOut( void) = 0;

};

interface ICreateTypeLib : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE CreateTypeInfo(
        /* [in] */ LPOLESTR szName,
        /* [in] */ TYPEKIND tkind,
        /* [out] */ ICreateTypeInfo  * *ppCTInfo) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetName(
        /* [in] */ LPOLESTR szName) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetVersion(
        /* [in] */ WORD wMajorVerNum,
        /* [in] */ WORD wMinorVerNum) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetGuid(
        /* [in] */ REMuid guid) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDocString(
        /* [in] */ LPOLESTR szDoc) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetHelpFileName(
        /* [in] */ LPOLESTR szHelpFileName) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetHelpContext(
        /* [in] */ DWORD dwHelpContext) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetLcid(
        /* [in] */ LCID lcid) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetLibFlags(
        /* [in] */ UINT uLibFlags) = 0;

    virtual HRESULT STDMETHODCALLTYPE SaveAllChanges( void) = 0;

};
#endif
//extern "C" HRESULT __stdcall CreateTypeLib(SYSKIND syskind, const OLECHAR  *szFile,
//            ICreateTypeLib ** ppctlib);

//extern "C" HRESULT __stdcall LoadTypeLib(OLECHAR *szFile, ITypeLib **pptlib );

#endif
