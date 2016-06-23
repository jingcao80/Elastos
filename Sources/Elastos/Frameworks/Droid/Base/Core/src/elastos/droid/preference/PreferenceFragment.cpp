
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/preference/PreferenceFragment.h"
#include "elastos/droid/preference/CPreferenceManager.h"
#include "elastos/droid/preference/DialogPreference.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::EIID_IFragment;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Preference {

const String PreferenceFragment::PREFERENCES_TAG("android:preferences");
const Int32 PreferenceFragment::FIRST_REQUEST_CODE;
const Int32 PreferenceFragment::MSG_BIND_PREFERENCES;

/////////////////////////////////////////////////////
// PreferenceFragment::PreferenceFragmentHandler
/////////////////////////////////////////////////////

PreferenceFragment::PreferenceFragmentHandler::PreferenceFragmentHandler(
    /* [in] */ PreferenceFragment* host)
    : mHost(host)
{}

ECode PreferenceFragment::PreferenceFragmentHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_BIND_PREFERENCES:
            mHost->BindPreferences();
            break;
    }
    return NOERROR;
}

/////////////////////////////////////////////////////
// PreferenceFragment::RequestFocus
/////////////////////////////////////////////////////

PreferenceFragment::RequestFocus::RequestFocus(
    /* [in] */ PreferenceFragment* host)
    : mHost(host)
{}

ECode PreferenceFragment::RequestFocus::Run()
{
    return IViewParent::Probe(mHost->mList)->FocusableViewAvailable(IView::Probe(mHost->mList));
}

/////////////////////////////////////////////////////
// PreferenceFragment::PreferenceFragmentOnKeyListener
/////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(PreferenceFragment::PreferenceFragmentOnKeyListener, Object, IViewOnKeyListener)

PreferenceFragment::PreferenceFragmentOnKeyListener::PreferenceFragmentOnKeyListener(
    /* [in] */ PreferenceFragment* host)
    : mHost(host)
{}

ECode PreferenceFragment::PreferenceFragmentOnKeyListener::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out]*/ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> selectedItem;
    IAdapterView::Probe(mHost->mList)->GetSelectedItem((IInterface**)&selectedItem);
    AutoPtr<IPreference> preferernce = IPreference::Probe(selectedItem);
    if (preferernce != NULL) {
        AutoPtr<IView> selectedView;
        IAdapterView::Probe(mHost->mList)->GetSelectedView((IView**)&selectedView);
        return preferernce->OnKey(selectedView, keyCode, event, result);
    }
    *result = FALSE;
    return NOERROR;
}


/////////////////////////////////////////////////////
// PreferenceFragment
/////////////////////////////////////////////////////

CAR_INTERFACE_IMPL_2(PreferenceFragment, Fragment, IPreferenceFragment,IPreferenceManagerOnPreferenceTreeClickListener)

PreferenceFragment::PreferenceFragment()
    : mHavePrefs(FALSE)
    , mInitDone(FALSE)
    , mLayoutResId(R::layout::preference_list_fragment)
{
}

ECode PreferenceFragment::OnCreate(
    /*[in]*/ IBundle* savedInstanceState)
{
    Fragment::OnCreate(savedInstanceState);

    mHandler = new PreferenceFragmentHandler(this);
    mHandler->constructor();
    mRequestFocus = new RequestFocus(this);
    mListOnKeyListener = new PreferenceFragmentOnKeyListener(this);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CPreferenceManager::New(activity, FIRST_REQUEST_CODE, (IPreferenceManager**)&mPreferenceManager);
    mPreferenceManager->SetFragment(this);
    return NOERROR;
}

ECode PreferenceFragment::OnCreateView(
    /*[in]*/ ILayoutInflater* inflater,
    /*[in]*/ IViewGroup* container,
    /*[in]*/ IBundle* savedInstanceState,
    /*[out]*/ IView** view)
{
    VALIDATE_NOT_NULL(view)

    AutoPtr<IActivity> activity;
    Fragment::GetActivity((IActivity**)&activity);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::PreferenceFragment);
    AutoPtr<ITypedArray> a;
    IContext::Probe(activity)->ObtainStyledAttributes(NULL, attrIds,
         R::attr::preferenceFragmentStyle, 0, (ITypedArray**)&a);

    a->GetResourceId(R::styleable::PreferenceFragment_layout, mLayoutResId, &mLayoutResId);
    a->Recycle();

    return inflater->Inflate(mLayoutResId, container, FALSE, view);
}

