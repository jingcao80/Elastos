
#ifndef __ELASTOS_DROID_JAVAPROXY_CMESSENGERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CMESSENGERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CMessengerNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IIMessenger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CMessengerNative)
    , public Object
    , public IIMessenger
    , public IBinder
{
public:
    ~CMessengerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI Send(
        /* [in] */ IMessage* message);

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

#endif //__ELASTOS_DROID_JAVAPROXY_CMESSENGERNATIVE_H__
