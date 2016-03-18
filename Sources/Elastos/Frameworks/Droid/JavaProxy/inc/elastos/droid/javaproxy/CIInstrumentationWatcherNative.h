
#ifndef __ELASTOS_DROID_JAVAPROXY_CIINSTRUMENTATIONWATCHERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIINSTRUMENTATIONWATCHERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIInstrumentationWatcherNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::IInstrumentationWatcher;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIInstrumentationWatcherNative)
    , public Object
    , public IInstrumentationWatcher
    , public IBinder
{
public:
    ~CIInstrumentationWatcherNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI InstrumentationStatus(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    CARAPI InstrumentationFinished(
        /* [in] */ IComponentName* name,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIINSTRUMENTATIONWATCHERNATIVE_H__
