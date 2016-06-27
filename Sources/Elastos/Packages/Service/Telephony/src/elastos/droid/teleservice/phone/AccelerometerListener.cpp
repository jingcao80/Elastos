
#include "elastos/droid/teleservice/phone/AccelerometerListener.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::EIID_ISensorEventListener;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL(AccelerometerListener::MySensorEventListener, Object, ISensorEventListener)

ECode AccelerometerListener::MySensorEventListener::OnSensorChanged(
    /* [in] */ ISensorEvent* event)
{
    AutoPtr<ArrayOf<Float> > values;
    event->GetValues((ArrayOf<Float>**)&values);
    mHost->OnSensorEvent((*values)[0], (*values)[1], (*values)[2]);
    return NOERROR;
}

ECode AccelerometerListener::MySensorEventListener::OnAccuracyChanged(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 accuracy)
{
    // ignore
    return NOERROR;
}

AccelerometerListener::MyHandler::MyHandler(
    /* [in] */ AccelerometerListener* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode AccelerometerListener::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case ORIENTATION_CHANGED:
            {
                AutoLock syncLock(this);
                mHost->mOrientation = mHost->mPendingOrientation;
                if (DEBUG) {
                    StringBuilder sb;
                    sb += "orientation: ";
                    sb += mHost->mOrientation == ORIENTATION_HORIZONTAL ? "horizontal"
                            : (mHost->mOrientation == ORIENTATION_VERTICAL ? "vertical"
                            : "unknown");
                    Logger::D(TAG, sb.ToString());
                }
                mHost->mListener->OrientationChanged(mHost->mOrientation);
            }
            break;
    }
    return NOERROR;
}

const String AccelerometerListener::TAG("AccelerometerListener");
const Boolean AccelerometerListener::DEBUG = TRUE;
const Boolean AccelerometerListener::VDEBUG = FALSE;

CAR_INTERFACE_IMPL(AccelerometerListener, Object, IAccelerometerListener)

AccelerometerListener::AccelerometerListener(
    /* [in] */ IContext* context,
    /* [in] */ IAccelerometerListenerOrientationListener* listener)
    : mOrientation(0)
    , mPendingOrientation(0)
    , mListener(listener)
{
    mSensorListener = new MySensorEventListener(this);
    mHandler = new MyHandler(this);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::SENSOR_SERVICE, (IInterface**)&obj);
    mSensorManager = ISensorManager::Probe(obj);
    mSensorManager->GetDefaultSensor(ISensor::TYPE_ACCELEROMETER, (ISensor**)&mSensor);
}

ECode AccelerometerListener::Enable(
    /* [in] */ Boolean enable)
{
    if (DEBUG) {
        StringBuilder sb;
        sb += "enable(";
        sb += enable;
        sb += ")";
        Logger::D(TAG, sb.ToString());
    }

    {
        AutoLock syncLock(this);
        if (enable) {
            mOrientation = ORIENTATION_UNKNOWN;
            mPendingOrientation = ORIENTATION_UNKNOWN;
            Boolean tmp = FALSE;
            mSensorManager->RegisterListener(mSensorListener, mSensor,
                    ISensorManager::SENSOR_DELAY_NORMAL, &tmp);
        }
        else {
            mSensorManager->UnregisterListener(mSensorListener);
            mHandler->RemoveMessages(ORIENTATION_CHANGED);
        }
    }
    return NOERROR;
}

void AccelerometerListener::SetOrientation(
    /* [in] */ Int32 orientation)
{
    {
        AutoLock syncLock(this);
        if (mPendingOrientation == orientation) {
            // Pending orientation has not changed, so do nothing.
            return;
        }

        // Cancel any pending messages.
        // We will either start a new timer or cancel alltogether
        // if the orientation has not changed.
        mHandler->RemoveMessages(ORIENTATION_CHANGED);

        if (mOrientation != orientation) {
            // Set timer to send an event if the orientation has changed since its
            // previously reported value.
            mPendingOrientation = orientation;
            AutoPtr<IMessage> m;
            mHandler->ObtainMessage(ORIENTATION_CHANGED, (IMessage**)&m);
            // set delay to our debounce timeout
            Int32 delay = (orientation == ORIENTATION_VERTICAL ? VERTICAL_DEBOUNCE : HORIZONTAL_DEBOUNCE);
            Boolean tmp = FALSE;
            mHandler->SendMessageDelayed(m, delay, &tmp);
        }
        else {
            // no message is pending
            mPendingOrientation = ORIENTATION_UNKNOWN;
        }
    }
    return;
}

void AccelerometerListener::OnSensorEvent(
    /* [in] */ Double x,
    /* [in] */ Double y,
    /* [in] */ Double z)
{
    if (VDEBUG) {
        StringBuilder sb;
        sb += "onSensorEvent(";
        sb += x;
        sb += ", ";
        sb += y;
        sb += ", ";
        sb += z;
        sb += ")";
        Logger::D(TAG, sb.ToString());
    }

    // If some values are exactly zero, then likely the sensor is not powered up yet.
    // ignore these events to avoid false horizontal positives.
    if (x == 0.0 || y == 0.0 || z == 0.0) return;

    // magnitude of the acceleration vector projected onto XY plane
    Double xy = Elastos::Core::Math::Sqrt(x*x + y*y);
    // compute the vertical angle
    Double angle = Elastos::Core::Math::Atan2(xy, z);
    // convert to degrees
    angle = angle * 180.0 / Elastos::Core::Math::PI;
    Int32 orientation = (angle >  VERTICAL_ANGLE ? ORIENTATION_VERTICAL : ORIENTATION_HORIZONTAL);
    if (VDEBUG) {
        StringBuilder sb;
        sb += "angle: ";
        sb += angle;
        sb += " orientation: ";
        sb += orientation;
        Logger::D(TAG, "angle: %f orientation: %d", angle, orientation);
    }
    SetOrientation(orientation);
    return;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos