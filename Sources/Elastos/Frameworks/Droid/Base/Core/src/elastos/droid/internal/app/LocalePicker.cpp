
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/internal/app/LocalePicker.h"
#include "elastos/droid/app/ActivityManagerNative.h"
//#include "elastos/droid/app/backup/CBackupManagerHelper.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/provider/Settings.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/R.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::Backup::IBackupManagerHelper;
//using Elastos::Droid::App::Backup::CBackupManagerHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::ISpinnerAdapter;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::ICharSequence;
using Elastos::Text::CCollatorHelper;
using Elastos::Text::ICollatorHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ICollections;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

ECode LocalePicker::LocalArrayAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ IList* objects,
    /* [in] */ ILayoutInflater* inflater)
{
    FAIL_RETURN(ArrayAdapter::constructor(context, resource, textViewResourceId, objects));
    mInflater = inflater;
    mLayoutId = resource;
    mFieldId = textViewResourceId;
    return NOERROR;
}

ECode LocalePicker::LocalArrayAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** retView)
{
    VALIDATE_NOT_NULL(retView);
    AutoPtr<IView> view;
    AutoPtr<ITextView> text;
    if (convertView == NULL) {
        mInflater->Inflate(mLayoutId, parent, FALSE, (IView**)&view);
        AutoPtr<IView> tempView;
        view->FindViewById(mFieldId, (IView**)&tempView);
        text = ITextView::Probe(tempView);
        view->SetTag(text);
    }
    else {
        view = convertView;
        AutoPtr<IInterface> tempView;
        view->GetTag((IInterface**)&tempView);
        text = ITextView::Probe(tempView);
    }

    AutoPtr<IInterface> _item;
    GetItem(position, (IInterface**)&_item);
    AutoPtr<LocaleInfo> item = (LocaleInfo*)ILocaleInfo::Probe(_item);
    String string;
    item->ToString(&string);
    text->SetText(CoreUtils::Convert(string));
    AutoPtr<ILocale> locale;
    item->GetLocale((ILocale**)&locale);
    text->SetTextLocale(locale);

    *retView = view;
    REFCOUNT_ADD(*retView);
    return NOERROR;
}

const String LocalePicker::TAG("LocalePicker");

CAR_INTERFACE_IMPL_2(LocalePicker::LocaleInfo, Object, ILocaleInfo, IComparable);

static AutoPtr<ICollator> InitCollator()
{
    AutoPtr<ICollatorHelper> helper;
    CCollatorHelper::AcquireSingleton((ICollatorHelper**)&helper);
    AutoPtr<ICollator> collator;
    helper->GetInstance((ICollator**)&collator);
    return collator;
}
const AutoPtr<ICollator> LocalePicker::LocaleInfo::sCollator = InitCollator();

LocalePicker::LocaleInfo::LocaleInfo(
    /* [in] */ const String& label,
    /* [in] */ ILocale* locale)
    : mLabel(label)
    , mLocale(locale)
{
}

ECode LocalePicker::LocaleInfo::GetLabel(
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label);

    *label = mLabel;
    return NOERROR;
}

ECode LocalePicker::LocaleInfo::GetLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);

    *locale = mLocale;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode LocalePicker::LocaleInfo::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    *string = mLabel;
    return NOERROR;
}

ECode LocalePicker::LocaleInfo::CompareTo(
    /* [in] */ IInterface* _another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    LocaleInfo* another = (LocaleInfo*)IComparable::Probe(_another);
    return sCollator->Compare(mLabel, another->mLabel, result);
}

CAR_INTERFACE_IMPL(LocalePicker, ListFragment, ILocalePicker)

