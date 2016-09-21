
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/CSettingsDialogFragment.h"
#include "elastos/droid/settings/HelpUtils.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/text/TextUtils.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceActivity;
using Elastos::Droid::Preference::IPreferenceGroupAdapter;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

const String SettingsPreferenceFragment::TAG("SettingsPreferenceFragment");

const Int32 SettingsPreferenceFragment::MENU_HELP = IMenu::FIRST + 100;
const Int32 SettingsPreferenceFragment::DELAY_HIGHLIGHT_DURATION_MILLIS = 600;

const String SettingsPreferenceFragment::SAVE_HIGHLIGHTED_KEY("android:preference_highlighted");

const String SettingsPreferenceFragment::SettingsDialogFragment::KEY_DIALOG_ID("key_dialog_id");
const String SettingsPreferenceFragment::SettingsDialogFragment::KEY_PARENT_FRAGMENT_ID("key_parent_fragment_id");

//===============================================================================
//                  SettingsPreferenceFragment::SettingsDialogFragment
//===============================================================================

CAR_INTERFACE_IMPL(SettingsPreferenceFragment::SettingsDialogFragment, DialogFragment, ISettingsDialogFragment)

SettingsPreferenceFragment::SettingsDialogFragment::SettingsDialogFragment()
    : mDialogId(0)
{
    /* do nothing */
}

SettingsPreferenceFragment::SettingsDialogFragment::~SettingsDialogFragment()
{}

ECode SettingsPreferenceFragment::SettingsDialogFragment::constructor()
{
    return DialogFragment::constructor();
}

ECode SettingsPreferenceFragment::SettingsDialogFragment::constructor(
    /* [in] */ IDialogCreatable* fragment,
    /* [in] */ Int32 dialogId)
{
    DialogFragment::constructor();
    mDialogId = dialogId;
    if (IFragment::Probe(fragment) == NULL) {
        Logger::E(TAG, "fragment argument must be an instance of %s", TO_CSTR(fragment));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("fragment argument must be an instance of "
        //         + Fragment.class->GetName());
    }

    mParentFragment = IFragment::Probe(fragment);
    return NOERROR;
}

ECode SettingsPreferenceFragment::SettingsDialogFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    DialogFragment::OnSaveInstanceState(outState);
    if (mParentFragment != NULL) {
        outState->PutInt32(KEY_DIALOG_ID, mDialogId);
        Int32 id;
        mParentFragment->GetId(&id);
        outState->PutInt32(KEY_PARENT_FRAGMENT_ID, id);
    }
    return NOERROR;
}

ECode SettingsPreferenceFragment::SettingsDialogFragment::OnStart()
{
    DialogFragment::OnStart();

    if (mParentFragment != NULL &&
            ISettingsPreferenceFragment::Probe(mParentFragment) != NULL) {
        ((SettingsPreferenceFragment*)ISettingsPreferenceFragment::Probe(mParentFragment))->OnDialogShowing();
    }
    return NOERROR;
}

ECode SettingsPreferenceFragment::SettingsDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog);
    *dialog = NULL;

    if (savedInstanceState != NULL) {
        savedInstanceState->GetInt32(KEY_DIALOG_ID, 0, &mDialogId);
        mParentFragment = NULL;
        GetParentFragment((IFragment**)&mParentFragment);
        Int32 mParentFragmentId;
        savedInstanceState->GetInt32(KEY_PARENT_FRAGMENT_ID, -1, &mParentFragmentId);
        if (mParentFragment == NULL) {
            AutoPtr<IFragmentManager> manager;
            GetFragmentManager((IFragmentManager**)&manager);
            manager->FindFragmentById(mParentFragmentId, (IFragment**)&mParentFragment);
        }
        if (IDialogCreatable::Probe(mParentFragment) == NULL) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException(
            //         (mParentFragment != NULL
            //                 ? mParentFragment->GetClass()->GetName()
            //                 : mParentFragmentId)
            //                 + " must implement "
            //                 + DialogCreatable.class->GetName());
        }
        // This dialog fragment could be created from non-SettingsPreferenceFragment
        if (ISettingsPreferenceFragment::Probe(mParentFragment) != NULL) {
            // restore mDialogFragment in mParentFragment
            ((SettingsPreferenceFragment*)ISettingsPreferenceFragment::Probe(mParentFragment))->mDialogFragment = this;
        }
    }
    return IDialogCreatable::Probe(mParentFragment)->OnCreateDialog(mDialogId, dialog);
}

