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

#ifndef  __ELASTOS_DROID_PHONE_CCHANGEICCPINSCREEN_H__
#define  __ELASTOS_DROID_PHONE_CCHANGEICCPINSCREEN_H__

#include "_Elastos_Droid_TeleService_Phone_CChangeIccPinScreen.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::Handler;
using Elastos::Core::Runnable;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IScrollView;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * "Change ICC PIN" UI for the Phone app.
 */
CarClass(CChangeIccPinScreen)
    , public Activity
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CChangeIccPinScreen::MyHandler")

        MyHandler(
            /* [in] */ CChangeIccPinScreen* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CChangeIccPinScreen* mHost;
    };

    class MyViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CChangeIccPinScreen::MyViewOnClickListener")

        CAR_INTERFACE_DECL()

        MyViewOnClickListener(
            /* [in] */ CChangeIccPinScreen* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CChangeIccPinScreen* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CChangeIccPinScreen::MyRunnable")

        MyRunnable(
            /* [in] */ CChangeIccPinScreen* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CChangeIccPinScreen* mHost;
    };

    class MyRunnable2
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CChangeIccPinScreen::MyRunnable2")

        MyRunnable2(
            /* [in] */ CChangeIccPinScreen* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CChangeIccPinScreen* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CChangeIccPinScreen();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

private:
    CARAPI_(void) ResolveIntent();

    CARAPI_(void) Reset();

    CARAPI_(Int32) ValidateNewPin(
        /* [in] */ const String& p1,
        /* [in] */ const String& p2);

    CARAPI_(void) HandleResult(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) DisplayPUKAlert();

    CARAPI_(void) ShowConfirmation();

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;
    static const Boolean DBG;

    static const Int32 EVENT_PIN_CHANGED;

    enum EntryState {
        ES_PIN,
        ES_PUK
    };

    EntryState mState;

    static const Int32 NO_ERROR;
    static const Int32 PIN_MISMATCH;
    static const Int32 PIN_INVALID_LENGTH;

    static const Int32 MIN_PIN_LENGTH;
    static const Int32 MAX_PIN_LENGTH;

    AutoPtr<IPhone> mPhone;
    Boolean mChangePin2;
    AutoPtr<ITextView> mBadPinError;
    AutoPtr<ITextView> mMismatchError;
    AutoPtr<IEditText> mOldPin;
    AutoPtr<IEditText> mNewPin1;
    AutoPtr<IEditText> mNewPin2;
    AutoPtr<IEditText> mPUKCode;
    AutoPtr<IButton> mButton;
    AutoPtr<IButton> mPUKSubmit;
    AutoPtr<IScrollView> mScrollView;

    AutoPtr<ILinearLayout> mIccPUKPanel;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IViewOnClickListener> mClicked;

    AutoPtr<IAlertDialog> mPUKAlert;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CCHANGEICCPINSCREENP_H__