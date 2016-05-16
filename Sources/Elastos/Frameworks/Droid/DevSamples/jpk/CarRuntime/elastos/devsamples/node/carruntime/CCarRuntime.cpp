
#include "CCarRuntime.h"

#include "CTestModuleInfo.h"

#include <cutils/log.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CAR_INTERFACE_IMPL(CCarRuntime, Object, ICarRuntime)

CAR_OBJECT_IMPL(CCarRuntime)

ECode CCarRuntime::Test_Require(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [out] */ IInterface ** ppOutObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_Require_ModuleInfo(
    /* [in] */ const String& moduleName,
    /* [out] */ IInterface ** ppOutObject)
{
    ALOGD("====00 File: %s, Function: %s ====", __FILE__, __FUNCTION__);
    assert(ppOutObject != NULL);

    ECode ec;
    AutoPtr<IModuleInfo> moduleInfo;
    ec = _CReflector_AcquireModuleInfo(
            moduleName, (IModuleInfo**)&moduleInfo);
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
    *ppOutObject = testModuleInfo;
    if (*ppOutObject != NULL) {
        moduleInfo->AddRef();
        (*ppOutObject)->AddRef();
    }

    return NOERROR;
}

ECode CCarRuntime::Test_Require_ClassInfo(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [out] */ IInterface ** ppOutObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_Require_Activity(
    /* [in] */ const String& activityId,
    /* [out] */ IInterface ** ppOutObject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CCarRuntime::Test_AddEventListener(
    /* [in] */ Elastos::DevSamples::Node::CarRuntime::ITestEventListener * pListener)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}

