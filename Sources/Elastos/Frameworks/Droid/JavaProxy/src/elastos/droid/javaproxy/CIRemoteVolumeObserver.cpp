
#include "elastos/droid/javaproxy/CIRemoteVolumeObserver.h"
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Media::EIID_IIRemoteVolumeObserver;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIRemoteVolumeObserver::TAG("CIRemoteVolumeObserver");

CAR_INTERFACE_IMPL_2(CIRemoteVolumeObserver, Object, IIRemoteVolumeObserver, IBinder)

CAR_OBJECT_IMPL(CIRemoteVolumeObserver)

CIRemoteVolumeObserver::~CIRemoteVolumeObserver()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIRemoteVolumeObserver::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIRemoteVolumeObserver::DispatchRemoteVolumeUpdate(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 value)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteVolumeObserver");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteVolumeObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchRemoteVolumeUpdate", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchRemoteVolumeUpdate %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, direction, value);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchRemoteVolumeUpdate %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteVolumeObserver::ToString(
    /* [out] */ String* str)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);
    return NOERROR;
}

}
}
}

