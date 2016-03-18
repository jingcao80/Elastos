
#ifndef __ELASTOS_DROID_JAVAPROXY_CICANCELLATIONSIGNALNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CICANCELLATIONSIGNALNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CICancellationSignalNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IICancellationSignal;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

class CContentProviderNative;

CarClass(CICancellationSignalNative)
    , public Object
    , public IICancellationSignal
    , public IBinder
{
    friend CContentProviderNative;
public:
    ~CICancellationSignalNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI Cancel();

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

#endif // __ELASTOS_DROID_JAVAPROXY_CICANCELLATIONSIGNALNATIVE_H__
