
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Preference.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Service.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.Droid.Text.h>
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/settings/CUserSpinnerAdapter.h"
#include "elastos/droid/settings/SettingsActivity.h"
#include "elastos/droid/settings/drawable/CircleFramedDrawable.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include "R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Internal::Utility::CUserIcons;
using Elastos::Droid::Internal::Utility::IUserIcons;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceFrameLayout;
using Elastos::Droid::Preference::IPreferenceFrameLayoutParams;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractSyncColumns;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsStructuredName;
using Elastos::Droid::Provider::IContactsContractContactsData;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractDataColumns;
using Elastos::Droid::Provider::CContactsContractProfile;
using Elastos::Droid::Provider::IContactsContractProfile;
using Elastos::Droid::Settings::Drawable::CircleFramedDrawable;
using Elastos::Droid::Service::Persistentdata::IPersistentDataBlockManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::Text::IBidiFormatterHelper;
using Elastos::Droid::Text::CBidiFormatterHelper;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;
using Elastos::Net::IInetAddress;
using Elastos::Text::INumberFormat;
using Elastos::Text::CNumberFormatHelper;
using Elastos::Text::INumberFormatHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::CLocale;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;

namespace Elastos {
namespace Droid {
namespace Settings {

const Int32 Utils::UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY = 1;

const Float Utils::DISABLED_ALPHA = 0.4f;

static AutoPtr< ArrayOf<Int32> > InitBADNESS_COLORS()
{
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(7);
    (*args)[0] = 0x00000000;
    (*args)[1] = 0xffc43828;
    (*args)[2] = 0xffe54918;
    (*args)[3] = 0xfff47b00;
    (*args)[4] = 0xfffabf2c;
    (*args)[5] = 0xff679e37;
    (*args)[6] = 0xff0a7f42;
    return args;
}

const AutoPtr< ArrayOf<Int32> > Utils::BADNESS_COLORS = InitBADNESS_COLORS();

const String Utils::TAG("Settings");

const String Utils::META_DATA_PREFERENCE_ICON("com.android.settings.icon");

const String Utils::META_DATA_PREFERENCE_TITLE("com.android.settings.title");

const String Utils::META_DATA_PREFERENCE_SUMMARY("com.android.settings.summary");

const String Utils::SETTINGS_PACKAGE_NAME("com.android.settings");

const Int32 Utils::SECONDS_PER_MINUTE = 60;
const Int32 Utils::SECONDS_PER_HOUR = 60 * 60;
const Int32 Utils::SECONDS_PER_DAY = 24 * 60 * 60;

AutoPtr< ArrayOf<ISignature*> > Utils::sSystemSignature;

//===============================================================================
//                  Utils::DialogInterfaceOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(Utils::DialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener);

Utils::DialogInterfaceOnClickListener::DialogInterfaceOnClickListener(
    /* [in] */ IRunnable* runnable)
    : mRunnable(runnable)
{}

Utils::DialogInterfaceOnClickListener::~DialogInterfaceOnClickListener()
{}

ECode Utils::DialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mRunnable->Run();
}

//===============================================================================
//                  Utils
//===============================================================================

Boolean Utils::UpdatePreferenceToSpecificActivityOrRemove(
    /* [in] */ IContext* context,
    /* [in] */ IPreferenceGroup* parentPreferenceGroup,
    /* [in] */ const String& preferenceKey,
    /* [in] */ Int32 flags)
{
    AutoPtr<IPreference> preference;
    parentPreferenceGroup->FindPreference(CoreUtils::Convert(preferenceKey),
            (IPreference**)&preference);
    if (preference == NULL) {
        return FALSE;
    }

    AutoPtr<IIntent> intent;
    preference->GetIntent((IIntent**)&intent);
    if (intent != NULL) {
        // Find the activity that is in the system image
        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IList> list;
        pm->QueryIntentActivities(intent, 0, (IList**)&list);
        Int32 listSize;
        list->GetSize(&listSize);
        for (Int32 i = 0; i < listSize; i++) {
            AutoPtr<IInterface> obj;
            list->Get(i, (IInterface**)&obj);
            AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(obj);
            AutoPtr<IActivityInfo> activityInfo;
            resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
            AutoPtr<IApplicationInfo> applicationInfo;
            IComponentInfo::Probe(activityInfo)->GetApplicationInfo(
                    (IApplicationInfo**)&applicationInfo);
            Int32 flags;
            applicationInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {

                // Replace the intent with this specific activity
                String packageName, name;
                IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
                IPackageItemInfo::Probe(activityInfo)->GetPackageName(&name);
                AutoPtr<IIntent> intent;
                CIntent::New((IIntent**)&intent);
                intent->SetClassName(packageName, name);
                preference->SetIntent(intent);

                if ((flags & UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY) != 0) {
                    // Set the preference title to the activity's label
                    AutoPtr<ICharSequence> seq;
                    resolveInfo->LoadLabel(pm, (ICharSequence**)&seq);
                    preference->SetTitle(seq);
                }

                return TRUE;
            }
        }
    }

    // Did not find a matching activity, so remove the preference
    Boolean res;
    parentPreferenceGroup->RemovePreference(preference, &res);

    return FALSE;
}

Boolean Utils::UpdateTileToSpecificActivityFromMetaDataOrRemove(
    /* [in] */ IContext* context,
    /* [in] */ DashboardTile* tile)
{
    AutoPtr<IIntent> intent = tile->mIntent;
    if (intent != NULL) {
        // Find the activity that is in the system image
        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IList> list;
        pm->QueryIntentActivities(intent, IPackageManager::GET_META_DATA,
                (IList**)&list);
        Int32 listSize;
        list->GetSize(&listSize);
        for (Int32 i = 0; i < listSize; i++) {
            AutoPtr<IInterface> obj;
            list->Get(i, (IInterface**)&obj);
            AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(obj);
            AutoPtr<IActivityInfo> activityInfo;
            resolveInfo->GetActivityInfo((IActivityInfo**)&activityInfo);
            AutoPtr<IApplicationInfo> applicationInfo;
            IComponentInfo::Probe(activityInfo)->GetApplicationInfo(
                    (IApplicationInfo**)&applicationInfo);
            Int32 flags;
            applicationInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                AutoPtr<IDrawable> icon;
                String title;
                String summary;

                // Get the activity's meta-data
                // try {
                String packageName;
                IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
                AutoPtr<IResources> res;
                pm->GetResourcesForApplication(packageName, (IResources**)&res);
                AutoPtr<IBundle> metaData;
                IPackageItemInfo::Probe(activityInfo)->GetMetaData((IBundle**)&metaData);

                if (res != NULL && metaData != NULL) {
                    Int32 value;
                    metaData->GetInt32(META_DATA_PREFERENCE_ICON, &value);
                    res->GetDrawable(value, (IDrawable**)&icon);
                    metaData->GetInt32(META_DATA_PREFERENCE_TITLE, &value);
                    res->GetString(value, &title);
                    metaData->GetInt32(META_DATA_PREFERENCE_SUMMARY, &value);
                    res->GetString(value, &summary);
                }
                // } catch (NameNotFoundException e) {
                //     // Ignore
                // } catch (NotFoundException e) {
                //     // Ignore
                // }

                // Set the preference title to the activity's label if no
                // meta-data is found
                if (TextUtils::IsEmpty(title)) {
                    AutoPtr<ICharSequence> seq;
                    seq->ToString(&title);
                }

                // Set icon, title and summary for the preference
                // TODO:
                //tile.icon = icon;
                tile->mTitle = CoreUtils::Convert(title);
                tile->mSummary = CoreUtils::Convert(summary);
                // Replace the intent with this specific activity
                String name;
                IPackageItemInfo::Probe(activityInfo)->GetPackageName(&name);
                AutoPtr<IIntent> intent;
                CIntent::New((IIntent**)&intent);
                intent->SetClassName(packageName, name);
                tile->mIntent = intent;

                return TRUE;
            }
        }
    }

