
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CSystemVibrator.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/CBinder.h"

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::CBinder;

namespace Elastos {
namespace Droid {
namespace Os {

const String CSystemVibrator::TAG("CSystemVibrator");

CAR_OBJECT_IMPL(CSystemVibrator)

CSystemVibrator::CSystemVibrator()
{
    CBinder::New((IBinder**)&mToken);
}

ECode CSystemVibrator::constructor()
{
    FAIL_RETURN(Vibrator::constructor())
    mService = IIVibratorService::Probe(ServiceManager::GetService(String("vibrator")).Get());
    return NOERROR;
}

ECode CSystemVibrator::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(Vibrator::constructor(context))
    mService = IIVibratorService::Probe(ServiceManager::GetService(String("vibrator")).Get());
    return NOERROR;
}

ECode CSystemVibrator::HasVibrator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mService == NULL) {
        Logger::W(TAG, "Failed to vibrate; no vibrator service.");
        *result = FALSE;
        return NOERROR;
    }

    ECode ec = mService->HasVibrator(result);
    if (FAILED(ec)) {
        *result = FALSE;
    }
    return ec;
}

ECode CSystemVibrator::Vibrate(
    /* [in] */ Int32 uid,
    /* [in] */ const String& opPkg,
    /* [in] */ Int64 milliseconds,
    /* [in] */ IAudioAttributes* attributes)
{
    if (mService == NULL) {
        Logger::W(TAG, "Failed to vibrate; no vibrator service.");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (FAILED(mService->Vibrate(uid, opPkg, milliseconds, UsageForAttributes(attributes), mToken))) {
         // Log.w(TAG, "Failed to vibrate.", e);
    }

    return NOERROR;
}

ECode CSystemVibrator::Vibrate(
    /* [in] */ Int32 uid,
    /* [in] */ const String& opPkg,
    /* [in] */ ArrayOf<Int64>* pattern,
    /* [in] */ Int32 repeat,
        /* [in] */ IAudioAttributes* attributes)
{
    if (mService == NULL) {
        Logger::W(TAG, "Failed to vibrate; no vibrator service.");
        return E_NULL_POINTER_EXCEPTION;
    }
    // catch this here because the server will do nothing.  pattern may
    // not be null, let that be checked, because the server will drop it
    // anyway
    if (pattern != NULL && repeat < pattern->GetLength()) {
        if(FAILED(mService->VibratePattern(uid, opPkg, pattern, repeat, UsageForAttributes(attributes), mToken))) {
            // Log.w(TAG, "Failed to vibrate.", e);
        }
        return NOERROR;
    }
    else {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
}

Int32 CSystemVibrator::UsageForAttributes(
    /* [in] */ IAudioAttributes* attributes)
{
    Int32 result;
    // if (attributes != NULL) {
    //     attributes->GetUsage(&result);
    // }
    // else {
    //     result = IAudioAttributes::USAGE_UNKNOWN;
    // }
    return result;
}

ECode CSystemVibrator::Cancel()
{
    if (mService == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (FAILED(mService->CancelVibrate(mToken))) {
        // Log.w(TAG, "Failed to cancel vibration.", e);
    }
    return NOERROR;
}


} // namespace Os
} // namespace Droid
} // namespace Elastos