ECode SettingsPreferenceFragment::SettingsDialogFragment::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    DialogFragment::OnCancel(dialog);
    if (mOnCancelListener != NULL) {
        mOnCancelListener->OnCancel(dialog);
    }
    return NOERROR;
}

ECode SettingsPreferenceFragment::SettingsDialogFragment::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    DialogFragment::OnDismiss(dialog);
    if (mOnDismissListener != NULL) {
        mOnDismissListener->OnDismiss(dialog);
    }
    return NOERROR;
}

Int32 SettingsPreferenceFragment::SettingsDialogFragment::GetDialogId()
{
    return mDialogId;
}

ECode SettingsPreferenceFragment::SettingsDialogFragment::OnDetach()
{
    DialogFragment::OnDetach();

    // This dialog fragment could be created from non-SettingsPreferenceFragment
    if (ISettingsPreferenceFragment::Probe(mParentFragment) != NULL ) {
        // in case the dialog is not explicitly removed by RemoveDialog()
        if (TO_IINTERFACE(((SettingsPreferenceFragment*)ISettingsPreferenceFragment::Probe(mParentFragment))->mDialogFragment)
                == TO_IINTERFACE(this)) {
            ((SettingsPreferenceFragment*)ISettingsPreferenceFragment::Probe(mParentFragment))->mDialogFragment = NULL;
        }
    }
    return NOERROR;
}

//===============================================================================
//                  SettingsPreferenceFragment::InitDataSetObserver
//===============================================================================

SettingsPreferenceFragment::InitDataSetObserver::InitDataSetObserver(
    /* [in] */ SettingsPreferenceFragment* host)
    : mHost(host)
{}

SettingsPreferenceFragment::InitDataSetObserver::~InitDataSetObserver()
{}

ECode SettingsPreferenceFragment::InitDataSetObserver::OnChanged()
{
    return mHost->HighlightPreferenceIfNeeded();
}

ECode SettingsPreferenceFragment::InitDataSetObserver::OnInvalidated()
{
    return mHost->HighlightPreferenceIfNeeded();
}

//===============================================================================
//                  SettingsPreferenceFragment::RunnableInHighlightPreference
//===============================================================================

SettingsPreferenceFragment::RunnableInHighlightPreference::RunnableInHighlightPreference(
    /* [in] */ IListView* listView,
    /* [in] */ Int32 position,
    /* [in] */ IDrawable* highlight)
    : mListView(listView)
    , mPosition(position)
    , mHighlight(highlight)
{}

SettingsPreferenceFragment::RunnableInHighlightPreference::~RunnableInHighlightPreference()
{}

ECode SettingsPreferenceFragment::RunnableInHighlightPreference::Run()
{
    IAdapterView::Probe(mListView)->SetSelection(mPosition);
    AutoPtr<RunnableInHighlightPreference2> runnable = new RunnableInHighlightPreference2(
            mListView, mPosition, mHighlight);
    Boolean res;
    IView::Probe(mListView)->PostDelayed((IRunnable*)runnable.Get(), DELAY_HIGHLIGHT_DURATION_MILLIS, &res);
    return NOERROR;
}

//===============================================================================
//                  SettingsPreferenceFragment::RunnableInHighlightPreference2
//===============================================================================

SettingsPreferenceFragment::RunnableInHighlightPreference2::RunnableInHighlightPreference2(
    /* [in] */ IListView* listView,
    /* [in] */ Int32 position,
    /* [in] */ IDrawable* highlight)
    : mListView(listView)
    , mPosition(position)
    , mHighlight(highlight)
{}

SettingsPreferenceFragment::RunnableInHighlightPreference2::~RunnableInHighlightPreference2()
{}

ECode SettingsPreferenceFragment::RunnableInHighlightPreference2::Run()
{
    Int32 pos;
    IAdapterView::Probe(mListView)->GetFirstVisiblePosition(&pos);
    const Int32 index = mPosition - pos;
    Int32 count;
    IViewGroup::Probe(mListView)->GetChildCount(&count);
    if (index >= 0 && index < count) {
        AutoPtr<IView> v;
        IViewGroup::Probe(mListView)->GetChildAt(index, (IView**)&v);
        Int32 width, height;
        v->GetWidth(&width);
        v->GetHeight(&height);
        const Int32 centerX = width / 2;
        const Int32 centerY = height / 2;
        mHighlight->SetHotspot(centerX, centerY);
        v->SetPressed(TRUE);
        v->SetPressed(FALSE);
    }
    return NOERROR;
}

