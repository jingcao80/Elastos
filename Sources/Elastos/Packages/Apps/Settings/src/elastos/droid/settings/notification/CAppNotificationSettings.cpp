
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/settings/notification/CAppNotificationSettings.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/text/TextUtils.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  CAppNotificationSettings::MyOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CAppNotificationSettings::MyOnClickListener, Object, IViewOnClickListener)

CAppNotificationSettings::MyOnClickListener::MyOnClickListener(
    /* [in] */ CAppNotificationSettings* host)
    : mHost(host)
{}

ECode CAppNotificationSettings::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->mContext->StartActivity(mHost->mAppRow->mSettingsIntent);
}

//===============================================================================
//                  CAppNotificationSettings::MyOnPreferenceChangeListener
//===============================================================================

CAR_INTERFACE_IMPL(CAppNotificationSettings::MyOnPreferenceChangeListener, Object, IPreferenceOnPreferenceChangeListener)

CAppNotificationSettings::MyOnPreferenceChangeListener::MyOnPreferenceChangeListener(
    /* [in] */ CAppNotificationSettings* host,
    /* [in] */ Int32 id,
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid)
    : mHost(host)
    , mId(id)
    , mPkg(pkg)
    , mUid(uid)
{}

ECode CAppNotificationSettings::MyOnPreferenceChangeListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;

    switch (mId) {
        case 0: {
            Boolean block;
            IBoolean::Probe(newValue)->GetValue(&block);
            *res = mHost->mBackend->SetNotificationsBanned(mPkg, mUid, block);
            return NOERROR;
        }
        case 1: {
            Boolean priority;
            IBoolean::Probe(newValue)->GetValue(&priority);
            *res = mHost->mBackend->SetHighPriority(mPkg, mUid, priority);
            return NOERROR;
        }
        case 2: {
            Boolean sensitive;
            IBoolean::Probe(newValue)->GetValue(&sensitive);
            *res = mHost->mBackend->SetSensitive(mPkg, mUid, sensitive);
            return NOERROR;
        }
    }

    return NOERROR;
}

//===============================================================================
//                  CAppNotificationSettings
//===============================================================================

const String CAppNotificationSettings::EXTRA_HAS_SETTINGS_INTENT("has_settings_intent");
const String CAppNotificationSettings::EXTRA_SETTINGS_INTENT("settings_intent");

const String CAppNotificationSettings::TAG("CAppNotificationSettings");
const Boolean CAppNotificationSettings::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);

const String CAppNotificationSettings::KEY_BLOCK("block");
const String CAppNotificationSettings::KEY_PRIORITY("priority");
const String CAppNotificationSettings::KEY_SENSITIVE("sensitive");

CAR_OBJECT_IMPL(CAppNotificationSettings)

CAppNotificationSettings::CAppNotificationSettings()
    : mCreated(FALSE)
{}

CAppNotificationSettings::~CAppNotificationSettings()
{}

ECode CAppNotificationSettings::constructor()
{
    mBackend = new CNotificationAppList::Backend();

    return SettingsPreferenceFragment::constructor();
}

