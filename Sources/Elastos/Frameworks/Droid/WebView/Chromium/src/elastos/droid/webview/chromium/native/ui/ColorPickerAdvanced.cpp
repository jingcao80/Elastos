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
#include "elastos/droid/webkit/webview/chromium/native/ui/ColorPickerAdvanced.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/R_Ui.h"
#include "elastos/core/Math.h"

using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::R;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::ILinearLayout;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

//=====================================================================
//                         ColorPickerAdvanced
//=====================================================================
const Int32 ColorPickerAdvanced::HUE_SEEK_BAR_MAX;
const Int32 ColorPickerAdvanced::HUE_COLOR_COUNT;
const Int32 ColorPickerAdvanced::SATURATION_SEEK_BAR_MAX;
const Int32 ColorPickerAdvanced::SATURATION_COLOR_COUNT;
const Int32 ColorPickerAdvanced::VALUE_SEEK_BAR_MAX;
const Int32 ColorPickerAdvanced::VALUE_COLOR_COUNT;

CAR_INTERFACE_IMPL(ColorPickerAdvanced, LinearLayout, ISeekBarOnSeekBarChangeListener)

ColorPickerAdvanced::ColorPickerAdvanced(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : mHueDetails(NULL)
    , mSaturationDetails(NULL)
    , mValueDetails(NULL)
    , mOnColorChangedListener(NULL)
    , mCurrentColor(0)
{
    // ==================before translated======================
    // super(context, attrs);
    // init();

    LinearLayout::constructor(context, attrs);
    Init();
}

ColorPickerAdvanced::ColorPickerAdvanced(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : mHueDetails(NULL)
    , mSaturationDetails(NULL)
    , mValueDetails(NULL)
    , mOnColorChangedListener(NULL)
    , mCurrentColor(0)
{
    // ==================before translated======================
    // super(context, attrs, defStyle);
    // init();

    LinearLayout::constructor(context, attrs, defStyle);
    Init();
}

ColorPickerAdvanced::ColorPickerAdvanced(
    /* [in] */ IContext* context)
    : mHueDetails(NULL)
    , mSaturationDetails(NULL)
    , mValueDetails(NULL)
    , mOnColorChangedListener(NULL)
    , mCurrentColor(0)
{
    // ==================before translated======================
    // super(context);
    // init();

    LinearLayout::constructor(context);
    Init();
}

AutoPtr<ColorPickerAdvancedComponent> ColorPickerAdvanced::CreateAndAddNewGradient(
    /* [in] */ Int32 textResourceId,
    /* [in] */ Int32 seekBarMax,
    /* [in] */ ISeekBarOnSeekBarChangeListener* seekBarListener)
{
    // ==================before translated======================
    // LayoutInflater inflater = (LayoutInflater) getContext()
    //         .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    // View newComponent = inflater.inflate(R.layout.color_picker_advanced_component, null);
    // addView(newComponent);
    //
    // return new ColorPickerAdvancedComponent(newComponent,
    //         textResourceId,
    //         seekBarMax,
    //         seekBarListener);

    AutoPtr<IContext> content;
    GetContext((IContext**)&content);

    AutoPtr<IInterface> interfaceTmp;
    content->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&interfaceTmp);
    ILayoutInflater* inflater = ILayoutInflater::Probe(interfaceTmp);

    AutoPtr<IView> newComponent;
    inflater->Inflate(R::layout::color_picker_advanced_component, NULL, (IView**)&newComponent);
    AddView(newComponent);
    AutoPtr<ColorPickerAdvancedComponent> result = new ColorPickerAdvancedComponent(newComponent, textResourceId, seekBarMax, seekBarListener);
    return result;
}

ECode ColorPickerAdvanced::SetListener(
    /* [in] */ IOnColorChangedListener* onColorChangedListener)
{
    VALIDATE_NOT_NULL(onColorChangedListener);
    // ==================before translated======================
    // mOnColorChangedListener = onColorChangedListener;

    mOnColorChangedListener = onColorChangedListener;
    return NOERROR;
}

Int32 ColorPickerAdvanced::GetColor()
{
    // ==================before translated======================
    // return mCurrentColor;

    return mCurrentColor;
}

ECode ColorPickerAdvanced::SetColor(
    /* [in] */ Int32 color)
{
    // ==================before translated======================
    // mCurrentColor = color;
    // Color.colorToHSV(mCurrentColor, mCurrentHsvValues);
    // refreshGradientComponents();

    mCurrentColor = color;
    AutoPtr<IColor> colorHsv;
    CColor::AcquireSingleton((IColor**)&colorHsv);
    colorHsv->ColorToHSV(mCurrentColor, mCurrentHsvValues);
    RefreshGradientComponents();
    return NOERROR;
}

ECode ColorPickerAdvanced::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    VALIDATE_NOT_NULL(seekBar);
    // ==================before translated======================
    // if (fromUser) {
    //     mCurrentHsvValues[0] = mHueDetails.getValue();
    //     mCurrentHsvValues[1] = mSaturationDetails.getValue() / 100.0f;
    //     mCurrentHsvValues[2] = mValueDetails.getValue() / 100.0f;
    //
    //     mCurrentColor = Color.HSVToColor(mCurrentHsvValues);
    //
    //     updateHueGradient();
    //     updateSaturationGradient();
    //     updateValueGradient();
    //
    //     notifyColorChanged();
    // }

    if (fromUser) {
        (*mCurrentHsvValues)[0] = mHueDetails->GetValue();
        (*mCurrentHsvValues)[1] = mSaturationDetails->GetValue() / 100.0f;
        (*mCurrentHsvValues)[2] = mValueDetails->GetValue() / 100.0f;

        AutoPtr<IColor> colorHsv;
        CColor::AcquireSingleton((IColor**)&colorHsv);
        colorHsv->HSVToColor(mCurrentHsvValues, &mCurrentColor);

        UpdateHueGradient();
        UpdateSaturationGradient();
        UpdateValueGradient();

        NotifyColorChanged();
    }

    return NOERROR;
}

ECode ColorPickerAdvanced::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    VALIDATE_NOT_NULL(seekBar);
    // ==================before translated======================
    // // Do nothing.

    return NOERROR;
}

ECode ColorPickerAdvanced::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    VALIDATE_NOT_NULL(seekBar);
    // ==================before translated======================
    // // Do nothing.

    return NOERROR;
}

AutoPtr< ArrayOf<Float> > ColorPickerAdvanced::MiddleInitMcurrenthsvvalues()
{
    // ==================before translated======================
    // float[] result = new float[3];

    AutoPtr< ArrayOf<Float> > result = ArrayOf<Float>::Alloc(3);
    return result;
}

ECode ColorPickerAdvanced::Init()
{
    // ==================before translated======================
    // setOrientation(LinearLayout.VERTICAL);
    //
    // mHueDetails = createAndAddNewGradient(R.string.color_picker_hue,
    //         HUE_SEEK_BAR_MAX, this);
    // mSaturationDetails = createAndAddNewGradient(R.string.color_picker_saturation,
    //         SATURATION_SEEK_BAR_MAX, this);
    // mValueDetails = createAndAddNewGradient(R.string.color_picker_value,
    //         VALUE_SEEK_BAR_MAX, this);
    // refreshGradientComponents();

    SetOrientation(ILinearLayout::VERTICAL);

    mHueDetails = CreateAndAddNewGradient(R::string::color_picker_hue,
            HUE_SEEK_BAR_MAX, this);
    mSaturationDetails = CreateAndAddNewGradient(R::string::color_picker_saturation,
            SATURATION_SEEK_BAR_MAX, this);
    mValueDetails = CreateAndAddNewGradient(R::string::color_picker_value,
            VALUE_SEEK_BAR_MAX, this);
    RefreshGradientComponents();

    return NOERROR;
}

ECode ColorPickerAdvanced::NotifyColorChanged()
{
    // ==================before translated======================
    // if (mOnColorChangedListener != null) {
    //     mOnColorChangedListener.onColorChanged(getColor());
    // }

    if (NULL != mOnColorChangedListener) {
        mOnColorChangedListener->OnColorChanged(GetColor());
    }
    return NOERROR;
}

ECode ColorPickerAdvanced::UpdateHueGradient()
{
    // ==================before translated======================
    // float[] tempHsvValues = new float[3];
    // tempHsvValues[1] = mCurrentHsvValues[1];
    // tempHsvValues[2] = mCurrentHsvValues[2];
    //
    // int[] newColors = new int[HUE_COLOR_COUNT];
    //
    // for (int i = 0; i < HUE_COLOR_COUNT; ++i) {
    //     tempHsvValues[0] = i * 60.0f;
    //     newColors[i] = Color.HSVToColor(tempHsvValues);
    // }
    // mHueDetails.setGradientColors(newColors);

    AutoPtr< ArrayOf<Float> > tempHsvValues = ArrayOf<Float>::Alloc(3);
    (*tempHsvValues)[1] = (*mCurrentHsvValues)[1];
    (*tempHsvValues)[2] = (*mCurrentHsvValues)[2];

    AutoPtr< ArrayOf<Int32> > newColors = ArrayOf<Int32>::Alloc(HUE_COLOR_COUNT);
    Int32 colorTemp = 0;
    AutoPtr<IColor> colorHsv;
    CColor::AcquireSingleton((IColor**)&colorHsv);

    for (Int32 i = 0; i < HUE_COLOR_COUNT; ++i) {
        tempHsvValues->Set(0, i * 60.0f);
        colorHsv->HSVToColor(tempHsvValues, &colorTemp);
        newColors->Set(i, colorTemp);
    }
    mHueDetails->SetGradientColors(newColors);
    return NOERROR;
}

