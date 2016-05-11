
#ifndef __ELASTOS_DROID_JAVAPROXY_CICONDITIONLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CICONDITIONLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIConditionListenerNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Service::Notification::IIConditionListener;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIConditionListenerNative)
    , public Object
    , public IIConditionListener
    , public IBinder
{
public:
    ~CIConditionListenerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnConditionsReceived(
        /* [in] */ ArrayOf<ICondition*>* conditions);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CICONDITIONLISTENERNATIVE_H__
