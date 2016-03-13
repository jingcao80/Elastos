
#ifndef __ELASTOS_DROID_JAVAPROXY_CICOUNTRYLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CICOUNTRYLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CICountryListenerNative.h"
#include <jni.h>

using Elastos::Droid::Location::ICountry;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CICountryListenerNative)
{
public:
    ~CICountryListenerNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

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
