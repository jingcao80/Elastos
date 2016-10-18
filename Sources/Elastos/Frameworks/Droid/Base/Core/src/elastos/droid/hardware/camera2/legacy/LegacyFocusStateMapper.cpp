
#include "elastos/droid/hardware/camera2/legacy/LegacyFocusStateMapper.h"
#include "elastos/droid/hardware/camera2/utils/ParamsUtils.h"
#include "elastos/droid/hardware/camera2/CaptureRequest.h"
#include "elastos/droid/hardware/camera2/CaptureResult.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Hardware::Camera2::Utils::ParamsUtils;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Objects;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(LegacyFocusStateMapper, Object, ILegacyFocusStateMapper)

const String LegacyFocusStateMapper::TAG("LegacyFocusStateMapper");
const Boolean LegacyFocusStateMapper::VERBOSE = FALSE;//Log.isLoggable(TAG, Log.VERBOSE);

CAR_INTERFACE_IMPL(LegacyFocusStateMapper::MyMoveCallback, Object, IAutoFocusMoveCallback)

LegacyFocusStateMapper::MyMoveCallback::MyMoveCallback(
    /* [in] */ LegacyFocusStateMapper* host,
    /* [in] */ Int32 currentAfRun,
    /* [in] */ const String& afMode)
    : mHost(host)
    , mCurrentAfRun(currentAfRun)
    , mAfMode(afMode)
{
}

