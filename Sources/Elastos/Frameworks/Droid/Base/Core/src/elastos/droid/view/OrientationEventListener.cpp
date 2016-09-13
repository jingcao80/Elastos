
#include "Elastos.Droid.Content.h"
#include "elastos/droid/view/OrientationEventListener.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::EIID_ISensorEventListener;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {

const String OrientationEventListener::TAG("OrientationEventListener");
const Boolean OrientationEventListener::DEBUG = FALSE;
const Boolean OrientationEventListener::localLOGV = FALSE;

//===========================================================================
// OrientationEventListener::SensorEventListenerImpl
//===========================================================================
CAR_INTERFACE_IMPL(OrientationEventListener::SensorEventListenerImpl, Object, ISensorEventListener)

OrientationEventListener::SensorEventListenerImpl::SensorEventListenerImpl(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{
}

ECode OrientationEventListener::SensorEventListenerImpl::OnSensorChanged(
    /* [in] */ ISensorEvent* event)
{
    AutoPtr<IOrientationEventListener> host;
    mWeakHost->Resolve(EIID_IOrientationEventListener, (IInterface**)&host);
    if (host == NULL) {
        return NOERROR;
    }

    OrientationEventListener* mHost = (OrientationEventListener*)host.Get();

    AutoPtr<ArrayOf<Float> > values;
    event->GetValues((ArrayOf<Float>**)&values);
    Int32 orientation = IOrientationEventListener::ORIENTATION_UNKNOWN;
    Float X = -(*values)[_DATA_X];
    Float Y = -(*values)[_DATA_Y];
    Float Z = -(*values)[_DATA_Z];
    Float magnitude = X*X + Y*Y;
    // Don't trust the angle if the magnitude is small compared to the y value
    if (magnitude * 4 >= Z*Z) {
        Float OneEightyOverPi = 57.29577957855f;
        Float angle = (Float)Elastos::Core::Math::Atan2(-Y, X) * OneEightyOverPi;
        orientation = 90 - (Int32)Elastos::Core::Math::Round(angle);
        // normalize to 0 - 359 range
        while (orientation >= 360) {
            orientation -= 360;
        }
        while (orientation < 0) {
            orientation += 360;
        }
    }
    if (mHost->mOldListener != NULL) {
        mHost->mOldListener->OnSensorChanged(ISensor::TYPE_ACCELEROMETER, values);
    }
    if (orientation != mHost->mOrientation) {
        mHost->mOrientation = orientation;
        mHost->OnOrientationChanged(orientation);
    }
    return NOERROR;
}

ECode OrientationEventListener::SensorEventListenerImpl::OnAccuracyChanged(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 accuracy)
{
    return NOERROR;
}

//===========================================================================
// OrientationEventListener
//===========================================================================
CAR_INTERFACE_IMPL(OrientationEventListener, Object, IOrientationEventListener)

OrientationEventListener::OrientationEventListener()
    : mOrientation(ORIENTATION_UNKNOWN)
    , mEnabled(FALSE)
    , mRate(0)
{}

OrientationEventListener::~OrientationEventListener()
{
    Disable();
}

ECode OrientationEventListener::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, ISensorManager::SENSOR_DELAY_NORMAL);
}

ECode OrientationEventListener::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 rate)
{
    AutoPtr<IInterface> tmp;
    context->GetSystemService(IContext::SENSOR_SERVICE, (IInterface**)&tmp);
    mSensorManager = ISensorManager::Probe(tmp);
    mRate = rate;
    mSensorManager->GetDefaultSensor(ISensor::TYPE_ACCELEROMETER, (ISensor**)&mSensor);
    if (mSensor != NULL) {
        // Create listener only if sensors do exist
        AutoPtr<IWeakReference> wr;
        GetWeakReference((IWeakReference**)&wr);
        mSensorEventListener = new SensorEventListenerImpl(wr);
    }
    return NOERROR;
}

ECode OrientationEventListener::RegisterListener(
    /* [in] */ ISensorListener* lis)
{
    mOldListener = lis;
    return NOERROR;
}

ECode OrientationEventListener::Enable()
{
    if (mSensor == NULL) {
        Slogger::W(TAG, "Cannot detect sensors. Not enabled");
        return NOERROR;
    }
    if (mEnabled == FALSE) {
        if (localLOGV) {
            Slogger::W(TAG, "OrientationEventListener enabled");
        }
        Boolean r;
        mSensorManager->RegisterListener(mSensorEventListener, mSensor, mRate, &r);
        mEnabled = TRUE;
    }
    return NOERROR;
}

ECode OrientationEventListener::Disable()
{
    if (mSensor == NULL) {
        Slogger::W(TAG, "Cannot detect sensors. Not disable");
        return NOERROR;
    }
    if (mEnabled == TRUE) {
        if (localLOGV) {
            Slogger::W(TAG, "OrientationEventListener disabled");
        }
        mSensorManager->UnregisterListener(mSensorEventListener);
        mEnabled = FALSE;
    }
    return NOERROR;
}

ECode OrientationEventListener::CanDetectOrientation(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSensor != NULL;
    return NOERROR;
}

ECode OrientationEventListener::OnOrientationChanged(
    /* [in] */ Int32 orientation)
{
    return NOERROR;
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
