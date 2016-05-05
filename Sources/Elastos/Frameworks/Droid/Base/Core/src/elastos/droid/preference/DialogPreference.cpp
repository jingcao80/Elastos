
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/CAlertDialogBuilder.h"
#include "elastos/droid/preference/DialogPreference.h"
#include "elastos/droid/preference/CDialogPreferenceSavedState.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/LayoutInflater.h"

using Elastos::Core::CString;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Preference::IDialogPreferenceSavedState;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IAbsSavedState;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Preference {

CAR_INTERFACE_IMPL_4(DialogPreference, Preference, IDialogPreference, IDialogInterfaceOnClickListener, IDialogInterfaceOnDismissListener, IPreferenceManagerOnActivityDestroyListener)

DialogPreference::DialogPreference()
    : mDialogLayoutResId(0)
    , mWhichButtonClicked(0)
{}

ECode DialogPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(Preference::constructor(context, attrs, defStyleAttr, defStyleRes));

    AutoPtr< ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(const_cast<Int32 *>(R::styleable::DialogPreference),
            ArraySize(R::styleable::DialogPreference));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    String tmpString;
    a->GetString(R::styleable::DialogPreference_dialogTitle, &tmpString);
    if (tmpString.IsNull()) {
        // Fallback on the regular title of the preference
        // (the one that is seen in the list)
        mDialogTitle = NULL;
        GetTitle((ICharSequence**)&mDialogTitle);
    }
    else {
        CString::New(tmpString, (ICharSequence**)&mDialogTitle);
    }

    a->GetString(R::styleable::DialogPreference_dialogMessage, &tmpString);
    CString::New(tmpString, (ICharSequence**)&mDialogMessage);
    a->GetDrawable(R::styleable::DialogPreference_dialogIcon, (IDrawable**)&mDialogIcon);
    a->GetString(R::styleable::DialogPreference_positiveButtonText, &tmpString);
    CString::New(tmpString, (ICharSequence**)&mPositiveButtonText);
    a->GetString(R::styleable::DialogPreference_negativeButtonText, &tmpString);
    CString::New(tmpString, (ICharSequence**)&mNegativeButtonText);
    a->GetResourceId(R::styleable::DialogPreference_dialogLayout, mDialogLayoutResId, &mDialogLayoutResId);
    a->Recycle();
    return NOERROR;
}

ECode DialogPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return constructor(context, attrs, defStyle, 0);
}

ECode DialogPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::dialogPreferenceStyle, 0);
}

ECode DialogPreference::SetDialogTitle(
    /* [in] */ ICharSequence* dialogTitle)
{
    mDialogTitle = dialogTitle;
    return NOERROR;
}

ECode DialogPreference::SetDialogTitle(
    /* [in] */ Int32 dialogTitleResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(dialogTitleResId, &str);
    AutoPtr<ICharSequence> title;
    CString::New(str, (ICharSequence**)&title);
    return SetDialogTitle(title);
}

ECode DialogPreference::GetDialogTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    *title = mDialogTitle;
    REFCOUNT_ADD(*title)
    return NOERROR;
}

ECode DialogPreference::SetDialogMessage(
    /* [in] */ ICharSequence* dialogMessage)
{
    mDialogMessage = dialogMessage;
    return NOERROR;
}

ECode DialogPreference::SetDialogMessage(
    /* [in] */ Int32 dialogMessageResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(dialogMessageResId, &str);
    AutoPtr<ICharSequence> msg;
    CString::New(str, (ICharSequence**)&msg);
    return SetDialogMessage(msg);
}

ECode DialogPreference::GetDialogMessage(
    /* [out] */ ICharSequence** message)
{
    VALIDATE_NOT_NULL(message)
    *message = mDialogMessage;
    REFCOUNT_ADD(*message)
    return NOERROR;
}

ECode DialogPreference::SetDialogIcon(
    /* [in] */ IDrawable* dialogIcon)
{
    mDialogIcon = dialogIcon;
    return NOERROR;
}

ECode DialogPreference::SetDialogIcon(
    /* [in] */ Int32 dialogIconRes)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IDrawable> drawable;
    context->GetDrawable(dialogIconRes, (IDrawable**)&drawable);
    return SetDialogIcon(drawable);
}

ECode DialogPreference::GetDialogIcon(
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon)
    *icon = mDialogIcon;
    REFCOUNT_ADD(*icon)
    return NOERROR;
}

ECode DialogPreference::SetPositiveButtonText(
    /* [in] */ ICharSequence* positiveButtonText)
{
    mPositiveButtonText = positiveButtonText;
    return NOERROR;
}

ECode DialogPreference::SetPositiveButtonText(
    /* [in] */ Int32 positiveButtonTextResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(positiveButtonTextResId, &str);
    AutoPtr<ICharSequence> text;
    CString::New(str, (ICharSequence**)&text);
    return SetPositiveButtonText(text);
}

ECode DialogPreference::GetPositiveButtonText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text)
    *text = mPositiveButtonText;
    REFCOUNT_ADD(*text)
    return NOERROR;
}

ECode DialogPreference::SetNegativeButtonText(
    /* [in] */ ICharSequence* negativeButtonText)
{
    mNegativeButtonText = negativeButtonText;
    return NOERROR;
}

ECode DialogPreference::SetNegativeButtonText(
    /* [in] */ Int32 negativeButtonTextResId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(negativeButtonTextResId, &str);
    AutoPtr<ICharSequence> text;
    CString::New(str, (ICharSequence**)&text);
    return SetNegativeButtonText(text);
}

ECode DialogPreference::GetNegativeButtonText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text)
    *text = mNegativeButtonText;
    REFCOUNT_ADD(*text)
    return NOERROR;
}

ECode DialogPreference::SetDialogLayoutResource(
    /* [in] */ Int32 dialogLayoutResId)
{
    mDialogLayoutResId = dialogLayoutResId;
    return NOERROR;
}

ECode DialogPreference::GetDialogLayoutResource(
    /* [out] */ Int32* layoutResId)
{
    VALIDATE_NOT_NULL(layoutResId)
    *layoutResId = mDialogLayoutResId;
    return NOERROR;
}

ECode DialogPreference::OnPrepareDialogBuilder(
    /* [in] */ IAlertDialogBuilder* builder)
{
    return NOERROR;
}

ECode DialogPreference::OnClick()
{
    Boolean isShowing;
    if (mDialog != NULL && (mDialog->IsShowing(&isShowing), isShowing)) {
        return NOERROR;
    }
    return ShowDialog(NULL);
}

ECode DialogPreference::ShowDialog(
    /* [in] */ IBundle* state)
{
    AutoPtr<IDialog> dialog;
    FAIL_RETURN(CreateDialog((IDialog**)&dialog));
    if (state != NULL) {
        dialog->OnRestoreInstanceState(state);
    }
    Boolean isNeed = FALSE;
    if (NeedInputMethod(&isNeed), isNeed) {
        RequestInputMethod(dialog);
    }
    dialog->SetOnDismissListener(this);
    dialog->Show();
    return NOERROR;
}

ECode DialogPreference::CreateDialog(
    /* [out] */ IDialog** isNeed)
{
    VALIDATE_NOT_NULL(isNeed);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    mWhichButtonClicked = IDialogInterface::BUTTON_NEGATIVE;

    mBuilder = NULL;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&mBuilder);
    mBuilder->SetTitle(mDialogTitle);
    mBuilder->SetIcon(mDialogIcon);
    mBuilder->SetPositiveButton(mPositiveButtonText, this);
    mBuilder->SetNegativeButton(mNegativeButtonText, this);

    AutoPtr<IView> contentView;
    OnCreateDialogView((IView**)&contentView);
    if (contentView != NULL) {
        OnBindDialogView(contentView);
        mBuilder->SetView(contentView);
    }
    else {
        mBuilder->SetMessage(mDialogMessage);
    }

    OnPrepareDialogBuilder(mBuilder);

    AutoPtr<IPreferenceManager> manager;
    GetPreferenceManager((IPreferenceManager**)&manager);
    manager->RegisterOnActivityDestroyListener(this);

    // Create the dialog
    AutoPtr<IAlertDialog> alertDialog;
    mBuilder->Create((IAlertDialog**)&alertDialog);
    *isNeed = IDialog::Probe(alertDialog);
    REFCOUNT_ADD(*isNeed);
    return NOERROR;
}

ECode DialogPreference::NeedInputMethod(
    /* [out] */ Boolean* isNeed)
{
    VALIDATE_NOT_NULL(isNeed)
    *isNeed = FALSE;
    return NOERROR;
}

ECode DialogPreference::RequestInputMethod(
    /* [in] */ IDialog* dialog)
{
    AutoPtr<IWindow> window;
    dialog->GetWindow((IWindow**)&window);
    window->SetSoftInputMode(IWindowManagerLayoutParams::SOFT_INPUT_STATE_ALWAYS_VISIBLE);
    return NOERROR;
}

ECode DialogPreference::OnCreateDialogView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    if (mDialogLayoutResId == 0) {
        *view = NULL;
        return NOERROR;
    }

    AutoPtr<IContext> context;
    mBuilder->GetContext((IContext**)&context);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    return inflater->Inflate(mDialogLayoutResId, NULL, view);
}

ECode DialogPreference::OnBindDialogView(
    /* [in] */ IView* view)
{
    AutoPtr<IView> dialogMessageView;
    view->FindViewById(R::id::message, (IView**)&dialogMessageView);

    if (dialogMessageView != NULL) {
        AutoPtr<ICharSequence> message;
        GetDialogMessage((ICharSequence**)&message);
        Int32 newVisibility = IView::GONE;
        assert(0);
        if (!TextUtils::IsEmpty(message)) {
            AutoPtr<ITextView> textView = ITextView::Probe(dialogMessageView);
            if (textView != NULL) {
                textView->SetText(message);
            }

            newVisibility = IView::VISIBLE;
        }

        Int32 visibility;
        if (dialogMessageView->GetVisibility(&visibility), visibility != newVisibility) {
            dialogMessageView->SetVisibility(newVisibility);
        }
    }
    return NOERROR;
}

ECode DialogPreference::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mWhichButtonClicked = which;
    return NOERROR;
}

ECode DialogPreference::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    AutoPtr<IPreferenceManager> manager;
    GetPreferenceManager((IPreferenceManager**)&manager);
    manager->UnregisterOnActivityDestroyListener(this);

    mDialog = NULL;
    OnDialogClosed(mWhichButtonClicked == IDialogInterface::BUTTON_POSITIVE);
    return NOERROR;
}

ECode DialogPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    return NOERROR;
}

ECode DialogPreference::GetDialog(
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    *dialog = mDialog;
    REFCOUNT_ADD(*dialog)
    return NOERROR;
}

ECode DialogPreference::OnActivityDestroy()
{
    Boolean isShowing;
    if (mDialog == NULL || (mDialog->IsShowing(&isShowing), !isShowing)) {
        return NOERROR;
    }
    AutoPtr<IDialogInterface> di = IDialogInterface::Probe(mDialog);
    return di->Dismiss();
}

ECode DialogPreference::OnSaveInstanceState(
    /* [out] */ IParcelable** parcel)
{
    VALIDATE_NOT_NULL(parcel)

    AutoPtr<IParcelable> superState;
    Preference::OnSaveInstanceState((IParcelable**)&superState);
    Boolean isShowing;
    if (mDialog == NULL || (mDialog->IsShowing(&isShowing), !isShowing)) {
        *parcel = superState;
        REFCOUNT_ADD(*parcel)
        return NOERROR;
    }

    AutoPtr<IDialogPreferenceSavedState> myState;
    CDialogPreferenceSavedState::New(superState, (IDialogPreferenceSavedState**)&myState);
    myState->SetIsDialogShowing(TRUE);
    AutoPtr<IBundle> bundle;
    mDialog->OnSaveInstanceState((IBundle**)&bundle);
    myState->SetDialogBundle(bundle);
    *parcel = IParcelable::Probe(myState);
    REFCOUNT_ADD(*parcel)
    return NOERROR;
}

ECode DialogPreference::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    if (state == NULL || IDialogPreferenceSavedState::Probe(state) == NULL) {
        // Didn't save state for us in onSaveInstanceState
        Preference::OnRestoreInstanceState(state);
        return NOERROR;
    }
    AutoPtr<IDialogPreferenceSavedState> myState = IDialogPreferenceSavedState::Probe(state);
    AutoPtr<IParcelable> superState;
    IAbsSavedState::Probe(myState)->GetSuperState((IParcelable**)&superState);
    Preference::OnRestoreInstanceState(superState);
    Boolean isDialogShowing;
    if (myState->GetIsDialogShowing(&isDialogShowing), isDialogShowing) {
        AutoPtr<IBundle> bundle;
        myState->GetDialogBundle((IBundle**)&bundle);
        ShowDialog(bundle);
    }
    return NOERROR;
}

ECode DialogPreference::OnGetDefaultValue(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 index,
    /* [out] */ IInterface** value)
{
    return Preference::OnGetDefaultValue(a, index, value);
}

}
}
}

