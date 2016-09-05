
#ifndef __ELASTOS_DROID_JAVAPROXY_CIMEDIAROUTERCLIENTNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIMEDIAROUTERCLIENTNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIMediaRouterClientNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Media::IIMediaRouterClient;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIMediaRouterClientNative)
    , public Object
    , public IIMediaRouterClient
    , public IBinder
{
public:
    ~CIMediaRouterClientNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnStateChanged();

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CIMEDIAROUTERCLIENTNATIVE_H__
