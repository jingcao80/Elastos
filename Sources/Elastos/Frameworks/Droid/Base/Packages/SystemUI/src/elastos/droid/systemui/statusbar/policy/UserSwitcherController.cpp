
#include "elastos/droid/systemui/statusbar/policy/UserSwitcherController.h"
#include "elastos/droid/systemui/BitmapHelper.h"
#include "elastos/droid/systemui/CGuestResumeSessionReceiver.h"
#include "../../R.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/droid/app/ActivityManagerNative.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/droid/R.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Internal::Utility::CUserIcons;
using Elastos::Droid::Internal::Utility::IUserIcons;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::SystemUI::BitmapHelper;
using Elastos::Droid::SystemUI::CGuestResumeSessionReceiver;
using Elastos::Droid::SystemUI::Qs::EIID_IQSTileDetailAdapter;
using Elastos::Droid::SystemUI::Qs::Tiles::IUserDetailView;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String UserSwitcherController::TAG("UserSwitcherController");
const Boolean UserSwitcherController::DEBUG = FALSE;
const String UserSwitcherController::SIMPLE_USER_SWITCHER_GLOBAL_SETTING("lockscreenSimpleUserSwitcher");

CAR_INTERFACE_IMPL(UserSwitcherController::BaseUserAdapter, BaseAdapter, IUserSwitcherControllerBaseUserAdapter)
UserSwitcherController::BaseUserAdapter::BaseUserAdapter(
    /* [in] */ UserSwitcherController* controller)
{
    mController = controller;
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    controller->mAdapters->Add(wr);
}

ECode UserSwitcherController::BaseUserAdapter::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean v1 = FALSE, v2 = FALSE;
    Boolean secureKeyguardShowing = (mController->mKeyguardMonitor->IsShowing(&v1), v1)
            && (mController->mKeyguardMonitor->IsSecure(&v2), v2);
    if (!secureKeyguardShowing) {
        return mController->mUsers->GetSize(result);
    }
    // The lock screen is secure and showing. Filter out restricted records.
    Int32 N = 0;
    mController->mUsers->GetSize(&N);
    Int32 count = 0;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mController->mUsers->Get(i, (IInterface**)&obj);
        if (((UserRecord*)IUserSwitcherControllerUserRecord::Probe(obj))->mIsRestricted) {
            break;
        }
        else {
            count++;
        }
    }
    *result = count;
    return NOERROR;
}

ECode UserSwitcherController::BaseUserAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    return mController->mUsers->Get(position, item);
}

ECode UserSwitcherController::BaseUserAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = position;
    return NOERROR;
}

ECode UserSwitcherController::BaseUserAdapter::SwitchTo(
    /* [in] */ IUserSwitcherControllerUserRecord* record)
{
    mController->SwitchTo(record);
    return NOERROR;
}

ECode UserSwitcherController::BaseUserAdapter::GetName(
    /* [in] */ IContext* context,
    /* [in] */ IUserSwitcherControllerUserRecord* _item,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    UserRecord* item = (UserRecord*)_item;
    if (item->mIsGuest) {
        if (item->mIsCurrent) {
            return context->GetString(R::string::guest_exit_guest, name);
        }
        else {
            return context->GetString(
                item->mInfo == NULL ? R::string::guest_new_guest : R::string::guest_nickname, name);
        }
    }
    else if (item->mIsAddUser) {
        return context->GetString(R::string::user_add_user, name);
    }
    else {
        return item->mInfo->GetName(name);
    }
    return NOERROR;
}

ECode UserSwitcherController::BaseUserAdapter::GetSwitchableUsers(
    /* [out] */ Int32* user)
{
    VALIDATE_NOT_NULL(user);
    Int32 result = 0;
    AutoPtr<IArrayList> users = mController->mUsers;  /*<UserRecord*/
    Int32 N = 0;
    users->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        users->Get(i, (IInterface**)&obj);
        if (((UserRecord*)IUserSwitcherControllerUserRecord::Probe(obj))->mInfo != NULL) {
            result++;
        }
    }
    *user = result;
    return NOERROR;
}