    return FALSE;
}

Boolean Utils::IsMonkeyRunning()
{
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Boolean res;
    helper->IsUserAMonkey(&res);
    return res;
}

Boolean Utils::IsVoiceCapable(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> telephony = ITelephonyManager::Probe(obj);
    Boolean res;
    return telephony != NULL && (telephony->IsVoiceCapable(&res), res);
}

Boolean Utils::IsWifiOnly(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    Boolean res;
    return (cm->IsNetworkSupported(IConnectivityManager::TYPE_MOBILE, &res), res) == FALSE;
}

String Utils::GetWifiIpAddresses(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    AutoPtr<ILinkProperties> prop;
    cm->GetLinkProperties(IConnectivityManager::TYPE_WIFI,(ILinkProperties**)&prop);
    return FormatIpAddresses(prop);
}

String Utils::GetDefaultIpAddresses(
    /* [in] */ IConnectivityManager* cm)
{
    AutoPtr<ILinkProperties> prop;
    cm->GetActiveLinkProperties((ILinkProperties**)&prop);
    return FormatIpAddresses(prop);
}

String Utils::FormatIpAddresses(
    /* [in] */ ILinkProperties* prop)
{
    if (prop == NULL) return String(NULL);

    AutoPtr<IList> list;
    prop->GetAllAddresses((IList**)&list);
    AutoPtr<IIterator> iter;
    list->GetIterator((IIterator**)&iter);
    // If there are no entries, return NULL
    Boolean res;
    if (iter->HasNext(&res), !res) return String(NULL);
    // Concatenate all available addresses, comma separated
    StringBuilder addresses("");
    while (iter->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        String str;
        IInetAddress::Probe(obj)->GetHostAddress(&str);
        addresses += str;
        if (iter->HasNext(&res), res) addresses += "\n";
    }
    return addresses.ToString();
}

AutoPtr<ILocale> Utils::CreateLocaleFromString(
    /* [in] */ const String& localeStr)
{
    // TODO: is there a better way to actually construct a locale that will match?
    // The main problem is, on top of Java specs, locale->ToString() and
    // new Locale(locale->ToString()).ToString() do not return Equal() strings in
    // many cases, because the constructor takes the only string as the language
    // code. So : new Locale("en", "US").ToString() => "en_US"
    // And : new Locale("en_US").ToString() => "en_us"
    if (localeStr.IsNull()) {
        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        AutoPtr<ILocale> locale;
        helper->GetDefault((ILocale**)&locale);
        return locale;
    }

    AutoPtr< ArrayOf<String> > brokenDownLocale;
    StringUtils::Split(localeStr, "_", 3, (ArrayOf<String>**)&brokenDownLocale);
    // split may not return a 0-length array.
    if (1 == brokenDownLocale->GetLength()) {
        AutoPtr<ILocale> locale;
        CLocale::New((*brokenDownLocale)[0], (ILocale**)&locale);
        return locale;
    }
    else if (2 == brokenDownLocale->GetLength()) {
        AutoPtr<ILocale> locale;
        CLocale::New((*brokenDownLocale)[0], (*brokenDownLocale)[1],
                (ILocale**)&locale);
        return locale;
    }
    else {
        AutoPtr<ILocale> locale;
        CLocale::New((*brokenDownLocale)[0], (*brokenDownLocale)[1],
                (*brokenDownLocale)[2], (ILocale**)&locale);
        return locale;
    }
}

String Utils::FormatPercentage(
    /* [in] */ Int64 amount,
    /* [in] */ Int64 total)
{
    return FormatPercentage(((Double) amount) / total);
}

String Utils::FormatPercentage(
    /* [in] */ Int32 percentage)
{
    return FormatPercentage(((Double) percentage) / 100.0);
}

String Utils::FormatPercentage(
    /* [in] */ Double percentage)
{
    AutoPtr<IBidiFormatterHelper> helper;
    CBidiFormatterHelper::AcquireSingleton((IBidiFormatterHelper**)&helper);
    AutoPtr<IBidiFormatter> bf;
    helper->GetInstance((IBidiFormatter**)&bf);

    AutoPtr<INumberFormatHelper> nfHelper;
    CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&nfHelper);
    AutoPtr<INumberFormat> nf;
    nfHelper->GetPercentInstance((INumberFormat**)&nf);
    String str;
    nf->Format(percentage, &str);
    String result;
    bf->UnicodeWrap(str, &result);

    return result;
}

Boolean Utils::IsBatteryPresent(
    /* [in] */ IIntent* batteryChangedIntent)
{
    Boolean res;
    batteryChangedIntent->GetBooleanExtra(IBatteryManager::EXTRA_PRESENT,
            TRUE, &res);
    return res;
}

String Utils::GetBatteryPercentage(
    /* [in] */ IIntent* batteryChangedIntent)
{
    return FormatPercentage(GetBatteryLevel(batteryChangedIntent));
}

Int32 Utils::GetBatteryLevel(
    /* [in] */ IIntent* batteryChangedIntent)
{
    Int32 level;
    batteryChangedIntent->GetInt32Extra(IBatteryManager::EXTRA_LEVEL, 0, &level);
    Int32 scale;
    batteryChangedIntent->GetInt32Extra(IBatteryManager::EXTRA_SCALE, 100, &scale);
    return (level * 100) / scale;
}

String Utils::GetBatteryStatus(
    /* [in] */ IResources* res,
    /* [in] */ IIntent* batteryChangedIntent)
{
    AutoPtr<IIntent> intent = batteryChangedIntent;

    Int32 plugType;
    intent->GetInt32Extra(IBatteryManager::EXTRA_PLUGGED, 0, &plugType);
    Int32 status;
    intent->GetInt32Extra(IBatteryManager::EXTRA_STATUS,
            IBatteryManager::BATTERY_STATUS_UNKNOWN, &status);
    String statusString;
    if (status == IBatteryManager::BATTERY_STATUS_CHARGING) {
        Int32 resId;
        if (plugType == IBatteryManager::BATTERY_PLUGGED_AC) {
            resId = R::string::battery_info_status_charging_ac;
        }
        else if (plugType == IBatteryManager::BATTERY_PLUGGED_USB) {
            resId = R::string::battery_info_status_charging_usb;
        }
        else if (plugType == IBatteryManager::BATTERY_PLUGGED_WIRELESS) {
            resId = R::string::battery_info_status_charging_wireless;
        }
        else {
            resId = R::string::battery_info_status_charging;
        }
        res->GetString(resId, &statusString);
    }
    else if (status == IBatteryManager::BATTERY_STATUS_DISCHARGING) {
        res->GetString(R::string::battery_info_status_discharging, &statusString);
    }
    else if (status == IBatteryManager::BATTERY_STATUS_NOT_CHARGING) {
        res->GetString(R::string::battery_info_status_not_charging, &statusString);
    }
    else if (status == IBatteryManager::BATTERY_STATUS_FULL) {
        res->GetString(R::string::battery_info_status_full, &statusString);
    }
    else {
        res->GetString(R::string::battery_info_status_unknown, &statusString);
    }

    return statusString;
}

void Utils::ForcePrepareCustomPreferencesList(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* child,
    /* [in] */ IListView* list,
    /* [in] */ Boolean ignoreSidePadding)
{
    IView::Probe(list)->SetScrollBarStyle(IView::SCROLLBARS_OUTSIDE_OVERLAY);
    IViewGroup::Probe(list)->SetClipToPadding(FALSE);
    PrepareCustomPreferencesList(parent, child, IView::Probe(list), ignoreSidePadding);
}

void Utils::PrepareCustomPreferencesList(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* child,
    /* [in] */ IView* list,
    /* [in] */ Boolean ignoreSidePadding)
{
    Int32 style;
    list->GetScrollBarStyle(&style);
    Boolean movePadding = style == IView::SCROLLBARS_OUTSIDE_OVERLAY;
    if (movePadding) {
        AutoPtr<IResources> res;
        list->GetResources((IResources**)&res);
        Int32 paddingSide;
        res->GetDimensionPixelSize(R::dimen::settings_side_margin, &paddingSide);
        Int32 paddingBottom;
        res->GetDimensionPixelSize(
                Elastos::Droid::R::dimen::preference_fragment_padding_bottom,
                &paddingBottom);

        if (IPreferenceFrameLayout::Probe(parent) != NULL) {
            AutoPtr<IViewGroupLayoutParams> params;
            child->GetLayoutParams((IViewGroupLayoutParams**)&params);
            IPreferenceFrameLayoutParams::Probe(params)->SetRemoveBorders(TRUE);

            Int32 effectivePaddingSide = ignoreSidePadding ? 0 : paddingSide;
            list->SetPaddingRelative(effectivePaddingSide, 0, effectivePaddingSide, paddingBottom);
        }
        else {
            list->SetPaddingRelative(paddingSide, 0, paddingSide, paddingBottom);
        }
    }
}