ECode LocalePicker::GetAllAssetLocales(
    /* [in] */ IContext* context,
    /* [in] */ Boolean isInDeveloperMode,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IAssetManager> assetMgr;
    CResources::GetSystem()->GetAssets((IAssetManager**)&assetMgr);
    AutoPtr<ArrayOf<String> > locales;
    assetMgr->GetLocales((ArrayOf<String>**)&locales);

    AutoPtr<IList> localeList;
    CArrayList::New(locales->GetLength(), (IList**)&localeList);
    for (Int32 i = 0; i < locales->GetLength(); i++) {
        Boolean b;
        localeList->Add(CoreUtils::Convert((*locales)[i]), &b);
    }

    // Don't show the pseudolocales unless we're in developer mode. http://b/17190407.
    if (!isInDeveloperMode) {
        localeList->Remove(CoreUtils::Convert(String("ar-XB")));
        localeList->Remove(CoreUtils::Convert(String("en-XA")));
    }

    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(localeList);
    AutoPtr<ArrayOf<String> > specialLocaleCodes;
    resources->GetStringArray(R::array::special_locale_codes, (ArrayOf<String>**)&specialLocaleCodes);
    AutoPtr<ArrayOf<String> > specialLocaleNames;
    resources->GetStringArray(R::array::special_locale_names, (ArrayOf<String>**)&specialLocaleNames);

    Int32 size;
    localeList->GetSize(&size);
    AutoPtr<IList> localeInfos;
    CArrayList::New(size, (IList**)&localeInfos);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        localeList->Get(i, (IInterface**)&item);
        String locale;
        ICharSequence::Probe(item)->ToString(&locale);
        AutoPtr<ILocaleHelper> lHelper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lHelper);
        AutoPtr<ILocale> l;
        lHelper->ForLanguageTag(locale.Replace('_', '-'), (ILocale**)&l);
        String language, country ;
        if (l == NULL || (l->GetLanguage(&language), language.Equals("und"))
            || language.IsEmpty() || (l->GetCountry(&country), country.IsEmpty())) {
            continue;
        }

        Boolean isEmpty;
        localeInfos->IsEmpty(&isEmpty);
        String dl;
        l->GetDisplayLanguage(l, &dl);
        if (isEmpty) {
            if (DEBUG) {
                Logger::V(TAG, "adding initial %s", ToTitleCase(dl).string());
            }
            AutoPtr<ILocaleInfo> info = new LocaleInfo(ToTitleCase(dl), l);
            localeInfos->Add(info);
        }
        else {
            // check previous entry:
            //  same lang and a country -> upgrade to full name and
            //    insert ours with full name
            //  diff lang -> insert ours with lang-only name
            Int32 count;
            localeInfos->GetSize(&count);
            AutoPtr<IInterface> item;
            localeInfos->Get(count - 1, (IInterface**)&item);
            LocaleInfo* previous = (LocaleInfo*)ILocaleInfo::Probe(item);
            String prevLang;
            previous->mLocale->GetLanguage(&prevLang);
            if (prevLang.Equals(language) && !prevLang.Equals("zz")) {
                if (DEBUG) {
                    Logger::V(TAG, "backing up and fixing %s to %s", previous->mLabel.string(),
                        GetDisplayName(previous->mLocale, specialLocaleCodes, specialLocaleNames).string());
                }
                previous->mLabel = ToTitleCase(GetDisplayName(
                        previous->mLocale, specialLocaleCodes, specialLocaleNames));
                if (DEBUG) {
                    Logger::V(TAG, "  and adding %s", ToTitleCase(
                        GetDisplayName(l, specialLocaleCodes, specialLocaleNames)).string());
                }
                AutoPtr<ILocaleInfo> info = new LocaleInfo(ToTitleCase(
                    GetDisplayName(l, specialLocaleCodes, specialLocaleNames)), l);
                localeInfos->Add(info);
            }
            else {
                String displayName = ToTitleCase(dl);
                if (DEBUG) {
                    Logger::V(TAG, "Adding %s", displayName.string());
                }
                AutoPtr<ILocaleInfo> info = new LocaleInfo(displayName, l);
                localeInfos->Add(info);
            }
        }
    }

    collections->Sort(localeInfos);
    *list = localeInfos;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode LocalePicker::ConstructAdapter(
    /* [in] */ IContext* context,
    /* [out] */ IArrayAdapter** adapter)
{
    return ConstructAdapter(context, R::layout::locale_picker_item,
        R::id::locale, adapter);
}

