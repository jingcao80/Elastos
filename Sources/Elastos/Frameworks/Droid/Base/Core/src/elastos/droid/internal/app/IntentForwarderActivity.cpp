
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/internal/app/IntentForwarderActivity.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/widget/Toast.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::Toast;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(IntentForwarderActivity, Activity, IIntentForwarderActivity)

const String IntentForwarderActivity::TAG("IntentForwarderActivity");

ECode IntentForwarderActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    AutoPtr<IIntent> intentReceived;
    GetIntent((IIntent**)&intentReceived);

    AutoPtr<IComponentName> compName;
    intentReceived->GetComponent((IComponentName**)&compName);
    String className;
    compName->GetClassName(&className);
    AutoPtr<IUserHandle> userDest;
    Int32 userMessageId;

    if (className.Equals(FORWARD_INTENT_TO_USER_OWNER)) {
        userMessageId = R::string::forward_intent_to_owner;
        userDest = UserHandle::OWNER;
    }
    else if (className.Equals(FORWARD_INTENT_TO_MANAGED_PROFILE)) {
        userMessageId = R::string::forward_intent_to_work;
        userDest = GetManagedProfile();
    }
    else {
        Slogger::W(TAG, "IntentForwarderActivity cannot be called directly");
        userMessageId = -1;
        userDest = NULL;
    }
    if (userDest == NULL) { // This covers the case where there is no managed profile.
        Finish();
        return NOERROR;
    }
    AutoPtr<IIntent> newIntent;
    CIntent::New(intentReceived, (IIntent**)&newIntent);
    newIntent->SetComponent(NULL);
    // Apps should not be allowed to target a specific package in the target user.
    newIntent->SetPackage(String(NULL));
    newIntent->AddFlags(IIntent::FLAG_ACTIVITY_FORWARD_RESULT
        | IIntent::FLAG_ACTIVITY_PREVIOUS_IS_TOP);
    Int32 callingUserId;
    GetUserId(&callingUserId);
    AutoPtr<IIPackageManager> ipm = AppGlobals::GetPackageManager();
    AutoPtr<IContentResolver> cr;
    GetContentResolver((IContentResolver**)&cr);
    String resolvedType;
    newIntent->ResolveTypeIfNeeded(cr, &resolvedType);
    Boolean canForward = FALSE;
    AutoPtr<IIntent> selector;
    newIntent->GetSelector((IIntent**)&selector);
    if (selector == NULL) {
        selector = newIntent;
    }

    Int32 id;
    userDest->GetIdentifier(&id);
    if (FAILED(ipm->CanForwardTo(selector, resolvedType, callingUserId, id, &canForward))) {
        Slogger::E(TAG, "PackageManagerService is dead?");
    }

    if (canForward) {
        newIntent->SetContentUserHint(callingUserId);

        AutoPtr<IPackageManager> pm;
        GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IResolveInfo> ri;
        pm->ResolveActivityAsUser(newIntent, IPackageManager::MATCH_DEFAULT_ONLY,
            id, (IResolveInfo**)&ri);

        AutoPtr<IActivityInfo> ai;
        ri->GetActivityInfo((IActivityInfo**)&ai);
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(ai)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 uid;
        appInfo->GetUid(&uid);
        // Only show a disclosure if this is a normal (non-OS) app
        Boolean shouldShowDisclosure = !UserHandle::IsSameApp(uid, IProcess::SYSTEM_UID);

        if (FAILED(StartActivityAsCaller(newIntent, NULL, id))) {
            Int32 launchedFromUid = -1;
            String launchedFromPackage("?");
            AutoPtr<IBinder> token;
            GetActivityToken((IBinder**)&token);
            ActivityManagerNative::GetDefault()->GetLaunchedFromUid(token, &launchedFromUid);
            ActivityManagerNative::GetDefault()->GetLaunchedFromPackage(token, &launchedFromPackage);

            Slogger::W(TAG, "Unable to launch as UID %d package %s, while running in %s",
                launchedFromUid, launchedFromPackage.string(),
                CActivityThread::GetCurrentProcessName().string());
        }

        if (shouldShowDisclosure) {
            AutoPtr<ICharSequence> text;
            GetText(userMessageId, (ICharSequence**)&text);
            AutoPtr<IToast> toast;
            Toast::MakeText(this, text, IToast::LENGTH_LONG, (IToast**)&toast);
            toast->Show();
        }
    }
    else {
        String str;
        IObject::Probe(newIntent)->ToString(&str);
        Slogger::W(TAG, "the intent: %s cannot be forwarded from user %d to user %d",
            str.string(), callingUserId, id);
    }
    Finish();
    return NOERROR;
}

AutoPtr<IUserHandle> IntentForwarderActivity::GetManagedProfile()
{
    AutoPtr<IInterface> service;
    GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
    AutoPtr<IUserManager> userManager = IUserManager::Probe(service) ;
    AutoPtr<IList> relatedUsers;
    userManager->GetProfiles(UserHandle::USER_OWNER, (IList**)&relatedUsers);
    Int32 size;
    relatedUsers->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        relatedUsers->Get(i, (IInterface**)&item);
        AutoPtr<IUserInfo> userInfo = IUserInfo::Probe(item);
        Boolean isManagedProfile;
        userInfo->IsManagedProfile(&isManagedProfile);
        if (isManagedProfile) {
            Int32 id;
            userInfo->GetId(&id);
            AutoPtr<IUserHandle> uh;
            CUserHandle::New(id, (IUserHandle**)&uh);
            return uh;
        }

    }
    Slogger::W(TAG, "%s has been called, but there is no managed profile",
        FORWARD_INTENT_TO_MANAGED_PROFILE.string());
    return NULL;
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
