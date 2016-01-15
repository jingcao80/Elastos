
#ifndef __CLOCATIONLISTENER_H__
#define __CLOCATIONLISTENER_H__

#include "_CLocationListener.h"
#include <jni.h>

using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CLocationListener)
{
public:
    ~CLocationListener();

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

#endif // __CLOCATIONLISTENER_H__
