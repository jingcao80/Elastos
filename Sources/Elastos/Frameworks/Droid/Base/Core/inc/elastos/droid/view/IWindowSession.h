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

#ifndef __ELASTOS_DROID_VIEW_IWINDOWSESSION_H__
#define __ELASTOS_DROID_VIEW_IWINDOWSESSION_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Content::IClipData;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

class IWindowSessionProxy
    : public Object
    , public IWindowSession
{
public:
    IWindowSessionProxy(
        /* [in] */ const android::sp<android::IBinder>& remote);

    CAR_INTERFACE_DECL();

    CARAPI Add(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ IRect* inContentInsets,
        /* [in] */ IInputChannel* inInputChannel,
        /* [out] */ IRect** outContentInsets,
        /* [out] */ IInputChannel** outInputChannel,
        /* [out] */ Int32* result);

    CARAPI AddToDisplay(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ Int32 layerStackId,
        /* [in] */ IRect* inContentInsets,
        /* [in] */ IInputChannel* inInputChannel,
        /* [out] */ IRect** outContentInsets,
        /* [out] */ IInputChannel** outInputChannel,
        /* [out] */ Int32* result);

    CARAPI AddWithoutInputChannel(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ IRect* inContentInsets,
        /* [out] */ IRect** outContentInsets,
        /* [out] */ Int32* result);

    CARAPI AddToDisplayWithoutInputChannel(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ Int32 layerStackId,
        /* [in] */ IRect* inContentInsets,
        /* [out] */ IRect** outContentInsets,
        /* [out] */ Int32* result);

    CARAPI Remove(
        /* [in] */ IIWindow* window);

    CARAPI Relayout(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 seq,
        /* [in] */ IWindowManagerLayoutParams* attrs,
        /* [in] */ Int32 requestedWidth,
        /* [in] */ Int32 requestedHeight,
        /* [in] */ Int32 viewVisibility,
        /* [in] */ Int32 flags,
        /* [in] */ IRect* inFrame,
        /* [in] */ IRect* inOverscanInsets,
        /* [in] */ IRect* inContentInsets,
        /* [in] */ IRect* inVisibleInsets,
        /* [in] */ IRect* inStableInsets,
        /* [in] */ IConfiguration* inConfig,
        /* [in] */ ISurface* inSurface,
        /* [out] */ IRect** outFrame,
        /* [out] */ IRect** outOverscanInsets,
        /* [out] */ IRect** outContentInsets,
        /* [out] */ IRect** outVisibleInsets,
        /* [out] */ IRect** outStableInsets,
        /* [out] */ IConfiguration** outConfig,
        /* [out] */ Int32* result,
        /* [out] */ ISurface** outSurface);

    CARAPI PerformDeferredDestroy(
        /* [in] */ IIWindow* window);

    CARAPI OutOfMemory(
        /* [in] */ IIWindow* window,
        /* [out] */ Boolean* result);

    CARAPI SetTransparentRegion(
        /* [in] */ IIWindow* window,
        /* [in] */ IRegion* region);

    CARAPI SetInsets(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 touchableInsets,
        /* [in] */ IRect* contentInsets,
        /* [in] */ IRect* visibleInsets,
        /* [in] */ IRegion* touchableRegion);

    CARAPI GetDisplayFrame(
        /* [in] */ IIWindow* window,
        /* [out] */ IRect** outDisplayFrame);

    CARAPI FinishDrawing(
        /* [in] */ IIWindow* window);

    CARAPI SetInTouchMode(
        /* [in] */ Boolean showFocus);

    CARAPI GetInTouchMode(
        /* [out] */ Boolean* result);

    CARAPI PerformHapticFeedback(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 effectId,
        /* [in] */ Boolean always,
        /* [out] */ Boolean* result);

    CARAPI PrepareDrag(
        /* [in] */ IIWindow* window,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 thumbnailWidth,
        /* [in] */ Int32 thumbnailHeight,
        /* [out] */ ISurface** outSurface,
        /* [out] */ IBinder** binder);

    CARAPI PerformDrag(
        /* [in] */ IIWindow* window,
        /* [in] */ IBinder* dragToken,
        /* [in] */ Float touchX,
        /* [in] */ Float touchY,
        /* [in] */ Float thumbCenterX,
        /* [in] */ Float thumbCenterY,
        /* [in] */ IClipData* data,
        /* [out] */ Boolean* result);

    CARAPI ReportDropResult(
        /* [in] */ IIWindow* window,
        /* [in] */ Boolean consumed);

    CARAPI DragRecipientEntered(
        /* [in] */ IIWindow* window);

    CARAPI DragRecipientExited(
        /* [in] */ IIWindow* window);

    CARAPI SetWallpaperPosition(
        /* [in] */ IBinder* windowToken,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float xstep,
        /* [in] */ Float ystep);

    CARAPI WallpaperOffsetsComplete(
        /* [in] */ IBinder* window);

    CARAPI SetWallpaperDisplayOffset(
        /* [in] */ IBinder* windowToken,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI GetLastWallpaperX(
        /* [out] */ Int32* x);

    CARAPI GetLastWallpaperY(
        /* [out] */ Int32* y);

    CARAPI SendWallpaperCommand(
        /* [in] */ IBinder* window,
        /* [in] */ const String& action,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ IBundle* extras,
        /* [in] */ Boolean sync,
        /* [out] */ IBundle** result);

    CARAPI WallpaperCommandComplete(
        /* [in] */ IBinder* window,
        /* [in] */ IBundle* result);

    CARAPI SetUniverseTransform(
        /* [in] */ IBinder* window,
        /* [in] */ Float alpha,
        /* [in] */ Float offx,
        /* [in] */ Float offy,
        /* [in] */ Float dsdx,
        /* [in] */ Float dtdx,
        /* [in] */ Float dsdy,
        /* [in] */ Float dtdy);

    CARAPI OnRectangleOnScreenRequested(
        /* [in] */ IBinder* token,
        /* [in] */ IRect* rectangle);

    CARAPI GetWindowId(
        /* [in] */ IBinder* window,
        /* [out] */ IIWindowId** windowId);

private:
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_addToDisplay;
    static const Int32 TRANSACTION_remove;
    static const Int32 TRANSACTION_relayout;
    static const Int32 TRANSACTION_finishDrawing;
    static const Int32 TRANSACTION_getInTouchMode;
    static const Int32 TRANSACTION_onRectangleOnScreenRequested;

    android::sp<android::IBinder> mRemote;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_IWINDOWSESSION_H__