void Utils::ForceCustomPadding(
    /* [in] */ IView* view,
    /* [in] */ Boolean additive)
{
    AutoPtr<IResources> res;
    view->GetResources((IResources**)&res);
    Int32 paddingSide;
    res->GetDimensionPixelSize(R::dimen::settings_side_margin, &paddingSide);

    Int32 start, end;
    Int32 paddingStart = paddingSide +
            (additive ? (view->GetPaddingStart(&start), start) : 0);
    Int32 paddingEnd = paddingSide +
            (additive ? (view->GetPaddingEnd(&end), end) : 0);
    Int32 paddingBottom;
    res->GetDimensionPixelSize(
            Elastos::Droid::R::dimen::preference_fragment_padding_bottom, &paddingBottom);

    view->SetPaddingRelative(paddingStart, 0, paddingEnd, paddingBottom);
}

Int32 Utils::GetTetheringLabel(
    /* [in] */ IConnectivityManager* cm)
{
    AutoPtr< ArrayOf<String> > usbRegexs;
    cm->GetTetherableUsbRegexs((ArrayOf<String>**)&usbRegexs);
    AutoPtr< ArrayOf<String> > wifiRegexs;
    cm->GetTetherableWifiRegexs((ArrayOf<String>**)&wifiRegexs);
    AutoPtr< ArrayOf<String> > bluetoothRegexs;
    cm->GetTetherableBluetoothRegexs((ArrayOf<String>**)&bluetoothRegexs);

    Boolean usbAvailable = usbRegexs->GetLength() != 0;
    Boolean wifiAvailable = wifiRegexs->GetLength() != 0;
    Boolean bluetoothAvailable = bluetoothRegexs->GetLength() != 0;

    if (wifiAvailable && usbAvailable && bluetoothAvailable) {
        return R::string::tether_settings_title_all;
    }
    else if (wifiAvailable && usbAvailable) {
        return R::string::tether_settings_title_all;
    }
    else if (wifiAvailable && bluetoothAvailable) {
        return R::string::tether_settings_title_all;
    }
    else if (wifiAvailable) {
        return R::string::tether_settings_title_wifi;
    }
    else if (usbAvailable && bluetoothAvailable) {
        return R::string::tether_settings_title_usb_bluetooth;
    }
    else if (usbAvailable) {
        return R::string::tether_settings_title_usb;
    }
    else {
        return R::string::tether_settings_title_bluetooth;
    }
}

Boolean Utils::CopyMeProfilePhoto(
    /* [in] */ IContext* context,
    /* [in] */ IUserInfo* user)
{
    AutoPtr<IContactsContractProfile> profile;
    CContactsContractProfile::AcquireSingleton((IContactsContractProfile**)&profile);
    AutoPtr<IUri> contactUri;
    profile->GetCONTENT_URI((IUri**)&contactUri);

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IContactsContractContacts> contact;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contact);
    AutoPtr<IInputStream> avatarDataStream;
    contact->OpenContactPhotoInputStream(
            resolver, contactUri, TRUE, (IInputStream**)&avatarDataStream);
    // If there's no profile photo, assign a default avatar
    if (avatarDataStream == NULL) {
        return FALSE;
    }
    Int32 id;
    Int32 userId = user != NULL ? (user->GetId(&id), id) : UserHandle::GetMyUserId();
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> um = IUserManager::Probe(obj);
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    AutoPtr<IBitmap> icon;
    factory->DecodeStream(avatarDataStream, (IBitmap**)&icon);
    um->SetUserIcon(userId, icon);
    // try {
    avatarDataStream->Close();
    // } catch (IOException ioe) { }
    return TRUE;
}

String Utils::GetMeProfileName(
    /* [in] */ IContext* context,
    /* [in] */ Boolean full)
{
    if (full) {
        return GetProfileDisplayName(context);
    }
    else {
        return GetShorterNameIfPossible(context);
    }
}

String Utils::GetShorterNameIfPossible(
    /* [in] */ IContext* context)
{
    String given = GetLocalProfileGivenName(context);
    return !TextUtils::IsEmpty(given) ? given : GetProfileDisplayName(context);
}

