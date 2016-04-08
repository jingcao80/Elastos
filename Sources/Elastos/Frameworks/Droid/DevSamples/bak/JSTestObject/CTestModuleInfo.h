
#ifndef __CTESTMODULEINFO_H__
#define __CTESTMODULEINFO_H__

#include "_CTestModuleInfo.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {

CarClass(CTestModuleInfo)
{
public:
    CARAPI GetPath(
        /* [out] */ StringBuf * pPath);

    CARAPI GetVersion(
        /* [out] */ Int32 * pMajor,
        /* [out] */ Int32 * pMinor,
        /* [out] */ Int32 * pBuild,
        /* [out] */ Int32 * pRevision);

    CARAPI GetClassCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllClassInfos(
        /* [out] */ BufferOf<IClassInfo *> * pClassInfos);

    CARAPI GetClassInfo(
        /* [in] */ CString name,
        /* [out] */ IClassInfo ** ppClassInfo);

    CARAPI GetInterfaceCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ BufferOf<IInterfaceInfo *> * pInterfaceInfos);

    CARAPI GetInterfaceInfo(
        /* [in] */ CString name,
        /* [out] */ IInterfaceInfo ** ppInterfaceInfo);

    CARAPI GetStructCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllStructInfos(
        /* [out] */ BufferOf<IStructInfo *> * pStructInfos);

    CARAPI GetStructInfo(
        /* [in] */ CString name,
        /* [out] */ IStructInfo ** ppStructInfo);

    CARAPI GetEnumCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllEnumInfos(
        /* [out] */ BufferOf<IEnumInfo *> * pEnumInfos);

    CARAPI GetEnumInfo(
        /* [in] */ CString name,
        /* [out] */ IEnumInfo ** ppEnumInfo);

    CARAPI GetTypeAliasCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllTypeAliasInfos(
        /* [out] */ BufferOf<ITypeAliasInfo *> * pTypeAliasInfos);

    CARAPI GetTypeAliasInfo(
        /* [in] */ CString name,
        /* [out] */ ITypeAliasInfo ** ppTypeAliasInfo);

    CARAPI GetConstantCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllConstantInfos(
        /* [out] */ BufferOf<IConstantInfo *> * pConstantInfos);

    CARAPI GetConstantInfo(
        /* [in] */ CString name,
        /* [out] */ IConstantInfo ** ppConstantInfo);

    CARAPI GetImportModuleInfoCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllImportModuleInfos(
        /* [out] */ BufferOf<IModuleInfo *> * pModuleInfos);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IModuleInfo* pModuleInfo);

private:
    IModuleInfo* moduleInfo;
};

}
}
}
}

#endif // __CTESTMODULEINFO_H__
