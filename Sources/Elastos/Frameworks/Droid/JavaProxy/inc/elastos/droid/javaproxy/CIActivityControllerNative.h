
#ifndef __ELASTOS_DROID_JAVAPROXY_CIACTIVITYCONTROLLERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIACTIVITYCONTROLLERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIActivityControllerNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::IIActivityController;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIActivityControllerNative)
    , public Object
    , public IIActivityController
    , public IBinder
{
public:
    ~CIActivityControllerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI ActivityStarting(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& pkg,
        /* [out] */ Boolean* res);

    CARAPI ActivityResuming(
        /* [in] */ const String& pkg,
        /* [out] */ Boolean * pRes);

    CARAPI AppCrashed(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& shortMsg,
        /* [in] */ const String& longMsg,
        /* [in] */ Int64 timeMillis,
        /* [in] */ const String& stackTrace,
        /* [out] */ Boolean* res);

    CARAPI AppEarlyNotResponding(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& annotation,
        /* [out] */ Int32* res);

    CARAPI AppNotResponding(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& processStats,
        /* [out] */ Int32* res);

    CARAPI SystemNotResponding(
        /* [in] */ const String& msg,
        /* [out] */ Int32* res);

    CARAPI ToString(
        /* [out] */ String* str);
private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CIACTIVITYCONTROLLERNATIVE_H__