String Utils::GetLocalProfileGivenName(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    // Find the raw contact ID for the local ME profile raw contact.
    Int64 localRowProfileId;

    AutoPtr<IContactsContractProfile> profile;
    CContactsContractProfile::AcquireSingleton((IContactsContractProfile**)&profile);
    AutoPtr<IUri> uri;
    profile->GetCONTENT_RAW_CONTACTS_URI((IUri**)&uri);

    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = IBaseColumns::ID;

    StringBuilder builder;
    builder += IContactsContractSyncColumns::ACCOUNT_TYPE;
    builder += " IS NULL AND ";
    builder += IContactsContractSyncColumns::ACCOUNT_NAME;
    builder += " IS NULL";

    AutoPtr<ICursor> localRawProfile;
    cr->Query(uri, args, builder.ToString(), NULL, String(NULL),
                (ICursor**)&localRawProfile);
    if (localRawProfile == NULL) return String(NULL);

    // try {
    Boolean res;
    if (localRawProfile->MoveToFirst(&res), !res) {
        ICloseable::Probe(localRawProfile)->Close();
        return String(NULL);
    }
    localRawProfile->GetInt64(0, &localRowProfileId);
    // } finally {
    ICloseable::Probe(localRawProfile)->Close();
    // }

    // Find the structured name for the raw contact.
    AutoPtr<IUri> contentUri;
    profile->GetCONTENT_URI((IUri**)&contentUri);
    AutoPtr<IUriBuilder> uriBuilder;
    contentUri->BuildUpon((IUriBuilder**)&uriBuilder);
    uriBuilder->AppendPath(IContactsContractContactsData::CONTENT_DIRECTORY);
    AutoPtr<IUri> resultUri;
    uriBuilder->Build((IUri**)&resultUri);
    AutoPtr< ArrayOf<String> > args2 = ArrayOf<String>::Alloc(2);
    (*args2)[0] = IContactsContractCommonDataKindsStructuredName::GIVEN_NAME;
    (*args2)[1] = IContactsContractCommonDataKindsStructuredName::FAMILY_NAME;

    builder.Reset();
    builder += IContactsContractDataColumns::RAW_CONTACT_ID;
    builder += "=";
    builder += localRowProfileId;
    AutoPtr<ICursor> structuredName;
    cr->Query(resultUri, args2, builder.ToString(),
            NULL, String(NULL),(ICursor**)&structuredName);
    if (structuredName == NULL) return String(NULL);

    // try {
    if (structuredName->MoveToFirst(&res), !res) {
        ICloseable::Probe(structuredName)->Close();
        return String(NULL);
    }
    String partialName;
    structuredName->GetString(0, &partialName);
    if (TextUtils::IsEmpty(partialName)) {
        structuredName->GetString(1, &partialName);
    }
    ICloseable::Probe(structuredName)->Close();
    return partialName;
    // } finally {
        // structuredName->Close();
    // }
}

String Utils::GetProfileDisplayName(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<IContactsContractProfile> ccProfile;
    CContactsContractProfile::AcquireSingleton((IContactsContractProfile**)&ccProfile);
    AutoPtr<IUri> uri;
    ccProfile->GetCONTENT_URI((IUri**)&uri);

    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = IContactsContractContactsColumns::DISPLAY_NAME;

    AutoPtr<ICursor> profile;
    cr->Query(uri, args, String(NULL), NULL, String(NULL), (ICursor**)&profile);
    if (profile == NULL) return String(NULL);

    // try {
    Boolean res;
    if (profile->MoveToFirst(&res), !res) {
        ICloseable::Probe(profile)->Close();
        return String(NULL);
    }
    String result;
    profile->GetString(0, &result);
    ICloseable::Probe(profile)->Close();
    return result;
    // } finally {
    //     profile->Close();
    // }
}

AutoPtr<IDialog> Utils::BuildGlobalChangeWarningDialog(
    /* [in] */ IContext* context,
    /* [in] */ Int32 titleResId,
    /* [in] */ IRunnable* positiveAction)
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(titleResId);
    builder->SetMessage(R::string::global_change_warning);
    AutoPtr<DialogInterfaceOnClickListener> listener =
            new DialogInterfaceOnClickListener(positiveAction);
    builder->SetPositiveButton(Elastos::Droid::R::string::ok, listener);
    builder->SetNegativeButton(Elastos::Droid::R::string::cancel, NULL);

    AutoPtr<IAlertDialog> dia;
    builder->Create((IAlertDialog**)&dia);
    return IDialog::Probe(dia);
}

Boolean Utils::HasMultipleUsers(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IList> users;
    IUserManager::Probe(obj)->GetUsers((IList**)&users);
    Int32 size;
    users->GetSize(&size);
    return size > 1;
}

void Utils::StartWithFragment(
    /* [in] */ IContext* context,
    /* [in] */ const String& fragmentName,
    /* [in] */ IBundle* args,
    /* [in] */ IFragment* resultTo,
    /* [in] */ Int32 resultRequestCode,
    /* [in] */ Int32 titleResId,
    /* [in] */ ICharSequence* title)
{
    StartWithFragment(context, fragmentName, args, resultTo, resultRequestCode,
            titleResId, title, FALSE /* not a shortcut */);
}

void Utils::StartWithFragment(
    /* [in] */ IContext* context,
    /* [in] */ const String& fragmentName,
    /* [in] */ IBundle* args,
    /* [in] */ IFragment* resultTo,
    /* [in] */ Int32 resultRequestCode,
    /* [in] */ Int32 titleResId,
    /* [in] */ ICharSequence* title,
    /* [in] */ Boolean isShortcut)
{
    AutoPtr<IIntent> intent = OnBuildStartFragmentIntent(context, fragmentName,
            args, titleResId, title, isShortcut);
    if (resultTo == NULL) {
        context->StartActivity(intent);
    }
    else {
        resultTo->StartActivityForResult(intent, resultRequestCode);
    }
}

void Utils::StartWithFragmentAsUser(
    /* [in] */ IContext* context,
    /* [in] */ const String& fragmentName,
    /* [in] */ IBundle* args,
    /* [in] */ Int32 titleResId,
    /* [in] */ ICharSequence* title,
    /* [in] */ Boolean isShortcut,
    /* [in] */ IUserHandle* userHandle)
{
    AutoPtr<IIntent> intent = OnBuildStartFragmentIntent(context, fragmentName,
            args, titleResId, title, isShortcut);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TASK);
    context->StartActivityAsUser(intent, userHandle);
}

