
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/VibrationProvider.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/VibrationProvider_dec.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Manifest;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                          VibrationProvider
//=====================================================================
const String VibrationProvider::TAG("VibrationProvider");

VibrationProvider::VibrationProvider(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> amObj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&amObj);
    mAudioManager = IAudioManager::Probe(amObj);
    AutoPtr<IInterface> vObj;
    context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&vObj);
    mVibrator = IVibrator::Probe(vObj);

    Int32 result;
    context->CheckCallingOrSelfPermission(Manifest::permission::VIBRATE, &result);
    mHasVibratePermission = result == IPackageManager::PERMISSION_GRANTED;
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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
