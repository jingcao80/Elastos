
#include "CTestModuleInfo.h"
#include "CTestClassInfo.h"
#include "CTestInterfaceInfo.h"
#include "CTestStructInfo.h"
#include "CTestEnumInfo.h"
#include "CTestTypeAliasInfo.h"
#include "CTestConstantInfo.h"

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
    ECode ec = NOERROR;

    Int32 count;
    ec = this->GetClassCount(&count);

    ArrayOf<IClassInfo *> * classInfos;
    classInfos = ArrayOf<IClassInfo *>::Alloc(count);
    ec = mModuleInfo->GetAllClassInfos(classInfos);

    //Int32 mUsed = classInfos->GetUsed();
    Int32 used = count;

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

    return ec;
}

ECode CTestModuleInfo::GetClassInfo(
    /* [in] */ const String& fullName,
    /* [out] */ ITestClassInfo ** ppClassInfo)
{
    ECode ec = NOERROR;

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
    //return E_NOT_IMPLEMENTED;

    ECode ec = NOERROR;

    Int32 interfaceCount;
    ec = this->GetInterfaceCount(&interfaceCount);

    ArrayOf<IInterfaceInfo *> * interfaceInfos;
    interfaceInfos = ArrayOf<IInterfaceInfo *>::Alloc(interfaceCount);
    ec = mModuleInfo->GetAllInterfaceInfos(interfaceInfos);

    //Int32 mUsed = interfaceInfos->GetUsed();
    Int32 mUsed = interfaceCount;

    *ppInterfaceInfos = ArrayOf<ITestInterfaceInfo *>::Alloc(mUsed);

    for(Int32 i = 0; i < mUsed; i++){
        AutoPtr<ITestInterfaceInfo> testInterfaceInfo;
        ec = CTestInterfaceInfo::New((*interfaceInfos)[i],(ITestInterfaceInfo**)&testInterfaceInfo);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestInterfaceInfo");
            return ec;
        }
        (*ppInterfaceInfos)->Set(i,testInterfaceInfo);

        (*interfaceInfos)[i]->AddRef();
        testInterfaceInfo->AddRef();
    }   //for

    return ec;
}

ECode CTestModuleInfo::GetInterfaceInfo(
    /* [in] */ const String& fullName,
    /* [out] */ ITestInterfaceInfo ** ppInterfaceInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IInterfaceInfo> interfaceInfo;
    ec = mModuleInfo->GetInterfaceInfo(fullName,(IInterfaceInfo**)&interfaceInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetInterfaceInfo error: GetInterfaceInfo fail!");
        return ec;
    }

    AutoPtr<ITestInterfaceInfo> testInterfaceInfo;
    ec = CTestInterfaceInfo::New(interfaceInfo,(ITestInterfaceInfo**)&testInterfaceInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetInterfaceInfo error: CTestInterfaceInfo::New fail!");
        return ec;
    }
    *ppInterfaceInfo = testInterfaceInfo;

    interfaceInfo->AddRef();
    testInterfaceInfo->AddRef();

    return ec;
}

ECode CTestModuleInfo::GetStructCount(
    /* [out] */ Int32 * pCount)
{
    return mModuleInfo->GetStructCount(pCount);
}

ECode CTestModuleInfo::GetAllStructInfos(
    /* [out] */ ArrayOf<ITestStructInfo *> ** ppStructInfos)
{
    ECode ec = NOERROR;

    Int32 mCount;
    ec = this->GetStructCount(&mCount);

    ArrayOf<IStructInfo *> * structInfos;
    structInfos = ArrayOf<IStructInfo *>::Alloc(mCount);
    ec = mModuleInfo->GetAllStructInfos(structInfos);

    //Int32 mUsed = structInfos->GetUsed();
    Int32 mUsed = mCount;

    *ppStructInfos = ArrayOf<ITestStructInfo *>::Alloc(mUsed);

    for(Int32 i = 0; i < mUsed; i++){
        AutoPtr<ITestStructInfo> testStructInfo;
        ec = CTestStructInfo::New((*structInfos)[i],(ITestStructInfo**)&testStructInfo);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestStructInfo");
            return ec;
        }
        (*ppStructInfos)->Set(i,testStructInfo);

        (*structInfos)[i]->AddRef();
        testStructInfo->AddRef();
    }   //for

    return ec;
}