LegacyFocusStateMapper::MyMoveCallback::OnAutoFocusMoving(
    /* [in] */ Boolean start,
    /* [in] */ IHardwareCamera* camera)
{
    Object& _lock = mHost->mLock;
    {    AutoLock syncLock(_lock);
        Int32 latestAfRun = mHost->mAfRun;

        if (mHost->VERBOSE) {
            Logger::V(TAG,
                    "onAutoFocusMoving - start %d latest AF run %d, last AF run %d",
                    start, latestAfRun, mCurrentAfRun);
        }

        if (mCurrentAfRun != latestAfRun) {
            Logger::D(TAG,
                    "onAutoFocusMoving - ignoring move callbacks from old af run %d"
                            ,mCurrentAfRun);
            return NOERROR;
        }

        Int32 newAfState = start ?
                ICameraMetadata::CONTROL_AF_STATE_PASSIVE_SCAN :
                ICameraMetadata::CONTROL_AF_STATE_PASSIVE_FOCUSED;
        // We never send CONTROL_AF_STATE_PASSIVE_UNFOCUSED

        if (mAfMode == IParameters::FOCUS_MODE_CONTINUOUS_PICTURE ||
            mAfMode == IParameters::FOCUS_MODE_CONTINUOUS_VIDEO) {
            // This callback should never be sent in any other AF mode
        }
        else {
            Logger::W(TAG, "onAutoFocus - got unexpected onAutoFocus in mode %s"
                    ,mAfMode.string());
        }

        mHost->mAfState = newAfState;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(LegacyFocusStateMapper::MyFocusCallback, Object, IAutoFocusCallback)

LegacyFocusStateMapper::MyFocusCallback::MyFocusCallback(
    /* [in] */ LegacyFocusStateMapper* host,
    /* [in] */ Int32 currentAfRun,
    /* [in] */ const String& afMode)
    : mHost(host)
    , mCurrentAfRun(currentAfRun)
    , mAfMode(afMode)
{
}

ECode LegacyFocusStateMapper::MyFocusCallback::OnAutoFocus(
    /* [in] */ Boolean success,
    /* [in] */ IHardwareCamera* camera)
{
    Object& _lock = mHost->mLock;
    {    AutoLock syncLock(_lock);
        Int32 latestAfRun = mHost->mAfRun;

        if (mHost->VERBOSE) {
            Logger::V(TAG, "onAutoFocus - success %d latest AF run %d, last AF run %d",
                    success, latestAfRun, mCurrentAfRun);
        }

        // Ignore old auto-focus results, since another trigger was requested
        if (latestAfRun != mCurrentAfRun) {
            Logger::D(TAG, "onAutoFocus - ignoring AF callback (old run %d, new run %d)",
                    mCurrentAfRun, latestAfRun);

            return NOERROR;
        }

        Int32 newAfState = success ?
                ICameraMetadata::CONTROL_AF_STATE_FOCUSED_LOCKED :
                ICameraMetadata::CONTROL_AF_STATE_NOT_FOCUSED_LOCKED;

        if (mAfMode == IParameters::FOCUS_MODE_AUTO ||
            mAfMode == IParameters::FOCUS_MODE_CONTINUOUS_PICTURE ||
            mAfMode == IParameters::FOCUS_MODE_CONTINUOUS_VIDEO ||
            mAfMode == IParameters::FOCUS_MODE_MACRO) {
            // This callback should never be sent in any other AF mode
        }
        else {
            Logger::W(TAG, "onAutoFocus - got unexpected onAutoFocus in mode %s"
                    ,mAfMode.string());
        }

        mHost->mAfState = newAfState;
    }
    return NOERROR;
}

LegacyFocusStateMapper::LegacyFocusStateMapper()
    : mAfStatePrevious(ICameraMetadata::CONTROL_AF_STATE_INACTIVE)
    , mAfModePrevious(NULL)
    , mAfRun(0)
    , mAfState(ICameraMetadata::CONTROL_AF_STATE_INACTIVE)
{
}

ECode LegacyFocusStateMapper::constructor()
{
    return NOERROR;
}

ECode LegacyFocusStateMapper::constructor(
    /* [in] */ IHardwareCamera* camera)
{
    FAIL_RETURN(Preconditions::CheckNotNull(camera, String("camera must not be null")))
    mCamera = camera;
    return NOERROR;
}

ECode LegacyFocusStateMapper::ProcessRequestTriggers(
    /* [in] */ ICaptureRequest* captureRequest,
    /* [in] */ IParameters* parameters)
{
    FAIL_RETURN(Preconditions::CheckNotNull(captureRequest, String("captureRequest must not be null")))

    /*
     * control.afTrigger
     */
    AutoPtr<IInteger32> intObj = CoreUtils::Convert(ICameraMetadata::CONTROL_AF_TRIGGER_IDLE);
    AutoPtr<IInterface> obj;
    ParamsUtils::GetOrDefault(captureRequest, CaptureRequest::CONTROL_AF_TRIGGER,
            TO_IINTERFACE(intObj), (IInterface**)&obj);
    AutoPtr<IInteger32> afTriggerObj = IInteger32::Probe(obj);
    Int32 afTrigger;
    afTriggerObj->GetValue(&afTrigger);

    String afMode;
    parameters->GetFocusMode(&afMode);
    if (mAfModePrevious != afMode) {
        if (VERBOSE) {
            Logger::V(TAG, "processRequestTriggers - AF mode switched from %s to %s",
                    mAfModePrevious.string(), afMode.string());
        }

        // Switching modes always goes back to INACTIVE; ignore callbacks from previous modes

        {    AutoLock syncLock(mLock);
            ++mAfRun;
            mAfState = ICameraMetadata::CONTROL_AF_STATE_INACTIVE;
        }
        mCamera->CancelAutoFocus();
    }

    mAfModePrevious = afMode;

    // Passive AF Scanning
    {
        Int32 currentAfRun = 0;

        {    AutoLock syncLock(mLock);
            currentAfRun = mAfRun;
        }

        AutoPtr<IAutoFocusMoveCallback> afMoveCallback = new MyMoveCallback(this, currentAfRun, afMode);
        // Only set move callback if we can call autofocus.
        if (afMode == IParameters::FOCUS_MODE_AUTO ||
            afMode == IParameters::FOCUS_MODE_MACRO ||
            afMode == IParameters::FOCUS_MODE_CONTINUOUS_PICTURE ||
            afMode == IParameters::FOCUS_MODE_CONTINUOUS_VIDEO) {
            mCamera->SetAutoFocusMoveCallback(afMoveCallback);
        }
    }

    // AF Locking
    switch (afTrigger) {
        case ICameraMetadata::CONTROL_AF_TRIGGER_START:
        {
            Int32 afStateAfterStart;

            if (afMode == IParameters::FOCUS_MODE_AUTO ||
                afMode == IParameters::FOCUS_MODE_MACRO) {
                afStateAfterStart = ICameraMetadata::CONTROL_AF_STATE_ACTIVE_SCAN;
                break;
            }
            else if (afMode == IParameters::FOCUS_MODE_CONTINUOUS_PICTURE ||
                afMode == IParameters::FOCUS_MODE_CONTINUOUS_VIDEO) {
                afStateAfterStart = ICameraMetadata::CONTROL_AF_STATE_PASSIVE_SCAN;
                break;
            }
            else {
                // EDOF, INFINITY
                afStateAfterStart = ICameraMetadata::CONTROL_AF_STATE_INACTIVE;
            }

            Int32 currentAfRun;
            {    AutoLock syncLock(mLock);
                currentAfRun = ++mAfRun;
                mAfState = afStateAfterStart;
            }

            if (VERBOSE) {
                Logger::V(TAG, "processRequestTriggers - got AF_TRIGGER_START, "
                        "new AF run is %d", currentAfRun);
            }

            // Avoid calling autofocus unless we are in a state that supports calling this.
            if (afStateAfterStart == ICameraMetadata::CONTROL_AF_STATE_INACTIVE) {
                break;
            }

            AutoPtr<MyFocusCallback> call = new MyFocusCallback(this, currentAfRun, afMode);
            mCamera->AutoFocus(call);
            break;
        }
        case ICameraMetadata::CONTROL_AF_TRIGGER_CANCEL:
        {
            {    AutoLock syncLock(mLock);
                Int32 updatedAfRun;

                {    AutoLock syncLock(mLock);
                    updatedAfRun = ++mAfRun;
                    mAfState = ICameraMetadata::CONTROL_AF_STATE_INACTIVE;
                }

                mCamera->CancelAutoFocus();

                if (VERBOSE) {
                    Logger::V(TAG, "processRequestTriggers - got AF_TRIGGER_CANCEL, %dnew AF run is ",
                            updatedAfRun);
                }
            }

            break;
        }
        case ICameraMetadata::CONTROL_AF_TRIGGER_IDLE:
            // No action necessary. The callbacks will handle transitions.
            break;
        default:
            Logger::W(TAG, "processRequestTriggers - ignoring unknown control.afTrigger = %d"
                    ,afTrigger);
    }
    return NOERROR;
}

ECode LegacyFocusStateMapper::MapResultTriggers(
    /* [in] */ ICameraMetadataNative* result)
{
    FAIL_RETURN(Preconditions::CheckNotNull(result, String("result must not be null")))

    Int32 newAfState = 0;
    {    AutoLock syncLock(mLock);
        newAfState = mAfState;
    }

    if (VERBOSE && newAfState != mAfStatePrevious) {
        Logger::V(TAG, "mapResultTriggers - afState changed from %s to %s",
                AfStateToString(mAfStatePrevious).string(), AfStateToString(newAfState).string());
    }

    AutoPtr<IInteger32> obj = CoreUtils::Convert(newAfState);
    result->Set(CaptureResult::CONTROL_AF_STATE, TO_IINTERFACE(obj));

    mAfStatePrevious = newAfState;
    return NOERROR;
}

String LegacyFocusStateMapper::AfStateToString(
    /* [in] */ Int32 afState)
{
    switch (afState) {
        case ICameraMetadata::CONTROL_AF_STATE_ACTIVE_SCAN:
            return String("ACTIVE_SCAN");
        case ICameraMetadata::CONTROL_AF_STATE_FOCUSED_LOCKED:
            return String("FOCUSED_LOCKED");
        case ICameraMetadata::CONTROL_AF_STATE_INACTIVE:
            return String("INACTIVE");
        case ICameraMetadata::CONTROL_AF_STATE_NOT_FOCUSED_LOCKED:
            return String("NOT_FOCUSED_LOCKED");
        case ICameraMetadata::CONTROL_AF_STATE_PASSIVE_FOCUSED:
            return String("PASSIVE_FOCUSED");
        case ICameraMetadata::CONTROL_AF_STATE_PASSIVE_SCAN:
            return String("PASSIVE_SCAN");
        case ICameraMetadata::CONTROL_AF_STATE_PASSIVE_UNFOCUSED:
            return String("PASSIVE_UNFOCUSED");
        default :
            StringBuilder sb;
            sb += "UNKNOWN(";
            sb += afState;
            sb += ")";
            return sb.ToString();
    }
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
