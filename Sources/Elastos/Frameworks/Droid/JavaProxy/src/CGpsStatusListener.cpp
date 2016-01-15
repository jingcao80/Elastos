
#include "CGpsStatusListener.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Droid::JavaProxy::Util;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CGpsStatusListener::TAG("CGpsStatusListener");

CGpsStatusListener::CGpsStatusListener(){

}

CGpsStatusListener::~CGpsStatusListener(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CGpsStatusListener::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);
    return NOERROR;
}

ECode CGpsStatusListener::OnGpsStarted()
{
    LOGGERD(TAG, String("CGpsStatusListener E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    return E_NOT_IMPLEMENTED;
}

ECode CGpsStatusListener::OnGpsStopped()
{
    LOGGERD(TAG, String("CGpsStatusListener E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    return E_NOT_IMPLEMENTED;
}

ECode CGpsStatusListener::OnFirstFix(
    /* [in] */ Int32 ttff)
{
    LOGGERD(TAG, String("CGpsStatusListener E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    return E_NOT_IMPLEMENTED;
}

ECode CGpsStatusListener::OnSvStatusChanged(
    /* [in] */ Int32 svCount,
    /* [in] */ ArrayOf<Int32>* prns,
    /* [in] */ ArrayOf<Float>* snrs,
    /* [in] */ ArrayOf<Float>* elevations,
    /* [in] */ ArrayOf<Float>* azimuths,
    /* [in] */ Int32 ephemerisMask,
    /* [in] */ Int32 almanacMask,
    /* [in] */ Int32 usedInFixMask)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);


    LOGGERD(TAG, String("CGpsStatusListener E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    return E_NOT_IMPLEMENTED;
}

ECode CGpsStatusListener::OnNmeaReceived(
    /* [in] */ Int64 timestamp,
    /* [in] */ const String& nmea)
{
    LOGGERD(TAG, String("CGpsStatusListener E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    return E_NOT_IMPLEMENTED;
}

}
}
}

