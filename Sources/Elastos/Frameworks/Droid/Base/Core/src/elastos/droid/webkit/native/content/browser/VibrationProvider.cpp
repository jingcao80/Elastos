
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/native/content/browser/VibrationProvider.h"
#include "elastos/droid/webkit/native/content/api/VibrationProvider_dec.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//=====================================================================
//                          VibrationProvider
//=====================================================================
const String VibrationProvider::TAG("VibrationProvider");

VibrationProvider::VibrationProvider(
    /* [in] */ IContext* context)
{
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&mAudioManager);
    context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&mVibrator);
    assert(0);
    // TODO
    // mHasVibratePermission = context.checkCallingOrSelfPermission(
    //         android.Manifest.permission.VIBRATE) == PackageManager.PERMISSION_GRANTED;
    if (!mHasVibratePermission) {
        Slogger::W(TAG, "Failed to use vibrate API, requires VIBRATE permission.");
    }
}

AutoPtr<VibrationProvider> VibrationProvider::Create(
    /* [in] */ IContext* context)
{
    return new VibrationProvider(context);
}

ECode VibrationProvider::Vibrate(
    /* [in] */ Int64 milliseconds)
{
    Int32 mode;
    mAudioManager->GetRingerMode(&mode);
    if (mode != IAudioManager::RINGER_MODE_SILENT &&
            mHasVibratePermission) {
        mVibrator->Vibrate(milliseconds);
    }

    return NOERROR;
}

ECode VibrationProvider::CancelVibration()
{
    if (mHasVibratePermission) mVibrator->Cancel();
    return NOERROR;
}

AutoPtr<IInterface> VibrationProvider::Create(
    /* [in] */ IInterface* context)
{
    AutoPtr<IContext> c = IContext::Probe(context);
    return TO_IINTERFACE(Create(c));
}

void VibrationProvider::Vibrate(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 milliseconds)
{
    AutoPtr<VibrationProvider> mObj = (VibrationProvider*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Slogger::E(TAG, "VibrationProvider::Vibrate, mObj is NULL");
        return;
    }
    mObj->Vibrate(milliseconds);
}

void VibrationProvider::CancelVibration(
    /* [in] */ IInterface* obj)
{
    AutoPtr<VibrationProvider> mObj = (VibrationProvider*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Slogger::E(TAG, "VibrationProvider::CancelVibration, mObj is NULL");
        return;
    }
    mObj->CancelVibration();
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