ECode CAppNotificationSettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnActivityCreated(savedInstanceState);
    if (DEBUG) Logger::D(TAG, "onActivityCreated mCreated=%d", mCreated);
    if (mCreated) {
        Logger::W(TAG, "onActivityCreated: ignoring duplicate call");
        return NOERROR;
    }
    mCreated = TRUE;
    if (mAppRow == NULL) return NOERROR;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IView> content;
    activity->FindViewById(R::id::main_content, (IView**)&content);
    AutoPtr<IViewParent> parent;
    content->GetParent((IViewParent**)&parent);
    IViewGroup* contentParent = IViewGroup::Probe(parent);
    AutoPtr<ILayoutInflater> inflater;
    activity->GetLayoutInflater((ILayoutInflater**)&inflater);
    AutoPtr<IView> bar;
    inflater->Inflate(R::layout::app_notification_header,
            contentParent, FALSE, (IView**)&bar);

    AutoPtr<IView> appIconTmp;
    bar->FindViewById(R::id::app_icon, (IView**)&appIconTmp);
    IImageView* appIcon = IImageView::Probe(appIconTmp);
    appIcon->SetImageDrawable(mAppRow->mIcon);

    AutoPtr<IView> appNameTmp;
    bar->FindViewById(R::id::app_name, (IView**)&appNameTmp);
    ITextView* appName = ITextView::Probe(appNameTmp);
    appName->SetText(mAppRow->mLabel);

    AutoPtr<IView> appSettings;
    bar->FindViewById(R::id::app_settings, (IView**)&appSettings);
    if (mAppRow->mSettingsIntent == NULL) {
        appSettings->SetVisibility(IView::GONE);
    }
    else {
        appSettings->SetClickable(TRUE);
        AutoPtr<MyOnClickListener> listener = new MyOnClickListener(this);
        appSettings->SetOnClickListener(listener);
    }
    contentParent->AddView(bar, 0);
    return NOERROR;
}

ECode CAppNotificationSettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnCreate(savedInstanceState);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mContext = IContext::Probe(activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    if (DEBUG) Logger::D(TAG, "onCreate GetIntent()=%s", TO_CSTR(intent));
    if (intent == NULL) {
        Logger::W(TAG, "No intent");
        ToastAndFinish();
        return NOERROR;
    }

    Int32 uid;
    intent->GetInt32Extra(ISettings::EXTRA_APP_UID, -1, &uid);
    String pkg;
    intent->GetStringExtra(ISettings::EXTRA_APP_PACKAGE, &pkg);
    if (uid == -1 || TextUtils::IsEmpty(pkg)) {
        Logger::W(TAG, "Missing extras: %s was %s, %s was %d",
                ISettings::EXTRA_APP_PACKAGE.string(), pkg.string(), ISettings::EXTRA_APP_UID.string(), uid);
        ToastAndFinish();
        return NOERROR;
    }

    if (DEBUG) Logger::D(TAG, "Load details for pkg=%s uid=%d", pkg.string(), uid);
    AutoPtr<IPackageManager> pm = GetPackageManager();
    AutoPtr<IPackageInfo> info = FindPackageInfo(pm, pkg, uid);
    if (info == NULL) {
        Logger::W(TAG, "Failed to find package info: %s was %s, %s was %d",
                ISettings::EXTRA_APP_PACKAGE.string(), pkg.string(), ISettings::EXTRA_APP_UID.string(), uid);
        ToastAndFinish();
        return NOERROR;
    }

    AddPreferencesFromResource(R::xml::app_notification_settings);
    AutoPtr<IPreference> blockPreference;
    FindPreference(CoreUtils::Convert(KEY_BLOCK), (IPreference**)&blockPreference);
    mBlock = ISwitchPreference::Probe(blockPreference);
    AutoPtr<IPreference> priorityPreference;
    FindPreference(CoreUtils::Convert(KEY_PRIORITY), (IPreference**)&priorityPreference);
    mPriority = ISwitchPreference::Probe(priorityPreference);
    AutoPtr<IPreference> sensitivePreference;
    FindPreference(CoreUtils::Convert(KEY_SENSITIVE), (IPreference**)&sensitivePreference);
    mSensitive = ISwitchPreference::Probe(sensitivePreference);

    AutoPtr<ILockPatternUtils> lpu;
    CLockPatternUtils::New(IContext::Probe(activity), (ILockPatternUtils**)&lpu);
    Boolean secure;
    lpu->IsSecure(&secure);
    const Boolean enabled = GetLockscreenNotificationsEnabled();
    const Boolean allowPrivate = GetLockscreenAllowPrivateNotifications();
    Boolean res;
    if (!secure || !enabled || !allowPrivate) {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreferenceGroup::Probe(screen)->RemovePreference(sensitivePreference, &res);
    }

    AutoPtr<IApplicationInfo> applicationInfo;
    info->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    mAppRow = CNotificationAppList::LoadAppRow(pm, applicationInfo, mBackend);
    if (intent->HasExtra(EXTRA_HAS_SETTINGS_INTENT, &res), res) {
        // use settings intent from extra
        intent->GetBooleanExtra(EXTRA_HAS_SETTINGS_INTENT, FALSE, &res);
        if (res) {
            AutoPtr<IParcelable> parcelable;
            intent->GetParcelableExtra(EXTRA_SETTINGS_INTENT, (IParcelable**)&parcelable);
            mAppRow->mSettingsIntent = IIntent::Probe(parcelable);
        }
    }
    else {
        // load settings intent
        // ArrayMap<String, AppRow> rows = new ArrayMap<String, AppRow>();
        AutoPtr<IArrayMap> rows;
        CArrayMap::New((IArrayMap**)&rows);
        rows->Put(CoreUtils::Convert(mAppRow->mPkg), (IAppRow*)mAppRow);
        CNotificationAppList::CollectConfigActivities(GetPackageManager(), rows);
    }

    ITwoStatePreference::Probe(mBlock)->SetChecked(mAppRow->mBanned);
    ITwoStatePreference::Probe(mPriority)->SetChecked(mAppRow->mPriority);
    if (mSensitive != NULL) {
        ITwoStatePreference::Probe(mSensitive)->SetChecked(mAppRow->mSensitive);
    }

    AutoPtr<MyOnPreferenceChangeListener> listener0 = new MyOnPreferenceChangeListener(this, 0, pkg, uid);
    blockPreference->SetOnPreferenceChangeListener(listener0);

    AutoPtr<MyOnPreferenceChangeListener> listener1 = new MyOnPreferenceChangeListener(this, 1, pkg, uid);
    priorityPreference->SetOnPreferenceChangeListener(listener1);

    if (mSensitive != NULL) {
        AutoPtr<MyOnPreferenceChangeListener> listener2 = new MyOnPreferenceChangeListener(this, 2, pkg, uid);
        sensitivePreference->SetOnPreferenceChangeListener(listener2);
    }

    // Users cannot block notifications from system/signature packages
    if (Utils::IsSystemPackage(pm, info)) {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreferenceGroup::Probe(screen)->RemovePreference(blockPreference, &res);
        priorityPreference->SetDependency(String(NULL)); // don't have it depend on a preference that's gone
    }
    return NOERROR;
}

