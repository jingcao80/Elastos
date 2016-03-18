
#ifndef __ELASTOS_DROID_JAVAPROXY_CISERVICECONNECTIONNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CISERVICECONNECTIONNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIServiceConnectionNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::App::IIServiceConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIServiceConnectionNative)
    , public Object
    , public IIServiceConnection
    , public IBinder
{
public:
    ~CIServiceConnectionNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI Connected(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* service);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CISERVICECONNECTIONNATIVE_H__