ECode UserSwitcherController::BaseUserAdapter::GetDrawable(
    /* [in] */ IContext* context,
    /* [in] */ IUserSwitcherControllerUserRecord* _item,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    UserRecord* item = (UserRecord*)_item;
    if (item->mIsAddUser) {
        return context->GetDrawable(R::drawable::ic_add_circle_qs, drawable);
    }

    AutoPtr<IUserIcons> ui;
    CUserIcons::AcquireSingleton((IUserIcons**)&ui);
    Int32 id = 0;
    item->mInfo->GetId(&id);
    return ui->GetDefaultUserIcon(item->mIsGuest ? IUserHandle::USER_NULL : id,
            /* light= */ TRUE, drawable);
}

CAR_INTERFACE_IMPL(UserSwitcherController::UserRecord, Object, IUserSwitcherControllerUserRecord)
UserSwitcherController::UserRecord::UserRecord(
    /* [in] */ IUserInfo* info,
    /* [in] */ IBitmap* picture,
    /* [in] */ Boolean isGuest,
    /* [in] */ Boolean isCurrent,
    /* [in] */ Boolean isAddUser,
    /* [in] */ Boolean isRestricted)
    : mInfo(info)
    , mPicture(picture)
    , mIsGuest(isGuest)
    , mIsCurrent(isCurrent)
    , mIsAddUser(isAddUser)
    , mIsRestricted(isRestricted)
{
}

ECode UserSwitcherController::UserRecord::CopyWithIsCurrent(
    /* [in] */ Boolean _isCurrent,
    /* [out] */ IUserSwitcherControllerUserRecord** result)
{
    VALIDATE_NOT_NULL(result);
    *result = new UserRecord(mInfo, mPicture, mIsGuest, _isCurrent, mIsAddUser, mIsRestricted);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UserSwitcherController::UserRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("UserRecord(");
    if (mInfo != NULL) {
        String name;
        mInfo->GetName(&name);
        Int32 id = 0;
        mInfo->GetId(&id);
        sb.Append(String("name=\"") + name + "\" id=" + StringUtils::ToString(id));
    }
    else {
        if (mIsGuest) {
            sb.Append("<add guest placeholder>");
        }
        else if (mIsAddUser) {
            sb.Append("<add user placeholder>");
        }
    }
    if (mIsGuest) sb.Append(" <isGuest>");
    if (mIsAddUser) sb.Append(" <isAddUser>");
    if (mIsCurrent) sb.Append(" <isCurrent>");
    if (mPicture != NULL) sb.Append(" <hasPicture>");
    if (mIsRestricted) sb.Append(" <isRestricted>");
    sb.AppendChar(')');
    return sb.ToString(str);
}

CAR_INTERFACE_IMPL(UserSwitcherController::ExitGuestDialog, SystemUIDialog, IDialogInterfaceOnClickListener)
UserSwitcherController::ExitGuestDialog::ExitGuestDialog(
    /* [in] */ IContext* context,
    /* [in] */ Int32 guestId,
    /* [in] */ UserSwitcherController* host)
    : mGuestId(0)
    , mHost(host)
{
    SystemUIDialog::constructor(context);
    SetTitle(R::string::guest_exit_guest_dialog_title);
    String value;
    context->GetString(R::string::guest_exit_guest_dialog_message, &value);
    AutoPtr<ICharSequence> cs;
    CString::New(value, (ICharSequence**)&cs);
    SetMessage(cs);

    context->GetString(Elastos::Droid::R::string::cancel, &value);
    cs = NULL;
    CString::New(value, (ICharSequence**)&cs);
    SetButton(IDialogInterface::BUTTON_NEGATIVE, cs, this);

    context->GetString(R::string::guest_exit_guest_dialog_remove, &value);
    cs = NULL;
    CString::New(value, (ICharSequence**)&cs);
    SetButton(IDialogInterface::BUTTON_POSITIVE, cs, this);
    SetCanceledOnTouchOutside(FALSE);
    mGuestId = guestId;
}

ECode UserSwitcherController::ExitGuestDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which == BUTTON_NEGATIVE) {
        Cancel();
    }
    else {
        Dismiss();
        mHost->ExitGuest(mGuestId);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(UserSwitcherController::AddUserDialog, SystemUIDialog, IDialogInterfaceOnClickListener)
UserSwitcherController::AddUserDialog::AddUserDialog(
    /* [in] */ IContext* context,
    /* [in] */ UserSwitcherController* host)
    : mHost(host)
{
    SystemUIDialog::constructor(context);
    SetTitle(R::string::user_add_user_title);

    String value;
    context->GetString(R::string::user_add_user_message_short, &value);
    AutoPtr<ICharSequence> cs;
    CString::New(value, (ICharSequence**)&cs);
    SetMessage(cs);
    context->GetString(Elastos::Droid::R::string::cancel, &value);

    cs = NULL;
    CString::New(value, (ICharSequence**)&cs);
    SetButton(IDialogInterface::BUTTON_NEGATIVE, cs, this);
    context->GetString(Elastos::Droid::R::string::ok, &value);

    cs = NULL;
    CString::New(value, (ICharSequence**)&cs);
    SetButton(IDialogInterface::BUTTON_POSITIVE, cs, this);
}

ECode UserSwitcherController::AddUserDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which == BUTTON_NEGATIVE) {
        Cancel();
    }
    else {
        Dismiss();
        AutoPtr<IActivityManagerHelper> helper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
        Boolean tmp = FALSE;
        if (helper->IsUserAMonkey(&tmp), tmp) {
            return NOERROR;
        }
        String value;
        mHost->mContext->GetString(R::string::user_new_user_name, &value);
        AutoPtr<IUserInfo> user;
        mHost->mUserManager->CreateSecondaryUser(
                value, 0 /* flags */, (IUserInfo**)&user);
        if (user == NULL) {
            // Couldn't create user, most likely because there are too many, but we haven't
            // been able to reload the list yet.
            return NOERROR;
        }
        Int32 id = 0;
        user->GetId(&id);
        AutoPtr<IUserIcons> ui;
        CUserIcons::AcquireSingleton((IUserIcons**)&ui);
        AutoPtr<IDrawable> d;
        ui->GetDefaultUserIcon(id, /* light= */ FALSE, (IDrawable**)&d);
        AutoPtr<IBitmap> icon;
        ui->ConvertToBitmap(d, (IBitmap**)&icon);
        mHost->mUserManager->SetUserIcon(id, icon);
        mHost->SwitchToUserId(id);
    }
    return NOERROR;
}

UserSwitcherController::Receiver::Receiver(
    /* [in] */ UserSwitcherController* host)
    : mHost(host)
{}

ECode UserSwitcherController::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (DEBUG) {
        Int32 value = 0;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &value);
        Logger::V(TAG, "Broadcast: a=%s user=%d", action.string(), value);
    }
    if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        Boolean tmp = FALSE;
        if (mHost->mExitGuestDialog != NULL && (mHost->mExitGuestDialog->IsShowing(&tmp), tmp)) {
            mHost->mExitGuestDialog->Cancel();
            mHost->mExitGuestDialog = NULL;
        }

        Int32 currentId = 0;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, -1, &currentId);
        Int32 N = 0;
        mHost->mUsers->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mHost->mUsers->Get(i, (IInterface**)&obj);
            UserRecord* record = (UserRecord*)IUserSwitcherControllerUserRecord::Probe(obj);
            if (record->mInfo == NULL) continue;
            Int32 id = 0;
            record->mInfo->GetId(&id);
            Boolean shouldBeCurrent = id == currentId;
            if (record->mIsCurrent != shouldBeCurrent) {
                AutoPtr<IUserSwitcherControllerUserRecord> uscu;
                record->CopyWithIsCurrent(shouldBeCurrent, (IUserSwitcherControllerUserRecord**)&uscu);
                mHost->mUsers->Set(i, uscu);
            }
            if (shouldBeCurrent && !record->mIsGuest) {
                mHost->mLastNonGuestUser = id;
            }
            if (currentId != IUserHandle::USER_OWNER && record->mIsRestricted) {
                // Immediately remove restricted records in case the AsyncTask is too slow.
                mHost->mUsers->Remove(i);
                i--;
            }
        }
        mHost->NotifyAdapters();
    }
    Int32 forcePictureLoadForId = IUserHandle::USER_NULL;
    if (IIntent::ACTION_USER_INFO_CHANGED.Equals(action)) {
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE,
                IUserHandle::USER_NULL, &forcePictureLoadForId);
    }
    mHost->RefreshUsers(forcePictureLoadForId);
    return NOERROR;
}

