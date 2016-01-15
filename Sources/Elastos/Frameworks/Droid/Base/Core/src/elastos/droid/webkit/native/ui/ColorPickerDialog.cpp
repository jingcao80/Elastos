
#include "elastos/droid/webkit/native/ui/ColorPickerDialog.h"

using Elastos::Droid::Content::EIID_IDialogInterface;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Webkit::Ui::EIID_IOnColorChangedListener;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;


namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {

//=====================================================================
//            ColorPickerDialog::InnerOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(ColorPickerDialog::InnerOnClickListener, Object, IDialogInterfaceOnClickListener)

ColorPickerDialog::InnerOnClickListener::InnerOnClickListener(
    /* [in] */ ColorPickerDialog* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ColorPickerDialog::InnerOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialogInterface,
    /* [in] */ Int32 i)
{
    VALIDATE_NOT_NULL(dialogInterface);
    // ==================before translated======================
    // tryNotifyColorSet(mCurrentColor);

    assert(NULL != mOwner);
    mOwner->TryNotifyColorSet(mOwner->mCurrentColor);
    return NOERROR;
}

//=====================================================================
//            ColorPickerDialog::InnerOnClickListener1
//=====================================================================
CAR_INTERFACE_IMPL(ColorPickerDialog::InnerOnClickListener1, Object, IDialogInterfaceOnClickListener)

ColorPickerDialog::InnerOnClickListener1::InnerOnClickListener1(
    /* [in] */ ColorPickerDialog* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ColorPickerDialog::InnerOnClickListener1::OnClick(
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
//       ColorPickerDialog::InnerOnCancelListener
//=====================================================================
CAR_INTERFACE_IMPL(ColorPickerDialog::InnerOnCancelListener, Object, IDialogInterfaceOnCancelListener)

ColorPickerDialog::InnerOnCancelListener::InnerOnCancelListener(
    /* [in] */ ColorPickerDialog* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ColorPickerDialog::InnerOnCancelListener::OnCancel(
    /* [in] */ IDialogInterface* arg0)
{
    VALIDATE_NOT_NULL(arg0);
    // ==================before translated======================
    // tryNotifyColorSet(mInitialColor);

    assert(NULL != mOwner);
    mOwner->TryNotifyColorSet(mOwner->mInitialColor);
    return NOERROR;
}

//=====================================================================
//            ColorPickerDialog::InnerButtonOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(ColorPickerDialog::InnerButtonOnClickListener, Object, IViewOnClickListener)

ColorPickerDialog::InnerButtonOnClickListener::InnerButtonOnClickListener(
    /* [in] */ ColorPickerDialog* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ColorPickerDialog::InnerButtonOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);
    // ==================before translated======================
    // showAdvancedView();

    assert(NULL != mOwner);
    mOwner->ShowAdvancedView();
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

    assert(0);
    AlertDialog::constructor(context, 0);
    // Initialize title
    AutoPtr<IInterface> interfaceTmp;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&interfaceTmp);
    ILayoutInflater* inflater = ILayoutInflater::Probe(interfaceTmp);

    AutoPtr<IView> title;
    inflater->Inflate(-1/*R::layout::color_picker_dialog_title*/, NULL, (IView**)&title);
    SetCustomTitle(title);

    title->FindViewById(-1/*R::id::selected_color_view*/, (IView**)&mCurrentColorView);

    AutoPtr<IView> titleTextTmp;
    title->FindViewById(-1/*R::id::title*/, (IView**)&titleTextTmp);
    ITextView* titleText = ITextView::Probe(titleTextTmp);

    AutoPtr<ICharSequence> charSequence;
    CString::New(String("")/*R::string::color_picker_dialog_title*/, (ICharSequence**)&charSequence);
    titleText->SetText(charSequence);

    // Initialize Set/Cancel buttons
    String positiveButtonText;
    context->GetString(-1/*R::string::color_picker_button_set*/, &positiveButtonText);
    AutoPtr<InnerOnClickListener> positiveOnClickListener = new InnerOnClickListener(this);
    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(positiveButtonText, (ICharSequence**)&charSequenceTmp);
    SetButton(BUTTON_POSITIVE, charSequenceTmp, positiveOnClickListener);

    // Note that with the color picker there's not really any such thing as
    // "cancelled".
    // The color picker flow only finishes when we return a color, so we
    // have to always
    // return something. The concept of "cancelled" in this case just means
    // returning
    // the color that we were initialized with.
    String negativeButtonText;
    context->GetString(-1/*R::string::color_picker_button_cancel*/, &negativeButtonText);
    AutoPtr<InnerOnClickListener1> negativeListener = new InnerOnClickListener1(this);
    AutoPtr<ICharSequence> charSequenceTmp1;
    CString::New(negativeButtonText, (ICharSequence**)&charSequenceTmp1);
    SetButton(BUTTON_NEGATIVE, charSequenceTmp1, negativeListener);

    AutoPtr<InnerOnCancelListener> onCancelListener = new InnerOnCancelListener(this);
    SetOnCancelListener(onCancelListener);

    // Initialize main content view
    AutoPtr<IView> content;
    inflater->Inflate(-1/*R::layout::color_picker_dialog_content*/, NULL, (IView**)&content);
    SetView(content);

    // Initialize More button.
    AutoPtr<IView> viewTmp;
    content->FindViewById(-1/*R::id::more_colors_button*/, (IView**)&viewTmp);
    mMoreButton = IButton::Probe(viewTmp);

    AutoPtr<InnerOnClickListener> onClickListener = new InnerOnClickListener(this);
    IView::Probe(mMoreButton)->SetOnClickListener(IViewOnClickListener::Probe(onClickListener));

    // Initialize advanced color view (hidden initially).
    AutoPtr<IView> viewTmp1;
    content->FindViewById(-1/*R::id::color_picker_advanced*/, (IView**)&viewTmp1);
    mAdvancedColorPicker = (ColorPickerAdvanced*)viewTmp1.Get();
    viewTmp1->SetVisibility(IView::GONE);

    // Initialize simple color view (default view).
    AutoPtr<IView> viewTmp2;
    content->FindViewById(-1/*R::id::color_picker_simple*/, (IView**)&viewTmp2);
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
    // ==================before translated======================
    // // Only need to hide the borders, not the Views themselves, since the Views are
    // // contained within the borders.
    // View buttonBorder = findViewById(R.id.more_colors_button_border);
    // buttonBorder.setVisibility(View.GONE);
    //
    // View simpleView = findViewById(R.id.color_picker_simple);
    // simpleView.setVisibility(View.GONE);
    //
    // mAdvancedColorPicker.setVisibility(View.VISIBLE);
    // mAdvancedColorPicker.setListener(this);
    // mAdvancedColorPicker.setColor(mCurrentColor);

    // Only need to hide the borders, not the Views themselves, since the Views are
    // contained within the borders.
    AutoPtr<IView> buttonBorder;
    FindViewById(-1/*R::id::more_colors_button_border*/, (IView**)&buttonBorder);
    buttonBorder->SetVisibility(IView::GONE);

    AutoPtr<IView> simpleView;
    FindViewById(-1/*R::id::color_picker_simple*/, (IView**)&simpleView);
    simpleView->SetVisibility(IView::GONE);

    IView::Probe(mAdvancedColorPicker)->SetVisibility(IView::VISIBLE);
    mAdvancedColorPicker->SetListener(this);
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


