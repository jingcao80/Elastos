
#ifndef __ELASTOS_DROID_JAVAPROXY_CREMOTEVIEWSADAPTERCONNECTIONNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CREMOTEVIEWSADAPTERCONNECTIONNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CRemoteViewsAdapterConnectionNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Internal::Widget::IIRemoteViewsAdapterConnection;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CRemoteViewsAdapterConnectionNative)
    , public Object
    , public IIRemoteViewsAdapterConnection
    , public IBinder
{
public:
    ~CRemoteViewsAdapterConnectionNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnServiceConnected(
        /* [in] */ IBinder* pService);

    CARAPI OnServiceDisconnected();

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

#endif // __ELASTOS_DROID_JAVAPROXY_CREMOTEVIEWSADAPTERCONNECTIONNATIVE_H__