UserSwitcherController::SettingsObserver::SettingsObserver(
    /* [in] */ UserSwitcherController* host)
    : mHost(host)
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    ContentObserver::constructor(handler);
}

ECode UserSwitcherController::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    mHost->mSimpleUserSwitcher = Elastos::Droid::Provider::Settings::Global::GetInt32(cr,
            SIMPLE_USER_SWITCHER_GLOBAL_SETTING, 0) != 0;
    mHost->mAddUsersWhenLocked = Elastos::Droid::Provider::Settings::Global::GetInt32(cr,
            ISettingsGlobal::ADD_USERS_WHEN_LOCKED, 0) != 0;
    mHost->RefreshUsers(IUserHandle::USER_NULL);
    return NOERROR;
}

CAR_INTERFACE_IMPL(UserSwitcherController::UserDetailAdapter, Object, IQSTileDetailAdapter)
UserSwitcherController::UserDetailAdapter::UserDetailAdapter(
    /* [in] */ UserSwitcherController* host)
    : mHost(host)
{
    CIntent::New(String("android.settings.USER_SETTINGS"), (IIntent**)&USER_SETTINGS_INTENT);
}

ECode UserSwitcherController::UserDetailAdapter::GetTitle(
    /* [out] */ Int32* title)
{
    VALIDATE_NOT_NULL(title);
    *title = R::string::quick_settings_user_title;
    return NOERROR;
}

