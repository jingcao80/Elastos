
#ifndef __ELASTOS_DROID_JAVAPROXY_CLOCATIONLISTENER_H__
#define __ELASTOS_DROID_JAVAPROXY_CLOCATIONLISTENER_H__

#include "_Elastos_Droid_JavaProxy_CLocationListener.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::IILocationListener;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CLocationListener)
    , public Object
    , public IILocationListener
    , public IBinder
{
public:
    ~CLocationListener();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnLocationChanged(
        /* [in] */ ILocation* location);

    CARAPI OnStatusChanged(
        /* [in] */ const String& provider,
        /* [in] */ Int32 status,
        /* [in] */ IBundle* extras);

    CARAPI OnProviderEnabled(
        /* [in] */ const String& provider);

    CARAPI OnProviderDisabled(
        /* [in] */ const String& provider);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CLOCATIONLISTENER_H__
