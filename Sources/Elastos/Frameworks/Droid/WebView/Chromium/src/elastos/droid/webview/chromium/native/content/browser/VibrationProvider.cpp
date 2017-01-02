//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
