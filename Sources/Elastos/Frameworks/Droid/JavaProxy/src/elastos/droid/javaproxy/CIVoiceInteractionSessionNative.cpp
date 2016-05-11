
#include "elastos/droid/javaproxy/CIVoiceInteractionSessionNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Service::Voice::EIID_IIVoiceInteractionSession;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIVoiceInteractionSessionNative::TAG("CIVoiceInteractionSessionNative");

CAR_INTERFACE_IMPL_2(CIVoiceInteractionSessionNative, Object, IIVoiceInteractionSession, IBinder)

CAR_OBJECT_IMPL(CIVoiceInteractionSessionNative)

CIVoiceInteractionSessionNative::~CIVoiceInteractionSessionNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIVoiceInteractionSessionNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIVoiceInteractionSessionNative::TaskStarted(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 taskId)
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractionSessionNative::TaskFinished(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 taskId)
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractionSessionNative::CloseSystemDialogs()
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractionSessionNative::Destroy()
{
    assert(0);
    return NOERROR;
}

ECode CIVoiceInteractionSessionNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIVoiceInteractionSessionNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIVoiceInteractionSessionNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos
