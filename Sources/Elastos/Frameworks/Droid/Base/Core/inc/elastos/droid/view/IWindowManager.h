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

#ifndef __ELASTOS_DROID_VIEW_IWINDOWMANAGER_H__
#define __ELASTOS_DROID_VIEW_IWINDOWMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Internal::View::IIInputContext;
using Elastos::Droid::Internal::View::IInputMethodClient;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IIRemoteCallback;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

class IWindowManagerProxy
    : public Object
    , public IIWindowManager
{
public:
    IWindowManagerProxy(
        /* [in] */ android::sp<android::IBinder>& wm);

    CAR_INTERFACE_DECL();

    CARAPI StartViewServer(
        /* [in] */ Int32 port,
        /* [out] */ Boolean* result);

    CARAPI StopViewServer(
        /* [out] */ Boolean* result);

    CARAPI IsViewServerRunning(
        /* [out] */ Boolean* result);

    CARAPI OpenSession(
        /* [in] */ IIWindowSessionCallback* cb,
        /* [in] */ IInputMethodClient* client,
        /* [in] */ IIInputContext* inputContext,
        /* [out] */ IWindowSession** session);

    CARAPI InputMethodClientHasFocus(
        /* [in] */ IInputMethodClient* client,
        /* [out] */ Boolean* result);

    CARAPI GetInitialDisplaySize(
        /* [in] */ Int32 displayId,
        /* [in] */ IPoint* inSize,
        /* [out] */ IPoint** outSize);

    CARAPI GetBaseDisplaySize(
        /* [in] */ Int32 displayId,
        /* [in] */ IPoint* inSize,
        /* [out] */ IPoint** outSize);

    CARAPI SetForcedDisplaySize(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI ClearForcedDisplaySize(
        /* [in] */ Int32 displayId);

    CARAPI GetInitialDisplayDensity(
        /* [in] */ Int32 displayId,
        /* [out] */ Int32* result);

    CARAPI GetBaseDisplayDensity(
        /* [in] */ Int32 displayId,
        /* [out] */ Int32* result);

    CARAPI SetForcedDisplayDensity(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 density);

    CARAPI ClearForcedDisplayDensity(
        /* [in] */ Int32 displayId);

    CARAPI SetOverscan(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI PauseKeyDispatching(
        /* [in] */ IBinder* token);

    CARAPI ResumeKeyDispatching(
        /* [in] */ IBinder* token);

    CARAPI SetEventDispatching(
        /* [in] */ Boolean enabled);

    CARAPI AddWindowToken(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 type);

    CARAPI RemoveWindowToken(
        /* [in] */ IBinder* token);

    CARAPI AddAppToken(
        /* [in] */ Int32 addPos,
        /* [in] */ IApplicationToken* token,
        /* [in] */ Int32 groupId,
        /* [in] */ Int32 stackId,
        /* [in] */ Int32 requestedOrientation,
        /* [in] */ Boolean fullscreen,
        /* [in] */ Boolean showWhenLocked,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 configChanges,
        /* [in] */ Boolean voiceInteraction,
        /* [in] */ Boolean launchTaskBehind);

    CARAPI SetAppGroupId(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 groupId);

    CARAPI SetAppOrientation(
        /* [in] */ IApplicationToken* token,
        /* [in] */ Int32 requestedOrientation);

    CARAPI GetAppOrientation(
        /* [in] */ IApplicationToken* token,
        /* [out] */ Int32* result);

    CARAPI SetFocusedApp(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean moveFocusNow);

    CARAPI PrepareAppTransition(
        /* [in] */ Int32 transit,
        /* [in] */ Boolean alwaysKeepCurrent);

    CARAPI GetPendingAppTransition(
        /* [in] */ Int32* result);

    CARAPI OverridePendingAppTransition(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 enterAnim,
        /* [in] */ Int32 exitAnim,
        /* [in] */ IIRemoteCallback* startedCallback);

    CARAPI OverridePendingAppTransitionScaleUp(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 startWidth,
        /* [in] */ Int32 startHeight);

    CARAPI OverridePendingAppTransitionThumb(
        /* [in] */ IBitmap* srcThumb,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ IIRemoteCallback* startedCallback,
        /* [in] */ Boolean scaleUp);

    CARAPI OverridePendingAppTransitionAspectScaledThumb(
        /* [in] */ IBitmap* srcThumb,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 targetWidth,
        /* [in] */ Int32 targetHeight,
        /* [in] */ IIRemoteCallback* startedCallback,
        /* [in] */ Boolean scaleUp);

    CARAPI ExecuteAppTransition();

    CARAPI SetAppStartingWindow(
        /* [in] */ IBinder* token,
        /* [in] */ const String& pkg,
        /* [in] */ Int32 theme,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ ICharSequence* nonLocalizedLabel,
        /* [in] */ Int32 labelRes,
        /* [in] */ Int32 icon,
        /* [in] */ Int32 logo,
        /* [in] */ Int32 windowFlags,
        /* [in] */ IBinder* transferFrom,
        /* [in] */ Boolean createIfNeeded);

    CARAPI SetAppWillBeHidden(
        /* [in] */ IBinder* token);

    CARAPI SetAppVisibility(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean visible);

    CARAPI StartAppFreezingScreen(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 configChanges);

    CARAPI StopAppFreezingScreen(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean force);

    CARAPI RemoveAppToken(
        /* [in] */ IBinder* token);

    CARAPI UpdateOrientationFromAppTokens(
        /* [in] */ IConfiguration* currentConfig,
        /* [in] */ IBinder* freezeThisOneIfNeeded,
        /* [out] */ IConfiguration** cfg);

    CARAPI SetNewConfiguration(
        /* [in] */ IConfiguration* config);

    CARAPI StartFreezingScreen(
        /* [in] */ Int32 exitAnim,
        /* [in] */ Int32 enterAnim);

    CARAPI StopFreezingScreen();

    CARAPI DisableKeyguard(
        /* [in] */ IBinder* token,
        /* [in] */ const String& tag);

    CARAPI ReenableKeyguard(
        /* [in] */ IBinder* token);

    CARAPI ExitKeyguardSecurely(
        /* [in] */ IOnKeyguardExitResult* cb);

    CARAPI IsKeyguardLocked(
        /* [out] */ Boolean* result);

    CARAPI IsKeyguardSecure(
        /* [out] */ Boolean* result);

    CARAPI InKeyguardRestrictedInputMode(
        /* [out] */ Boolean* result);

    CARAPI DismissKeyguard();

    CARAPI KeyguardGoingAway(
        /* [in] */ Boolean disableWindowAnimations,
        /* [in] */ Boolean keyguardGoingToNotificationShade,
        /* [in] */ Boolean keyguardShowingMedia);

    CARAPI CloseSystemDialogs(
        /* [in] */ const String& reason);

    // These can only be called with the SET_ANIMATON_SCALE permission.
    CARAPI GetAnimationScale(
        /* [in] */ Int32 which,
        /* [out] */ Float* result);

    CARAPI GetAnimationScales(
        /* [out, callee] */ ArrayOf<Float>** scales);

    CARAPI SetAnimationScale(
        /* [in] */ Int32 which,
        /* [in] */ Float scale);

    CARAPI SetAnimationScales(
        /* [in] */ ArrayOf<Float>* scales);

    CARAPI GetCurrentAnimatorScale(
        /* [out] */ Float* result);

    CARAPI SetInTouchMode(
        /* [in] */ Boolean showFocus);

    CARAPI ShowStrictModeViolation(
        /* [in] */ Boolean on);

    CARAPI SetStrictModeVisualIndicatorPreference(
        /* [in] */ const String& enabled);

    CARAPI SetScreenCaptureDisabled(
        /* [in] */ Int32 userId,
        /* [in] */ Boolean disabled);

    CARAPI UpdateRotation(
        /* [in] */ Boolean alwaysSendConfiguration,
        /* [in] */ Boolean forceRelayout);

    CARAPI GetRotation(
        /* [out] */ Int32* result);

    CARAPI WatchRotation(
        /* [in] */ IRotationWatcher* watcher,
        /* [out] */ Int32* result);

    CARAPI RemoveRotationWatcher(
        /* [in] */ IRotationWatcher* watcher);

    CARAPI GetPreferredOptionsPanelGravity(
        /* [out] */ Int32* result);

    CARAPI FreezeRotation(
        /* [in] */ Int32 rotation);

    CARAPI ThawRotation();

    CARAPI IsRotationFrozen(
        /* [out] */ Boolean* result);

    CARAPI ScreenshotApplications(
        /* [in] */ IBinder* appToken,
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 maxWidth,
        /* [in] */ Int32 maxHeight,
        /* [in] */ Boolean force565,
        /* [out] */ IBitmap** bitmap);

    CARAPI GetLastWallpaperX(
        /* [out] */ Int32* x);

    CARAPI GetLastWallpaperY(
        /* [out] */ Int32* y);

    CARAPI StatusBarVisibilityChanged(
        /* [in] */ Int32 visibility);

    CARAPI HasNavigationBar(
        /* [out] */ Boolean* result);

    CARAPI HasPermanentMenuKey(
        /* [out] */ Boolean* result);

    CARAPI NeedsNavigationBar(
        /* [out] */ Boolean* result);

    CARAPI LockNow(
        /* [in] */ IBundle* options);

    CARAPI IsSafeModeEnabled(
        /* [out] */ Boolean* result);

    CARAPI EnableScreenIfNeeded();

    CARAPI ClearWindowContentFrameStats(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI GetWindowContentFrameStats(
        /* [in] */ IBinder* token,
        /* [out] */ IWindowContentFrameStats** stats);

private:
    static const String TAG;
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_openSession;
    static const Int32 TRANSACTION_getCurrentAnimatorScale;

    android::sp<android::IBinder> mWm;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_IWINDOWMANAGER_H__
