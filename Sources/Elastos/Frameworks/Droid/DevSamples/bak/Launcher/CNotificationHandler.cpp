
#include "CNotificationHandler.h"
#include "CApp.h"
#include <stdio.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::App::INotificationManager;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

ECode CNotificationHandler::OnReciever(
    /* [in] */ IContext* pContext,
    /* [in] */ IIntent *pIntent)
{
    if (pIntent) {
        return HandleIntent(pContext, pIntent);
    }
    return NOERROR;
}

ECode CNotificationHandler::HandleIntent(
    /* [in] */ IContext* pContext,
    /* [in] */ IIntent* pNotificationIntent)
{
    String action;
    ECode ec = pNotificationIntent->GetAction(&action);
    if (FAILED(ec)) return ec;

    String alertName("");
    String alertCookie("");
    AutoPtr<IUri> pData;
    ec = pNotificationIntent->GetData((IUri**)&pData);
    if (FAILED(ec)) return ec;
    if (pData) {
        pData->GetSchemeSpecificPart(&alertName);
        pData->GetFragment(&alertCookie);
        if (alertCookie.IsNull()) {
            alertCookie = "";
        }
    }

    printf("GeckoAppJava", "NotificationHandler.handleIntent\n"
          "- action = '%s'\n- alertName = '%s'\n- alertCookie = '%s'\n",
          action.string(), alertName.string(), alertCookie.string());

    Int32 notificationID = alertName.GetHashCode();

    printf("GeckoAppJava Handle notification ID %d\n", notificationID);

    if (CApp::sAppContext) {
        // This should call the observer, if any
        CApp::sAppContext->HandleNotification(action, alertName, alertCookie);
    }
    else {
        // The app is not running, just cancel this notification
        AutoPtr<INotificationManager> pNotificationManager;
        ec = pContext->GetSystemService(String(IContext::NOTIFICATION_SERVICE),
                            (IInterface**)&pNotificationManager);
        if (FAILED(ec)) return ec;
        ec = pNotificationManager->Cancel(notificationID);
        if (FAILED(ec)) return ec;
    }

    if (action.Equals(GeckoApp_ACTION_ALERT_CLICK)) {
        // Start or bring to front the main activity
        AutoPtr<IIntent> pAppIntent;
        ec = CIntent::New(String(IIntent::ACTION_MAIN), (IIntent**)&pAppIntent);
        if (FAILED(ec)) return ec;

        pAppIntent->SetClassName(pContext,
                 String("org.mozilla.fennec_unofficial.App"));
        pAppIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        String args("-alert ");
        args += alertName;
        if (!alertCookie.IsNullOrEmpty()) {
            args += alertCookie + "#";
        }
        pAppIntent->PutExtra(String("args"), args);
        //try {
            //Log.i("GeckoAppJava", "startActivity with intent: Action='" + appIntent.getAction() + "'" +
            //      ", args='" + appIntent.getStringExtra("args") + "'" );
            ec = pContext->StartActivity(pAppIntent);
            return ec;
        //} catch (ActivityNotFoundException e) {
        //    Log.e("GeckoAppJava", "NotificationHandler Exception: ", e);
        //}
    }
    return NOERROR;
}

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
