
#ifndef __ELASTOS_DROID_JAVAPROXY_CREMOTEVIEWSADAPTERCONNECTIONNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CREMOTEVIEWSADAPTERCONNECTIONNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CRemoteViewsAdapterConnectionNative.h"
#include <jni.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CRemoteViewsAdapterConnectionNative)
{
public:
    CRemoteViewsAdapterConnectionNative();
    ~CRemoteViewsAdapterConnectionNative();

    CARAPI OnServiceConnected(
        /* [in] */ IBinder* pService);

    CARAPI OnServiceDisconnected();

    CARAPI ToString(
        /* [out] */ String* str);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CREMOTEVIEWSADAPTERCONNECTIONNATIVE_H__