ECode UserSwitcherController::UserDetailAdapter::CreateDetailView(
    /* [in] */ IContext* context,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IUserDetailView> v;
    if (IUserDetailView::Probe(convertView) == NULL) {
        assert(0 && "TODO");
        // v = CUserDetailView::Inflate(context, parent, FALSE);
        v->CreateAndSetAdapter(mHost);
    }
    else {
        v = IUserDetailView::Probe(convertView);
    }
    *view = IView::Probe(v);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode UserSwitcherController::UserDetailAdapter::GetSettingsIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    *intent = USER_SETTINGS_INTENT;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode UserSwitcherController::UserDetailAdapter::GetToggleState(
    /* [out] */ IBoolean** state)
{
    VALIDATE_NOT_NULL(state);
    *state = NULL;
    return NOERROR;
}

ECode UserSwitcherController::UserDetailAdapter::SetToggleState(
    /* [in] */ IBoolean* state)
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(UserSwitcherController::KeyguardMonitorCallback, Object, IKeyguardMonitorCallback)
UserSwitcherController::KeyguardMonitorCallback::KeyguardMonitorCallback(
    /* [in] */ UserSwitcherController* host)
    : mHost(host)
{}

ECode UserSwitcherController::KeyguardMonitorCallback::OnKeyguardChanged()
{
    mHost->NotifyAdapters();
    return NOERROR;
}

UserSwitcherController::UserAsyncTask::UserAsyncTask(
    /* [in] */ UserSwitcherController* host,
    /* [in] */ Boolean addUsersWhenLocked)
    : mHost(host)
    , mAddUsersWhenLocked(addUsersWhenLocked)
{}

ECode UserSwitcherController::UserAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISparseArray> bitmaps = ISparseArray::Probe((*params)[0]);  /*<Bitmap*/
    AutoPtr<IList> infos;  /*<UserInfo*/
    mHost->mUserManager->GetUsers(TRUE, (IList**)&infos);
    if (infos == NULL) {
        *result = NULL;
        return NOERROR;
    }

    Int32 size = 0;
    infos->GetSize(&size);
    AutoPtr<IArrayList> records;  /*<UserRecord*/
    CArrayList::New(size, (IArrayList**)&records);

    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 currentId = 0;
    helper->GetCurrentUser(&currentId);
    AutoPtr<IUserSwitcherControllerUserRecord> guestRecord;

    AutoPtr<IResources> res;
    mHost->mContext->GetResources((IResources**)&res);
    Int32 avatarSize = 0;
    res->GetDimensionPixelSize(R::dimen::max_avatar_size, &avatarSize);

    AutoPtr<IIterator> ator;
    infos->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<IUserInfo> info = IUserInfo::Probe(obj);

        Int32 id = 0;
        info->GetId(&id);
        Boolean isCurrent = currentId == id;
        Boolean tmp = FALSE;
        if (info->IsGuest(&tmp), tmp) {
            guestRecord = new UserRecord(info, NULL /* picture */,
                    TRUE /* isGuest */, isCurrent, FALSE /* isAddUser */,
                    FALSE /* isRestricted */);
        }
        else if (info->SupportsSwitchTo(&tmp), tmp) {
            AutoPtr<IInterface> o;
            bitmaps->Get(id, (IInterface**)&o);
            AutoPtr<IBitmap> picture = IBitmap::Probe(o);
            if (picture == NULL) {
                mHost->mUserManager->GetUserIcon(id, (IBitmap**)&picture);
            }
            if (picture != NULL) {
                picture = BitmapHelper::CreateCircularClip(
                        picture, avatarSize, avatarSize);
            }
            Int32 s = 0;
            Int32 index = isCurrent ? 0 : (records->GetSize(&s), s);

            AutoPtr<IUserSwitcherControllerUserRecord> uc = new UserRecord(info, picture, FALSE /* isGuest */,
                    isCurrent, FALSE /* isAddUser */, FALSE /* isRestricted */);
            records->Add(index, uc);
        }
    }

    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    AutoPtr<IUserHandle> uh;
    uhHelper->GetOWNER((IUserHandle**)&uh);
    Boolean tmp = FALSE;
    mHost->mUserManager->HasUserRestriction(IUserManager::DISALLOW_ADD_USER, uh, &tmp);
    Boolean ownerCanCreateUsers = !tmp;
    Boolean currentUserCanCreateUsers =
            (currentId == IUserHandle::USER_OWNER) && ownerCanCreateUsers;
    Boolean anyoneCanCreateUsers = ownerCanCreateUsers && mAddUsersWhenLocked;
    Boolean canCreateGuest = (currentUserCanCreateUsers || anyoneCanCreateUsers)
            && guestRecord == NULL;
    Boolean canCreateUser = (currentUserCanCreateUsers || anyoneCanCreateUsers)
            && (mHost->mUserManager->CanAddMoreUsers(&tmp), tmp);
    Boolean createIsRestricted = !mAddUsersWhenLocked;

    if (!mHost->mSimpleUserSwitcher) {
        if (guestRecord == NULL) {
            if (canCreateGuest) {
                AutoPtr<IUserSwitcherControllerUserRecord> uc = new UserRecord(NULL /* info */,
                        NULL /* picture */, TRUE /* isGuest */, FALSE /* isCurrent */,
                        FALSE /* isAddUser */, createIsRestricted);
                records->Add(uc);
            }
        }
        else {
            Int32 s = 0;
            Int32 index = ((UserRecord*)guestRecord.Get())->mIsCurrent ? 0 : (records->GetSize(&s), s);
            records->Add(index, guestRecord);
        }
    }

    if (!mHost->mSimpleUserSwitcher && canCreateUser) {
        AutoPtr<IUserSwitcherControllerUserRecord> uc = new UserRecord(NULL /* info */, NULL /* picture */,
                FALSE /* isGuest */, FALSE /* isCurrent */, TRUE /* isAddUser */,
                createIsRestricted);
        records->Add(uc);
    }

    *result = records;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode UserSwitcherController::UserAsyncTask::OnPostExecute(
    /* [in] */ /*ArrayList<UserRecord>*/IInterface* userRecords)
{
    if (userRecords != NULL) {
        mHost->mUsers = IArrayList::Probe(userRecords);
        mHost->NotifyAdapters();
    }
    return NOERROR;
}


