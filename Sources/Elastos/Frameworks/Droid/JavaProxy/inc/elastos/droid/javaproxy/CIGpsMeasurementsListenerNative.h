
#ifndef __ELASTOS_DROID_JAVAPROXY_CIGPSMEASUREMENTSLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIGPSMEASUREMENTSLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIGpsMeasurementsListenerNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Location::IIGpsMeasurementsListener;
using Elastos::Droid::Location::IGpsMeasurementsEvent;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIGpsMeasurementsListenerNative)
    , public Object
    , public IIGpsMeasurementsListener
    , public IBinder
{
public:
    ~CIGpsMeasurementsListenerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnGpsMeasurementsReceived(
        /* [in] */ IGpsMeasurementsEvent* event);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIGPSMEASUREMENTSLISTENERNATIVE_H__
