
#ifndef __ELASTOS_DROID_JAVAPROXY_CGPSSTATUSLISTENER_H__
#define __ELASTOS_DROID_JAVAPROXY_CGPSSTATUSLISTENER_H__

#include "_Elastos_Droid_JavaProxy_CGpsStatusListener.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Location::IIGpsStatusListener;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CGpsStatusListener)
    , public Object
    , public IIGpsStatusListener
    , public IBinder
{
public:
    ~CGpsStatusListener();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnGpsStarted();

    CARAPI OnGpsStopped();

    CARAPI OnFirstFix(
        /* [in] */ Int32 ttff);

    CARAPI OnSvStatusChanged(
        /* [in] */ Int32 svCount,
        /* [in] */ ArrayOf<Int32>* prns,
        /* [in] */ ArrayOf<Float>* snrs,
        /* [in] */ ArrayOf<Float>* elevations,
        /* [in] */ ArrayOf<Float>* azimuths,
        /* [in] */ Int32 ephemerisMask,
        /* [in] */ Int32 almanacMask,
        /* [in] */ Int32 usedInFixMask);

    CARAPI OnNmeaReceived(
        /* [in] */ Int64 timestamp,
        /* [in] */ const String& nmea);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CGPSSTATUSLISTENER_H__