//===============================================================================
//                  SettingsPreferenceFragment
//===============================================================================

CAR_INTERFACE_IMPL_2(SettingsPreferenceFragment, PreferenceFragment,
        ISettingsPreferenceFragment, IDialogCreatable);

SettingsPreferenceFragment::SettingsPreferenceFragment()
    : mPreferenceHighlighted(FALSE)
    , mIsDataSetObserverRegistered(FALSE)
{
    mDataSetObserver = new InitDataSetObserver(this);
}

SettingsPreferenceFragment::~SettingsPreferenceFragment()
{}

ECode SettingsPreferenceFragment::constructor()
{
    return PreferenceFragment::constructor();
}

ECode SettingsPreferenceFragment::OnCreate(
    /* [in] */ IBundle* icicle)
{
    PreferenceFragment::OnCreate(icicle);

    if (icicle != NULL) {
        icicle->GetBoolean(SAVE_HIGHLIGHTED_KEY, &mPreferenceHighlighted);
    }

    // Prepare help url and enable menu if necessary
    Int32 helpResource = GetHelpResource();
    if (helpResource != 0) {
        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        res->GetString(helpResource, &mHelpUrl);
    }
    return NOERROR;
}

ECode SettingsPreferenceFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IView> root;
    PreferenceFragment::OnCreateView(inflater, container, savedInstanceState, (IView**)&root);
    AutoPtr<IView> _view;
    root->FindViewById(R::id::pinned_header, (IView**)&_view);
    mPinnedHeaderFrameLayout = IViewGroup::Probe(_view);

    *view = root;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode SettingsPreferenceFragment::SetPinnedHeaderView(
    /* [in] */ IView* pinnedHeader)
{
    mPinnedHeaderFrameLayout->AddView(pinnedHeader);
    IView::Probe(mPinnedHeaderFrameLayout)->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

ECode SettingsPreferenceFragment::ClearPinnedHeaderView()
{
    mPinnedHeaderFrameLayout->RemoveAllViews();
    IView::Probe(mPinnedHeaderFrameLayout)->SetVisibility(IView::GONE);
    return NOERROR;
}

ECode SettingsPreferenceFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    PreferenceFragment::OnSaveInstanceState(outState);

    return outState->PutBoolean(SAVE_HIGHLIGHTED_KEY, mPreferenceHighlighted);
}

ECode SettingsPreferenceFragment::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    PreferenceFragment::OnActivityCreated(savedInstanceState);
    if (!TextUtils::IsEmpty(mHelpUrl)) {
        return SetHasOptionsMenu(TRUE);
    }
    return NOERROR;
}

ECode SettingsPreferenceFragment::OnResume()
{
    PreferenceFragment::OnResume();

    AutoPtr<IBundle> args;
    GetArguments((IBundle**)&args);
    if (args != NULL) {
        args->GetString(CSettingsActivity::EXTRA_FRAGMENT_ARG_KEY, &mPreferenceKey);
        return HighlightPreferenceIfNeeded();
    }
    return NOERROR;
}

ECode SettingsPreferenceFragment::OnBindPreferences()
{
    return RegisterObserverIfNeeded();
}

ECode SettingsPreferenceFragment::OnUnbindPreferences()
{
    return UnregisterObserverIfNeeded();
}

ECode SettingsPreferenceFragment::OnStop()
{
    PreferenceFragment::OnStop();

    return UnregisterObserverIfNeeded();
}

ECode SettingsPreferenceFragment::RegisterObserverIfNeeded()
{
    if (!mIsDataSetObserverRegistered) {
        if (mCurrentRootAdapter != NULL) {
            IAdapter::Probe(mCurrentRootAdapter)->UnregisterDataSetObserver(mDataSetObserver);
        }

        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        screen->GetRootAdapter((IListAdapter**)&mCurrentRootAdapter);
        IAdapter::Probe(mCurrentRootAdapter)->RegisterDataSetObserver(mDataSetObserver);
        mIsDataSetObserverRegistered = TRUE;
    }
    return NOERROR;
}

