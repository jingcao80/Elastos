
#ifndef __CICOUNTRYLISTENERNATIVE_H__
#define __CICOUNTRYLISTENERNATIVE_H__

#include "_CICountryListenerNative.h"
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

#endif // __CICOUNTRYLISTENERNATIVE_H__
