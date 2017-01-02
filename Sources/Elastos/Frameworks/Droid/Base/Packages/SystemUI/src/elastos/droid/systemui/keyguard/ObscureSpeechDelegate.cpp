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

#include "elastos/droid/systemui/keyguard/ObscureSpeechDelegate.h"
#include <elastos/core/CoreUtils.h>
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "R.h"

using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

Boolean ObscureSpeechDelegate::sAnnouncedHeadset = FALSE;

ObscureSpeechDelegate::ObscureSpeechDelegate(
    /* [in] */ IContext* context)
{
    context->GetContentResolver((IContentResolver**)&mContentResolver);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);
}

ECode ObscureSpeechDelegate::SendAccessibilityEvent(
    /* [in] */ IView* host,
    /* [in] */ Int32 eventType)
{
    AccessibilityDelegate::SendAccessibilityEvent(host, eventType);

    // Play the "headset required" announcement the first time the user
    // places accessibility focus on a key.
    if ((eventType == IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED)
            && !sAnnouncedHeadset && ShouldObscureSpeech()) {
        sAnnouncedHeadset = TRUE;

        AutoPtr<IContext> context;
        host->GetContext((IContext**)&context);
        String passwords;
        assert(0);
        //context->GetString(R::string::keyboard_headset_required_to_hear_password, &passwords);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(passwords);
        host->AnnounceForAccessibility(cchar);
    }
    return NOERROR;
}

ECode ObscureSpeechDelegate::OnPopulateAccessibilityEvent(
    /* [in] */ IView* host,
    /* [in] */ IAccessibilityEvent* event)
{
    AccessibilityDelegate::OnPopulateAccessibilityEvent(host, event);

    Int32 type;
    event->GetEventType(&type);
    if ((type != IAccessibilityEvent::TYPE_ANNOUNCEMENT)
            && ShouldObscureSpeech()) {
        AutoPtr<IList> list;
        IAccessibilityRecord::Probe(event)->GetText((IList**)&list);
        list->Clear();

        AutoPtr<IContext> context;
        host->GetContext((IContext**)&context);
        String headset;
        assert(0);
        //context->GetString(R::string::keyboard_password_character_no_headset, &headset);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(headset);
        IAccessibilityRecord::Probe(event)->SetContentDescription(cchar);
    }
    return NOERROR;
}

ECode ObscureSpeechDelegate::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IView* host,
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AccessibilityDelegate::OnInitializeAccessibilityNodeInfo(host, info);

    if (ShouldObscureSpeech()) {
        AutoPtr<IContext> ctx;
        host->GetContext((IContext**)&ctx);
        info->SetText(NULL);
        String headset;
        assert(0);
        //ctx->GetString(R::string::keyboard_password_character_no_headset, &headset);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(headset);
        info->SetContentDescription(cchar);
    }
    return NOERROR;
}

Boolean ObscureSpeechDelegate::ShouldObscureSpeech()
{
    // The user can optionally force speaking passwords.
    AutoPtr<ISettingsSecure> helper;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&helper);
    Int32 password;
    helper->GetInt32(mContentResolver, ISettingsSecure::ACCESSIBILITY_SPEAK_PASSWORD, 0, &password);
    if (password != 0) {
        return FALSE;
    }

    // Always speak if the user is listening through headphones.
    Boolean res1, res2;
    if ((mAudioManager->IsWiredHeadsetOn(&res1), res1) ||
        (mAudioManager->IsBluetoothA2dpOn(&res2), res2)) {
        return FALSE;
    }

    // Don't speak since this key is used to type a password.
    return TRUE;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
