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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_OBSCURESPEECHDELEGATE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_OBSCURESPEECHDELEGATE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/view/View.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::View::View;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Accessibility delegate that obscures speech for a view when the user has
 * not turned on the "speak passwords" preference and is not listening
 * through headphones.
 */
class ObscureSpeechDelegate
    : public Elastos::Droid::View::View::AccessibilityDelegate
{
public:
    TO_STRING_IMPL("ObscureSpeechDelegate")

    ObscureSpeechDelegate(
        /* [in] */ IContext* context);

    //@Override
    CARAPI SendAccessibilityEvent(
        /* [in] */ IView* host,
        /* [in] */ Int32 eventType);

    //@Override
    CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IView* host,
        /* [in] */ IAccessibilityEvent* event);

    //@Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IView* host,
        /* [in] */ IAccessibilityNodeInfo* info);

private:
    //@SuppressWarnings("deprecation")
    CARAPI_(Boolean) ShouldObscureSpeech();

private:
    friend class CNumPadKey;
    /** Whether any client has announced the "headset" notification. */
    static Boolean sAnnouncedHeadset;

    AutoPtr<IContentResolver> mContentResolver;
    AutoPtr<IAudioManager> mAudioManager;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_OBSCURESPEECHDELEGATE_H__