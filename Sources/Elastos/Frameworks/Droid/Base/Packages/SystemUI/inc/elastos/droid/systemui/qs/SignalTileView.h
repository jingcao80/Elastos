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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_SIGNALTILEVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_SIGNALTILEVIEW_H__

#include "elastos/droid/systemui/qs/QSTileView.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

/** View that represents a custom quick settings tile for displaying signal info (wifi/cell). **/
class SignalTileView
    : public QSTileView
{
public:
    SignalTileView(
        /* [in] */ IContext* context);

protected:
    // @Override
    CARAPI_(AutoPtr<IView>) CreateIcon();

    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // @Override
    CARAPI HandleStateChanged(
        /* [in] */ IQSTileState* state);

private:
    CARAPI_(AutoPtr<IImageView>) AddTrafficView(
        /* [in] */ Int32 icon);

    CARAPI_(void) LayoutIndicator(
        /* [in] */ IView* indicator);

    using View::SetVisibility;

    CARAPI_(void) SetVisibility(
        /* [in] */ IView* view,
        /* [in] */ Boolean shown,
        /* [in] */ Boolean visible);

    static CARAPI_(Int64) InitStatic();

private:
    static const Int64 DEFAULT_DURATION;
    static const Int64 SHORT_DURATION;

    AutoPtr<IFrameLayout> mIconFrame;
    AutoPtr<IImageView> mSignal;
    AutoPtr<IImageView> mOverlay;
    AutoPtr<IImageView> mIn;
    AutoPtr<IImageView> mOut;

    Int32 mWideOverlayIconStartPadding;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_SIGNALTILEVIEW_H__