CAR_INTERFACE_IMPL(UserSwitcherController, Object, IUserSwitcherController)
UserSwitcherController::UserSwitcherController(
    /* [in] */ IContext* context,
    /* [in] */ IKeyguardMonitor* keyguardMonitor)
    : mLastNonGuestUser(IUserHandle::USER_OWNER)
    , mSimpleUserSwitcher(FALSE)
    , mAddUsersWhenLocked(FALSE)
{
    CArrayList::New((IArrayList**)&mAdapters);
    CGuestResumeSessionReceiver::New((IGuestResumeSessionReceiver**)&mGuestResumeSessionReceiver);
    CArrayList::New((IArrayList**)&mUsers);
    mReceiver = new Receiver(this);
    mSettingsObserver = new SettingsObserver(this);
    mUserDetailAdapter = new UserDetailAdapter(this);
    mCallback = new KeyguardMonitorCallback(this);
    mContext = context;
    mGuestResumeSessionReceiver->Register(context);
    mKeyguardMonitor = keyguardMonitor;

    AutoPtr<IUserManagerHelper> helper;
    CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
    helper->Get(context, (IUserManager**)&mUserManager);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_USER_ADDED);
    filter->AddAction(IIntent::ACTION_USER_REMOVED);
    filter->AddAction(IIntent::ACTION_USER_INFO_CHANGED);
    filter->AddAction(IIntent::ACTION_USER_SWITCHED);
    filter->AddAction(IIntent::ACTION_USER_STOPPING);

    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    AutoPtr<IUserHandle> uh;
    uhHelper->GetOWNER((IUserHandle**)&uh);
    AutoPtr<IIntent> i;
    mContext->RegisterReceiverAsUser(mReceiver, uh, filter,
            String(NULL) /* permission */, NULL /* scheduler */, (IIntent**)&i);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uri;
    Elastos::Droid::Provider::Settings::Global::GetUriFor(SIMPLE_USER_SWITCHER_GLOBAL_SETTING, (IUri**)&uri);
    cr->RegisterContentObserver(uri, TRUE, mSettingsObserver);

    uri = NULL;
    Elastos::Droid::Provider::Settings::Global::GetUriFor(ISettingsGlobal::ADD_USERS_WHEN_LOCKED, (IUri**)&uri);
    cr->RegisterContentObserver(uri, TRUE, mSettingsObserver);
    // Fetch initial values.
    mSettingsObserver->OnChange(FALSE);

    keyguardMonitor->AddCallback(mCallback);

    RefreshUsers(IUserHandle::USER_NULL);
}

void UserSwitcherController::RefreshUsers(
    /* [in] */ Int32 forcePictureLoadForId)
{
    AutoPtr<ISparseArray> bitmaps;  /*<Bitmap*/
    Int32 size = 0;
    mUsers->GetSize(&size);
    CSparseArray::New(size, (ISparseArray**)&bitmaps);
    const Int32 N = size;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mUsers->Get(i, (IInterface**)&obj);
        UserRecord* r = (UserRecord*)IUserSwitcherControllerUserRecord::Probe(obj);
        Int32 id = 0;
        if (r == NULL || r->mInfo == NULL
                || (r->mInfo->GetId(&id), id) == forcePictureLoadForId || r->mPicture == NULL) {
            continue;
        }
        r->mInfo->GetId(&id);
        bitmaps->Put(id, r->mPicture);
    }

    const Boolean addUsersWhenLocked = mAddUsersWhenLocked;
    AutoPtr<UserAsyncTask> task = new UserAsyncTask(this, addUsersWhenLocked);

    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    array->Set(0, bitmaps);
    task->Execute(array);
}

void UserSwitcherController::NotifyAdapters()
{
    Int32 size = 0;
    mAdapters->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> wr;
        mAdapters->Get(i, (IInterface**)&wr);
        AutoPtr<IInterface> obj;
        IWeakReference::Probe(wr)->Resolve(EIID_IInterface, (IInterface**)&obj);
        AutoPtr<BaseUserAdapter> adapter = (BaseUserAdapter*)IUserSwitcherControllerBaseUserAdapter::Probe(obj);
        if (adapter != NULL) {
            adapter->NotifyDataSetChanged();
        }
        else {
            mAdapters->Remove(i);
        }
    }
}

ECode UserSwitcherController::IsSimpleUserSwitcher(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSimpleUserSwitcher;
    return NOERROR;
}