ECode PreferenceFragment::OnActivityCreated(
    /*[in]*/ IBundle* savedInstanceState)
{
    Fragment::OnActivityCreated(savedInstanceState);

    if (mHavePrefs) {
        BindPreferences();
    }

    mInitDone = TRUE;

    if (savedInstanceState != NULL) {
        AutoPtr<IBundle> container;
        savedInstanceState->GetBundle(PREFERENCES_TAG, (IBundle**)&container);
        if (container != NULL) {
          AutoPtr<IPreferenceScreen> preferenceScreen;
          GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
          if (preferenceScreen != NULL) {
              IPreference::Probe(preferenceScreen)->RestoreHierarchyState(container);
          }
        }
    }
    return NOERROR;
}

ECode PreferenceFragment::OnStart()
{
    Fragment::OnStart();
    mPreferenceManager->SetOnPreferenceTreeClickListener(this);
    return NOERROR;
}

ECode PreferenceFragment::OnStop()
{
    Fragment::OnStop();
    mPreferenceManager->DispatchActivityStop();
    mPreferenceManager->SetOnPreferenceTreeClickListener(NULL);
    return NOERROR;
}

ECode PreferenceFragment::OnDestroyView()
{
    mList = NULL;
    mHandler->RemoveCallbacks(mRequestFocus);
    mHandler->RemoveMessages(MSG_BIND_PREFERENCES);
    return Fragment::OnDestroyView();
}

ECode PreferenceFragment::OnDestroy()
{
    Fragment::OnDestroy();
    mPreferenceManager->DispatchActivityDestroy();
    return NOERROR;
}

ECode PreferenceFragment::OnSaveInstanceState(
    /*[in]*/ IBundle* outState)
{
    Fragment::OnSaveInstanceState(outState);

    AutoPtr<IPreferenceScreen> preferenceScreen;
    GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    if (preferenceScreen != NULL) {
        AutoPtr<IBundle> container;
        CBundle::New((IBundle**)&container);
        IPreference::Probe(preferenceScreen)->SaveHierarchyState(container);
        outState->PutBundle(PREFERENCES_TAG, container);
    }
    return NOERROR;
}

ECode PreferenceFragment::OnActivityResult(
    /*[in]*/ Int32 requestCode,
    /*[in]*/ Int32 resultCode,
    /*[in]*/ IIntent* data)
{
    Fragment::OnActivityResult(requestCode, resultCode, data);
    mPreferenceManager->DispatchActivityResult(requestCode, resultCode, data);
    return NOERROR;
}

ECode PreferenceFragment::GetPreferenceManager(
    /*[out]*/ IPreferenceManager** preferencemanager)
{
    VALIDATE_NOT_NULL(preferencemanager)
    *preferencemanager = mPreferenceManager;
    REFCOUNT_ADD(*preferencemanager)
    return NOERROR;
}

ECode PreferenceFragment::SetPreferenceScreen(
    /*[in]*/ IPreferenceScreen* preferenceScreen)
{
    Boolean result;
    if ((mPreferenceManager->SetPreferences(preferenceScreen, &result), result) && preferenceScreen != NULL) {
        OnUnbindPreferences();
        mHavePrefs = TRUE;
        if (mInitDone) {
            PostBindPreferences();
        }
    }
    return NOERROR;
}

ECode PreferenceFragment::GetPreferenceScreen(
    /*[out]*/ IPreferenceScreen** preferencescreen)
{
    VALIDATE_NOT_NULL(preferencescreen)
    AutoPtr<IPreferenceScreen> s;
    mPreferenceManager->GetPreferenceScreen((IPreferenceScreen**)&s);
    *preferencescreen = s;
    REFCOUNT_ADD(*preferencescreen)
    return NOERROR;
}

ECode PreferenceFragment::AddPreferencesFromIntent(
    /*[in]*/ IIntent* intent)
{
    RequirePreferenceManager();
    AutoPtr<IPreferenceScreen> rootPreferences;
    GetPreferenceScreen((IPreferenceScreen**)&rootPreferences);
    AutoPtr<IPreferenceScreen> screen;
    mPreferenceManager->InflateFromIntent(intent, rootPreferences, (IPreferenceScreen**)&screen);
    SetPreferenceScreen(screen);
    return NOERROR;
}

ECode PreferenceFragment::AddPreferencesFromResource(
    /*[in]*/ Int32 preferencesResId)
{
    RequirePreferenceManager();

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IPreferenceScreen> rootscreen;
    GetPreferenceScreen((IPreferenceScreen**)&rootscreen);
    AutoPtr<IPreferenceScreen> screen;
    mPreferenceManager->InflateFromResource(IContext::Probe(activity), preferencesResId, rootscreen, (IPreferenceScreen**)&screen);
    SetPreferenceScreen(screen);
    return NOERROR;
}

ECode PreferenceFragment::OnPreferenceTreeClick(
    /*[in]*/ IPreferenceScreen* preferenceScreen,
    /*[in]*/ IPreference* preference,
    /*[out]*/ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    String fragment;
    AutoPtr<IActivity> activity;
    if ((preference->GetFragment(&fragment), !fragment.IsNull()) &&
            (GetActivity((IActivity**)&activity),  IPreferenceFragmentOnPreferenceStartFragmentCallback::Probe(activity) != NULL)) {
        AutoPtr<IPreferenceFragmentOnPreferenceStartFragmentCallback> cb = IPreferenceFragmentOnPreferenceStartFragmentCallback::Probe(activity);
        return cb->OnPreferenceStartFragment(this, preference, result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode PreferenceFragment::FindPreference(
    /*[in]*/ ICharSequence* key,
    /*[out]*/ IPreference** preference)
{
    VALIDATE_NOT_NULL(preference)
    if (mPreferenceManager == NULL) {
        *preference = NULL;
        return NOERROR;
    }

    return mPreferenceManager->FindPreference(key, preference);
}

ECode PreferenceFragment::RequirePreferenceManager()
{
    if (mPreferenceManager == NULL) {
        Slogger::E(PREFERENCES_TAG, "This should be called after super.onCreate.");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

void PreferenceFragment::PostBindPreferences()
{
    Boolean result;
    if (mHandler->HasMessages(MSG_BIND_PREFERENCES, &result), result) return;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_BIND_PREFERENCES, (IMessage**)&msg);
    msg->SendToTarget();
}

void PreferenceFragment::BindPreferences()
{
    AutoPtr<IPreferenceScreen> preferenceScreen;
    GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    if (preferenceScreen != NULL) {
        AutoPtr<IListView> listview;
        GetListView((IListView**)&listview);
        preferenceScreen->Bind(listview);
    }
    OnBindPreferences();
}

ECode PreferenceFragment::OnBindPreferences()
{
    return NOERROR;
}

ECode PreferenceFragment::OnUnbindPreferences()
{
    return NOERROR;
}

ECode PreferenceFragment::GetListView(
    /*[out]*/ IListView** listview)
{
    VALIDATE_NOT_NULL(listview)
    EnsureList();
    *listview = mList;
    REFCOUNT_ADD(*listview)
    return NOERROR;
}

ECode PreferenceFragment::HasListView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mList != NULL) {
        *result = TRUE;
        return NOERROR;
    }
    AutoPtr<IView> root;
    GetView((IView**)&root);
    if (root == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IView> rawListView;
    root->FindViewById(R::id::list, (IView**)&rawListView);
    mList = IListView::Probe(rawListView);
    if (mList == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode PreferenceFragment::EnsureList()
{
    if (mList != NULL) {
        return NOERROR;
    }
    AutoPtr<IView> root;
    GetView((IView**)&root);

    if (root == NULL) {
        Slogger::E(PREFERENCES_TAG, "Content view not yet created");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IView> rawListView;
    root->FindViewById(R::id::list, (IView**)&rawListView);
    if (IListView::Probe(rawListView) == NULL) {
        Slogger::E(PREFERENCES_TAG, "Content has view with id attribute 'android.R.id.list' that is not a ListView class");
        return E_RUNTIME_EXCEPTION;
    }
    mList = IListView::Probe(rawListView);
    if (mList == NULL) {
        Slogger::E(PREFERENCES_TAG, "Your content must have a ListView whose id attribute is 'android.R.id.list'");
        return E_RUNTIME_EXCEPTION;
    }
    IView::Probe(mList)->SetOnKeyListener(mListOnKeyListener);
    Boolean result;
    mHandler->Post(mRequestFocus, &result);
    return NOERROR;
}

}
}
}
