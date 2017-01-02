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
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ApiCompatibilityUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/ColorPickerAdvancedComponent.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/R_Ui.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::CGradientDrawable;
using Elastos::Droid::Graphics::Drawable::GradientDrawableOrientation;
using Elastos::Droid::Graphics::Drawable::GradientDrawableOrientation_LEFT_RIGHT;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ApiCompatibilityUtils;
using Elastos::Droid::Widget::IAbsSeekBar;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::R;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

//=====================================================================
//                     ColorPickerAdvancedComponent
//=====================================================================
ColorPickerAdvancedComponent::ColorPickerAdvancedComponent(
    /* [in] */ IView* rootView,
    /* [in] */ const Int32& textResourceId,
    /* [in] */ const Int32& seekBarMax,
    /* [in] */ ISeekBarOnSeekBarChangeListener* seekBarListener)
{
    // ==================before translated======================
    // mGradientView = rootView.findViewById(R.id.gradient);
    // mText = (TextView) rootView.findViewById(R.id.text);
    // mText.setText(textResourceId);
    // mGradientDrawable = new GradientDrawable(GradientDrawable.Orientation.LEFT_RIGHT, null);
    // mSeekBar = (SeekBar) rootView.findViewById(R.id.seek_bar);
    // mSeekBar.setOnSeekBarChangeListener(seekBarListener);
    // mSeekBar.setMax(seekBarMax);
    // // Setting the thumb offset means the seek bar thumb can move all the way to each end
    // // of the gradient view.
    // Context context = rootView.getContext();
    // int offset = context.getResources()
    //                     .getDrawable(R.drawable.color_picker_advanced_select_handle)
    //                     .getIntrinsicWidth();
    // mSeekBar.setThumbOffset(offset / 2);

    rootView->FindViewById(R::id::gradient, (IView**)&mGradientView);
    AutoPtr<IView> viewTmp;
    rootView->FindViewById(R::id::text, (IView**)&viewTmp);
    mText = ITextView::Probe(viewTmp);
    mText->SetText(textResourceId);
    CGradientDrawable::New(GradientDrawableOrientation_LEFT_RIGHT, NULL, (IGradientDrawable**)&mGradientDrawable);

    AutoPtr<IView> viewTmp1;
    rootView->FindViewById(R::id::seek_bar, (IView**)&viewTmp1);
    mSeekBar = ISeekBar::Probe(viewTmp1);
    mSeekBar->SetOnSeekBarChangeListener(seekBarListener);

    IProgressBar* progressBar = IProgressBar::Probe(mSeekBar);
    progressBar->SetMax(seekBarMax);
    // Setting the thumb offset means the seek bar thumb can move all the way to each end
    // of the gradient view.
    AutoPtr<IContext> context;
    rootView->GetContext((IContext**)&context);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);

    AutoPtr<IDrawable> drawable;
    resources->GetDrawable(R::drawable::color_picker_advanced_select_handle, (IDrawable**)&drawable);
    Int32 offset = 0;
    drawable->GetIntrinsicWidth(&offset);

    IAbsSeekBar* absSeekBar = IAbsSeekBar::Probe(mSeekBar);
    absSeekBar->SetThumbOffset(offset / 2);
}

Float ColorPickerAdvancedComponent::GetValue()
{
    // ==================before translated======================
    // return mSeekBar.getProgress();

    IProgressBar* progressBar = IProgressBar::Probe(mSeekBar);
    Int32 result = 0;
    progressBar->GetProgress(&result);
    return (Float)result;
}

ECode ColorPickerAdvancedComponent::SetValue(
    /* [in] */ Float newValue)
{
    // ==================before translated======================
    // mSeekBar.setProgress((int) newValue);

    IProgressBar* progressBar = IProgressBar::Probe(mSeekBar);
    progressBar->SetProgress((Int32)newValue);
    return NOERROR;
}

ECode ColorPickerAdvancedComponent::SetGradientColors(
    /* [in] */ ArrayOf<Int32>* newColors)
{
    VALIDATE_NOT_NULL(newColors);
    // ==================before translated======================
    // mGradientColors = newColors.clone();
    // if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN) {
    //     Orientation currentOrientation = Orientation.LEFT_RIGHT;
    //     mGradientDrawable = new GradientDrawable(currentOrientation, mGradientColors);
    // } else {
    //     mGradientDrawable.setColors(mGradientColors);
    // }
    // ApiCompatibilityUtils.setBackgroundForView(mGradientView, mGradientDrawable);

    mGradientColors = newColors->Clone();
    if (Build::VERSION::SDK_INT < Build::VERSION_CODES::JELLY_BEAN) {
        GradientDrawableOrientation currentOrientation = GradientDrawableOrientation_LEFT_RIGHT;
        CGradientDrawable::New(currentOrientation, mGradientColors, (IGradientDrawable**)&mGradientDrawable);
    }
    else {
        mGradientDrawable->SetColors(mGradientColors);
    }

    IDrawable* drawable = IDrawable::Probe(mGradientDrawable);
    ApiCompatibilityUtils::SetBackgroundForView(mGradientView, drawable);
    return NOERROR;
}

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


