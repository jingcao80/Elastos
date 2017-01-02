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

#include "CActivityOne.h"
#include "R.h"
#include <elastos/droid/R.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

#include <Elastos.Droid.Widget.h>
#include <Elastos.Droid.Os.h>

using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::EIID_INotificationManager;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::ITaskStackBuilder;
using Elastos::Droid::App::ITaskStackBuilderHelper;
using Elastos::Droid::App::CTaskStackBuilderHelper;
using Elastos::Droid::App::EIID_INotificationManager;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Core::CString;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::App::IIActivityManager;

namespace Elastos {
namespace DevSamples {
namespace NotificationDemo {

//=============================================================================
// CActivityOne::MyListener
//=============================================================================
CAR_INTERFACE_IMPL(CActivityOne::MyListener, Object, IViewOnClickListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);

    if (id == R::id::statusBarNotificationButton) {
        mHost->StartStatusBarNotification();
    }
    else if (id == R::id::startNotificationButton) {
        mHost->StartNotification();
    }
    else if (id == R::id::updateNotificationButton) {
        mHost->UpdateNotification();
    }
    else if (id == R::id::cancelNotificationButton) {
        mHost->CancelNotification();
    }
    return NOERROR;
}

//=============================================================================
// CActivityOne
//=============================================================================
CAR_OBJECT_IMPL(CActivityOne)

const String CActivityOne::TAG("NotificationDemo::CActivityOne");

CActivityOne::CActivityOne()
    : mNotificationID(100)
    , mNumMessages(0)
{
}

ECode CActivityOne::constructor()
{
    Logger::I(TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::StartStatusBarNotification()
{
    Slogger::D(TAG, " >> Start StatusBarNotification...");

    String title("Title: Meeting with Business");
    String message("Msg: Kesalin 10:00 AM EST ");
    String ticker("New message!");

    AutoPtr<INotificationManager> notificationManager;
    AutoPtr<IInterface> svTemp;
    GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&svTemp);
    notificationManager = INotificationManager::Probe(svTemp.Get());
    assert(notificationManager != NULL);

    String packageName("Elastos.DevSamples.NotificationDemo");
    String actionName("android.intent.action.NOTIFICATION");
    String className("Elastos.DevSamples.NotificationDemo.CNotificationActivity");
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    // intent->SetPackage(packageName);
    // intent->SetAction(actionName);
    intent->SetClassName(packageName, className);

    // The PendingIntent to launch our activity if the user selects this notification
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pendingIntent;
    helper->GetActivity(this, 0, intent, 0, (IPendingIntent**)&pendingIntent);

    AutoPtr<ICharSequence> tickerText;
    CString::New(ticker, (ICharSequence**)&tickerText);

    AutoPtr<ICharSequence> titleSeq, msgSeq;
    CString::New(title, (ICharSequence**)&titleSeq);
    CString::New(message, (ICharSequence**)&msgSeq);

    // Create notifcation
    //
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    AutoPtr<INotification> notification;
    ASSERT_SUCCEEDED(CNotification::New(
        R::drawable::icon, tickerText, now, (INotification**)&notification));

    Int32 flags = 0;
    notification->GetFlags(&flags);
    flags |= INotification::FLAG_AUTO_CANCEL;
    notification->SetFlags(flags);

    // Set the info for the views that show in the notification panel.
    ASSERT_SUCCEEDED(notification->SetLatestEventInfo(
        this, titleSeq, msgSeq, pendingIntent));

    Slogger::D(TAG, " >> notify (%s)", TO_CSTR(notification));

    // Notify notification
    //
    ASSERT_SUCCEEDED(notificationManager->Notify(mNotificationID, notification));
    return NOERROR;
}

ECode CActivityOne::StartNotification()
{
    Slogger::D("CActivityOne", " >> Start notificaiton...");

    return NotifyNotification(FALSE);
}

ECode CActivityOne::UpdateNotification()
{
    Slogger::D(TAG, " >> UpdateNotification...");
    return NotifyNotification(TRUE);
}

ECode CActivityOne::NotifyNotification(
    /* [in] */ Boolean isUpdate)
{
    String title(!isUpdate ? "New Message" : "Updated Message");
    String content(!isUpdate ? "You've received new message." : "You've got updated message.");
    String ticker(!isUpdate ? "New Message Alert!" : "Updated Message Alert!");

    AutoPtr<INotificationManager> notificationManager;
    AutoPtr<IInterface> svTemp;
    GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&svTemp);
    notificationManager = INotificationManager::Probe(svTemp.Get());
    assert(notificationManager != NULL);

    /* Invoking the default notification service */
    AutoPtr<IContext> context =  this;
    AutoPtr<INotificationBuilder> builder;
    CNotificationBuilder::New(context, (INotificationBuilder**)&builder);

    AutoPtr<ICharSequence> tickerSeq, titleSeq, contentSeq;
    CString::New(ticker, (ICharSequence**)&tickerSeq);
    CString::New(title, (ICharSequence**)&titleSeq);
    CString::New(content, (ICharSequence**)&contentSeq);

    builder->SetContentTitle(titleSeq);
    builder->SetContentText(contentSeq);
    builder->SetTicker(tickerSeq);
    builder->SetSmallIcon(R::drawable::icon);

    /* Increase notification number every time a new notification arrives */
    builder->SetNumber(++mNumMessages);

    /* Creates an explicit intent for an Activity in your app */
    String packageName("Elastos.DevSamples.NotificationDemo");
    String actionName("android.intent.action.NOTIFICATION");
    String className("Elastos.DevSamples.NotificationDemo.CNotificationActivity");
    String parentClassName("Elastos.DevSamples.NotificationDemo.CActivityOne");

    /* Creates an explicit intent for an Activity in your app */
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    // intent->SetPackage(packageName);
    // intent->SetAction(actionName);
    intent->SetClassName(packageName, className);

    AutoPtr<ITaskStackBuilderHelper> helper;
    CTaskStackBuilderHelper::AcquireSingleton((ITaskStackBuilderHelper**)&helper);
    AutoPtr<ITaskStackBuilder> stackBuilder;
    helper->Create(context, (ITaskStackBuilder**)&stackBuilder);

    AutoPtr<IComponentName> parentCompName;
    CComponentName::New(packageName, parentClassName, ((IComponentName**)&parentCompName));
    stackBuilder->AddParentStack(parentCompName);

    /* Adds the Intent that starts the Activity to the top of the stack */
    stackBuilder->AddNextIntent(intent);

    AutoPtr<IPendingIntent> resultPendingIntent;
    stackBuilder->GetPendingIntent(0, IPendingIntent::FLAG_UPDATE_CURRENT, (IPendingIntent**)&resultPendingIntent);
    builder->SetContentIntent(resultPendingIntent);

    AutoPtr<INotification> notification;
    builder->Build((INotification**)&notification);

    // Set the info for the views that show in the notification panel.
    ASSERT_SUCCEEDED(notification->SetLatestEventInfo(this, titleSeq, contentSeq, resultPendingIntent));

    Slogger::D(TAG, " >> notify (%s)", TO_CSTR(notification));

    ASSERT_SUCCEEDED(notificationManager->Notify(mNotificationID, notification));

    return NOERROR;
}

ECode CActivityOne::CancelNotification()
{
    Slogger::D(TAG, " CancelNotification...");

    AutoPtr<INotificationManager> notificationManager;
    AutoPtr<IInterface> svTemp;
    GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&svTemp);
    notificationManager = INotificationManager::Probe(svTemp.Get());
    assert(notificationManager != NULL);

