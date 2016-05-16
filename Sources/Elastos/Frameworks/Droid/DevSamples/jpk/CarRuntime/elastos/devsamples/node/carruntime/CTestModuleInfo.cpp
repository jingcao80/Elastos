
#include "CTestModuleInfo.h"

#include "CTestClassInfo.h"

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CTestModuleInfo, Object, ITestModuleInfo)

CAR_OBJECT_IMPL(CTestModuleInfo)

ECode CTestModuleInfo::GetPath(
    /* [out] */ String * pPath)
{
    return mModuleInfo->GetPath(pPath);
}

ECode CTestModuleInfo::GetVersion(
    /* [out] */ Int32 * pMajor,
    /* [out] */ Int32 * pMinor,
    /* [out] */ Int32 * pBuild,
    /* [out] */ Int32 * pRevision)
{
    return mModuleInfo->GetVersion(pMajor,pMinor,pBuild,pRevision);
}

ECode CTestModuleInfo::GetClassCount(
    /* [out] */ Int32 * pCount)
{
    return mModuleInfo->GetClassCount(pCount);
}

ECode CTestModuleInfo::GetAllClassInfos(
    /* [out, callee] */ ArrayOf<ITestClassInfo *> ** ppClassInfos)
{
    ECode ec;

    Int32 classCount;
    ec = this->GetClassCount(&classCount);

    ArrayOf<IClassInfo *> * classInfos;
    classInfos = ArrayOf<IClassInfo *>::Alloc(classCount);
    ec = mModuleInfo->GetAllClassInfos(classInfos);

    //Int32 used = classInfos->GetUsed();
    Int32 used = classCount;

    *ppClassInfos = ArrayOf<ITestClassInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestClassInfo> testClassInfo;
        ec = CTestClassInfo::New((*classInfos)[i],(ITestClassInfo**)&testClassInfo);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestClassInfo");
            return ec;
        }
        (*ppClassInfos)->Set(i,testClassInfo);

        (*classInfos)[i]->AddRef();
        testClassInfo->AddRef();
    }   //for

    ec = NOERROR;

    return ec;
}

ECode CTestModuleInfo::GetClassInfo(
    /* [in] */ const String& fullName,
    /* [out] */ ITestClassInfo ** ppClassInfo)
{
    ECode ec;

    AutoPtr<IClassInfo> classInfo;
    ec = mModuleInfo->GetClassInfo(fullName,(IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetClassInfo error: GetClassInfo fail!");
        return ec;
    }

    AutoPtr<ITestClassInfo> testClassInfo;
    ec = CTestClassInfo::New(classInfo,(ITestClassInfo**)&testClassInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetClassInfo error: CTestClassInfo::New fail!");
        return ec;
    }
    *ppClassInfo = testClassInfo;

    classInfo->AddRef();
    testClassInfo->AddRef();

    ec = NOERROR;

    return ec;
}

ECode CTestModuleInfo::GetInterfaceCount(
    /* [out] */ Int32 * pCount)
{
    return mModuleInfo->GetInterfaceCount(pCount);
}

ECode CTestModuleInfo::GetAllInterfaceInfos(
    /* [out] */ ArrayOf<ITestInterfaceInfo *> ** ppInterfaceInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestModuleInfo::GetInterfaceInfo(
    /* [in] */ const String& fullName,
    /* [out] */ ITestInterfaceInfo ** ppInterfaceInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestModuleInfo::GetStructCount(
    /* [out] */ Int32 * pCount)
{
    return mModuleInfo->GetStructCount(pCount);
}

ECode CTestModuleInfo::GetAllStructInfos(
    /* [out] */ ArrayOf<ITestStructInfo *> ** ppStructInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestModuleInfo::GetStructInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestStructInfo ** ppStructInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestModuleInfo::GetEnumCount(
    /* [out] */ Int32 * pCount)
{
    return mModuleInfo->GetEnumCount(pCount);
}

ECode CTestModuleInfo::GetAllEnumInfos(
    /* [out] */ ArrayOf<ITestEnumInfo *> ** ppEnumInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestModuleInfo::GetEnumInfo(
    /* [in] */ const String& fullName,
    /* [out] */ ITestEnumInfo ** ppEnumInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestModuleInfo::GetTypeAliasCount(
    /* [out] */ Int32 * pCount)
{
    return mModuleInfo->GetTypeAliasCount(pCount);
}

ECode CTestModuleInfo::GetAllTypeAliasInfos(
    /* [out] */ ArrayOf<ITestTypeAliasInfo *> ** ppTypeAliasInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestModuleInfo::GetTypeAliasInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestTypeAliasInfo ** ppTypeAliasInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestModuleInfo::GetConstantCount(
    /* [out] */ Int32 * pCount)
{
    return mModuleInfo->GetConstantCount(pCount);
}

ECode CTestModuleInfo::GetAllConstantInfos(
    /* [out] */ ArrayOf<ITestConstantInfo *> ** ppConstantInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestModuleInfo::GetConstantInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestConstantInfo ** ppConstantInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestModuleInfo::GetImportModuleInfoCount(
    /* [out] */ Int32 * pCount)
{
    return mModuleInfo->GetImportModuleInfoCount(pCount);
}

ECode CTestModuleInfo::GetAllImportModuleInfos(
    /* [out] */ ArrayOf<ITestModuleInfo *> ** ppModuleInfos)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestModuleInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestModuleInfo::constructor(
    /* [in] */ IModuleInfo * pModuleInfo)
{
    mModuleInfo = pModuleInfo;

    //moduleInfo->AddRef();

    return NOERROR;
}

}
}
}
}