ECode CTestModuleInfo::GetStructInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestStructInfo ** ppStructInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IStructInfo> structInfo;
    ec = mModuleInfo->GetStructInfo(name,(IStructInfo**)&structInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetStructInfo error: GetStructInfo fail!");
        return ec;
    }

    AutoPtr<ITestStructInfo> testStructInfo;
    ec = CTestStructInfo::New(structInfo,(ITestStructInfo**)&testStructInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetStructInfo error: CTestStructInfo::New fail!");
        return ec;
    }
    *ppStructInfo = testStructInfo;

    structInfo->AddRef();
    testStructInfo->AddRef();

    return ec;
}

ECode CTestModuleInfo::GetEnumCount(
    /* [out] */ Int32 * pCount)
{
    return mModuleInfo->GetEnumCount(pCount);
}

ECode CTestModuleInfo::GetAllEnumInfos(
    /* [out] */ ArrayOf<ITestEnumInfo *> ** ppEnumInfos)
{
    ECode ec = NOERROR;

    Int32 mCount;
    ec = this->GetEnumCount(&mCount);

    ArrayOf<IEnumInfo *> * enumInfos;
    enumInfos = ArrayOf<IEnumInfo *>::Alloc(mCount);
    ec = mModuleInfo->GetAllEnumInfos(enumInfos);

    //Int32 mUsed = enumInfos->GetUsed();
    Int32 mUsed = mCount;

    *ppEnumInfos = ArrayOf<ITestEnumInfo *>::Alloc(mUsed);

    for(Int32 i = 0; i < mUsed; i++){
        AutoPtr<ITestEnumInfo> testEnumInfo;
        ec = CTestEnumInfo::New((*enumInfos)[i],(ITestEnumInfo**)&testEnumInfo);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestEnumInfo");
            return ec;
        }
        (*ppEnumInfos)->Set(i,testEnumInfo);

        (*enumInfos)[i]->AddRef();
        testEnumInfo->AddRef();
    }   //for

    return ec;
}

ECode CTestModuleInfo::GetEnumInfo(
    /* [in] */ const String& fullName,
    /* [out] */ ITestEnumInfo ** ppEnumInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IEnumInfo> enumInfo;
    ec = mModuleInfo->GetEnumInfo(fullName,(IEnumInfo**)&enumInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetEnumInfo error: GetEnumInfo fail!");
        return ec;
    }

    AutoPtr<ITestEnumInfo> testEnumInfo;
    ec = CTestEnumInfo::New(enumInfo,(ITestEnumInfo**)&testEnumInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetEnumInfo error: CTestEnumInfo::New fail!");
        return ec;
    }
    *ppEnumInfo = testEnumInfo;

    enumInfo->AddRef();
    testEnumInfo->AddRef();

    return ec;
}

ECode CTestModuleInfo::GetTypeAliasCount(
    /* [out] */ Int32 * pCount)
{
    return mModuleInfo->GetTypeAliasCount(pCount);
}

ECode CTestModuleInfo::GetAllTypeAliasInfos(
    /* [out] */ ArrayOf<ITestTypeAliasInfo *> ** ppTypeAliasInfos)
{
    ECode ec = NOERROR;

    Int32 mCount;
    ec = this->GetTypeAliasCount(&mCount);

    ArrayOf<ITypeAliasInfo *> * typeAliasInfos;
    typeAliasInfos = ArrayOf<ITypeAliasInfo *>::Alloc(mCount);
    ec = mModuleInfo->GetAllTypeAliasInfos(typeAliasInfos);

    //Int32 mUsed = typeAliasInfos->GetUsed();
    Int32 mUsed = mCount;

    *ppTypeAliasInfos = ArrayOf<ITestTypeAliasInfo *>::Alloc(mUsed);

    for(Int32 i = 0; i < mUsed; i++){
        AutoPtr<ITestTypeAliasInfo> testTypeAliasInfo;
        ec = CTestTypeAliasInfo::New((*typeAliasInfos)[i],(ITestTypeAliasInfo**)&testTypeAliasInfo);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestTypeAliasInfo");
            return ec;
        }
        (*ppTypeAliasInfos)->Set(i,testTypeAliasInfo);

        //(*typeAliasInfos)[i]->AddRef();
        testTypeAliasInfo->AddRef();
    }   //for

   return ec;
}

