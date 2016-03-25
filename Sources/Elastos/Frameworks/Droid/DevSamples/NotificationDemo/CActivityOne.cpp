
#include "CActivityOne.h"
#include <elastos/System.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <R.h>
#include "R.h"

using Elastos::Core::System;
using Elastos::Core::CStringWrapper;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;
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

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace NotificationDemo {

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
}

PInterface CActivityOne::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return this;
    }

    return NULL;
}

UInt32 CActivityOne::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CActivityOne::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode CActivityOne::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
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

//=============================================================================================

CActivityOne::CActivityOne()
    : mNotificationID(100)
    , mNumMessages(0)
{
}

ECode CActivityOne::StartStatusBarNotification()
{
    Slogger::D("CActivityOne", " >> Start StatusBarNotification...");

    String title("Title: Meeting with Business");
    String message("Msg: Kesalin 10:00 AM EST ");
    String ticker("New message!");

    AutoPtr<INotificationManager> notificationManager;
    AutoPtr<IInterface> svTemp;
    GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&svTemp);
    notificationManager = INotificationManager::Probe(svTemp.Get());
    assert(notificationManager != NULL);

    String packageName("NotificationDemo"); //Elastos.Droid.DevSamples.SpinnerDemo
    String actionName("android.intent.action.NOTIFICATION");
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetPackage(packageName);
    intent->SetAction(actionName);

    // The PendingIntent to launch our activity if the user selects this notification
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pendingIntent;
    helper->GetActivity(this, 0, intent, 0, (IPendingIntent**)&pendingIntent);

    AutoPtr<ICharSequence> tickerText;
    CStringWrapper::New(ticker, (ICharSequence**)&tickerText);

    AutoPtr<ICharSequence> titleSeq, msgSeq;
    CStringWrapper::New(title, (ICharSequence**)&titleSeq);
    CStringWrapper::New(message, (ICharSequence**)&msgSeq);

    // Create notifcation
    //
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    AutoPtr<INotification> notification;
    ASSERT_SUCCEEDED(CNotification::New(R::drawable::icon, tickerText,
        now, (INotification**)&notification));
    assert(notification != NULL);

    Int32 flags = 0;
    notification->GetFlags(&flags);
    flags |= INotification::FLAG_AUTO_CANCEL;
    notification->SetFlags(flags);

    // Set the info for the views that show in the notification panel.
    ASSERT_SUCCEEDED(notification->SetLatestEventInfo(
        this, titleSeq, msgSeq, pendingIntent));

    String notificationStr;
    notification->ToString(&notificationStr);
    Slogger::D("CActivityOne::StartStatusBarNotification", " >> notify (%s)", notificationStr.string());

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
    Slogger::D("CActivityOne", " >> UpdateNotification...");
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
    CStringWrapper::New(ticker, (ICharSequence**)&tickerSeq);
    CStringWrapper::New(title, (ICharSequence**)&titleSeq);
    CStringWrapper::New(content, (ICharSequence**)&contentSeq);

    builder->SetContentTitle(titleSeq);
    builder->SetContentText(contentSeq);
    builder->SetTicker(tickerSeq);
    builder->SetSmallIcon(R::drawable::icon);

    /* Increase notification number every time a new notification arrives */
    builder->SetNumber(++mNumMessages);

    /* Creates an explicit intent for an Activity in your app */
    String packageName("NotificationDemo");
    String actionName("android.intent.action.NOTIFICATION");
    String className("CNotificationActivity");
    String parentClassName("CActivityOne");

    /* Creates an explicit intent for an Activity in your app */
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetPackage(packageName);
    intent->SetAction(actionName);

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

    String notificationStr;
    notification->ToString(&notificationStr);
    Slogger::D("CActivityOne::NotifyNotification", " >> notify (%s)", notificationStr.string());

    ASSERT_SUCCEEDED(notificationManager->Notify(mNotificationID, notification));

    return NOERROR;
}

ECode CActivityOne::CancelNotification()
{
    Slogger::D("CActivityOne", " CancelNotification...");

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
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<MyListener> l = new MyListener(this);
    IViewOnClickListener* clickListener = (IViewOnClickListener*)(l->Probe(EIID_IViewOnClickListener));

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

    String packageName("NotificationDemo");
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
    CStringWrapper::New(String("New Message"), (ICharSequence**)&tickerText);

    AutoPtr<ICharSequence> titleSeq, msgSeq;
    CStringWrapper::New(title, (ICharSequence**)&titleSeq);
    CStringWrapper::New(message, (ICharSequence**)&msgSeq);

    // Create notifcation
    //
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    AutoPtr<INotification> notification;
    ASSERT_SUCCEEDED(CNotification::New(
        Elastos::Droid::R::drawable::ic_launcher_android,
        tickerText,
        now,
        (INotification**)&notification));
    assert(notification != NULL);
    ASSERT_SUCCEEDED(notification->SetLatestEventInfo(this, titleSeq, msgSeq, pendingIntent));

    // Notify notification
    //
    ASSERT_SUCCEEDED(notificationManager->Notify(9999, notification));

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}

} // namespace NotificationDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
