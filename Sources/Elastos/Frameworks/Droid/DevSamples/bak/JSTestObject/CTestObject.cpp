
#include "CTestObject.h"
#include "CTestModuleInfo.h"

#include <cutils/log.h>

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace JSTestObject {

ECode CTestObject::Test_Require(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [out] */ IInterface** outObject)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(outObject != NULL);

    //String moduleName("/data/elastos/JSTestObject.eco");
    //String className("CTestObject");

    AutoPtr<IModuleInfo> moduleInfo;
    ECode ec = _CReflector_AcquireModuleInfo(
            moduleName.string(), (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }

    AutoPtr<IClassInfo> classInfo;
    ec = moduleInfo->GetClassInfo(
            className.string(), (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" class info failed!\n", className.string());
        return ec;
    }

    AutoPtr<IInterface> tmpJSTestObject;

    ec = classInfo->CreateObject((IInterface**)&tmpJSTestObject);
    if (FAILED(ec)) {
        ALOGD("Create object failed!\n");
        return ec;
    }

    *outObject = (IInterface*)tmpJSTestObject;
    if (*outObject != NULL) (*outObject)->AddRef();
    return NOERROR;
}

ECode CTestObject::Test_Require_ModuleInfo(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [out] */ IInterface** outObject)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(outObject != NULL);

    //String moduleName("/data/elastos/JSTestObject.eco");
    //String className("CTestObject");

    ECode ec;
    AutoPtr<IModuleInfo> moduleInfo;
    ec = _CReflector_AcquireModuleInfo(
            moduleName.string(), (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }

    AutoPtr<ITestModuleInfo> testModuleInfo;
    ec = CTestModuleInfo::New(moduleInfo,(ITestModuleInfo**)&testModuleInfo);
    if (FAILED(ec)) {
        ALOGD("Create \"%s\" instance failed!\n", "CTestModuleInfo");
        return ec;
    }
    *outObject = testModuleInfo;
    if (*outObject != NULL) {
        moduleInfo->AddRef();
        (*outObject)->AddRef();
    }

    return NOERROR;
}

ECode CTestObject::Test_Require_ClassInfo(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [out] */ IInterface** outObject)
{
    ALOGD("==== File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(outObject != NULL);

    //String moduleName("/data/elastos/JSTestObject.eco");
    //String className("CTestObject");

    AutoPtr<IModuleInfo> moduleInfo;
    ECode ec = _CReflector_AcquireModuleInfo(
            moduleName.string(), (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }

    AutoPtr<IClassInfo> classInfo;
    ec = moduleInfo->GetClassInfo(
            className.string(), (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" class info failed!\n", className.string());
        return ec;
    }

    *outObject = (IInterface*)classInfo;
    if (*outObject != NULL) (*outObject)->AddRef();
    return NOERROR;
}

}
}
}
}

