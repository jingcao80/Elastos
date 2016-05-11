
#include "elastos/droid/javaproxy/CIVoiceInteractorNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::App::EIID_IIVoiceInteractor;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIVoiceInteractorNative::TAG("CIVoiceInteractorNative");

CAR_INTERFACE_IMPL_2(CIVoiceInteractorNative, Object, IIVoiceInteractor, IBinder)

CAR_OBJECT_IMPL(CIVoiceInteractorNative)

CIVoiceInteractorNative::~CIVoiceInteractorNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIVoiceInteractorNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIVoiceInteractorNative::StartConfirmation(
    /* [in] */ const String& callingPackage,
    /* [in] */ IIVoiceInteractorCallback* clb,
    /* [in] */ ICharSequence* prompt,
    /* [in] */ IBundle* extras,
    /* [out] */ IIVoiceInteractorRequest** result)
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractorNative::StartCompleteVoice(
    /* [in] */ const String& callingPackage,
    /* [in] */ IIVoiceInteractorCallback* clb,
    /* [in] */ ICharSequence* message,
    /* [in] */ IBundle* extras,
    /* [out] */ IIVoiceInteractorRequest** result)
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractorNative::StartAbortVoice(
    /* [in] */ const String& callingPackage,
    /* [in] */ IIVoiceInteractorCallback* clb,
    /* [in] */ ICharSequence* message,
    /* [in] */ IBundle* extras,
    /* [out] */ IIVoiceInteractorRequest** result)
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractorNative::StartCommand(
    /* [in] */ const String& callingPackage,
    /* [in] */ IIVoiceInteractorCallback* clb,
    /* [in] */ const String& command,
    /* [in] */ IBundle* extras,
    /* [out] */ IIVoiceInteractorRequest** result)
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractorNative::SupportsCommands(
    /* [in] */ const String& callingPackage,
    /* [in] */ ArrayOf<String>* commands,
    /* [out, callee] */ ArrayOf<Boolean>** result)
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractorNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIVoiceInteractorNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIVoiceInteractorNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos
