
#ifndef __CGPSSTATUSLISTENER_H__
#define __CGPSSTATUSLISTENER_H__

#include "_CGpsStatusListener.h"
#include <jni.h>
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CGpsStatusListener)
{
public:
    CGpsStatusListener();
    ~CGpsStatusListener();

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

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __CGPSSTATUSLISTENER_H__