ECode SettingsPreferenceFragment::UnregisterObserverIfNeeded()
{
    if (mIsDataSetObserverRegistered) {
        if (mCurrentRootAdapter != NULL) {
            IAdapter::Probe(mCurrentRootAdapter)->UnregisterDataSetObserver(mDataSetObserver);
            mCurrentRootAdapter = NULL;
        }
        mIsDataSetObserverRegistered = FALSE;
    }
    return NOERROR;
}

ECode SettingsPreferenceFragment::HighlightPreferenceIfNeeded()
{
    Boolean res;
    if ((IsAdded(&res), res) && !mPreferenceHighlighted && !TextUtils::IsEmpty(mPreferenceKey)) {
        HighlightPreference(mPreferenceKey);
    }
    return NOERROR;
}

AutoPtr<IDrawable> SettingsPreferenceFragment::GetHighlightDrawable()
{
    if (mHighlightDrawable == NULL) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        IContext::Probe(activity)->GetDrawable(R::drawable::preference_highlight,
                (IDrawable**)&mHighlightDrawable);
    }
    return mHighlightDrawable;
}

Int32 SettingsPreferenceFragment::CanUseListViewForHighLighting(
    /* [in] */ const String& key)
{
    Boolean res;
    if (HasListView(&res), !res) {
        return -1;
    }

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    AutoPtr<IAdapter> adap;
    IAdapterView::Probe(listView)->GetAdapter((IAdapter**)&adap);
    IListAdapter* adapter = IListAdapter::Probe(adap);

    if (adapter != NULL && IPreferenceGroupAdapter::Probe(adapter) != NULL) {
        return FindListPositionFromKey(adapter, key);
    }

    return -1;
}

void SettingsPreferenceFragment::HighlightPreference(
    /* [in] */ const String& key)
{
    const Int32 position = CanUseListViewForHighLighting(key);
    if (position >= 0) {
        mPreferenceHighlighted = TRUE;

        AutoPtr<IListView> listView;
        GetListView((IListView**)&listView);
        AutoPtr<IAdapter> adap;
        IAdapterView::Probe(listView)->GetAdapter((IAdapter**)&adap);
        IPreferenceGroupAdapter* adapter = IPreferenceGroupAdapter::Probe(adap);

        AutoPtr<IDrawable> highlight = GetHighlightDrawable();
        adapter->SetHighlightedDrawable(highlight);
        adapter->SetHighlighted(position);

        AutoPtr<RunnableInHighlightPreference> runnable = new RunnableInHighlightPreference(
                listView, position, highlight);
        Boolean res;
        IView::Probe(listView)->Post((IRunnable*)runnable.Get(), &res);
    }
}

Int32 SettingsPreferenceFragment::FindListPositionFromKey(
    /* [in] */ IListAdapter* adapter,
    /* [in] */ const String& key)
{
    IAdapter* ad = IAdapter::Probe(adapter);
    Int32 count;
    ad->GetCount(&count);
    for (Int32 n = 0; n < count; n++) {
        AutoPtr<IInterface> item;
        ad->GetItem(n, (IInterface**)&item);
        IPreference* preference = IPreference::Probe(item);
        if (preference != NULL) {
            String preferenceKey;
            preference->GetKey(&preferenceKey);
            if (!preferenceKey.IsNull() && preferenceKey.Equals(key)) {
                return n;
            }
        }
    }
    return -1;
}

void SettingsPreferenceFragment::RemovePreference(
    /* [in] */ const String& key)
{
    AutoPtr<IPreference> pref;
    FindPreference(CoreUtils::Convert(key), (IPreference**)&pref);
    if (pref != NULL) {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        Boolean res;
        IPreferenceGroup::Probe(screen)->RemovePreference(pref, &res);
    }
}

Int32 SettingsPreferenceFragment::GetHelpResource()
{
    return 0;
}

ECode SettingsPreferenceFragment::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (!mHelpUrl.IsNull() && activity != NULL) {
        AutoPtr<IMenuItem> helpItem;
        menu->Add(0, MENU_HELP, 0, R::string::help_label, (IMenuItem**)&helpItem);
        HelpUtils::PrepareHelpMenuItem(IContext::Probe(activity), helpItem, mHelpUrl);
    }
    return NOERROR;
}

void SettingsPreferenceFragment::FinishFragment()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    activity->OnBackPressed();
}

AutoPtr<IContentResolver> SettingsPreferenceFragment::GetContentResolver()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    if (context != NULL) {
        mContentResolver = NULL;
        context->GetContentResolver((IContentResolver**)&mContentResolver);
    }
    return mContentResolver;
}

