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

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/internal/app/NetInitiatedActivity.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/location/LocationManager.h"
#include "elastos/droid/widget/Toast.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Internal::Location::IGpsNetInitiatedHandler;
using Elastos::Droid::Location::LocationManager;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::Toast;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

const String NetInitiatedActivity::TAG("NetInitiatedActivity");
const Boolean NetInitiatedActivity::DEBUG = TRUE;
const Boolean NetInitiatedActivity::VERBOSE = FALSE;
const Int32 NetInitiatedActivity::POSITIVE_BUTTON = IDialogInterface::BUTTON_POSITIVE;
const Int32 NetInitiatedActivity::NEGATIVE_BUTTON = IDialogInterface::BUTTON_NEGATIVE;
const Int32 NetInitiatedActivity::GPS_NO_RESPONSE_TIME_OUT = 1;

ECode NetInitiatedActivity::NetInitiatedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (DEBUG)
        Logger::D(TAG, "NetInitiatedReceiver onReceive: %s", action.string());
    if (action == IGpsNetInitiatedHandler::ACTION_NI_VERIFY) {
        mHost->HandleNIVerify(intent);
    }
    return NOERROR;
}

ECode NetInitiatedActivity::NetInitiatedReceiver::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("NetInitiatedActivity::NetInitiatedReceiver: ");
    (*info).AppendFormat("%p", this);
    return NOERROR;
}

ECode NetInitiatedActivity::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case NetInitiatedActivity::GPS_NO_RESPONSE_TIME_OUT:
            mHost->HandleGPSNoResponseTimeout();
            break;
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(NetInitiatedActivity::OnClickListener, Object, IDialogInterfaceOnClickListener)

NetInitiatedActivity::OnClickListener::OnClickListener(
    /* [in] */ NetInitiatedActivity* host)
    : mHost(host)
{
}

ECode NetInitiatedActivity::OnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->OnClick(dialog, which);
}

CAR_INTERFACE_IMPL(NetInitiatedActivity, AlertActivity, INetInitiatedActivity);

NetInitiatedActivity::NetInitiatedActivity()
    : mNotificationId(-1)
    , mTimeout(-1)
    , mDefault_response(-1)
    , mDefault_response_timeout(6)
    , mNetInitiatedReceiver(new NetInitiatedReceiver(this))
{
}

ECode NetInitiatedActivity::HandleGPSNoResponseTimeout()
{
    if (mNotificationId != -1) {
        SendUserResponse(mDefault_response);
    }
    Finish();
    return NOERROR;
}

ECode NetInitiatedActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    AlertActivity::OnCreate(savedInstanceState);

    mHandler = new MyHandler(this);
    mHandler->constructor();

    AutoPtr<OnClickListener> listener = new OnClickListener(this);

    // Set up the "dialog"
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IAlertControllerAlertParams> p = mAlertParams;
    AutoPtr<IContext> context;
    GetApplicationContext((IContext**)&context);
    String title;
    intent->GetStringExtra(IGpsNetInitiatedHandler::NI_INTENT_KEY_TITLE, &title);
    p->SetTitle(CoreUtils::Convert(title));
    String message;
    intent->GetStringExtra(IGpsNetInitiatedHandler::NI_INTENT_KEY_MESSAGE, &message);
    p->SetMessage(CoreUtils::Convert(message));

    String format;
    context->GetString(R::string::gpsVerifYes, &format);
    p->SetPositiveButtonText(CoreUtils::Convert(StringUtils::Format(format, NULL)));
    p->SetPositiveButtonListener(listener);
    context->GetString(R::string::gpsVerifNo, &format);
    p->SetNegativeButtonText(CoreUtils::Convert(StringUtils::Format(format, NULL)));
    p->SetNegativeButtonListener(listener);

    intent->GetInt32Extra(IGpsNetInitiatedHandler::NI_INTENT_KEY_NOTIF_ID, -1, &mNotificationId);
    intent->GetInt32Extra(IGpsNetInitiatedHandler::NI_INTENT_KEY_TIMEOUT,
            mDefault_response_timeout, &mTimeout);
    intent->GetInt32Extra(IGpsNetInitiatedHandler::NI_INTENT_KEY_DEFAULT_RESPONSE,
            IGpsNetInitiatedHandler::GPS_NI_RESPONSE_ACCEPT, &mDefault_response);
    if (DEBUG)
        Logger::D(TAG, "onCreate() : notificationId: %d timeout: %d default_response:%d",
            mNotificationId, mTimeout, mDefault_response);

    Boolean result;
    mHandler->SendEmptyMessageDelayed(GPS_NO_RESPONSE_TIME_OUT, (mTimeout * 1000), &result);
    SetupAlert();
    return NOERROR;
}

ECode NetInitiatedActivity::OnResume()
{
    AlertActivity::OnResume();
    if (DEBUG) Logger::D(TAG, "onResume");
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IGpsNetInitiatedHandler::ACTION_NI_VERIFY, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> intent;
    RegisterReceiver(mNetInitiatedReceiver, intentFilter, (IIntent**)&intent);
    return NOERROR;
}

ECode NetInitiatedActivity::OnPause()
{
    AlertActivity::OnPause();
    if (DEBUG) Logger::D(TAG, "onPause");
    UnregisterReceiver(mNetInitiatedReceiver);
    return NOERROR;
}

ECode NetInitiatedActivity::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which == POSITIVE_BUTTON) {
        SendUserResponse(IGpsNetInitiatedHandler::GPS_NI_RESPONSE_ACCEPT);
    }
    if (which == NEGATIVE_BUTTON) {
        SendUserResponse(IGpsNetInitiatedHandler::GPS_NI_RESPONSE_DENY);
    }

    // No matter what, finish the activity
    Finish();
    mNotificationId = -1;
    return NOERROR;
}

void NetInitiatedActivity::SendUserResponse(
    /* [in] */ Int32 response)
{
    if (DEBUG) Logger::D(TAG, "sendUserResponse, response: %d", response);
    AutoPtr<IInterface> service;
    GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&service);
    AutoPtr<ILocationManager> locationManager = ILocationManager::Probe(service);
    Boolean result;
    locationManager->SendNiResponse(mNotificationId, response, &result);
}

void NetInitiatedActivity::HandleNIVerify(
    /* [in] */ IIntent* intent)
{
    Int32 notifId;
    intent->GetInt32Extra(IGpsNetInitiatedHandler::NI_INTENT_KEY_NOTIF_ID, -1, &notifId);
    mNotificationId = notifId;

    String action;
    intent->GetAction(&action);
    if (DEBUG) Logger::D(TAG, "handleNIVerify action: %s", action.string());
}

void NetInitiatedActivity::ShowNIError()
{
    AutoPtr<IToast> toast;
    Toast::MakeText(this, CoreUtils::Convert(String("NI error"/* com.android.internal.R.string.usb_storage_error_message */))
       , IToast::LENGTH_LONG, (IToast**)&toast);
    toast->Show();
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