    notificationManager->Cancel(mNotificationID);
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate()");

    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<MyListener> l = new MyListener(this);
    AutoPtr<IViewOnClickListener> clickListener = IViewOnClickListener::Probe(l);

    AutoPtr<IView> temp;

    temp = FindViewById(R::id::statusBarNotificationButton);
    assert(temp != NULL);
    temp->SetOnClickListener(clickListener);

    temp = FindViewById(R::id::startNotificationButton);
    assert(temp != NULL);
    temp->SetOnClickListener(clickListener);

    temp = FindViewById(R::id::updateNotificationButton);
    assert(temp != NULL);
    temp->SetOnClickListener(clickListener);

    temp = FindViewById(R::id::cancelNotificationButton);
    assert(temp != NULL);
    temp->SetOnClickListener(clickListener);
    return NOERROR;
}

ECode CActivityOne::SendNotification(
    /* [in] */ const String& title,
    /* [in] */ const String& message)
{
    AutoPtr<INotificationManager> notificationManager;
    AutoPtr<IInterface> svTemp;
    GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&svTemp);
    notificationManager = INotificationManager::Probe(svTemp.Get());
    assert(notificationManager != NULL);

    // Intent intent = new Intent(this, MainActivity.class);
    // PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, intent, 0);

    String packageName("Elastos.DevSamples.NotificationDemo");
    String actionName("android.intent.action.Main");
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetPackage(packageName);
    intent->SetAction(actionName);

    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pendingIntent;
    helper->GetActivity(this, 0, intent, 0, (IPendingIntent**)&pendingIntent);

    AutoPtr<ICharSequence> tickerText;
    CString::New(String("New Message"), (ICharSequence**)&tickerText);

    AutoPtr<ICharSequence> titleSeq, msgSeq;
    CString::New(title, (ICharSequence**)&titleSeq);
    CString::New(message, (ICharSequence**)&msgSeq);

    // Create notifcation
    //
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    AutoPtr<INotification> notification;
    ASSERT_SUCCEEDED(CNotification::New(
        Elastos::Droid::R::drawable::emo_im_kissing/* ic_launcher_android */,
        tickerText,
        now,
        (INotification**)&notification));
    assert(notification != NULL);
    ASSERT_SUCCEEDED(notification->SetLatestEventInfo(this, titleSeq, msgSeq, pendingIntent));

    Slogger::D(TAG, " >> notify (%s)", TO_CSTR(notification));

    // Notify notification
    //
    ASSERT_SUCCEEDED(notificationManager->Notify(9999, notification));

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    Logger::I(TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Logger::I(TAG, " >> OnActivityResult()");
    return Activity::OnActivityResult(requestCode, resultCode, data);
}

} // namespace NotificationDemo
} // namespace DevSamples
} // namespace Elastos
