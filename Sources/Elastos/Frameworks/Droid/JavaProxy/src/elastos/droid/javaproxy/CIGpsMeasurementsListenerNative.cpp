
#include "elastos/droid/javaproxy/CIGpsMeasurementsListenerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Location::EIID_IIGpsMeasurementsListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIGpsMeasurementsListenerNative::TAG("CIGpsMeasurementsListenerNative");

CAR_INTERFACE_IMPL_2(CIGpsMeasurementsListenerNative, Object, IIGpsMeasurementsListener, IBinder)

CAR_OBJECT_IMPL(CIGpsMeasurementsListenerNative)

CIGpsMeasurementsListenerNative::~CIGpsMeasurementsListenerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIGpsMeasurementsListenerNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIGpsMeasurementsListenerNative::OnGpsMeasurementsReceived(
    /* [in] */ IGpsMeasurementsEvent* event)
{
    assert(0);
    return NOERROR;
}

ECode CIGpsMeasurementsListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIGpsMeasurementsListenerNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIGpsMeasurementsListenerNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos
