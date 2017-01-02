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

#include "elastos/droid/webkit/webview/chromium/native/ui/ColorPickerDialog.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/R_Ui.h"

using Elastos::Droid::Content::EIID_IDialogInterface;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::R;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::EIID_IOnColorChangedListener;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;


namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

//=====================================================================
//            ColorPickerDialog::InnerListener
//=====================================================================
CAR_INTERFACE_IMPL(ColorPickerDialog::InnerListener, Object, IDialogInterfaceOnClickListener)

ColorPickerDialog::InnerListener::InnerListener(
    /* [in] */ ColorPickerDialog* owner)
    : mOwner(owner)
{
}

ECode ColorPickerDialog::InnerListener::OnClick(
    /* [in] */ IDialogInterface* dialogInterface,
    /* [in] */ Int32 i)
{
    VALIDATE_NOT_NULL(dialogInterface);
    assert(NULL != mOwner);
    mOwner->TryNotifyColorSet(mOwner->mCurrentColor);
    return NOERROR;
}

ECode ColorPickerDialog::InnerListener::OnCancel(
    /* [in] */ IDialogInterface* arg0)
{
    VALIDATE_NOT_NULL(arg0);
    assert(NULL != mOwner);
    mOwner->TryNotifyColorSet(mOwner->mInitialColor);
    return NOERROR;
}

ECode ColorPickerDialog::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);
    assert(NULL != mOwner);
    mOwner->ShowAdvancedView();
    return NOERROR;
}

ECode ColorPickerDialog::InnerListener::OnColorChanged(
    /* [in] */ Int32 color)
{
    return mOwner->OnColorChanged(color);
}

//=====================================================================
//            ColorPickerDialog::NegativeButtonListener
//=====================================================================
CAR_INTERFACE_IMPL(ColorPickerDialog::NegativeButtonListener, Object, IDialogInterfaceOnClickListener)

