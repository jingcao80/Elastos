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

#ifndef __ELASTOS_DROID_APP_PROGRESSDIALOG_H__
#define __ELASTOS_DROID_APP_PROGRESSDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/AlertDialog.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;
using Elastos::Text::INumberFormat;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * <p>A dialog showing a progress indicator and an optional text message or view.
 * Only a text message or a view can be used at the same time.</p>
 * <p>The dialog can be made cancelable on back key press.</p>
 * <p>The progress range is 0..10000.</p>
 */
class ProgressDialog
    : public AlertDialog
    , public IProgressDialog
{
private:
    class ViewUpdateHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ProgressDialog::ViewUpdateHandler")

        ViewUpdateHandler(
            /* [in] */ ProgressDialog* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        ProgressDialog* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ProgressDialog();

    virtual ~ProgressDialog();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme);

    static AutoPtr<IProgressDialog> Show(
        /* [in] */ IContext* context,
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* message);

    static AutoPtr<IProgressDialog> Show(
        /* [in] */ IContext* context,
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* message,
        /* [in] */ Boolean indeterminate);

    static AutoPtr<IProgressDialog> Show(
        /* [in] */ IContext* context,
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* message,
        /* [in] */ Boolean indeterminate,
        /* [in] */ Boolean cancelable);

    static AutoPtr<IProgressDialog> Show(
        /* [in] */ IContext* context,
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* message,
        /* [in] */ Boolean indeterminate,
        /* [in] */ Boolean cancelable,
        /* [in] */ IDialogInterfaceOnCancelListener* cancelListener);

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnStop();

    CARAPI SetProgress(
        /* [in] */ Int32 value);

    CARAPI SetSecondaryProgress(
        /* [in] */ Int32 secondaryProgress);

    CARAPI GetProgress(
        /* [out] */ Int32* progress);

    CARAPI GetSecondaryProgress(
        /* [out] */ Int32* secondaryProgress);

    CARAPI GetMax(
        /* [out] */ Int32* max);

    CARAPI SetMax(
        /* [in] */ Int32 max);

    CARAPI IncrementProgressBy(
        /* [in] */ Int32 diff);

    CARAPI IncrementSecondaryProgressBy(
        /* [in] */ Int32 diff);

    CARAPI SetProgressDrawable(
        /* [in] */ IDrawable* d);

    CARAPI SetIndeterminateDrawable(
        /* [in] */ IDrawable* d);

    CARAPI SetIndeterminate(
        /* [in] */ Boolean indeterminate);

    CARAPI IsIndeterminate(
        /* [out] */ Boolean* isIndeterminate);

    // @Override
    CARAPI SetMessage(
        /* [in] */ ICharSequence* message);

    CARAPI SetProgressStyle(
        /* [in] */ Int32 style);

    CARAPI SetProgressNumberFormat(
        /* [in] */ const String& format);

    CARAPI SetProgressPercentFormat(
        /* [in] */ INumberFormat* format);

private:
    CARAPI_(void) InitFormats();

    CARAPI_(void) OnProgressChanged();

protected:

    AutoPtr<IProgressBar> mProgress;
    AutoPtr<ITextView> mMessageView;

    Int32 mProgressStyle;
    AutoPtr<ITextView> mProgressNumber;
    String mProgressNumberFormat;
    AutoPtr<ITextView> mProgressPercent;
    AutoPtr<INumberFormat> mProgressPercentFormat;

    Int32 mMax;
    Int32 mProgressVal;
    Int32 mSecondaryProgressVal;
    Int32 mIncrementBy;
    Int32 mIncrementSecondaryBy;
    AutoPtr<IDrawable> mProgressDrawable;
    AutoPtr<IDrawable> mIndeterminateDrawable;
    AutoPtr<ICharSequence> mMessage;
    Boolean mIndeterminate;

    Boolean mHasStarted;
    AutoPtr<ViewUpdateHandler> mViewUpdateHandler;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_PROGRESSDIALOG_H__