Boolean CAppNotificationSettings::GetLockscreenNotificationsEnabled()
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 data;
    secure->GetInt32(GetContentResolver(),
            ISettingsSecure::LOCK_SCREEN_SHOW_NOTIFICATIONS, 0, &data);
    return data != 0;
}

Boolean CAppNotificationSettings::GetLockscreenAllowPrivateNotifications()
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 data;
    secure->GetInt32(GetContentResolver(),
            ISettingsSecure::LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS, 0, &data);
    return data != 0;
}

void CAppNotificationSettings::ToastAndFinish()
{
    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    helper->MakeText(mContext, R::string::app_not_found_dlg_text, IToast::LENGTH_SHORT, (IToast**)&toast);
    toast->Show();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    activity->Finish();
}

AutoPtr<IPackageInfo> CAppNotificationSettings::FindPackageInfo(
    /* [in] */ IPackageManager* pm,
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid)
{
    AutoPtr< ArrayOf<String> > packages;
    pm->GetPackagesForUid(uid, (ArrayOf<String>**)&packages);
    if (packages != NULL && !pkg.IsNull()) {
        Int32 N = packages->GetLength();
        for (Int32 i = 0; i < N; i++) {
            String p = (*packages)[i];
            if (pkg.Equals(p)) {
                // try {
                AutoPtr<IPackageInfo> info;
                ECode ec = pm->GetPackageInfo(pkg, IPackageManager::GET_SIGNATURES, (IPackageInfo**)&info);
                if (SUCCEEDED(ec)) {
                    return info;
                }
                else {
                    Logger::W(TAG, "Failed to load package %s , 0x%08x", pkg.string(), ec);
                }
            }
        }
    }
    return NULL;
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos