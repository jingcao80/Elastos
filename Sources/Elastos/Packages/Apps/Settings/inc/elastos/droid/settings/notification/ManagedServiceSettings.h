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

#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_MANAGEDSERVICESETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_MANAGEDSERVICESETTINGS_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/app/DialogFragment.h"
#include "elastos/droid/app/ListFragment.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/widget/ArrayAdapter.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::App::ListFragment;
using Elastos::Droid::App::IListFragment;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

class ManagedServiceSettings
    : public ListFragment
{
public:
    class ScaryWarningDialogFragment
        : public DialogFragment
    {
    public:
        TO_STRING_IMPL("ManagedServiceSettings::ScaryWarningDialogFragment")

        ScaryWarningDialogFragment();

        CARAPI constructor(
            /* [in] */ IListFragment* host);

        CARAPI SetServiceInfo(
            /* [in] */ IComponentName* cn,
            /* [in] */ const String& label);

        //@Override
        CARAPI OnCreateDialog(
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IDialog** result);

    public:
        static const String KEY_COMPONENT;
        static const String KEY_LABEL;

    private:
        ManagedServiceSettings* mHost;
    };

protected:
    class Config
        : public Object
    {
    public:
        TO_STRING_IMPL("ManagedServiceSettings::Config")

        Config()
            : mWarningDialogTitle(0)
            , mWarningDialogSummary(0)
            , mEmptyText(0)
        {}

    public:
        String mTag;
        String mSetting;
        String mIntentAction;
        String mPermission;
        String mNoun;
        Int32 mWarningDialogTitle;
        Int32 mWarningDialogSummary;
        Int32 mEmptyText;
    };

private:
    class ViewHolder
        : public Object
    {
    public:
        AutoPtr<IImageView> mIcon;
        AutoPtr<ITextView> mName;
        AutoPtr<ICheckBox> mCheckbox;
        AutoPtr<ITextView> mDescription;
    };

    class ServiceListAdapter
        : public ArrayAdapter
    {
    public:
        TO_STRING_IMPL("ManagedServiceSettings::ServiceListAdapter")

        ServiceListAdapter();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ ManagedServiceSettings* host);

        CARAPI HasStableIds(
            /* [out] */ Boolean* result);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* itemId);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        virtual CARAPI_(AutoPtr<IView>) NewView(
            /* [in] */ IViewGroup* parent);

        CARAPI BindView(
            /* [in] */ IView* view,
            /* [in] */ Int32 position);

    protected:
        AutoPtr<ILayoutInflater> mInflater;

    private:
        ManagedServiceSettings* mHost;
    };

    class MyContentObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("ManagedServiceSettings::MyContentObserver")

        MyContentObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ ManagedServiceSettings* host);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        ManagedServiceSettings* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("ManagedServiceSettings::MyBroadcastReceiver")

        MyBroadcastReceiver(
            /* [in] */ ManagedServiceSettings* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        ManagedServiceSettings* mHost;
    };

    class DialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("ManagedServiceSettings::DialogInterfaceOnClickListener")

        DialogInterfaceOnClickListener(
            /* [in] */ ManagedServiceSettings* host,
            /* [in] */ Int32 id,
            /* [in] */ IComponentName* cn);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 id);

    private:
        ManagedServiceSettings* mHost;
        Int32 mId;
        AutoPtr<IComponentName> mCn;
    };

public:
    TO_STRING_IMPL("ManagedServiceSettings")

    ManagedServiceSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnListItemClick(
        /* [in] */ IListView* l,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

protected:
    virtual CARAPI_(AutoPtr<Config>) GetConfig() = 0;

    static CARAPI_(Int32) GetEnabledServicesCount(
        /* [in] */ Config* config,
        /* [in] */ IContext* context);

    static CARAPI_(Int32) GetServicesCount(
        /* [in] */ Config* c,
        /* [in] */ IPackageManager* pm);

private:
    static CARAPI_(Int32) GetServices(
        /* [in] */ Config* c,
        /* [in] */ IArrayAdapter* adapter,
        /* [in] */ IPackageManager* pm);

    CARAPI_(void) LoadEnabledServices();

    CARAPI_(void) SaveEnabledServices();

    CARAPI_(void) UpdateList();

    CARAPI_(Boolean) IsServiceEnabled(
        /* [in] */ IServiceInfo* info);

private:
    static const Boolean SHOW_PACKAGE_NAME;

    AutoPtr<Config> mConfig;
    AutoPtr<IPackageManager> mPM;
    AutoPtr<IContentResolver> mCR;

    // HashSet<ComponentName> mEnabledServices = new HashSet<ComponentName>();
    AutoPtr<IHashSet> mEnabledServices;
    AutoPtr<ServiceListAdapter> mListAdapter;

    AutoPtr<ContentObserver> mSettingsObserver;

    AutoPtr<BroadcastReceiver> mPackageReceiver;
};

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_MANAGEDSERVICESETTINGS_H__