AutoPtr<IInterface> SettingsPreferenceFragment::GetSystemService(
    /* [in] */ const String& name)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(name, (IInterface**)&obj);
    return obj;
}

AutoPtr<IPackageManager> SettingsPreferenceFragment::GetPackageManager()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IPackageManager> pkm;
    IContext::Probe(activity)->GetPackageManager((IPackageManager**)&pkm);
    return pkm;
}

ECode SettingsPreferenceFragment::OnDetach()
{
    Boolean res;
    if (IsRemoving(&res), res) {
        if (mDialogFragment != NULL) {
            IDialogFragment::Probe(mDialogFragment)->Dismiss();
            mDialogFragment = NULL;
        }
    }
    return PreferenceFragment::OnDetach();
}

void SettingsPreferenceFragment::ShowDialog(
    /* [in] */ Int32 dialogId)
{
    if (mDialogFragment != NULL) {
        Logger::E(TAG, "Old dialog fragment not NULL!");
    }
    CSettingsDialogFragment::New(this, dialogId,
            (ISettingsDialogFragment**)&mDialogFragment);
    AutoPtr<IFragmentManager> manager;
    GetChildFragmentManager((IFragmentManager**)&manager);
    IDialogFragment::Probe(mDialogFragment)->Show(manager, StringUtils::ToString(dialogId));
}

ECode SettingsPreferenceFragment::OnCreateDialog(
    /* [in] */ Int32 dialogId,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog);
    *dialog = NULL;
    return NOERROR;
}

void SettingsPreferenceFragment::RemoveDialog(
    /* [in] */ Int32 dialogId)
{
    // mDialogFragment may not be visible yet in parent fragment's OnResume().
    // To be able to dismiss dialog at that time, don't check
    // mDialogFragment->IsVisible().
    if (mDialogFragment != NULL && ((CSettingsDialogFragment*)mDialogFragment.Get())->GetDialogId() == dialogId) {
        IDialogFragment::Probe(mDialogFragment)->Dismiss();
    }
    mDialogFragment = NULL;
}

void SettingsPreferenceFragment::SetOnCancelListener(
    /* [in] */ IDialogInterfaceOnCancelListener* listener)
{
    if (mDialogFragment != NULL) {
        ((CSettingsDialogFragment*)mDialogFragment.Get())->mOnCancelListener = listener;
    }
}

void SettingsPreferenceFragment::SetOnDismissListener(
    /* [in] */ IDialogInterfaceOnDismissListener* listener)
{
    if (mDialogFragment != NULL) {
        ((CSettingsDialogFragment*)mDialogFragment.Get())->mOnDismissListener = listener;
    }
}

ECode SettingsPreferenceFragment::OnDialogShowing()
{
    // override in subclass to attach a dismiss listener, for instance
    return NOERROR;
}

Boolean SettingsPreferenceFragment::HasNextButton()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    Boolean res;
    IButtonBarHandler::Probe(activity)->HasNextButton(&res);
    return res;
}

AutoPtr<IButton> SettingsPreferenceFragment::GetNextButton()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IButton> button;
    IButtonBarHandler::Probe(activity)->GetNextButton((IButton**)&button);
    return button;
}

ECode SettingsPreferenceFragment::Finish()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    return activity->OnBackPressed();
}

Boolean SettingsPreferenceFragment::StartFragment(
    /* [in] */ IFragment* caller,
    /* [in] */ const String& fragmentClass,
    /* [in] */ Int32 titleRes,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* extras)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);

    if (ISettingsActivity::Probe(activity) != NULL) {
        ISettingsActivity* sa = ISettingsActivity::Probe(activity);
        ((CSettingsActivity*)sa)->StartPreferencePanel(fragmentClass,
                extras, titleRes, NULL, caller, requestCode);
        return TRUE;
    }
    else if (IPreferenceActivity::Probe(activity) != NULL) {
        IPreferenceActivity* sa = IPreferenceActivity::Probe(activity);
        sa->StartPreferencePanel(fragmentClass, extras, titleRes, NULL,
                caller, requestCode);
        return TRUE;
    }
    else {
        Logger::W(TAG,
                "Parent isn't SettingsActivity nor PreferenceActivity, thus there's no way to launch the given Fragment (name: %s, requestCode: %d)",
                fragmentClass.string(), requestCode);
        return FALSE;
    }
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
