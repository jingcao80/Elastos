
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/graphics/drawable/CGradientDrawable.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/webkit/native/base/ApiCompatibilityUtils.h"
#include "elastos/droid/webkit/native/ui/ColorPickerAdvancedComponent.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::CGradientDrawable;
using Elastos::Droid::Graphics::Drawable::GradientDrawableOrientation;
using Elastos::Droid::Graphics::Drawable::GradientDrawableOrientation_LEFT_RIGHT;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Webkit::Base::ApiCompatibilityUtils;
using Elastos::Droid::Widget::IAbsSeekBar;
using Elastos::Droid::Widget::IProgressBar;

namespace Elastos {
namespace Droid {
namespace Webkit {
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

    assert(0);
    rootView->FindViewById(-1/*R::id::gradient*/, (IView**)&mGradientView);
    IView* viewTmp = IView::Probe(mText);
    rootView->FindViewById(-1/*R::id::text*/, (IView**)&viewTmp);
    mText->SetText(textResourceId);
    CGradientDrawable::New(GradientDrawableOrientation_LEFT_RIGHT, NULL, (IGradientDrawable**)&mGradientDrawable);

    IView* viewTmp1 = IView::Probe(mSeekBar);
    rootView->FindViewById(-1/*R::id::seek_bar*/, (IView**)&viewTmp1);
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
    resources->GetDrawable(-1/*R::drawable::color_picker_advanced_select_handle*/, (IDrawable**)&drawable);
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