ECode ColorPickerAdvanced::UpdateSaturationGradient()
{
    // ==================before translated======================
    // float[] tempHsvValues = new float[3];
    // tempHsvValues[0] = mCurrentHsvValues[0];
    // tempHsvValues[1] = 0.0f;
    // tempHsvValues[2] = mCurrentHsvValues[2];
    //
    // int[] newColors = new int[SATURATION_COLOR_COUNT];
    //
    // newColors[0] = Color.HSVToColor(tempHsvValues);
    //
    // tempHsvValues[1] = 1.0f;
    // newColors[1] = Color.HSVToColor(tempHsvValues);
    // mSaturationDetails.setGradientColors(newColors);

    AutoPtr< ArrayOf<Float> > tempHsvValues = ArrayOf<Float>::Alloc(3);
    (*tempHsvValues)[0] = (*mCurrentHsvValues)[0];
    (*tempHsvValues)[1] = 0.0f;
    (*tempHsvValues)[2] = (*mCurrentHsvValues)[2];

    AutoPtr< ArrayOf<Int32> > newColors = ArrayOf<Int32>::Alloc(SATURATION_COLOR_COUNT);
    Int32 hsvColor = 0;
    AutoPtr<IColor> colorHsv;
    CColor::AcquireSingleton((IColor**)&colorHsv);
    colorHsv->HSVToColor(tempHsvValues, &hsvColor);
    (*newColors)[0] = hsvColor;

    (*tempHsvValues)[1] = 1.0f;
    colorHsv->HSVToColor(tempHsvValues, &hsvColor);
    (*newColors)[1] = hsvColor;
    mSaturationDetails->SetGradientColors(newColors);
    return NOERROR;
}

ECode ColorPickerAdvanced::UpdateValueGradient()
{
    // ==================before translated======================
    // float[] tempHsvValues = new float[3];
    // tempHsvValues[0] = mCurrentHsvValues[0];
    // tempHsvValues[1] = mCurrentHsvValues[1];
    // tempHsvValues[2] = 0.0f;
    //
    // int[] newColors = new int[VALUE_COLOR_COUNT];
    //
    // newColors[0] = Color.HSVToColor(tempHsvValues);
    //
    // tempHsvValues[2] = 1.0f;
    // newColors[1] = Color.HSVToColor(tempHsvValues);
    // mValueDetails.setGradientColors(newColors);

    AutoPtr< ArrayOf<Float> > tempHsvValues = ArrayOf<Float>::Alloc(3);
    (*tempHsvValues)[0] = (*mCurrentHsvValues)[0];
    (*tempHsvValues)[1] = (*mCurrentHsvValues)[1];
    (*tempHsvValues)[2] = 0.0f;

    AutoPtr< ArrayOf<Int32> > newColors = ArrayOf<Int32>::Alloc(VALUE_COLOR_COUNT);
    Int32 hsvColor = 0;
    AutoPtr<IColor> colorHsv;
    CColor::AcquireSingleton((IColor**)&colorHsv);
    colorHsv->HSVToColor(tempHsvValues, &hsvColor);
    (*newColors)[0] = hsvColor;

    (*tempHsvValues)[2] = 1.0f;
    colorHsv->HSVToColor(tempHsvValues, &hsvColor);
    (*newColors)[1] = hsvColor;
    mValueDetails->SetGradientColors(newColors);
    return NOERROR;
}

ECode ColorPickerAdvanced::RefreshGradientComponents()
{
    // ==================before translated======================
    // // Round and bound the saturation value.
    // int saturationValue = Math.round(mCurrentHsvValues[1] * 100.0f);
    // saturationValue = Math.min(saturationValue, SATURATION_SEEK_BAR_MAX);
    // saturationValue = Math.max(saturationValue, 0);
    //
    // // Round and bound the Value amount.
    // int valueValue = Math.round(mCurrentHsvValues[2] * 100.0f);
    //
    // valueValue = Math.min(valueValue, VALUE_SEEK_BAR_MAX);
    // valueValue = Math.max(valueValue, 0);
    //
    // // Don't need to round the hue value since its possible values match the seek bar
    // // range directly.
    // mHueDetails.setValue(mCurrentHsvValues[0]);
    // mSaturationDetails.setValue(saturationValue);
    // mValueDetails.setValue(valueValue);
    //
    // updateHueGradient();
    // updateSaturationGradient();
    // updateValueGradient();

    // Round and bound the saturation value.
    Int32 saturationValue = Elastos::Core::Math::Round((*mCurrentHsvValues)[1] * 100.0f);
    saturationValue = Elastos::Core::Math::Min(saturationValue, SATURATION_SEEK_BAR_MAX);
    saturationValue = Elastos::Core::Math::Max(saturationValue, 0);

    // Round and bound the Value amount.
    Int32 valueValue = Elastos::Core::Math::Round((*mCurrentHsvValues)[2] * 100.0f);
    valueValue = Elastos::Core::Math::Min(valueValue, VALUE_SEEK_BAR_MAX);
    valueValue = Elastos::Core::Math::Max(valueValue, 0);

    // Don't need to round the hue value since its possible values match the seek bar
    // range directly.
    mHueDetails->SetValue((*mCurrentHsvValues)[0]);
    mSaturationDetails->SetValue(saturationValue);
    mValueDetails->SetValue(valueValue);

    UpdateHueGradient();
    UpdateSaturationGradient();
    UpdateValueGradient();
    return NOERROR;
}

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