AutoPtr<IIntent> Utils::OnBuildStartFragmentIntent(
    /* [in] */ IContext* context,
    /* [in] */ const String& fragmentName,
    /* [in] */ IBundle* args,
    /* [in] */ Int32 titleResId,
    /* [in] */ ICharSequence* title,
    /* [in] */ Boolean isShortcut)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    assert(0 && "TODO");
    // intent->SetClass(context, SubSettings.class);
    intent->PutExtra(SettingsActivity::EXTRA_SHOW_FRAGMENT, fragmentName);
    intent->PutExtra(SettingsActivity::EXTRA_SHOW_FRAGMENT_ARGUMENTS, args);
    intent->PutExtra(SettingsActivity::EXTRA_SHOW_FRAGMENT_TITLE_RESID, titleResId);
    intent->PutExtra(SettingsActivity::EXTRA_SHOW_FRAGMENT_TITLE, title);
    intent->PutExtra(SettingsActivity::EXTRA_SHOW_FRAGMENT_AS_SHORTCUT, isShortcut);
    return intent;
}

AutoPtr<IUserHandle> Utils::GetManagedProfile(
    /* [in] */ IUserManager* userManager)
{
    AutoPtr<IList> userProfiles;
    userManager->GetUserProfiles((IList**)&userProfiles);
    Int32 count;
    userProfiles->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        userProfiles->Get(i, (IInterface**)&obj);
        AutoPtr<IUserHandle> profile = IUserHandle::Probe(obj);
        Int32 id, userHandle;
        profile->GetIdentifier(&id);
        if (id == (userManager->GetUserHandle(&userHandle), userHandle)) {
            continue;
        }
        AutoPtr<IUserInfo> userInfo;
        userManager->GetUserInfo(id, (IUserInfo**)&userInfo);
        Boolean res;
        if (userInfo->IsManagedProfile(&res), res) {
            return profile;
        }
    }
    return NULL;
}

Boolean Utils::IsManagedProfile(
    /* [in] */ IUserManager* userManager)
{
    Int32 userHandle;
    userManager->GetUserHandle(&userHandle);
    AutoPtr<IUserInfo> currentUser;
    userManager->GetUserInfo(userHandle, (IUserInfo**)&currentUser);
    Boolean res;
    currentUser->IsManagedProfile(&res);

    return res;
}

AutoPtr<UserSpinnerAdapter> Utils::CreateUserSpinnerAdapter(
    /* [in] */ IUserManager* userManager,
    /* [in] */ IContext* context)
{
    AutoPtr<IList> userProfiles;
    userManager->GetUserProfiles((IList**)&userProfiles);
    Int32 size;
    if ((userProfiles->GetSize(&size), size) < 2) {
        return NULL;
    }

    AutoPtr<IUserHandle> myUserHandle;
    CUserHandle::New(UserHandle::GetMyUserId(), (IUserHandle**)&myUserHandle);
    // The first option should be the current profile
    userProfiles->Remove(myUserHandle);
    userProfiles->Add(0, myUserHandle);

    Int32 count;
    userProfiles->GetSize(&count);
    AutoPtr<IArrayList> userDetails;
    CArrayList::New(count, (IArrayList**)&userDetails);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        userProfiles->Get(i, (IInterface**)&obj);
        AutoPtr<UserSpinnerAdapter::UserDetails> detail =
                new UserSpinnerAdapter::UserDetails(IUserHandle::Probe(obj),
                userManager, context);
        userDetails->Add((IObject*)detail);
    }

    AutoPtr<ISpinnerAdapter> adapter;
    CUserSpinnerAdapter::New(context, userDetails, (ISpinnerAdapter**)&adapter);

    return (UserSpinnerAdapter*)adapter.Get();
}

AutoPtr<IUserHandle> Utils::GetSecureTargetUser(
    /* [in] */ IBinder* activityToken,
    /* [in] */ IUserManager* um,
    /* [in] */ IBundle* arguments,
    /* [in] */ IBundle* intentExtras)
{
    AutoPtr<IUserHandle> currentUser;
    CUserHandle::New(UserHandle::GetMyUserId(), (IUserHandle**)&currentUser);
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    // try {
    String launchedFromPackage;
    am->GetLaunchedFromPackage(activityToken, &launchedFromPackage);
    Boolean launchedFromSettingsApp = SETTINGS_PACKAGE_NAME.Equals(launchedFromPackage);

    Int32 value;
    am->GetLaunchedFromUid(activityToken, &value);

    AutoPtr<IUserHandle> launchedFromUser;
    CUserHandle::New(UserHandle::GetUserId(value), (IUserHandle**)&launchedFromUser);
    Boolean res;
    if (launchedFromUser != NULL && (launchedFromUser->Equals(currentUser, &res), !res)) {
        // Check it's secure
        if (IsProfileOf(um, launchedFromUser)) {
            return launchedFromUser;
        }
    }
    AutoPtr<IUserHandle> extrasUser;
    if (intentExtras != NULL) {
        AutoPtr<IParcelable> parcelable;
        intentExtras->GetParcelable(IIntent::EXTRA_USER, (IParcelable**)&parcelable);
        extrasUser = IUserHandle::Probe(parcelable);
    }
    if (extrasUser != NULL && (extrasUser->Equals(currentUser, &res), !res)) {
        // Check it's secure
        if (launchedFromSettingsApp && IsProfileOf(um, extrasUser)) {
            return extrasUser;
        }
    }
    AutoPtr<IUserHandle> argumentsUser;
    if (arguments != NULL) {
        AutoPtr<IParcelable> parcelable;
        arguments->GetParcelable(IIntent::EXTRA_USER, (IParcelable**)&parcelable);
        argumentsUser = IUserHandle::Probe(parcelable);
    }
    if (argumentsUser != NULL && (argumentsUser->Equals(currentUser, &res), !res)) {
        // Check it's secure
        if (launchedFromSettingsApp && IsProfileOf(um, argumentsUser)) {
            return argumentsUser;
        }
    }
    // } catch (RemoteException e) {
    //     // Should not happen
    //     Logger::V(TAG, "Could not talk to activity manager.", e);
    // }
    return currentUser;
}

