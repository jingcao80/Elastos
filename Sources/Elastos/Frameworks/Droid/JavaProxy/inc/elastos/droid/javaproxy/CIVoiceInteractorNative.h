
#ifndef __ELASTOS_DROID_JAVAPROXY_CIVOICEINTERACTORNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIVOICEINTERACTORNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIVoiceInteractorNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Internal::App::IIVoiceInteractor;
using Elastos::Droid::Internal::App::IIVoiceInteractorCallback;
using Elastos::Droid::Internal::App::IIVoiceInteractorRequest;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIVoiceInteractorNative)
    , public Object
    , public IIVoiceInteractor
    , public IBinder
{
public:
    ~CIVoiceInteractorNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI StartConfirmation(
        /* [in] */ const String& callingPackage,
        /* [in] */ IIVoiceInteractorCallback* clb,
        /* [in] */ ICharSequence* prompt,
        /* [in] */ IBundle* extras,
        /* [out] */ IIVoiceInteractorRequest** result);

    CARAPI StartCompleteVoice(
        /* [in] */ const String& callingPackage,
        /* [in] */ IIVoiceInteractorCallback* clb,
        /* [in] */ ICharSequence* message,
        /* [in] */ IBundle* extras,
        /* [out] */ IIVoiceInteractorRequest** result);

    CARAPI StartAbortVoice(
        /* [in] */ const String& callingPackage,
        /* [in] */ IIVoiceInteractorCallback* clb,
        /* [in] */ ICharSequence* message,
        /* [in] */ IBundle* extras,
        /* [out] */ IIVoiceInteractorRequest** result);

    CARAPI StartCommand(
        /* [in] */ const String& callingPackage,
        /* [in] */ IIVoiceInteractorCallback* clb,
        /* [in] */ const String& command,
        /* [in] */ IBundle* extras,
        /* [out] */ IIVoiceInteractorRequest** result);

    CARAPI SupportsCommands(
        /* [in] */ const String& callingPackage,
        /* [in] */ ArrayOf<String>* commands,
        /* [out, callee] */ ArrayOf<Boolean>** result);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIVOICEINTERACTORNATIVE_H__
