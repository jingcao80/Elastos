
#include <Elastos.CoreLibrary.Text.h>
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/ProgressDialog.h"
#include "elastos/droid/app/CProgressDialog.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/CSpannableString.h"
#include "elastos/droid/text/style/CStyleSpan.h"
#include "elastos/droid/view/LayoutInflater.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::R;
using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::CSpannableString;
using Elastos::Droid::Text::Style::IStyleSpan;
using Elastos::Droid::Text::Style::CStyleSpan;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::EIID_IProgressBar;
using Elastos::Droid::Widget::EIID_ITextView;
using Elastos::Text::INumberFormatHelper;
using Elastos::Text::CNumberFormatHelper;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace App {

ProgressDialog::ViewUpdateHandler::ViewUpdateHandler(
    /* [in] */ ProgressDialog* host)
    : mHost(host)
{}

ECode ProgressDialog::ViewUpdateHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Handler::HandleMessage(msg);

    /* Update the number and percent */
    Int32 progress;
    mHost->mProgress->GetProgress(&progress);
    Int32 max;
    mHost->mProgress->GetMax(&max);
    if (mHost->mProgressNumberFormat != NULL) {
        String format = mHost->mProgressNumberFormat;
        String text;
        text.AppendFormat(format, progress, max);
        mHost->mProgressNumber->SetText(CoreUtils::Convert(text));
    }
    else {
        mHost->mProgressNumber->SetText(CoreUtils::Convert(""));
    }

    if (mHost->mProgressPercentFormat != NULL) {
        Double percent = (Double) progress / (Double) max;
        String str;
        mHost->mProgressPercentFormat->Format(percent, &str);
        AutoPtr<ISpannableString> tmp;
        CSpannableString::New(CoreUtils::Convert(str), (ISpannableString**)&tmp);
        Int32 length;
        ICharSequence::Probe(tmp)->GetLength(&length);
        AutoPtr<IStyleSpan> styleSpan;
        CStyleSpan::New(ITypeface::BOLD, (IStyleSpan**)&styleSpan);
        ISpannable::Probe(tmp)->SetSpan(styleSpan.Get(), 0, length, ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
        mHost->mProgressPercent->SetText(ICharSequence::Probe(tmp));
    }
    else {
        mHost->mProgressPercent->SetText(CoreUtils::Convert(""));
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(ProgressDialog, AlertDialog, IProgressDialog)

ProgressDialog::ProgressDialog()
    : mProgressStyle(IProgressDialog::STYLE_SPINNER)
    , mMax(0)
    , mProgressVal(0)
    , mSecondaryProgressVal(0)
    , mIncrementBy(0)
    , mIncrementSecondaryBy(0)
    , mIndeterminate(FALSE)
    , mHasStarted(FALSE)
{
    InitFormats();
}

ProgressDialog::~ProgressDialog()
{}

ECode ProgressDialog::constructor(
    /* [in] */ IContext* context)
{
    AlertDialog::constructor(context);
    InitFormats();
    return NOERROR;
}

ECode ProgressDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme)
{
    AlertDialog::constructor(context, theme);
    InitFormats();
    return NOERROR;
}

void ProgressDialog::InitFormats()
{
    mProgressNumberFormat = "%1d/%2d";
    AutoPtr<INumberFormatHelper> helper;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&helper);
    helper->GetPercentInstance((INumberFormat**)&mProgressPercentFormat);
    mProgressPercentFormat->SetMaximumFractionDigits(0);
}

AutoPtr<IProgressDialog> ProgressDialog::Show(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* message)
{
    return Show(context, title, message, FALSE);
}

AutoPtr<IProgressDialog> ProgressDialog::Show(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* message,
    /* [in] */ Boolean indeterminate)
{
    return Show(context, title, message, indeterminate, FALSE, NULL);
}

AutoPtr<IProgressDialog> ProgressDialog::Show(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* message,
    /* [in] */ Boolean indeterminate,
    /* [in] */ Boolean cancelable)
{
    return Show(context, title, message, indeterminate, cancelable, NULL);
}

AutoPtr<IProgressDialog> ProgressDialog::Show(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* message,
    /* [in] */ Boolean indeterminate,
    /* [in] */ Boolean cancelable,
    /* [in] */ IDialogInterfaceOnCancelListener* cancelListener)
{
    AutoPtr<CProgressDialog> dialog;
    CProgressDialog::NewByFriend(context, (CProgressDialog**)&dialog);
    dialog->SetTitle(title);
    dialog->SetMessage(message);
    dialog->SetIndeterminate(indeterminate);
    dialog->SetCancelable(cancelable);
    dialog->SetOnCancelListener(cancelListener);
    IDialog::Probe(dialog)->Show();
    return dialog;
}

ECode ProgressDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::AlertDialog);
    AutoPtr<ITypedArray> a;
    mContext->ObtainStyledAttributes(NULL,
            attrIds, R::attr::alertDialogStyle, 0, (ITypedArray**)&a);

    if (mProgressStyle == IProgressDialog::STYLE_HORIZONTAL) {
        /* Use a separate handler to update the text views as they
         * must be updated on the same thread that created them.
         */
        mViewUpdateHandler = new ViewUpdateHandler(this);
        mViewUpdateHandler->constructor();

        Int32 resourceId = 0;
        a->GetResourceId(
            R::styleable::AlertDialog_horizontalProgressLayout,
            R::layout::alert_dialog_progress, &resourceId);
        AutoPtr<IView> view;
        inflater->Inflate(resourceId, NULL, (IView**)&view);

        AutoPtr<IView> tmp;
        view->FindViewById(R::id::progress, (IView**)&tmp);
        mProgress = IProgressBar::Probe(tmp);

        tmp = NULL;
        view->FindViewById(R::id::progress, (IView**)&tmp);
        mProgressNumber = ITextView::Probe(tmp);

        tmp = NULL;
        view->FindViewById(R::id::progress, (IView**)&tmp);
        mProgressPercent = ITextView::Probe(tmp);

        SetView(view);
    }
    else {
        AutoPtr<IView> view;
        Int32 resourceId = 0;
        a->GetResourceId(
            R::styleable::AlertDialog_progressLayout,
            R::layout::progress_dialog, &resourceId);
        inflater->Inflate(resourceId, NULL, (IView**)&view);

        AutoPtr<IView> temp1;
        view->FindViewById(Elastos::Droid::R::id::progress, (IView**)&temp1);
        mProgress = IProgressBar::Probe(temp1);

        AutoPtr<IView> temp2;
        view->FindViewById(R::id::message, (IView**)&temp2);
        mMessageView = ITextView::Probe(temp2);

        AlertDialog::SetView(view);
    }

    a->Recycle();
    if (mMax > 0) {
        SetMax(mMax);
    }
    if (mProgressVal > 0) {
        SetProgress(mProgressVal);
    }
    if (mSecondaryProgressVal > 0) {
        SetSecondaryProgress(mSecondaryProgressVal);
    }
    if (mIncrementBy > 0) {
        IncrementProgressBy(mIncrementBy);
    }
    if (mIncrementSecondaryBy > 0) {
        IncrementSecondaryProgressBy(mIncrementSecondaryBy);
    }
    if (mProgressDrawable != NULL) {
        SetProgressDrawable(mProgressDrawable);
    }
    if (mIndeterminateDrawable != NULL) {
        SetIndeterminateDrawable(mIndeterminateDrawable);
    }
    if (mMessage != NULL) {
        SetMessage(mMessage);
    }
    SetIndeterminate(mIndeterminate);
    OnProgressChanged();
    AlertDialog::OnCreate(savedInstanceState);
    return NOERROR;
}

ECode ProgressDialog::OnStart()
{
    AlertDialog::OnStart();
    mHasStarted = TRUE;
    return NOERROR;
}

ECode ProgressDialog::OnStop()
{
    AlertDialog::OnStop();
    mHasStarted = FALSE;
    return NOERROR;
}

ECode ProgressDialog::SetProgress(
    /* [in] */ Int32 value)
{
    if (mHasStarted) {
        mProgress->SetProgress(value);
        OnProgressChanged();
    } else {
        mProgressVal = value;
    }

    return NOERROR;
}

ECode ProgressDialog::SetSecondaryProgress(
    /* [in] */ Int32 secondaryProgress)
{
    if (mProgress != NULL) {
        mProgress->SetSecondaryProgress(secondaryProgress);
        OnProgressChanged();
    } else {
        mSecondaryProgressVal = secondaryProgress;
    }

    return NOERROR;
}

ECode ProgressDialog::GetProgress(
    /* [out] */ Int32* progress)
{
    VALIDATE_NOT_NULL(progress);

    if (mProgress != NULL) {
        return mProgress->GetProgress(progress);
    }

    *progress = mProgressVal;

    return NOERROR;
}

ECode ProgressDialog::GetSecondaryProgress(
    /* [out] */ Int32* secondaryProgress)
{
    VALIDATE_NOT_NULL(secondaryProgress);

    if (mProgress != NULL) {
        return mProgress->GetSecondaryProgress(secondaryProgress);
    }

    *secondaryProgress = mSecondaryProgressVal;

    return NOERROR;
}

ECode ProgressDialog::GetMax(
    /* [out] */ Int32* max)
{
    VALIDATE_NOT_NULL(max);

    if (mProgress != NULL) {
        return mProgress->GetMax(max);
    }

    *max = mMax;

    return NOERROR;
}

ECode ProgressDialog::SetMax(
    /* [in] */ Int32 max)
{
    if (mProgress != NULL) {
        mProgress->SetMax(max);
        OnProgressChanged();
    } else {
        mMax = max;
    }

    return NOERROR;
}

ECode ProgressDialog::IncrementProgressBy(
    /* [in] */ Int32 diff)
{
    if (mProgress != NULL) {
        mProgress->IncrementProgressBy(diff);
        OnProgressChanged();
    } else {
        mIncrementBy += diff;
    }

    return NOERROR;
}

ECode ProgressDialog::IncrementSecondaryProgressBy(
    /* [in] */ Int32 diff)
{
    if (mProgress != NULL) {
        mProgress->IncrementSecondaryProgressBy(diff);
        OnProgressChanged();
    } else {
        mIncrementSecondaryBy += diff;
    }

    return NOERROR;
}

ECode ProgressDialog::SetProgressDrawable(
    /* [in] */ IDrawable* d)
{
    if (mProgress != NULL) {
        mProgress->SetProgressDrawable(d);
    } else {
        mProgressDrawable = d;
    }

    return NOERROR;
}

ECode ProgressDialog::SetIndeterminateDrawable(
    /* [in] */ IDrawable* d)
{
    if (mProgress != NULL) {
        mProgress->SetIndeterminateDrawable(d);
    } else {
        mIndeterminateDrawable = d;
    }

    return NOERROR;
}

ECode ProgressDialog::SetIndeterminate(
    /* [in] */ Boolean indeterminate)
{
    if (mProgress != NULL) {
        mProgress->SetIndeterminate(indeterminate);
    } else {
        mIndeterminate = indeterminate;
    }

    return NOERROR;
}

ECode ProgressDialog::IsIndeterminate(
    /* [out] */ Boolean* isIndeterminate)
{
    VALIDATE_NOT_NULL(isIndeterminate);

    if (mProgress != NULL) {
        return mProgress->IsIndeterminate(isIndeterminate);
    }

    *isIndeterminate = mIndeterminate;

    return NOERROR;
}

// @Override
ECode ProgressDialog::SetMessage(
    /* [in] */ ICharSequence* message)
{
    if (mProgress != NULL) {
        if (mProgressStyle == IProgressDialog::STYLE_HORIZONTAL) {
            return AlertDialog::SetMessage(message);
        } else {
            return mMessageView->SetText(message);
        }
    } else {
        mMessage = message;
    }

    return NOERROR;
}

ECode ProgressDialog::SetProgressStyle(
    /* [in] */ Int32 style)
{
    mProgressStyle = style;

    return NOERROR;
}

ECode ProgressDialog::SetProgressNumberFormat(
    /* [in] */ const String& format)
{
    mProgressNumberFormat = format;
    OnProgressChanged();
    return NOERROR;
}

ECode ProgressDialog::SetProgressPercentFormat(
    /* [in] */ INumberFormat* format)
{
    mProgressPercentFormat = format;
    OnProgressChanged();
    return NOERROR;
}

void ProgressDialog::OnProgressChanged()
{
    if (mProgressStyle == IProgressDialog::STYLE_HORIZONTAL && mViewUpdateHandler != NULL) {
        Boolean has;
        mViewUpdateHandler->HasMessages(0, &has);
        if (!has) {
            mViewUpdateHandler->SendEmptyMessage(0, &has);
        }
    }
}

}
}
}