AutoPtr<IUserHandle> Utils::GetInsecureTargetUser(
    /* [in] */ IBinder* activityToken,
    /* [in] */ IBundle* arguments,
    /* [in] */ IBundle* intentExtras)
{
    AutoPtr<IUserHandle> currentUser;
    CUserHandle::New(UserHandle::GetMyUserId(), (IUserHandle**)&currentUser);
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();

   // try {
    Int32 value;
    am->GetLaunchedFromUid(activityToken, &value);
    AutoPtr<IUserHandle> launchedFromUser;
    CUserHandle::New(UserHandle::GetUserId(value), (IUserHandle**)&launchedFromUser);
    Boolean res;
    if (launchedFromUser != NULL && (launchedFromUser->Equals(currentUser, &res), res)) {
       return launchedFromUser;
    }

    AutoPtr<IUserHandle> extrasUser;
    if (intentExtras != NULL) {
        AutoPtr<IParcelable> parcelable;
        intentExtras->GetParcelable(IIntent::EXTRA_USER, (IParcelable**)&parcelable);
        extrasUser = IUserHandle::Probe(parcelable);
    }
    if (extrasUser != NULL && (extrasUser->Equals(currentUser, &res), !res)) {
        return extrasUser;
    }

    AutoPtr<IUserHandle> argumentsUser;
    if (arguments != NULL) {
        AutoPtr<IParcelable> parcelable;
        arguments->GetParcelable(IIntent::EXTRA_USER, (IParcelable**)&parcelable);
        argumentsUser = IUserHandle::Probe(parcelable);
    }
    if (argumentsUser != NULL && (argumentsUser->Equals(currentUser, &res), !res)) {
        return argumentsUser;
    }
   // } catch (RemoteException e) {
   //     // Should not happen
   //     Logger::V(TAG, "Could not talk to activity manager.", e);
   //     return NULL;
   // }
   return currentUser;
}

Boolean Utils::IsProfileOf(
    /* [in] */ IUserManager* um,
    /* [in] */ IUserHandle* otherUser)
{
   if (um == NULL || otherUser == NULL) return FALSE;
   Int32 identifier;
   otherUser->GetIdentifier(&identifier);
   AutoPtr<IList> list;
   um->GetUserProfiles((IList**)&list);
   Boolean res;
   list->Contains(otherUser, &res);
   return (UserHandle::GetMyUserId() == identifier)
           || res;
}

AutoPtr<IDialog> Utils::CreateRemoveConfirmationDialog(
    /* [in] */ IContext* context,
    /* [in] */ Int32 removingUserId,
    /* [in] */ IDialogInterfaceOnClickListener* onConfirmListener)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> um = IUserManager::Probe(obj);
    AutoPtr<IUserInfo> userInfo;
    um->GetUserInfo(removingUserId, (IUserInfo**)&userInfo);
    Int32 titleResId;
    Int32 messageResId;
    Boolean res;
    if (UserHandle::GetMyUserId() == removingUserId) {
        titleResId = R::string::user_confirm_remove_self_title;
        messageResId = R::string::user_confirm_remove_self_message;
    }
    else if (userInfo->IsRestricted(&res), res) {
        titleResId = R::string::user_profile_confirm_remove_title;
        messageResId = R::string::user_profile_confirm_remove_message;
    }
    else if (userInfo->IsManagedProfile(&res), res) {
        titleResId = R::string::work_profile_confirm_remove_title;
        messageResId = R::string::work_profile_confirm_remove_message;
    }
    else {
        titleResId = R::string::user_confirm_remove_title;
        messageResId = R::string::user_confirm_remove_message;
    }

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(titleResId);
    builder->SetMessage(messageResId);
    builder->SetPositiveButton(R::string::user_delete_button, onConfirmListener);
    builder->SetNegativeButton(Elastos::Droid::R::string::cancel, NULL);
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);
    AutoPtr<IDialog> dlg = IDialog::Probe(dialog);
    return dlg;
}

Boolean Utils::IsOemUnlockEnabled(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::PERSISTENT_DATA_BLOCK_SERVICE,
            (IInterface**)&obj);
    AutoPtr<IPersistentDataBlockManager> manager =
            IPersistentDataBlockManager::Probe(obj);
    Boolean res;
    manager->GetOemUnlockEnabled(&res);
    return res;
}

void Utils::SetOemUnlockEnabled(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::PERSISTENT_DATA_BLOCK_SERVICE,
            (IInterface**)&obj);
    AutoPtr<IPersistentDataBlockManager> manager =
            IPersistentDataBlockManager::Probe(obj);
    manager->SetOemUnlockEnabled(enabled);
}