ECode LocalePicker::ConstructAdapter(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layoutId,
    /* [in] */ Int32 fieldId,
    /* [out] */ IArrayAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter);

    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 value;
    Settings::Global::GetInt32(cr, ISettingsGlobal::DEVELOPMENT_SETTINGS_ENABLED, 0, &value);
    Boolean isInDeveloperMode = value != 0;
    AutoPtr<IList> localeInfos;
    GetAllAssetLocales(context, isInDeveloperMode, (IList**)&localeInfos);

    AutoPtr<IInterface> service;
    FAIL_RETURN(context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service))
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(service);
    AutoPtr<LocalArrayAdapter> localAdapter = new LocalArrayAdapter();
    localAdapter->constructor(context, layoutId, fieldId, localeInfos, inflater);
    *adapter = localAdapter;
    REFCOUNT_ADD(*adapter);
    return NOERROR;
}

String LocalePicker::ToTitleCase(
    /* [in] */ const String& s)
{
    if (s.IsNullOrEmpty()) {
        return s;
    }

    return s.ToUpperCase(0, 1);
}

String LocalePicker::GetDisplayName(
    /* [in] */ ILocale* l,
    /* [in] */ ArrayOf<String>* specialLocaleCodes,
    /* [in] */ ArrayOf<String>* specialLocaleNames)
{
    String code;
    l->ToString(&code);

    for (Int32 i = 0; i < specialLocaleCodes->GetLength(); i++) {
        if ((*specialLocaleCodes)[i] == code) {
            return (*specialLocaleNames)[i];
        }
    }

    String name;
    l->GetDisplayName(l, &name);
    return name;
}

ECode LocalePicker::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    ListFragment::OnActivityCreated(savedInstanceState);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IArrayAdapter> adapter;
    ConstructAdapter(IContext::Probe(activity), (IArrayAdapter**)&adapter);
    SetListAdapter(IListAdapter::Probe(adapter));
    return NOERROR;
}

ECode LocalePicker::SetLocaleSelectionListener(
    /* [in] */ ILocaleSelectionListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode LocalePicker::OnResume()
{
    ListFragment::OnResume();
    AutoPtr<IListView> lview;
    GetListView((IListView**)&lview);
    Boolean result;
    IView::Probe(lview)->RequestFocus(&result);
    return NOERROR;
}

ECode LocalePicker::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    if (mListener != NULL) {
        AutoPtr<IListAdapter> ladapter;
        GetListAdapter((IListAdapter**)&ladapter);
        AutoPtr<IInterface> item;
        IAdapter::Probe(ladapter)->GetItem(position, (IInterface**)&item);
        AutoPtr<ILocale> locale = ((LocaleInfo*)ILocaleInfo::Probe(item))->mLocale;
        mListener->OnLocaleSelected(locale);
    }
    return NOERROR;
}

ECode LocalePicker::UpdateLocale(
    /* [in] */ ILocale* locale)
{
//     try {
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IConfiguration> config;
    am->GetConfiguration((IConfiguration**)&config);

    // Will set userSetLocale to indicate this isn't some passing default - the user
    // wants this remembered
    config->SetLocale(locale);

    am->UpdateConfiguration(config);
    // Trigger the dirty bit for the Settings Provider.
    // TODO
    Logger::D("LocalePicker::UpdateLocale", "CBackupManagerHelper is TODO");
    // AutoPtr<IBackupManagerHelper> backupMgrHelper;
    // CBackupManagerHelper::AcquireSingleton((IBackupManagerHelper**)&backupMgrHelper);
    // backupMgrHelper->DataChanged(String("com.android.providers.settings"));

//     } catch (RemoteException e) {
//         // Intentionally left blank
//     }
    return NOERROR;
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
