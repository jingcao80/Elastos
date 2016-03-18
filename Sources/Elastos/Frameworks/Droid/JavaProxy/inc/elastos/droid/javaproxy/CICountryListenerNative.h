
#ifndef __ELASTOS_DROID_JAVAPROXY_CICOUNTRYLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CICOUNTRYLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CICountryListenerNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::IICountryListener;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CICountryListenerNative)
    , public Object
    , public IICountryListener
    , public IBinder
{
public:
    ~CICountryListenerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnCountryDetected(
        /* [in] */ ICountry* country);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CICOUNTRYLISTENERNATIVE_H__
