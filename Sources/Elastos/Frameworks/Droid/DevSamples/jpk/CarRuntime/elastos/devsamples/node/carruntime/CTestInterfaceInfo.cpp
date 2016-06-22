
#include "CTestInterfaceInfo.h"

#include "CTestModuleInfo.h"
#include "CTestMethodInfo.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL_2(CTestInterfaceInfo, Object, ITestDataTypeInfo, ITestInterfaceInfo)

CAR_OBJECT_IMPL(CTestInterfaceInfo)

ECode CTestInterfaceInfo::GetName(
    /* [out] */ String * pName)
{
    ECode ec = mInterfaceInfo->GetName(pName);

    if (FAILED(ec)) {
        //E_INVALID_ARGUMENT
        //0x0800a000    E_NO_INTERFACE
        ALOGD("CTestInterfaceInfo::\"%s\" failed! ECode:%x\n", "GetName", ec);
        return ec;
    }

    return ec;
}

ECode CTestInterfaceInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    return mInterfaceInfo->GetSize(pSize);
}

ECode CTestInterfaceInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    return mInterfaceInfo->GetDataType(pDataType);
}

ECode CTestInterfaceInfo::GetNamespace(
    /* [out] */ String * pNs)
{
    return mInterfaceInfo->GetNamespace(pNs);
}

ECode CTestInterfaceInfo::GetId(
    /* [out] */ InterfaceID * pIid)
{
    return mInterfaceInfo->GetId(pIid);
}

ECode CTestInterfaceInfo::GetModuleInfo(
    /* [out] */ ITestModuleInfo ** ppModuleInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IModuleInfo> info;
    ec = mInterfaceInfo->GetModuleInfo((IModuleInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestInterfaceInfo::GetModuleInfo error: GetModuleInfo fail!");
        return ec;
    }

    AutoPtr<ITestModuleInfo> testInfo;
    ec = CTestModuleInfo::New(info,(ITestModuleInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestInterfaceInfo::GetModuleInfo error: CTestModlueInfo::New fail!");
        return ec;
    }
    *ppModuleInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CTestInterfaceInfo::IsLocal(
    /* [out] */ Boolean * pIsLocal)
{
    return mInterfaceInfo->IsLocal(pIsLocal);
}

ECode CTestInterfaceInfo::HasBase(
    /* [out] */ Boolean * pHasBase)
{
    return mInterfaceInfo->HasBase(pHasBase);
}

ECode CTestInterfaceInfo::GetBaseInfo(
    /* [out] */ ITestInterfaceInfo ** ppBaseInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IInterfaceInfo> info;
    ec = mInterfaceInfo->GetBaseInfo((IInterfaceInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestInterfaceInfo::GetBaseInfo error: GetBaseInfo fail!");
        return ec;
    }

    AutoPtr<ITestInterfaceInfo> testInfo;
    ec = CTestInterfaceInfo::New(info,(ITestInterfaceInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestInterfaceInfo::GetBaseInfo error: CTestInterfaceInfo::New fail!");
        return ec;
    }
    *ppBaseInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CTestInterfaceInfo::GetMethodCount(
    /* [out] */ Int32 * pCount)
{
    return mInterfaceInfo->GetMethodCount(pCount);
}

ECode CTestInterfaceInfo::GetAllMethodInfos(
    /* [out] */ ArrayOf<ITestMethodInfo *> ** ppMethodInfos)
{
    ECode ec = NOERROR;

    Int32 count;
    ec = this->GetMethodCount(&count);

    ArrayOf<IMethodInfo *> * infos;
    infos = ArrayOf<IMethodInfo *>::Alloc(count);
    ec = mInterfaceInfo->GetAllMethodInfos(infos);

    //Int32 used = infos->GetUsed();
    Int32 used = count;

    *ppMethodInfos = ArrayOf<ITestMethodInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestMethodInfo> info;
        ec = CTestMethodInfo::New((*infos)[i],(ITestMethodInfo**)&info);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestMethodInfo");
            return ec;
        }
        (*ppMethodInfos)->Set(i,info);

        (*infos)[i]->AddRef();
        info->AddRef();
    }   //for

    return ec;
}

ECode CTestInterfaceInfo::GetMethodInfo(
    /* [in] */ const String& name,
    /* [in] */ const String& signature,
    /* [out] */ ITestMethodInfo ** ppMethodInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IMethodInfo> info;
    ec = mInterfaceInfo->GetMethodInfo(name, signature, (IMethodInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestInterfaceInfo::GetMethodInfo error: GetMethodInfo fail!");
        return ec;
    }

    AutoPtr<ITestMethodInfo> testInfo;
    ec = CTestMethodInfo::New(info,(ITestMethodInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestInterfaceInfo::GetMethodInfo error: CTestMethodInfo::New fail!");
        return ec;
    }
    *ppMethodInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CTestInterfaceInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestInterfaceInfo::constructor(
    /* [in] */ IInterfaceInfo * pInterfaceInfo)
{
    mInterfaceInfo = pInterfaceInfo;

    //interfaceInfo->AddRef();

    return NOERROR;
}

}
}
}
}

