
#include "CTestStructInfo.h"

#include "CTestModuleInfo.h"
#include "CTestFieldInfo.h"
#include "CTestVariableOfStruct.h"

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL_2(CTestStructInfo, Object, ITestDataTypeInfo, ITestStructInfo)

CAR_OBJECT_IMPL(CTestStructInfo)

ECode CTestStructInfo::GetName(
    /* [out] */ String * pName)
{
    return mStructInfo->GetName(pName);
}

ECode CTestStructInfo::GetSize(
    /* [out] */ MemorySize * pSize)
{
    return mStructInfo->GetSize(pSize);
}

ECode CTestStructInfo::GetDataType(
    /* [out] */ CarDataType * pDataType)
{
    return mStructInfo->GetDataType(pDataType);
}

ECode CTestStructInfo::GetModuleInfo(
    /* [out] */ ITestModuleInfo ** ppModuleInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IModuleInfo> info;
    ec = mStructInfo->GetModuleInfo((IModuleInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestStructInfo::GetModuleInfo error: GetModuleInfo fail!");
        return ec;
    }

    AutoPtr<ITestModuleInfo> testInfo;
    ec = CTestModuleInfo::New(info,(ITestModuleInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestStructInfo::GetModuleInfo error: CTestModlueInfo::New fail!");
        return ec;
    }
    *ppModuleInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CTestStructInfo::GetFieldCount(
    /* [out] */ Int32 * pCount)
{
    return mStructInfo->GetFieldCount(pCount);
}

ECode CTestStructInfo::GetAllFieldInfos(
    /* [out] */ ArrayOf<ITestFieldInfo *> ** ppFieldInfos)
{
    ECode ec = NOERROR;

    Int32 count;
    ec = this->GetFieldCount(&count);

    ArrayOf<IFieldInfo *> * infos;
    infos = ArrayOf<IFieldInfo *>::Alloc(count);
    ec = mStructInfo->GetAllFieldInfos(infos);

    //Int32 used = infos->GetUsed();
    Int32 used = count;

    *ppFieldInfos = ArrayOf<ITestFieldInfo *>::Alloc(used);

    for(Int32 i = 0; i < used; i++){
        AutoPtr<ITestFieldInfo> info;
        ec = CTestFieldInfo::New((*infos)[i],(ITestFieldInfo**)&info);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestFieldInfo");
            return ec;
        }
        (*ppFieldInfos)->Set(i,info);

        (*infos)[i]->AddRef();
        info->AddRef();
    }   //for

    return ec;
}

ECode CTestStructInfo::GetFieldInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestFieldInfo ** ppFieldInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IFieldInfo> info;
    ec = mStructInfo->GetFieldInfo(name, (IFieldInfo**)&info);
    if (FAILED(ec)) {
        ALOGD("CTestStructInfo::GetFieldInfo error: GetFieldInfo fail!");
        return ec;
    }

    AutoPtr<ITestFieldInfo> testInfo;
    ec = CTestFieldInfo::New(info,(ITestFieldInfo**)&testInfo);
    if (FAILED(ec)) {
        ALOGD("CTestStructInfo::GetFieldInfo error: CTestFieldInfo::New fail!");
        return ec;
    }
    *ppFieldInfo = testInfo;

    info->AddRef();
    testInfo->AddRef();

    return ec;
}

ECode CTestStructInfo::CreateVariable(
    /* [out] */ ITestVariableOfStruct ** ppVariableBox)
{
    ECode ec = NOERROR;

    AutoPtr<IVariableOfStruct> variableOfStruct;
    ec = mStructInfo->CreateVariable((IVariableOfStruct**)&variableOfStruct);
    if (FAILED(ec)) {
        ALOGD("CTestStructInfo::CreateVariable error: CreateVariable fail!");
        return ec;
    }

    AutoPtr<ITestVariableOfStruct> testVariableOfStruct;
    ec = CTestVariableOfStruct::New(variableOfStruct,(ITestVariableOfStruct**)&testVariableOfStruct);
    if (FAILED(ec)) {
        ALOGD("CTestStructInfo::CreateVariable error: CTestVariableOfStruct::New fail!");
        return ec;
    }
    *ppVariableBox = testVariableOfStruct;

    variableOfStruct->AddRef();
    testVariableOfStruct->AddRef();

    return ec;
}

ECode CTestStructInfo::CreateVariableBox(
    /* [in] */ PCarQuintet pVariableDescriptor,
    /* [out] */ ITestVariableOfStruct ** ppVariableBox)
{
    ECode ec = NOERROR;

    AutoPtr<IVariableOfStruct> variableOfStruct;
    ec = mStructInfo->CreateVariableBox(pVariableDescriptor, (IVariableOfStruct**)&variableOfStruct);
    if (FAILED(ec)) {
        ALOGD("CTestStructInfo::CreateVariable error: CreateVariable fail!");
        return ec;
    }

    AutoPtr<ITestVariableOfStruct> testVariableOfStruct;
    ec = CTestVariableOfStruct::New(variableOfStruct,(ITestVariableOfStruct**)&testVariableOfStruct);
    if (FAILED(ec)) {
        ALOGD("CTestStructInfo::CreateVariable error: CTestVariableOfStruct::New fail!");
        return ec;
    }
    *ppVariableBox = testVariableOfStruct;

    variableOfStruct->AddRef();
    testVariableOfStruct->AddRef();

    return ec;
}

ECode CTestStructInfo::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTestStructInfo::constructor(
    /* [in] */ IStructInfo * pStructInfo)
{
    mStructInfo = pStructInfo;
    return NOERROR;
}

}
}
}
}

