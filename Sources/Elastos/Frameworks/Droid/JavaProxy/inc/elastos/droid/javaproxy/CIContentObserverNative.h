
#ifndef __ELASTOS_DROID_JAVAPROXY_CICONTENTOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CICONTENTOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIContentObserverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Database::IIContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIContentObserverNative)
    , public Object
    , public IIContentObserver
    , public IBinder
{
public:
    ~CIContentObserverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnChange(
        /* [in] */ Boolean selfUpdate,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 userId);

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
