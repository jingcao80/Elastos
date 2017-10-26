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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_USERSWITCHERCONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_USERSWITCHERCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/statusbar/phone/SystemUIDialog.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/AsyncTask.h>
#include <elastos/droid/widget/BaseAdapter.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::SystemUI::Qs::IQSTileDetailAdapter;
using Elastos::Droid::SystemUI::StatusBar::Phone::SystemUIDialog;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Core::IBoolean;
using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

/**
 * Keeps a list of all users on the device for user switching.
 */
class UserSwitcherController
    : public Object
    , public IUserSwitcherController
{
public:
    class BaseUserAdapter
        : public BaseAdapter
        , public IUserSwitcherControllerBaseUserAdapter
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI GetCount(
            /* [out] */ Int32* result);

        // @Override
        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        // @Override
        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* id);

        CARAPI SwitchTo(
            /* [in] */ IUserSwitcherControllerUserRecord* record);

        CARAPI GetName(
            /* [in] */ IContext* context,
            /* [in] */ IUserSwitcherControllerUserRecord* item,
            /* [out] */ String* name);

        CARAPI GetSwitchableUsers(
            /* [out] */ Int32* user);

        CARAPI GetDrawable(
            /* [in] */ IContext* context,
            /* [in] */ IUserSwitcherControllerUserRecord* item,
            /* [out] */ IDrawable** drawable);

    protected:
        BaseUserAdapter(
            /* [in] */ UserSwitcherController* controller);

    private:
        AutoPtr<UserSwitcherController> mController;
    };

    class UserRecord
        : public Object
        , public IUserSwitcherControllerUserRecord
    {
    public:
        CAR_INTERFACE_DECL()

        UserRecord(
            /* [in] */ IUserInfo* info,
            /* [in] */ IBitmap* picture,
            /* [in] */ Boolean isGuest,
            /* [in] */ Boolean isCurrent,
            /* [in] */ Boolean isAddUser,
            /* [in] */ Boolean isRestricted);

        CARAPI CopyWithIsCurrent(
            /* [in] */ Boolean _isCurrent,
            /* [out] */ IUserSwitcherControllerUserRecord** result);

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        AutoPtr<IUserInfo> mInfo;
        AutoPtr<IBitmap> mPicture;
        Boolean mIsGuest;
        Boolean mIsCurrent;
        Boolean mIsAddUser;
        /** If TRUE, the record is only visible to the owner and only when unlocked. */
        Boolean mIsRestricted;
    };

    class SettingsObserver: public ContentObserver
    {
    public:
        CARAPI constructor(
            /* [in] */ IUserSwitcherController* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        UserSwitcherController* mHost;
    };

private:
    class ExitGuestDialog
        : public SystemUIDialog
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ExitGuestDialog(
            /* [in] */ IContext* context,
            /* [in] */ Int32 guestId,
            /* [in] */ UserSwitcherController* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        Int32 mGuestId;
        UserSwitcherController* mHost;
    };

    class AddUserDialog
        : public SystemUIDialog
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        AddUserDialog(
            /* [in] */ IContext* context,
            /* [in] */ UserSwitcherController* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        UserSwitcherController* mHost;
    };

    class Receiver: public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("UserSwitcherController::Receiver")

        Receiver(
            /* [in] */ UserSwitcherController* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        UserSwitcherController* mHost;
    };

    class UserDetailAdapter
        : public Object
        , public IQSTileDetailAdapter
    {
    public:
        CAR_INTERFACE_DECL()

        UserDetailAdapter(
            /* [in] */ UserSwitcherController* host);

        // @Override
        CARAPI GetTitle(
            /* [out] */ Int32* title);

        // @Override
        CARAPI CreateDetailView(
            /* [in] */ IContext* context,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        // @Override
        CARAPI GetSettingsIntent(
            /* [out] */ IIntent** intent);

        // @Override
        CARAPI GetToggleState(
            /* [out] */ IBoolean** state);

        // @Override
        CARAPI SetToggleState(
            /* [in] */ IBoolean* state);

    private:
        UserSwitcherController* mHost;
        AutoPtr<IIntent> USER_SETTINGS_INTENT;
    };

    class KeyguardMonitorCallback
        : public Object
        , public IKeyguardMonitorCallback
    {
    public:
        CAR_INTERFACE_DECL()

        KeyguardMonitorCallback(
            /* [in] */ UserSwitcherController* host);

        // @Override
        CARAPI OnKeyguardChanged();

    private:
        UserSwitcherController* mHost;
    };

    class UserAsyncTask: public AsyncTask
    {
    public:
        UserAsyncTask(
            /* [in] */ UserSwitcherController* host,
            /* [in] */ Boolean addUsersWhenLocked);

    protected:
        // @SuppressWarnings("unchecked")
        // @Override
        /*ArrayList<UserRecord>*/CARAPI DoInBackground(
            /* [in] */ /*SparseArray<Bitmap>...*/ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        // @Override
        CARAPI OnPostExecute(
            /* [in] */ /*ArrayList<UserRecord>*/IInterface* userRecords);

    private:
        UserSwitcherController* mHost;
        Boolean mAddUsersWhenLocked;
    };

public:
    CAR_INTERFACE_DECL()

    UserSwitcherController();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IKeyguardMonitor* keyguardMonitor);

    CARAPI IsSimpleUserSwitcher(
        /* [out] */ Boolean* result);

    CARAPI SwitchTo(
        /* [in] */ IUserSwitcherControllerUserRecord* record);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI GetCurrentUserName(
        /* [in] */ IContext* context,
        /* [out] */ String* name);

private:
    /**
     * Refreshes users from UserManager.
     *
     * The pictures are only loaded if they have not been loaded yet.
     *
     * @param forcePictureLoadForId forces the picture of the given user to be reloaded.
     */
    // @SuppressWarnings("unchecked")
    CARAPI_(void) RefreshUsers(
        /* [in] */ Int32 forcePictureLoadForId);

    CARAPI_(void) NotifyAdapters();

    CARAPI_(void) SwitchToUserId(
        /* [in] */ Int32 id);

    CARAPI_(void) ShowExitGuestDialog(
        /* [in] */ Int32 id);

    CARAPI_(void) ShowAddUserDialog();

    CARAPI_(void) ExitGuest(
        /* [in] */ Int32 id);

public:
    AutoPtr<IQSTileDetailAdapter> mUserDetailAdapter;

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const String SIMPLE_USER_SWITCHER_GLOBAL_SETTING;

    AutoPtr<IContext> mContext;
    AutoPtr<IUserManager> mUserManager;
    AutoPtr<IArrayList> mAdapters;  /*<WeakReference<BaseUserAdapter>*/
    AutoPtr<IGuestResumeSessionReceiver> mGuestResumeSessionReceiver;
    AutoPtr<IKeyguardMonitor> mKeyguardMonitor;

    AutoPtr<IArrayList> mUsers;  /*<UserRecord*/
    AutoPtr<IDialog> mExitGuestDialog;
    AutoPtr<IDialog> mAddUserDialog;
    Int32 mLastNonGuestUser;
    Boolean mSimpleUserSwitcher;
    Boolean mAddUsersWhenLocked;
    AutoPtr<IBroadcastReceiver> mReceiver;
    AutoPtr<IContentObserver> mSettingsObserver;
    AutoPtr<IKeyguardMonitorCallback> mCallback;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_USERSWITCHERCONTROLLER_H__