ECode CTestModuleInfo::GetTypeAliasInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestTypeAliasInfo ** ppTypeAliasInfo)
{
    ECode ec = NOERROR;

    AutoPtr<ITypeAliasInfo> typeAliasInfo;
    ec = mModuleInfo->GetTypeAliasInfo(name,(ITypeAliasInfo**)&typeAliasInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetTypeAliasInfo error: GetTypeAliasInfo fail!");
        return ec;
    }

    AutoPtr<ITestTypeAliasInfo> testTypeAliasInfo;
    ec = CTestTypeAliasInfo::New(typeAliasInfo,(ITestTypeAliasInfo**)&testTypeAliasInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetTypeAliasInfo error: CTestTypeAliasInfo::New fail!");
        return ec;
    }
    *ppTypeAliasInfo = testTypeAliasInfo;

    //typeAliasInfo->AddRef();
    testTypeAliasInfo->AddRef();

    return ec;
}

ECode CTestModuleInfo::GetConstantCount(
    /* [out] */ Int32 * pCount)
{
    return mModuleInfo->GetConstantCount(pCount);
}

ECode CTestModuleInfo::GetAllConstantInfos(
    /* [out] */ ArrayOf<ITestConstantInfo *> ** ppConstantInfos)
{
    ECode ec = NOERROR;

    Int32 mCount;
    ec = this->GetConstantCount(&mCount);

    ArrayOf<IConstantInfo *> * constantInfos;
    constantInfos = ArrayOf<IConstantInfo *>::Alloc(mCount);
    ec = mModuleInfo->GetAllConstantInfos(constantInfos);

    //Int32 mUsed = constantInfos->GetUsed();
    Int32 mUsed = mCount;

    *ppConstantInfos = ArrayOf<ITestConstantInfo *>::Alloc(mUsed);

    for(Int32 i = 0; i < mUsed; i++){
        AutoPtr<ITestConstantInfo> testConstantInfo;
        ec = CTestConstantInfo::New((*constantInfos)[i],(ITestConstantInfo**)&testConstantInfo);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestConstantInfo");
            return ec;
        }
        (*ppConstantInfos)->Set(i,testConstantInfo);

        (*constantInfos)[i]->AddRef();
        testConstantInfo->AddRef();
    }   //for

    return ec;
}

ECode CTestModuleInfo::GetConstantInfo(
    /* [in] */ const String& name,
    /* [out] */ ITestConstantInfo ** ppConstantInfo)
{
    ECode ec = NOERROR;

    AutoPtr<IConstantInfo> constantInfo;
    ec = mModuleInfo->GetConstantInfo(name,(IConstantInfo**)&constantInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetConstantInfo error: GetConstantInfo fail!");
        return ec;
    }

    AutoPtr<ITestConstantInfo> testConstantInfo;
    ec = CTestConstantInfo::New(constantInfo,(ITestConstantInfo**)&testConstantInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetConstantInfo error: CTestConstantInfo::New fail!");
        return ec;
    }
    *ppConstantInfo = testConstantInfo;

    constantInfo->AddRef();
    testConstantInfo->AddRef();

    return ec;
}

ECode CTestModuleInfo::GetImportModuleInfoCount(
    /* [out] */ Int32 * pCount)
{
    return mModuleInfo->GetImportModuleInfoCount(pCount);
}

ECode CTestModuleInfo::GetAllImportModuleInfos(
    /* [out] */ ArrayOf<ITestModuleInfo *> ** ppModuleInfos)
{
    ECode ec = NOERROR;

    Int32 mCount;
    ec = this->GetImportModuleInfoCount(&mCount);

    ArrayOf<IModuleInfo *> * importModuleInfos;
    importModuleInfos = ArrayOf<IModuleInfo *>::Alloc(mCount);
    ec = mModuleInfo->GetAllImportModuleInfos(importModuleInfos);

    //Int32 mUsed = importModuleInfos->GetUsed();
    Int32 mUsed = mCount;

    *ppModuleInfos = ArrayOf<ITestModuleInfo *>::Alloc(mUsed);

    for(Int32 i = 0; i < mUsed; i++){
        AutoPtr<ITestModuleInfo> testImportModuleInfo;
        ec = CTestModuleInfo::New((*importModuleInfos)[i],(ITestModuleInfo**)&testImportModuleInfo);
        if (FAILED(ec)) {
            ALOGD("Create \"%s\" instance failed!\n", "CTestImportModuleInfo");
            return ec;
        }
        (*ppModuleInfos)->Set(i,testImportModuleInfo);

        (*importModuleInfos)[i]->AddRef();
        testImportModuleInfo->AddRef();
    }   //for

    return ec;
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

ECode CTestModuleInfo::GetInternalObject(
    /* [out] */ PInterface* ppObject){
    *ppObject = mModuleInfo;
    return NOERROR;
}

}
}
}
}

