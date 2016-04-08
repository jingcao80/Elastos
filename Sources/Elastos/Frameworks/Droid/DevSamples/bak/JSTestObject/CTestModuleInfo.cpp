
#include "CTestModuleInfo.h"
#include "CTestClassInfo.h"

#include <cutils/log.h>

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {


ECode CTestModuleInfo::GetPath(
    /* [out] */ StringBuf * pPath)
{
    return moduleInfo->GetPath(pPath);
}

ECode CTestModuleInfo::GetVersion(
    /* [out] */ Int32 * pMajor,
    /* [out] */ Int32 * pMinor,
    /* [out] */ Int32 * pBuild,
    /* [out] */ Int32 * pRevision)
{
    return moduleInfo->GetVersion(pMajor,pMinor,pBuild,pRevision);
}

ECode CTestModuleInfo::GetClassCount(
    /* [out] */ Int32 * pCount)
{
    return moduleInfo->GetClassCount(pCount);
}

ECode CTestModuleInfo::GetAllClassInfos(
    /* [out] */ BufferOf<IClassInfo *> * pClassInfos)
{
    //return moduleInfo->GetAllClassInfos(pClassInfos);

    //pClassInfos = BufferOf<IClassInfo *>::Alloc(512);

    BufferOf<IClassInfo *> * classinfos;
    classinfos = BufferOf<IClassInfo *>::Alloc(4096);
    //classinfos = BufferOf<IClassInfo *>::Alloc(1023);
    ECode ec = moduleInfo->GetAllClassInfos(classinfos);
    
    Int32 used = classinfos->GetUsed();

    //Bugs to be fix: several problems when used count gt 1023,
    //related with elastos runtime reflection and v8bindings,v8 core.

    if (used > 512) used = 512;

    pClassInfos->SetUsed(used);

    ALOGD("CTestModuleInfo::GetAllClassInfos used:%d \n",used);
    for(Int32 i = 0; i < used; i++){
        ALOGD("CTestModuleInfo::GetAllClassInfos used:%d cnt:%d \n",used,i);
        ((*classinfos)[i])->AddRef();

//--------------
    AutoPtr<ITestClassInfo> testClassInfo;
    ec = CTestClassInfo::New(((*classinfos)[i]),(ITestClassInfo**)&testClassInfo);
    if (FAILED(ec)) {
        ALOGD("Create \"%s\" instance failed!\n", "CTestClassInfo");
        return ec;
    }
    *(IClassInfo **)(&pClassInfos[i]) = *(IClassInfo**)&testClassInfo;

    IClassInfo* tttt = *(IClassInfo **)(&pClassInfos[i]);
    tttt->AddRef();

//----test begin----
    //strange:only add these code, classinfo can be get in CarClassCobject::CarClassCobject
    //may be problem of refer count, to be fixed

    AutoPtr<IClassInfo> temp_classInfo;
    ec = CObject::ReflectClassInfo(testClassInfo,(IClassInfo**)&temp_classInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetClassInfo.1 ITestClassInfo failed\n");
    } else {
        ALOGD("CTestModuleInfo::GetClassInfo.1 ITestClassInfo success\n");
    }
    ec = CObject::ReflectClassInfo(*(IClassInfo **)(&pClassInfos[i]),(IClassInfo**)&temp_classInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetClassInfo.2 ITestClassInfo failed\n");
    } else {
        ALOGD("CTestModuleInfo::GetClassInfo.2 ITestClassInfo success\n");
    }

//----test end----


    }   //for
    
    return ec;

}

ECode CTestModuleInfo::GetClassInfo(
    /* [in] */ CString name,
    /* [out] */ IClassInfo ** ppClassInfo)
{
    //return moduleInfo->GetClassInfo(name,ppClassInfo);

    ALOGD("CTestModuleInfo::GetClassInfo.begin");
    ALOGD("CTestModuleInfo::GetClassInfo name:%s \n", (const char *)name);

    ECode ec;
    AutoPtr<IClassInfo> classInfo;
    ec = moduleInfo->GetClassInfo(name,(IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" class info failed!\n", (const char *)name);
        return ec;
    }

    AutoPtr<ITestClassInfo> testClassInfo;
    ec = CTestClassInfo::New(classInfo,(ITestClassInfo**)&testClassInfo);
    if (FAILED(ec)) {
        ALOGD("Create \"%s\" instance failed!\n", "CTestClassInfo");
        return ec;
    }
    *ppClassInfo = *(IClassInfo**)&testClassInfo;

//----test begin----
    //strange:only add these code, classinfo can be get in CarClassCobject::CarClassCobject
    //may be problem of refer count, to be fixed

    AutoPtr<IClassInfo> temp_classInfo;
    ec = CObject::ReflectClassInfo(testClassInfo,(IClassInfo**)&temp_classInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetClassInfo.1 ITestClassInfo failed\n");
    } else {
        ALOGD("CTestModuleInfo::GetClassInfo.1 ITestClassInfo success\n");
    }
    ec = CObject::ReflectClassInfo(*ppClassInfo,(IClassInfo**)&temp_classInfo);
    if (FAILED(ec)) {
        ALOGD("CTestModuleInfo::GetClassInfo.2 ITestClassInfo failed\n");
    } else {
        ALOGD("CTestModuleInfo::GetClassInfo.2 ITestClassInfo success\n");
    }
//----test end----

    if (*ppClassInfo != NULL) {
        ALOGD("CTestModuleInfo::GetClassInfo ITestClassInfo is ok");
        classInfo->AddRef();
        (*ppClassInfo)->AddRef();
    } else {
        ALOGD("CTestModuleInfo::GetClassInfo ITestClassInfo is null");
    }

    return ec;

}

ECode CTestModuleInfo::GetInterfaceCount(
    /* [out] */ Int32 * pCount)
{
    return moduleInfo->GetInterfaceCount(pCount);
}

ECode CTestModuleInfo::GetAllInterfaceInfos(
    /* [out] */ BufferOf<IInterfaceInfo *> * pInterfaceInfos)
{
    return moduleInfo->GetAllInterfaceInfos(pInterfaceInfos);
}

ECode CTestModuleInfo::GetInterfaceInfo(
    /* [in] */ CString name,
    /* [out] */ IInterfaceInfo ** ppInterfaceInfo)
{
    return moduleInfo->GetInterfaceInfo(name,ppInterfaceInfo);
}

ECode CTestModuleInfo::GetStructCount(
    /* [out] */ Int32 * pCount)
{
    return moduleInfo->GetStructCount(pCount);
}

ECode CTestModuleInfo::GetAllStructInfos(
    /* [out] */ BufferOf<IStructInfo *> * pStructInfos)
{
    return moduleInfo->GetAllStructInfos(pStructInfos);
}

ECode CTestModuleInfo::GetStructInfo(
    /* [in] */ CString name,
    /* [out] */ IStructInfo ** ppStructInfo)
{
    return moduleInfo->GetStructInfo(name,ppStructInfo);
}

ECode CTestModuleInfo::GetEnumCount(
    /* [out] */ Int32 * pCount)
{
    return moduleInfo->GetEnumCount(pCount);
}

ECode CTestModuleInfo::GetAllEnumInfos(
    /* [out] */ BufferOf<IEnumInfo *> * pEnumInfos)
{
    return moduleInfo->GetAllEnumInfos(pEnumInfos);
}

ECode CTestModuleInfo::GetEnumInfo(
    /* [in] */ CString name,
    /* [out] */ IEnumInfo ** ppEnumInfo)
{
    return moduleInfo->GetEnumInfo(name,ppEnumInfo);
}

ECode CTestModuleInfo::GetTypeAliasCount(
    /* [out] */ Int32 * pCount)
{
    return moduleInfo->GetTypeAliasCount(pCount);
}

ECode CTestModuleInfo::GetAllTypeAliasInfos(
    /* [out] */ BufferOf<ITypeAliasInfo *> * pTypeAliasInfos)
{
    return moduleInfo->GetAllTypeAliasInfos(pTypeAliasInfos);
}

ECode CTestModuleInfo::GetTypeAliasInfo(
    /* [in] */ CString name,
    /* [out] */ ITypeAliasInfo ** ppTypeAliasInfo)
{
    return moduleInfo->GetTypeAliasInfo(name,ppTypeAliasInfo);
}

ECode CTestModuleInfo::GetConstantCount(
    /* [out] */ Int32 * pCount)
{
    return moduleInfo->GetConstantCount(pCount);
}

ECode CTestModuleInfo::GetAllConstantInfos(
    /* [out] */ BufferOf<IConstantInfo *> * pConstantInfos)
{
    return moduleInfo->GetAllConstantInfos(pConstantInfos);
}

ECode CTestModuleInfo::GetConstantInfo(
    /* [in] */ CString name,
    /* [out] */ IConstantInfo ** ppConstantInfo)
{
    return moduleInfo->GetConstantInfo(name,ppConstantInfo);
}

ECode CTestModuleInfo::GetImportModuleInfoCount(
    /* [out] */ Int32 * pCount)
{
    return moduleInfo->GetImportModuleInfoCount(pCount);
}

ECode CTestModuleInfo::GetAllImportModuleInfos(
    /* [out] */ BufferOf<IModuleInfo *> * pModuleInfos)
{
    return moduleInfo->GetAllImportModuleInfos(pModuleInfos);
}

ECode CTestModuleInfo::constructor()
{
    return NOERROR;
}

ECode CTestModuleInfo::constructor(
    /* [in] */ IModuleInfo* pModuleInfo)
{
    moduleInfo = pModuleInfo;
    return NOERROR;
}

}
}
}
}

