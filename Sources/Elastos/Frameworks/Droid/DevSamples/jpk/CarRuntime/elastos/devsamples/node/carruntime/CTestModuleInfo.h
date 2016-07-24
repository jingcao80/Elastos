
#ifndef __CTESTMODULEINFO_H__
#define __CTESTMODULEINFO_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CTestModuleInfo.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CTestModuleInfo)
    , public Object
    , public ITestModuleInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI createObject(
        /* [in] */ const String& className,
        /* [out] */ IInterface ** ppOutObject);

    CARAPI GetPath(
        /* [out] */ String * pPath);

    CARAPI GetVersion(
        /* [out] */ Int32 * pMajor,
        /* [out] */ Int32 * pMinor,
        /* [out] */ Int32 * pBuild,
        /* [out] */ Int32 * pRevision);

    CARAPI GetClassCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllClassInfos(
        /* [out, callee] */ ArrayOf<ITestClassInfo *> ** ppClassInfos);

    CARAPI GetClassInfo(
        /* [in] */ const String& fullName,
        /* [out] */ ITestClassInfo ** ppClassInfo);

    CARAPI GetInterfaceCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ ArrayOf<ITestInterfaceInfo *> ** ppInterfaceInfos);

    CARAPI GetInterfaceInfo(
        /* [in] */ const String& fullName,
        /* [out] */ ITestInterfaceInfo ** ppInterfaceInfo);

    CARAPI GetStructCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllStructInfos(
        /* [out] */ ArrayOf<ITestStructInfo *> ** ppStructInfos);

    CARAPI GetStructInfo(
        /* [in] */ const String& name,
        /* [out] */ ITestStructInfo ** ppStructInfo);

    CARAPI GetEnumCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllEnumInfos(
        /* [out] */ ArrayOf<ITestEnumInfo *> ** ppEnumInfos);

    CARAPI GetEnumInfo(
        /* [in] */ const String& fullName,
        /* [out] */ ITestEnumInfo ** ppEnumInfo);

    CARAPI GetTypeAliasCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllTypeAliasInfos(
        /* [out] */ ArrayOf<ITestTypeAliasInfo *> ** ppTypeAliasInfos);

    CARAPI GetTypeAliasInfo(
        /* [in] */ const String& name,
        /* [out] */ ITestTypeAliasInfo ** ppTypeAliasInfo);

    CARAPI GetConstantCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllConstantInfos(
        /* [out] */ ArrayOf<ITestConstantInfo *> ** ppConstantInfos);

    CARAPI GetConstantInfo(
        /* [in] */ const String& name,
        /* [out] */ ITestConstantInfo ** ppConstantInfo);

    CARAPI GetImportModuleInfoCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllImportModuleInfos(
        /* [out] */ ArrayOf<ITestModuleInfo *> ** ppModuleInfos);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IModuleInfo * pModuleInfo);

    CARAPI GetInternalObject(
        /* [out] */ PInterface* ppObject);

private:
    AutoPtr<IModuleInfo> mModuleInfo;
};

}
}
}
}

#endif // __CTESTMODULEINFO_H__
