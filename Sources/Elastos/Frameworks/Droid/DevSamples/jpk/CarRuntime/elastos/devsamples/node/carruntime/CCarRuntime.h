
#ifndef __CCARRUNTIME_H__
#define __CCARRUNTIME_H__

#include "_Elastos_DevSamples_Node_CarRuntime_CCarRuntime.h"

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace DevSamples {
namespace Node {
namespace CarRuntime {

CarClass(CCarRuntime)
    , public Object
    , public ICarRuntime
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Test_Require(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [out] */ IInterface ** ppOutObject);

    CARAPI Test_Require_ModuleInfo(
        /* [in] */ const String& moduleName,
        /* [out] */ IInterface ** ppOutObject);

    CARAPI Test_Require_ClassInfo(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [out] */ IInterface ** ppOutObject);

    CARAPI Test_Require_Activity(
        /* [in] */ const String& activityId,
        /* [out] */ IInterface ** ppOutObject);

    CARAPI Test_AddEventListener(
        /* [in] */ Elastos::DevSamples::Node::CarRuntime::ITestEventListener * pListener);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}

#endif // __CCARRUNTIME_H__