AutoPtr<IDrawable> Utils::GetUserIcon(
    /* [in] */ IContext* context,
    /* [in] */ IUserManager* um,
    /* [in] */ IUserInfo* user)
{
    String iconPath;
    user->GetIconPath(&iconPath);
    Int32 id;
    user->GetId(&id);
    if (!iconPath.IsNull()) {
        AutoPtr<IBitmap> icon;
        um->GetUserIcon(id, (IBitmap**)&icon);
        if (icon != NULL) {
            AutoPtr<CircleFramedDrawable> cfd = CircleFramedDrawable::GetInstance(context, icon);
            return (IDrawable*)cfd.Get();
        }
    }
    AutoPtr<IUserIcons> userIcons;
    CUserIcons::AcquireSingleton((IUserIcons**)&userIcons);
    AutoPtr<IDrawable> drawable;
    userIcons->GetDefaultUserIcon(id, /* light= */ FALSE, (IDrawable**)&drawable);
    return drawable;
}

Boolean Utils::ShowSimCardTile(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(obj);

    // TODO: Uncomment to re-enable SimSettings.
    // return tm->GetSimCount() > 0;
    return FALSE;
}

Boolean Utils::IsSystemPackage(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IPackageInfo* pkg)
{
    if (sSystemSignature == NULL) {
        AutoPtr< ArrayOf<ISignature*> > signatures = ArrayOf<ISignature*>::Alloc(1);
        signatures->Set(0, GetSystemSignature(pm));
        sSystemSignature = signatures;
    }
    Boolean res;
    return (*sSystemSignature)[0] != NULL &&
            (IObject::Probe((*sSystemSignature)[0])->Equals(GetFirstSignature(pkg), &res), res);
}

AutoPtr<ISignature> Utils::GetFirstSignature(
    /* [in] */ IPackageInfo* pkg)
{
    AutoPtr< ArrayOf<ISignature*> > signatures;
    if (pkg != NULL) {
        pkg->GetSignatures((ArrayOf<ISignature*>**)&signatures);
        if (signatures != NULL) {
            if (signatures->GetLength() > 0) {
                return (*signatures)[0];
            }
        }
    }
    return NULL;
}

AutoPtr<ISignature> Utils::GetSystemSignature(
    /* [in] */ IPackageManager* pm)
{
    // try {
    AutoPtr<IPackageInfo> sys;
    ECode ec = pm->GetPackageInfo(String("android"),
            IPackageManager::GET_SIGNATURES, (IPackageInfo**)&sys);
    if (SUCCEEDED(ec)) {
        return GetFirstSignature(sys);
    }
    // } catch (NameNotFoundException e) {
    // }
    return NULL;
}

String Utils::FormatElapsedTime(
    /* [in] */ IContext* context,
    /* [in] */ Double millis,
    /* [in] */ Boolean withSeconds)
{
    Int32 seconds = (Int32) Elastos::Core::Math::Floor(millis / 1000);
    if (!withSeconds) {
        // Round up.
        seconds += 30;
    }

    Int32 days = 0, hours = 0, minutes = 0;
    if (seconds >= SECONDS_PER_DAY) {
        days = seconds / SECONDS_PER_DAY;
        seconds -= days * SECONDS_PER_DAY;
    }
    if (seconds >= SECONDS_PER_HOUR) {
        hours = seconds / SECONDS_PER_HOUR;
        seconds -= hours * SECONDS_PER_HOUR;
    }
    if (seconds >= SECONDS_PER_MINUTE) {
        minutes = seconds / SECONDS_PER_MINUTE;
        seconds -= minutes * SECONDS_PER_MINUTE;
    }

    StringBuilder sb;
    String str;
    if (withSeconds) {
        if (days > 0) {
            AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(4);
            (*formatArgs)[0] = CoreUtils::Convert(days);
            (*formatArgs)[1] = CoreUtils::Convert(hours);
            (*formatArgs)[2] = CoreUtils::Convert(minutes);
            (*formatArgs)[3] = CoreUtils::Convert(seconds);

            context->GetString(R::string::battery_history_days, formatArgs, &str);
            sb.Append(str);
        }
        else if (hours > 0) {
            AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(3);
            (*formatArgs)[0] = CoreUtils::Convert(hours);
            (*formatArgs)[1] = CoreUtils::Convert(minutes);
            (*formatArgs)[2] = CoreUtils::Convert(seconds);

            context->GetString(R::string::battery_history_hours, formatArgs, &str);
            sb.Append(str);
        }
        else if (minutes > 0) {
            AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(2);
            (*formatArgs)[0] = CoreUtils::Convert(minutes);
            (*formatArgs)[1] = CoreUtils::Convert(seconds);

            context->GetString(R::string::battery_history_minutes, formatArgs, &str);
            sb.Append(str);
        }
        else {
            AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
            (*formatArgs)[0] = CoreUtils::Convert(seconds);

            context->GetString(R::string::battery_history_seconds, formatArgs, &str);
            sb.Append(str);
        }
    }
    else {
        if (days > 0) {
            AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(3);
            (*formatArgs)[0] = CoreUtils::Convert(days);
            (*formatArgs)[1] = CoreUtils::Convert(hours);
            (*formatArgs)[2] = CoreUtils::Convert(minutes);

            context->GetString(R::string::battery_history_days_no_seconds,
                    formatArgs, &str);
            sb.Append(str);
        }
        else if (hours > 0) {
            AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(2);
            (*formatArgs)[0] = CoreUtils::Convert(hours);
            (*formatArgs)[1] = CoreUtils::Convert(minutes);

            context->GetString(R::string::battery_history_hours_no_seconds,
                    formatArgs, &str);
            sb.Append(str);
        }
        else {
            AutoPtr< ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
            (*formatArgs)[0] = CoreUtils::Convert(minutes);

            context->GetString(R::string::battery_history_minutes_no_seconds,
                    formatArgs, &str);
            sb.Append(str);
        }
    }
    return sb.ToString();
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
