
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/notification/ManagedServiceSettings.h"
#include "elastos/droid/settings/notification/CManagedServiceSettingsScaryWarningDialogFragment.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::CPackageItemInfoDisplayNameComparator;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  ManagedServiceSettings::ScaryWarningDialogFragment
//===============================================================================

const String ManagedServiceSettings::ScaryWarningDialogFragment::KEY_COMPONENT("c");
const String ManagedServiceSettings::ScaryWarningDialogFragment::KEY_LABEL("l");

ManagedServiceSettings::ScaryWarningDialogFragment::ScaryWarningDialogFragment()
{}

ECode ManagedServiceSettings::ScaryWarningDialogFragment::constructor(
    /* [in] */ IListFragment* host)
{
    mHost = (ManagedServiceSettings*)host;

    return DialogFragment::constructor();
}

ECode ManagedServiceSettings::ScaryWarningDialogFragment::SetServiceInfo(
    /* [in] */ IComponentName* cn,
    /* [in] */ const String& label)
{
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    String str;
    cn->FlattenToString(&str);
    args->PutString(KEY_COMPONENT, str);
    args->PutString(KEY_LABEL, label);
    SetArguments(args);
    return NOERROR;
}

ECode ManagedServiceSettings::ScaryWarningDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** result)
{
    VALIDATE_NOT_NULL(result)

    DialogFragment::OnCreate(savedInstanceState);
    AutoPtr<IBundle> args;
    GetArguments((IBundle**)&args);
    String label;
    args->GetString(KEY_LABEL, &label);
    String str;
    args->GetString(KEY_COMPONENT, &str);
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    AutoPtr<IComponentName> cn;
    helper->UnflattenFromString(str, (IComponentName**)&cn);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);

    AutoPtr< ArrayOf<IInterface*> > argsLabel = ArrayOf<IInterface*>::Alloc(1);
    argsLabel->Set(0, CoreUtils::Convert(label));

    String title;
    resources->GetString(mHost->mConfig->mWarningDialogTitle, argsLabel, &title);
    String summary;
    resources->GetString(mHost->mConfig->mWarningDialogSummary, argsLabel, &summary);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
    builder->SetMessage(CoreUtils::Convert(summary));
    builder->SetTitle(CoreUtils::Convert(title));
    builder->SetCancelable(TRUE);
    AutoPtr<DialogInterfaceOnClickListener> listener = new DialogInterfaceOnClickListener(mHost, 0, cn);
    builder->SetPositiveButton(Elastos::Droid::R::string::ok, listener);
    AutoPtr<DialogInterfaceOnClickListener> listener1 = new DialogInterfaceOnClickListener(mHost, 1, NULL);
    builder->SetNegativeButton(Elastos::Droid::R::string::cancel, listener1);
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);
    *result = IDialog::Probe(dialog);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===============================================================================
//                  ManagedServiceSettings::ServiceListAdapter
//===============================================================================

ManagedServiceSettings::ServiceListAdapter::ServiceListAdapter(
    /* [in] */ IContext* context,
    /* [in] */ ManagedServiceSettings* host)
    : mHost(host)
{
    ArrayAdapter::constructor(context, 0, 0);
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);
}

ECode ManagedServiceSettings::ServiceListAdapter::HasStableIds(
    /* [out] */ Boolean* result)
{
    *result = TRUE;
    return NOERROR;
}

ECode ManagedServiceSettings::ServiceListAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    *itemId = position;
    return NOERROR;
}

ECode ManagedServiceSettings::ServiceListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    AutoPtr<IView> v;
    if (convertView == NULL) {
        v = NewView(parent);
    }
    else {
        v = convertView;
    }
    BindView(v, position);
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

AutoPtr<IView> ManagedServiceSettings::ServiceListAdapter::NewView(
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> v;
    mInflater->Inflate(R::layout::managed_service_item, parent, FALSE, (IView**)&v);
    AutoPtr<ViewHolder> h = new ViewHolder();
    AutoPtr<IView> tmp;
    v->FindViewById(R::id::icon, (IView**)&tmp);
    h->mIcon = IImageView::Probe(tmp);
    tmp = NULL;
    v->FindViewById(R::id::name, (IView**)&tmp);
    h->mName = ITextView::Probe(tmp);
    tmp = NULL;
    v->FindViewById(R::id::checkbox, (IView**)&tmp);
    h->mCheckbox = ICheckBox::Probe(tmp);
    tmp = NULL;
    v->FindViewById(R::id::description, (IView**)&tmp);
    h->mDescription = ITextView::Probe(tmp);
    v->SetTag((IObject*)h);
    return v;
}

ECode ManagedServiceSettings::ServiceListAdapter::BindView(
    /* [in] */ IView* view,
    /* [in] */ Int32 position)
{
    AutoPtr<IInterface> obj;
    view->GetTag((IInterface**)&obj);
    AutoPtr<ViewHolder> vh = (ViewHolder*)IObject::Probe(obj);
    AutoPtr<IInterface> objItem;
    GetItem(position, (IInterface**)&objItem);
    AutoPtr<IServiceInfo> info = IServiceInfo::Probe(objItem);

    IPackageItemInfo* _info = IPackageItemInfo::Probe(info);
    AutoPtr<IDrawable> drawable;
    _info->LoadIcon(mHost->mPM, (IDrawable**)&drawable);
    vh->mIcon->SetImageDrawable(drawable);
    AutoPtr<ICharSequence> cs;
    _info->LoadLabel(mHost->mPM, (ICharSequence**)&cs);
    vh->mName->SetText(cs);
    if (SHOW_PACKAGE_NAME) {
        String packageName;
        _info->GetPackageName(&packageName);
        vh->mDescription->SetText(CoreUtils::Convert(packageName));
        IView::Probe(vh->mDescription)->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(vh->mDescription)->SetVisibility(IView::GONE);
    }
    ICheckable::Probe(vh->mCheckbox)->SetChecked(mHost->IsServiceEnabled(info));
    return NOERROR;
}

//===============================================================================
//                  ManagedServiceSettings::MyContentObserver
//===============================================================================

ManagedServiceSettings::MyContentObserver::MyContentObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ ManagedServiceSettings* host)
    : mHost(host)
{}

ECode ManagedServiceSettings::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    mHost->UpdateList();
    return NOERROR;
}

//===============================================================================
//                  ManagedServiceSettings::MyBroadcastReceiver
//===============================================================================

ManagedServiceSettings::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ ManagedServiceSettings* host)
    : mHost(host)
{}

ECode ManagedServiceSettings::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->UpdateList();
    return NOERROR;
}

//===============================================================================
//                  ManagedServiceSettings::DialogInterfaceOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(ManagedServiceSettings::DialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

ManagedServiceSettings::DialogInterfaceOnClickListener::DialogInterfaceOnClickListener(
    /* [in] */ ManagedServiceSettings* host,
    /* [in] */ Int32 id,
    /* [in] */ IComponentName* cn)
    : mHost(host)
    , mId(id)
    , mCn(cn)
{}

ECode ManagedServiceSettings::DialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 id)
{
    switch (mId) {
        case 0:
            mHost->mEnabledServices->Add(mCn);
            mHost->SaveEnabledServices();
            break;
        case 1:
            // pass
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  ManagedServiceSettings
//===============================================================================

const Boolean ManagedServiceSettings::SHOW_PACKAGE_NAME = FALSE;

ManagedServiceSettings::ManagedServiceSettings()
{
    CHashSet::New((IHashSet**)&mEnabledServices);

    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mSettingsObserver = new MyContentObserver(handler, this);
    mPackageReceiver = new MyBroadcastReceiver(this);
}

ECode ManagedServiceSettings::constructor()
{
    ListFragment::constructor();

    mConfig = GetConfig();
    return NOERROR;
}

ECode ManagedServiceSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    ListFragment::OnCreate(icicle);

    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    activity->GetPackageManager((IPackageManager**)&mPM);
    activity->GetContentResolver((IContentResolver**)&mCR);
    mListAdapter = new ServiceListAdapter(activity, this);
    return NOERROR;
}

ECode ManagedServiceSettings::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IView> v;
    inflater->Inflate(R::layout::managed_service_settings, container, FALSE, (IView**)&v);
    AutoPtr<IView> tmp;
    v->FindViewById(Elastos::Droid::R::id::empty, (IView**)&tmp);
    ITextView* empty = ITextView::Probe(tmp);
    empty->SetText(mConfig->mEmptyText);
    *result = v;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ManagedServiceSettings::OnResume()
{
    ListFragment::OnResume();
    UpdateList();

    // listen for package changes
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
    filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
    filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    filter->AddAction(IIntent::ACTION_PACKAGE_REPLACED);
    filter->AddDataScheme(String("package"));

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    IContext::Probe(activity)->RegisterReceiver(mPackageReceiver, filter, (IIntent**)&intent);

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IUri> uri;
    secure->GetUriFor(mConfig->mSetting, (IUri**)&uri);
    mCR->RegisterContentObserver(uri, FALSE, mSettingsObserver);
    return NOERROR;
}

ECode ManagedServiceSettings::OnPause()
{
    ListFragment::OnPause();

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->UnregisterReceiver(mPackageReceiver);
    mCR->UnregisterContentObserver(mSettingsObserver);
    return NOERROR;
}

void ManagedServiceSettings::LoadEnabledServices()
{
    mEnabledServices->Clear();

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    String flat;
    secure->GetString(mCR, mConfig->mSetting, &flat);
    if (!flat.IsNull() && !String("").Equals(flat)) {
        AutoPtr< ArrayOf<String> > names;
        StringUtils::Split(flat, ":", (ArrayOf<String>**)&names);
        for (Int32 i = 0; i < names->GetLength(); i++) {
            AutoPtr<IComponentNameHelper> helper;
            CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
            AutoPtr<IComponentName> cn;
            helper->UnflattenFromString((*names)[i], (IComponentName**)&cn);
            if (cn != NULL) {
                mEnabledServices->Add(cn);
            }
        }
    }
}

void ManagedServiceSettings::SaveEnabledServices()
{
    AutoPtr<StringBuilder> sb;

    AutoPtr<IIterator> it;
    mEnabledServices->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IComponentName> cn = IComponentName::Probe(obj);
        if (sb == NULL) {
            sb = new StringBuilder();
        }
        else {
            sb->AppendChar(':');
        }
        String str;
        cn->FlattenToString(&str);
        sb->Append(str);
    }

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Boolean res;
    secure->PutString(mCR, mConfig->mSetting,
            sb != NULL ? sb->ToString() : String(""), &res);
}

void ManagedServiceSettings::UpdateList()
{
    LoadEnabledServices();

    GetServices(mConfig, mListAdapter, mPM);
    AutoPtr<IComparator> comparator;
    CPackageItemInfoDisplayNameComparator::New(mPM, (IComparator**)&comparator);
    mListAdapter->Sort(comparator);

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    IAdapterView::Probe(listView)->SetAdapter(mListAdapter);
}

Int32 ManagedServiceSettings::GetEnabledServicesCount(
    /* [in] */ Config* config,
    /* [in] */ IContext* context)
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    String flat;
    secure->GetString(resolver, config->mSetting, &flat);
    if (flat.IsNull() || String("").Equals(flat)) return 0;
    AutoPtr< ArrayOf<String> > components;
    StringUtils::Split(flat, ":", (ArrayOf<String>**)&components);
    return components->GetLength();
}

Int32 ManagedServiceSettings::GetServicesCount(
    /* [in] */ Config* c,
    /* [in] */ IPackageManager* pm)
{
    return GetServices(c, NULL, pm);
}

Int32 ManagedServiceSettings::GetServices(
    /* [in] */ Config* c,
    /* [in] */ IArrayAdapter* adapter,
    /* [in] */ IPackageManager* pm)
{
    Int32 services = 0;
    if (adapter != NULL) {
        adapter->Clear();
    }
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 user;
    helper->GetCurrentUser(&user);

    AutoPtr<IIntent> intent;
    CIntent::New(c->mIntentAction, (IIntent**)&intent);
    AutoPtr<IList> installedServices;//List<ResolveInfo>
    pm->QueryIntentServicesAsUser(
            intent, IPackageManager::GET_SERVICES | IPackageManager::GET_META_DATA,
            user, (IList**)&installedServices);

    Int32 count;
    installedServices->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        installedServices->Get(i, (IInterface**)&obj);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(obj);
        AutoPtr<IServiceInfo> info;
        resolveInfo->GetServiceInfo((IServiceInfo**)&info);

        String permission;
        info->GetPermission(&permission);
        if (!c->mPermission.Equals(permission)) {
            IPackageItemInfo* _info = IPackageItemInfo::Probe(info);
            String packageName, name;
            _info->GetPackageName(&packageName);
            _info->GetName(&name);
            Slogger::W(c->mTag, "Skipping %s service %s/%s: it does not require the permission %s",
                    c->mNoun.string(), packageName.string(), name.string(), c->mPermission.string());
            continue;
        }
        if (adapter != NULL) {
            adapter->Add(info);
        }
        services++;
    }
    return services;
}

Boolean ManagedServiceSettings::IsServiceEnabled(
    /* [in] */ IServiceInfo* info)
{
    IPackageItemInfo* _info = IPackageItemInfo::Probe(info);

    String packageName, name;
    _info->GetPackageName(&packageName);
    _info->GetName(&name);
    AutoPtr<IComponentName> cn;
    CComponentName::New(packageName, name, (IComponentName**)&cn);

    Boolean res;
    mEnabledServices->Contains(cn, &res);
    return res;
}

ECode ManagedServiceSettings::OnListItemClick(
    /* [in] */ IListView* l,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> obj;
    mListAdapter->GetItem(position, (IInterface**)&obj);
    IPackageItemInfo* /*IServiceInfo*/ info = IPackageItemInfo::Probe(obj);

    String packageName, name;
    info->GetPackageName(&packageName);
    info->GetName(&name);
    AutoPtr<IComponentName> cn;
    CComponentName::New(packageName, name, (IComponentName**)&cn);
    Boolean res;
    mEnabledServices->Contains(cn, &res);
    if (res) {
        // the simple version: disabling
        mEnabledServices->Remove(cn);
        SaveEnabledServices();
    }
    else {
        // show a scary dialog
        AutoPtr<ICharSequence> cs;
        info->LoadLabel(mPM, (ICharSequence**)&cs);
        AutoPtr<CManagedServiceSettingsScaryWarningDialogFragment> fragment;
        CManagedServiceSettingsScaryWarningDialogFragment::NewByFriend((IListFragment*) this,
                (CManagedServiceSettingsScaryWarningDialogFragment**)&fragment);
        fragment->SetServiceInfo(cn, Object::ToString(cs));
        AutoPtr<IFragmentManager> fm;
        GetFragmentManager((IFragmentManager**)&fm);
        fragment->Show(fm, String("dialog"));
    }
    return NOERROR;
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos