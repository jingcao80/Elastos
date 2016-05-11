
#ifndef __ELASTOS_DROID_JAVAPROXY_CIVOICEINTERACTIONSESSIONNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIVOICEINTERACTIONSESSIONNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIVoiceInteractionSessionNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Service::Voice::IIVoiceInteractionSession;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIVoiceInteractionSessionNative)
    , public Object
    , public IIVoiceInteractionSession
    , public IBinder
{
public:
    ~CIVoiceInteractionSessionNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI TaskStarted(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 taskId);

    CARAPI TaskFinished(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 taskId);

    CARAPI CloseSystemDialogs();

    CARAPI Destroy();

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIVOICEINTERACTIONSESSIONNATIVE_H__
