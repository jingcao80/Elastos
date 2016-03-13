
#include "elastos/droid/systemui/statusbar/policy/UserInfoController.h"
#include "elastos/droid/systemui/BitmapHelper.h"
#include "../../R.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Internal::Utility::CUserIcons;
using Elastos::Droid::Internal::Utility::IUserIcons;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Provider::CContactsContractProfile;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractIntents;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractProfile;
using Elastos::Droid::Utility::CPair;
using Elastos::Droid::Utility::IPair;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::IO::ICloseable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String UserInfoController::TAG("UserInfoController");

UserInfoController::Receiver::Receiver(
    /* [in] */ UserInfoController* host)
    : mHost(host)
{}

ECode UserInfoController::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        mHost->ReloadUserInfo();
    }
    else if (IIntent::ACTION_CONFIGURATION_CHANGED.Equals(action)) {
        if (mHost->mUseDefaultAvatar) {
            mHost->ReloadUserInfo();
        }
    }
    return NOERROR;
}

UserInfoController::ProfileReceiver::ProfileReceiver(
    /* [in] */ UserInfoController* host)
    : mHost(host)
{}

ECode UserInfoController::ProfileReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IContactsContractIntents::ACTION_PROFILE_CHANGED.Equals(action) ||
            IIntent::ACTION_USER_INFO_CHANGED.Equals(action)) {
        ECode ec = NOERROR;
        do {
            Int32 currentUser = 0;
            AutoPtr<IUserInfo> info;
            ec = ActivityManagerNative::GetDefault()->GetCurrentUser((IUserInfo**)&info);
            if (FAILED(ec)) break;

            info->GetId(&currentUser);
            Int32 changedUser = 0, id = 0;
            GetSendingUserId(&id);
            ec = intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, id, &changedUser);
            if (FAILED(ec)) break;
            if (changedUser == currentUser) {
                mHost->ReloadUserInfo();
            }
        } while (/*RemoteException e*/0);

        if (FAILED(ec)) {
            Logger::E(TAG, "Couldn't get current user id for profile change");
        }
    }
    return NOERROR;
}

UserInfoController::UserInfoTask::UserInfoTask(
    /* [in] */ UserInfoController* host,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean isGuest,
    /* [in] */ const String& userName,
    /* [in] */ Int32 avatarSize,
    /* [in] */ IContext* context)
    : mHost(host)
    , mUserId(userId)
    , mIsGuest(isGuest)
    , mUserName(userName)
    , mAvatarSize(avatarSize)
    , mContext(context)
{}