ColorPickerDialog::NegativeButtonListener::NegativeButtonListener(
    /* [in] */ ColorPickerDialog* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ColorPickerDialog::NegativeButtonListener::OnClick(
    /* [in] */ IDialogInterface* dialogInterface,
    /* [in] */ Int32 i)
{
    VALIDATE_NOT_NULL(dialogInterface);
    // ==================before translated======================
    // tryNotifyColorSet(mInitialColor);

    assert(NULL != mOwner);
    mOwner->TryNotifyColorSet(mOwner->mInitialColor);
    return NOERROR;
}


//=====================================================================
//                          ColorPickerDialog
//=====================================================================
CAR_INTERFACE_IMPL(ColorPickerDialog, AlertDialog, IOnColorChangedListener)

ColorPickerDialog::ColorPickerDialog(
    /* [in] */ IContext* context,
    /* [in] */ IOnColorChangedListener* listener,
    /* [in] */ Int32 color,
    /* [in] */ ArrayOf<IInterface*>* suggestions)
    : mListener(listener)
    , mInitialColor(color)
    , mCurrentColor(mInitialColor)
{
    // ==================before translated======================
    // super(context, 0);
    //
    // mListener = listener;
    // mInitialColor = color;
    // mCurrentColor = mInitialColor;
    //
    // // Initialize title
    // LayoutInflater inflater = (LayoutInflater) context
    //         .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    // View title = inflater.inflate(R.layout.color_picker_dialog_title, null);
    // setCustomTitle(title);
    //
    // mCurrentColorView = title.findViewById(R.id.selected_color_view);
    //
    // TextView titleText = (TextView) title.findViewById(R.id.title);
    // titleText.setText(R.string.color_picker_dialog_title);
    //
    // // Initialize Set/Cancel buttons
    // String positiveButtonText = context.getString(R.string.color_picker_button_set);
    // setButton(BUTTON_POSITIVE, positiveButtonText,
    //         new Dialog.OnClickListener() {
    //             @Override
    //             public void onClick(DialogInterface dialogInterface, int i) {
    //                 tryNotifyColorSet(mCurrentColor);
    //             }
    //         });
    //
    // // Note that with the color picker there's not really any such thing as
    // // "cancelled".
    // // The color picker flow only finishes when we return a color, so we
    // // have to always
    // // return something. The concept of "cancelled" in this case just means
    // // returning
    // // the color that we were initialized with.
    // String negativeButtonText = context.getString(R.string.color_picker_button_cancel);
    // setButton(BUTTON_NEGATIVE, negativeButtonText,
    //         new Dialog.OnClickListener() {
    //             @Override
    //             public void onClick(DialogInterface dialogInterface, int i) {
    //                 tryNotifyColorSet(mInitialColor);
    //             }
    //         });
    //
    // setOnCancelListener(new DialogInterface.OnCancelListener() {
    //     @Override
    //     public void onCancel(DialogInterface arg0) {
    //         tryNotifyColorSet(mInitialColor);
    //     }
    // });
    //
    // // Initialize main content view
    // View content = inflater.inflate(R.layout.color_picker_dialog_content, null);
    // setView(content);
    //
    // // Initialize More button.
    // mMoreButton = (Button) content.findViewById(R.id.more_colors_button);
    // mMoreButton.setOnClickListener(new Button.OnClickListener() {
    //     @Override
    //     public void onClick(View v) {
    //         showAdvancedView();
    //     }
    // });
    //
    // // Initialize advanced color view (hidden initially).
    // mAdvancedColorPicker =
    //         (ColorPickerAdvanced) content.findViewById(R.id.color_picker_advanced);
    // mAdvancedColorPicker.setVisibility(View.GONE);
    //
    // // Initialize simple color view (default view).
    // mSimpleColorPicker = (ColorPickerSimple) content.findViewById(R.id.color_picker_simple);
    // mSimpleColorPicker.init(suggestions, this);
    //
    // updateCurrentColor(mInitialColor);

    AlertDialog::constructor(context, 0);
    // Initialize title
    AutoPtr<IInterface> interfaceTmp;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&interfaceTmp);
    ILayoutInflater* inflater = ILayoutInflater::Probe(interfaceTmp);

    AutoPtr<IView> title;
    inflater->Inflate(R::layout::color_picker_dialog_title, NULL, (IView**)&title);
    SetCustomTitle(title);

    title->FindViewById(R::id::selected_color_view, (IView**)&mCurrentColorView);

    AutoPtr<IView> titleTextTmp;
    title->FindViewById(R::id::title, (IView**)&titleTextTmp);
    ITextView* titleText = ITextView::Probe(titleTextTmp);
    titleText->SetText(R::string::color_picker_dialog_title);

    // Initialize Set/Cancel buttons
    String positiveButtonText;
    context->GetString(R::string::color_picker_button_set, &positiveButtonText);
    AutoPtr<InnerListener> innerListener = new InnerListener(this);
    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(positiveButtonText, (ICharSequence**)&charSequenceTmp);
    SetButton(BUTTON_POSITIVE, charSequenceTmp, innerListener);

    // Note that with the color picker there's not really any such thing as
    // "cancelled".
    // The color picker flow only finishes when we return a color, so we
    // have to always
    // return something. The concept of "cancelled" in this case just means
    // returning
    // the color that we were initialized with.
    String negativeButtonText;
    context->GetString(R::string::color_picker_button_cancel, &negativeButtonText);
    AutoPtr<NegativeButtonListener> negativeListener = new NegativeButtonListener(this);
    AutoPtr<ICharSequence> charSequenceTmp1;
    CString::New(negativeButtonText, (ICharSequence**)&charSequenceTmp1);
    SetButton(BUTTON_NEGATIVE, charSequenceTmp1, negativeListener);

    SetOnCancelListener(innerListener);

    // Initialize main content view
    AutoPtr<IView> content;
    inflater->Inflate(R::layout::color_picker_dialog_content, NULL, (IView**)&content);
    SetView(content);

    // Initialize More button.
    AutoPtr<IView> viewTmp;
    content->FindViewById(R::id::more_colors_button, (IView**)&viewTmp);
    mMoreButton = IButton::Probe(viewTmp);

    IView::Probe(mMoreButton)->SetOnClickListener(innerListener);

    // Initialize advanced color view (hidden initially).
    AutoPtr<IView> viewTmp1;
    content->FindViewById(R::id::color_picker_advanced, (IView**)&viewTmp1);
    mAdvancedColorPicker = (ColorPickerAdvanced*)viewTmp1.Get();
    viewTmp1->SetVisibility(IView::GONE);

    // Initialize simple color view (default view).
    AutoPtr<IView> viewTmp2;
    content->FindViewById(R::id::color_picker_simple, (IView**)&viewTmp2);
    mSimpleColorPicker = (ColorPickerSimple*)viewTmp2.Get();
    mSimpleColorPicker->Init(suggestions, this);

    UpdateCurrentColor(mInitialColor);
}

ECode ColorPickerDialog::OnColorChanged(
    /* [in] */ Int32 color)
{
    // ==================before translated======================
    // updateCurrentColor(color);

    UpdateCurrentColor(color);
    return NOERROR;
}

ECode ColorPickerDialog::ShowAdvancedView()
{
    // Only need to hide the borders, not the Views themselves, since the Views are
    // contained within the borders.
    AutoPtr<IView> buttonBorder;
    FindViewById(R::id::more_colors_button_border, (IView**)&buttonBorder);
    buttonBorder->SetVisibility(IView::GONE);

    AutoPtr<IView> simpleView;
    FindViewById(R::id::color_picker_simple, (IView**)&simpleView);
    simpleView->SetVisibility(IView::GONE);

    IView::Probe(mAdvancedColorPicker)->SetVisibility(IView::VISIBLE);
    AutoPtr<InnerListener> listener = new InnerListener(this);
    mAdvancedColorPicker->SetListener(listener);
    mAdvancedColorPicker->SetColor(mCurrentColor);
    return NOERROR;
}

ECode ColorPickerDialog::TryNotifyColorSet(
    /* [in] */ Int32 color)
{
    // ==================before translated======================
    // if (mListener != null) mListener.onColorChanged(color);

    if (mListener != NULL)
        mListener->OnColorChanged(color);
    return NOERROR;
}

ECode ColorPickerDialog::UpdateCurrentColor(
    /* [in] */ Int32 color)
{
    // ==================before translated======================
    // mCurrentColor = color;
    // if (mCurrentColorView != null) mCurrentColorView.setBackgroundColor(color);

    mCurrentColor = color;
    if (mCurrentColorView != NULL)
        mCurrentColorView->SetBackgroundColor(color);
    return NOERROR;
}

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


