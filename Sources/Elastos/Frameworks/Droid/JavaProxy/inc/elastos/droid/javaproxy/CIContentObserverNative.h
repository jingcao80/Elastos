
#ifndef __ELASTOS_DROID_JAVAPROXY_CICONTENTOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CICONTENTOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIContentObserverNative.h"
#include <jni.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIContentObserverNative)
{
public:
    ~CIContentObserverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnChange(
        /* [in] */ Boolean selfUpdate,
        /* [in] */ IUri* uri);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CICONTENTOBSERVERNATIVE_H__
