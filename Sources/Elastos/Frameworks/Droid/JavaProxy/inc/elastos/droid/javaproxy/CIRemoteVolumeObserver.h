
#ifndef __ELASTOS_DROID_JAVAPROXY_CIREMOTEVOLUMEOBSERVER_H__
#define __ELASTOS_DROID_JAVAPROXY_CIREMOTEVOLUMEOBSERVER_H__

#include "_Elastos_Droid_JavaProxy_CIRemoteVolumeObserver.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Media::IIRemoteVolumeObserver;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRemoteVolumeObserver)
    , public Object
    , public IIRemoteVolumeObserver
    , public IBinder
{
public:
    ~CIRemoteVolumeObserver();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI DispatchRemoteVolumeUpdate(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 value);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIREMOTEVOLUMEOBSERVER_H__