ECode UserSwitcherController::SwitchTo(
    /* [in] */ IUserSwitcherControllerUserRecord* _record)
{
    Int32 id = 0;
    UserRecord* record = (UserRecord*)_record;
    if (record->mIsGuest && record->mInfo == NULL) {
        // No guest user. Create one.
        String value;
        mContext->GetString(R::string::guest_nickname, &value);
        AutoPtr<IUserInfo> guest;
        mUserManager->CreateGuest(mContext, value, (IUserInfo**)&guest);
        if (guest == NULL) {
            // Couldn't create guest, most likely because there already exists one, we just
            // haven't reloaded the user list yet.
            return NOERROR;
        }
        guest->GetId(&id);
    }
    else if (record->mIsAddUser) {
        ShowAddUserDialog();
        return NOERROR;
    }
    else {
        record->mInfo->GetId(&id);
    }

    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 currentId = 0;
    helper->GetCurrentUser(&currentId);
    if (currentId == id) {
        if (record->mIsGuest) {
            ShowExitGuestDialog(id);
        }
        return NOERROR;
    }

    SwitchToUserId(id);
    return NOERROR;
}

void UserSwitcherController::SwitchToUserId(
    /* [in] */ Int32 id)
{
    Boolean tmp = FALSE;
    if (FAILED(ActivityManagerNative::GetDefault()->SwitchUser(id, &tmp))) {
        Logger::E(TAG, "Couldn't switch user.");
    }
}

void UserSwitcherController::ShowExitGuestDialog(
    /* [in] */ Int32 id)
{
    Boolean tmp = FALSE;
    if (mExitGuestDialog != NULL && (mExitGuestDialog->IsShowing(&tmp), tmp)) {
        mExitGuestDialog->Cancel();
    }
    mExitGuestDialog = new ExitGuestDialog(mContext, id, this);
    mExitGuestDialog->Show();
}

void UserSwitcherController::ShowAddUserDialog()
{
    Boolean tmp = FALSE;
    if (mAddUserDialog != NULL && (mAddUserDialog->IsShowing(&tmp), tmp)) {
        mAddUserDialog->Cancel();
    }
    mAddUserDialog = new AddUserDialog(mContext, this);
    mAddUserDialog->Show();
}

void UserSwitcherController::ExitGuest(
    /* [in] */ Int32 id)
{
    Int32 newId = IUserHandle::USER_OWNER;
    if (mLastNonGuestUser != IUserHandle::USER_OWNER) {
        AutoPtr<IUserInfo> info;
        mUserManager->GetUserInfo(mLastNonGuestUser, (IUserInfo**)&info);
        Boolean tmp = FALSE;
        if (info != NULL && (info->IsEnabled(&tmp), tmp) && (info->SupportsSwitchTo(&tmp), tmp)) {
            info->GetId(&newId);
        }
    }
    SwitchToUserId(newId);
    Boolean tmp = FALSE;
    mUserManager->RemoveUser(id, &tmp);
}

ECode UserSwitcherController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("UserSwitcherController state:"));
    pw->Println(String("  mLastNonGuestUser=") + StringUtils::ToString(mLastNonGuestUser));
    pw->Print(String("  mUsers.size="));
    Int32 size = 0;
    mUsers->GetSize(&size);
    pw->Println(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mUsers->Get(i, (IInterface**)&obj);
        UserRecord* u = (UserRecord*)IUserSwitcherControllerUserRecord::Probe(obj);
        pw->Print(String("    "));
        String s;
        u->ToString(&s);
        pw->Println(s);
    }
    return NOERROR;
}

ECode UserSwitcherController::GetCurrentUserName(
    /* [in] */ IContext* context,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    Boolean empty = FALSE;
    mUsers->IsEmpty(&empty);
    if (empty) {
        *name = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    mUsers->Get(0, (IInterface**)&obj);
    UserRecord* item = (UserRecord*)IUserSwitcherControllerUserRecord::Probe(obj);
    if (item == NULL || item->mInfo == NULL) {
        *name = NULL;
        return NOERROR;
    }
    if (item->mIsGuest) {
        return context->GetString(R::string::guest_nickname, name);
    }
    return item->mInfo->GetName(name);
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
