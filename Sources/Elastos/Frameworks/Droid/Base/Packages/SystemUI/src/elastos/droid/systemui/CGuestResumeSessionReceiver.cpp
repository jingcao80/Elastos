
#include "elastos/droid/systemui/CGuestResumeSessionReceiver.h"
#include "elastos/droid/systemui/CResetSessionDialog.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::View::CWindowManagerGlobalHelper;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IWindowManagerGlobalHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

const String CGuestResumeSessionReceiver::TAG("GuestResumeSessionReceiver");
const String CGuestResumeSessionReceiver::SETTING_GUEST_HAS_LOGGED_IN("systemui.guest_has_logged_in");

CAR_OBJECT_IMPL(CGuestResumeSessionReceiver);
CAR_INTERFACE_IMPL(CGuestResumeSessionReceiver, BroadcastReceiver, IGuestResumeSessionReceiver);
ECode CGuestResumeSessionReceiver::Register(
    /* [in] */ IContext* context)
{
    AutoPtr<IIntentFilter> f;
    CIntentFilter::New(IIntent::ACTION_USER_SWITCHED, (IIntentFilter**)&f);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> user;
    helper->GetOWNER((IUserHandle**)&user);
    AutoPtr<IIntent> intent;
    context->RegisterReceiverAsUser(THIS_PROBE(IBroadcastReceiver), user,
            f, String(NULL) /* permission */, NULL /* scheduler */, (IIntent**)&intent);
    return NOERROR;
}

ECode CGuestResumeSessionReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        CancelDialog();

        Int32 userId = 0;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &userId);
        if (userId == IUserHandle::USER_NULL) {
            Logger::E(TAG, "%p sent to %s without EXTRA_USER_HANDLE", intent, TAG.string());
            return NOERROR;
        }

        AutoPtr<IUserInfo> currentUser;
        AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
        if ((Int32)E_REMOTE_EXCEPTION == am->GetCurrentUser((IUserInfo**)&currentUser)) {
            return NOERROR;
        }

        Boolean guest = FALSE;
        if (currentUser->IsGuest(&guest), !guest) {
            return NOERROR;
        }

        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        Int32 notFirstLogin = 0;
        Elastos::Droid::Provider::Settings::System::GetInt32ForUser(cr, SETTING_GUEST_HAS_LOGGED_IN, 0,
                userId, &notFirstLogin);
        if (notFirstLogin != 0) {
            CResetSessionDialog::New(context, userId, (IDialog**)&mNewSessionDialog);
            mNewSessionDialog->Show();
        }
        else {
            Boolean tmp = FALSE;
            Elastos::Droid::Provider::Settings::System::PutInt32ForUser(cr, SETTING_GUEST_HAS_LOGGED_IN,
                1, userId, &tmp);
        }
    }
    return NOERROR;
}

void CGuestResumeSessionReceiver::WipeGuestSession(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> userManager = IUserManager::Probe(obj);
    AutoPtr<IUserInfo> currentUser;
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    if ((Int32)E_REMOTE_EXCEPTION == am->GetCurrentUser((IUserInfo**)&currentUser)) {
        Logger::E(TAG, "Couldn't wipe session because ActivityManager is dead");
        return;
    }

    Int32 id = 0;
    currentUser->GetId(&id);
    if (id != userId) {
        Logger::W(TAG, "User requesting to start a new session (%d) is not current user (%d)", userId, id);
        return;
    }

    Boolean guest = FALSE;
    if (currentUser->IsGuest(&guest), !guest) {
        Logger::W(TAG, "User requesting to start a new session (%d) is not a guest", userId);
        return;
    }

    Boolean marked = FALSE;
    userManager->MarkGuestForDeletion(id, &marked);
    if (!marked) {
        Logger::W(TAG, "Couldn't mark the guest for deletion for user %d", userId);
        return;
    }

    String name;
    currentUser->GetName(&name);
    AutoPtr<IUserInfo> newGuest;
    userManager->CreateGuest(context, name, (IUserInfo**)&newGuest);

    Boolean tmp = FALSE;
    if (newGuest == NULL) {
        Logger::E(TAG, "Could not create new guest, switching back to owner");
        if ((Int32)E_REMOTE_EXCEPTION == am->SwitchUser(IUserHandle::USER_OWNER, &tmp)) {
            Logger::E(TAG, "Couldn't wipe session because ActivityManager or WindowManager is dead");
        }
        userManager->RemoveUser(id, &tmp);
        AutoPtr<IWindowManagerGlobalHelper> wmHelper;
        CWindowManagerGlobalHelper::AcquireSingleton((IWindowManagerGlobalHelper**)&wmHelper);
        AutoPtr<IIWindowManager> wm;
        wmHelper->GetWindowManagerService((IIWindowManager**)&wm);
        wm->LockNow(NULL /* options */);
        return;
    }
    Int32 newId = 0;
    newGuest->GetId(&newId);
    if ((Int32)E_REMOTE_EXCEPTION == am->SwitchUser(newId, &tmp)) {
        Logger::E(TAG, "Couldn't wipe session because ActivityManager or WindowManager is dead");
    }
    userManager->RemoveUser(id, &tmp);
}

void CGuestResumeSessionReceiver::CancelDialog()
{
    Boolean showing = FALSE;
    if (mNewSessionDialog != NULL && (mNewSessionDialog->IsShowing(&showing), showing)) {
        mNewSessionDialog->Cancel();
        mNewSessionDialog = NULL;
    }
}

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