ECode UserInfoController::UserInfoTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    AutoPtr<IUserManagerHelper> helper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
    AutoPtr<IUserManager> um;
    helper->Get(mHost->mContext, (IUserManager**)&um);

    // Fall back to the UserManager nickname if we can't read the name from the local
    // profile below.
    String name = mUserName;
    AutoPtr<IDrawable> avatar;
    AutoPtr<IBitmap> rawAvatar;
    um->GetUserIcon(mUserId, (IBitmap**)&rawAvatar);
    if (rawAvatar != NULL) {
        AutoPtr<IResources> res;
        mHost->mContext->GetResources((IResources**)&res);
        CBitmapDrawable::New(res, BitmapHelper::CreateCircularClip(rawAvatar, mAvatarSize, mAvatarSize)
                , (IDrawable**)&avatar);
    }
    else {
        AutoPtr<IUserIcons> ui;
        CUserIcons::AcquireSingleton((IUserIcons**)&ui);
        ui->GetDefaultUserIcon(mIsGuest? IUserHandle::USER_NULL : mUserId,
                /* light= */ TRUE, (IDrawable**)&avatar);
        mHost->mUseDefaultAvatar = TRUE;
    }

    // If it's a single-user device, get the profile name, since the nickname is not
    // usually valid
    AutoPtr<IList> users;
    um->GetUsers((IList**)&users);
    Int32 size = 0;
    if ((users->GetSize(&size), size) <= 1) {
        // Try and read the display name from the local profile
        AutoPtr<ICursor> cursor;
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IContactsContractProfile> ccp;
        CContactsContractProfile::AcquireSingleton((IContactsContractProfile**)&ccp);
        AutoPtr<IUri> curi;
        ccp->GetCONTENT_URI((IUri**)&curi);
        AutoPtr<ArrayOf<String> > values = ArrayOf<String>::Alloc(2);
        values->Set(0, IBaseColumns::ID);
        values->Set(1, IContactsContractContactsColumns::DISPLAY_NAME);
        cr->Query(curi, values, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
        if (cursor != NULL) {
            // try {
            Boolean has = FALSE;
            if (cursor->MoveToFirst(&has), has) {
                Int32 index = 0;
                cursor->GetColumnIndex(IContactsContractContactsColumns::DISPLAY_NAME, &index);
                cursor->GetString(index, &name);
            }
            // } finally {
            ICloseable::Probe(cursor)->Close();
            // }
        }
    }

    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<IPair> pair;
    CPair::New(cs, avatar, (IPair**)&pair);
    *result = IInterface::Probe(pair);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UserInfoController::UserInfoTask::OnPostExecute(
    /* [in] */ IInterface* /*Pair<String, Drawable>*/ result)
{
    AutoPtr<IPair> pair = IPair::Probe(result);
    AutoPtr<IInterface> first;
    pair->GetFirst((IInterface**)&first);
    ICharSequence::Probe(first)->ToString(&mHost->mUserName);

    AutoPtr<IInterface> second;
    pair->GetSecond((IInterface**)&second);
    mHost->mUserDrawable = IDrawable::Probe(second);
    mHost->mUserInfoTask = NULL;
    mHost->NotifyChanged();
    return NOERROR;
}

CAR_INTERFACE_IMPL(UserInfoController, Object, IUserInfoController);
UserInfoController::UserInfoController(
    /* [in] */ IContext* context)
    : mUseDefaultAvatar(FALSE)
{
    mReceiver = new Receiver(this);
    mProfileReceiver = new ProfileReceiver(this);
    CArrayList::New((IArrayList**)&mCallbacks);
    mContext = context;
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_USER_SWITCHED);
    filter->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);

    AutoPtr<IIntent> i;
    mContext->RegisterReceiver(mReceiver, filter, (IIntent**)&i);

    AutoPtr<IIntentFilter> profileFilter;
    CIntentFilter::New((IIntentFilter**)&profileFilter);
    profileFilter->AddAction(IContactsContractIntents::ACTION_PROFILE_CHANGED);
    profileFilter->AddAction(IIntent::ACTION_USER_INFO_CHANGED);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> all;
    helper->GetALL((IUserHandle**)&all);
    i = NULL;
    mContext->RegisterReceiverAsUser(mProfileReceiver, all, profileFilter,
            String(NULL), NULL, (IIntent**)&i);
}

ECode UserInfoController::AddListener(
    /* [in] */ IUserInfoControllerOnUserInfoChangedListener* callback)
{
    mCallbacks->Add(callback);
    return NOERROR;
}

ECode UserInfoController::ReloadUserInfo()
{
    if (mUserInfoTask != NULL) {
        mUserInfoTask->Cancel(FALSE);
        mUserInfoTask = NULL;
    }
    FAIL_RETURN(QueryForUserInformation());
    return NOERROR;
}

ECode UserInfoController::QueryForUserInformation()
{
    AutoPtr<IContext> currentUserContext;
    AutoPtr<IUserInfo> userInfo;
    ECode ec = NOERROR;
    do {
        ec = ActivityManagerNative::GetDefault()->GetCurrentUser((IUserInfo**)&userInfo);
        if (FAILED(ec)) break;
        AutoPtr<IUserHandle> uh;
        Int32 id = 0;
        userInfo->GetId(&id);
        CUserHandle::New(id, (IUserHandle**)&uh);
        ec = mContext->CreatePackageContextAsUser(String("android"), 0,
                uh, (IContext**)&currentUserContext);
    } while (0);

    if (FAILED(ec)) {
        if (ec == (Int32)E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION) {
            Logger::E(TAG, "Couldn't create user context");
            return E_RUNTIME_EXCEPTION;
        }

        Logger::E(TAG, "Couldn't get user info");
        return E_RUNTIME_EXCEPTION;
    }

    Int32 userId = 0;
    userInfo->GetId(&userId);
    Boolean isGuest = FALSE;
    userInfo->IsGuest(&isGuest);
    String userName;
    userInfo->GetName(&userName);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 avatarSize = 0;
    res->GetDimensionPixelSize(R::dimen::max_avatar_size, &avatarSize);

    AutoPtr<IContext> context = currentUserContext;
    mUserInfoTask = new UserInfoTask(this, userId, isGuest, userName, avatarSize, context);
    mUserInfoTask->Execute((ArrayOf<IInterface*>*)NULL);
    return NOERROR;
}

void UserInfoController::NotifyChanged()
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> listener;
        ator->GetNext((IInterface**)&listener);
        IUserInfoControllerOnUserInfoChangedListener::Probe(listener)->OnUserInfoChanged(mUserName,
                 mUserDrawable);
    }
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
