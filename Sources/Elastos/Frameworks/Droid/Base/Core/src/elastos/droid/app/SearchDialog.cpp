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

#include "Elastos.Droid.Speech.h"
#include "elastos/droid/app/SearchDialog.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/ViewConfiguration.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Speech::IRecognizerIntent;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ICollapsibleActionView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::ViewConfiguration;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::EIID_ISearchViewOnCloseListener;
using Elastos::Droid::Widget::EIID_ISearchViewOnQueryTextListener;
using Elastos::Droid::Widget::EIID_ISearchViewOnSuggestionListener;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

//===================================================================
// SearchDialog::SearchBar
//===================================================================
CAR_INTERFACE_IMPL(SearchDialog::SearchBar, LinearLayout, ISearchBar)

SearchDialog::SearchBar::SearchBar()
{}

SearchDialog::SearchBar::~SearchBar()
{}

ECode SearchDialog::SearchBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(context, attrs);
}

ECode SearchDialog::SearchBar::constructor(
    /* [in] */ IContext* context)
{
    return LinearLayout::constructor(context);
}

ECode SearchDialog::SearchBar::SetSearchDialog(
    /* [in] */ ISearchDialog* searchDialog)
{
    mSearchDialog = searchDialog;
    return NOERROR;
}

ECode SearchDialog::SearchBar::StartActionModeForChild(
    /* [in] */ IView* child,
    /* [in] */ IActionModeCallback* cb,
    /* [out] */ IActionMode** mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = NULL;
    return NOERROR;
}

//===================================================================
// SearchDialog::ConfChangeBroadcastReceiver
//===================================================================
SearchDialog::ConfChangeBroadcastReceiver::ConfChangeBroadcastReceiver(
    /* [in] */ SearchDialog* host)
    : mHost(host)
{}

ECode SearchDialog::ConfChangeBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_CONFIGURATION_CHANGED)) {
        mHost->OnConfigurationChanged();
    }
    return NOERROR;
}

//===================================================================
// SearchDialog::MySearchViewOnCloseListener
//===================================================================
CAR_INTERFACE_IMPL(SearchDialog::MySearchViewOnCloseListener, Object, ISearchViewOnCloseListener)

SearchDialog::MySearchViewOnCloseListener::MySearchViewOnCloseListener(
    /* [in] */ SearchDialog* host)
    : mHost(host)
{}

ECode SearchDialog::MySearchViewOnCloseListener::OnClose(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->OnClosePressed();
    return NOERROR;
}

//===================================================================
// SearchDialog::MySearchViewOnQueryTextListener
//===================================================================
CAR_INTERFACE_IMPL(SearchDialog::MySearchViewOnQueryTextListener, Object, ISearchViewOnQueryTextListener)

SearchDialog::MySearchViewOnQueryTextListener::MySearchViewOnQueryTextListener(
    /* [in] */ SearchDialog* host)
    : mHost(host)
{}

ECode SearchDialog::MySearchViewOnQueryTextListener::OnQueryTextSubmit(
    /* [in] */ const String& query,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    mHost->Dismiss();
    return NOERROR;
}

ECode SearchDialog::MySearchViewOnQueryTextListener::OnQueryTextChange(
    /* [in] */ const String& newText,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

//===================================================================
// SearchDialog::MySearchViewOnSuggestionListener
//===================================================================
CAR_INTERFACE_IMPL(SearchDialog::MySearchViewOnSuggestionListener, Object, ISearchViewOnSuggestionListener)

SearchDialog::MySearchViewOnSuggestionListener::MySearchViewOnSuggestionListener(
    /* [in] */ SearchDialog* host)
    : mHost(host)
{}

ECode SearchDialog::MySearchViewOnSuggestionListener::OnSuggestionSelect(
    /* [in] * */ Int32 position,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode SearchDialog::MySearchViewOnSuggestionListener::OnSuggestionClick(
    /* [in] * */ Int32 position,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->Dismiss();
    *result = FALSE;
    return NOERROR;
}

//===================================================================
// SearchDialog::MyViewOnClickListener
//===================================================================
CAR_INTERFACE_IMPL(SearchDialog::MyViewOnClickListener, Object, IViewOnClickListener)

SearchDialog::MyViewOnClickListener::MyViewOnClickListener(
    /* [in] */ SearchDialog* host)
    : mHost(host)
{}

ECode SearchDialog::MyViewOnClickListener::OnClick(
    /* [in] */ IView*view)
{
    mHost->Dismiss();
    return NOERROR;
}

//===================================================================
// SearchDialog
//===================================================================

// Debugging support
const Boolean SearchDialog::DBG(FALSE);
const String SearchDialog::TAG("SearchDialog");

const String SearchDialog::INSTANCE_KEY_COMPONENT("comp");
const String SearchDialog::INSTANCE_KEY_APPDATA("data");
const String SearchDialog::INSTANCE_KEY_USER_QUERY("uQry");

// The string used for privateImeOptions to identify to the IME that it should not show
// a microphone button since one already exists in the search dialog.
const String SearchDialog::IME_OPTION_NO_MICROPHONE("nm");

const Int32 SearchDialog::SEARCH_PLATE_LEFT_PADDING_NON_GLOBAL = 7;

CAR_INTERFACE_IMPL(SearchDialog, Dialog, ISearchDialog)

SearchDialog::SearchDialog()
    : mSearchAutoCompleteImeOptions(0)
{}

SearchDialog::~SearchDialog()
{}

Int32 SearchDialog::ResolveDialogTheme(
    /* [in] */ IContext* context)
{
    AutoPtr<ITypedValue> outValue;
    CTypedValue::New((ITypedValue**)&outValue);
    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);
    Boolean bval;
    theme->ResolveAttribute(R::attr::searchDialogTheme, outValue, TRUE, &bval);
    Int32 id;
    outValue->GetResourceId(&id);
    return id;
}

ECode SearchDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ISearchManager* searchManager)
{
    Dialog::constructor(context, ResolveDialogTheme(context));

    // Save voice intent for later queries/launching
    CIntent::New(IRecognizerIntent::ACTION_WEB_SEARCH, (IIntent**)&mVoiceWebSearchIntent);
    mVoiceWebSearchIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    mVoiceWebSearchIntent->PutExtra(
        IRecognizerIntent::EXTRA_LANGUAGE_MODEL,
        IRecognizerIntent::LANGUAGE_MODEL_WEB_SEARCH);


    CIntent::New(IRecognizerIntent::ACTION_RECOGNIZE_SPEECH, (IIntent**)&mVoiceAppSearchIntent);
    mVoiceAppSearchIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    return NOERROR;
}

ECode SearchDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Dialog::OnCreate(savedInstanceState);

    AutoPtr<IWindow> theWindow;
    GetWindow((IWindow**)&theWindow);
    AutoPtr<IWindowManagerLayoutParams> lp;
    theWindow->GetAttributes((IWindowManagerLayoutParams**)&lp);
    IViewGroupLayoutParams* vglp = IViewGroupLayoutParams::Probe(lp);
    vglp->SetWidth(IViewGroupLayoutParams::MATCH_PARENT);
    // taking up the whole window (even when transparent) is less than ideal,
    // but necessary to show the popup window until the window manager supports
    // having windows anchored by their parent but not clipped by them.
    vglp->SetHeight(IViewGroupLayoutParams::MATCH_PARENT);
    lp->SetGravity(IGravity::TOP | IGravity::FILL_HORIZONTAL);
    lp->SetSoftInputMode(IWindowManagerLayoutParams::SOFT_INPUT_ADJUST_RESIZE);
    theWindow->SetAttributes(lp);

    // Touching outside of the search dialog will Dismiss it
    SetCanceledOnTouchOutside(TRUE);
    return NOERROR;
}

ECode SearchDialog::CreateContentView()
{
    SetContentView(R::layout::search_bar);

    // get the view elements for local access
    AutoPtr<IView> tmp;
    FindViewById(R::id::search_bar, (IView**)&tmp);
    AutoPtr<ISearchBar> searchBar = ISearchBar::Probe(tmp) ;
    searchBar->SetSearchDialog(this);

    tmp = NULL;
    FindViewById(R::id::search_view, (IView**)&tmp);
    mSearchView = ISearchView::Probe(tmp) ;
    mSearchView->SetIconified(FALSE);
    mSearchView->SetOnCloseListener(mOnCloseListener);
    mSearchView->SetOnQueryTextListener(mOnQueryChangeListener);
    mSearchView->SetOnSuggestionListener(mOnSuggestionSelectionListener);
    ICollapsibleActionView::Probe(mSearchView)->OnActionViewExpanded();

    mCloseSearch = NULL;
    FindViewById(R::id::closeButton, (IView**)&mCloseSearch);
    AutoPtr<IViewOnClickListener> listener = new MyViewOnClickListener(this);
    mCloseSearch->SetOnClickListener(listener);

    IView* sv = IView::Probe(mSearchView);
    // TODO: Move the badge logic to SearchView or move the badge to search_bar.xml
    tmp = NULL;
    sv->FindViewById(R::id::search_badge, (IView**)&tmp);
    mBadgeLabel = ITextView::Probe(tmp) ;

    tmp = NULL;
    sv->FindViewById(R::id::search_src_text, (IView**)&tmp);
    mSearchAutoComplete = IAutoCompleteTextView::Probe(tmp);

    tmp = NULL;
    FindViewById(R::id::search_app_icon, (IView**)&tmp);
    mAppIcon = IImageView::Probe(tmp) ;

    mSearchPlate = NULL;
    sv->FindViewById(R::id::search_plate, (IView**)&mSearchPlate);

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mWorkingSpinner = NULL;
    ctx->GetDrawable(R::drawable::search_spinner, (IDrawable**)&mWorkingSpinner);
    // TODO: Restore the spinner for slow suggestion lookups
    // mSearchAutoComplete->SetCompoundDrawablesWithIntrinsicBounds(
    //        NULL, NULL, mWorkingSpinner, NULL);
    SetWorking(FALSE);

    // pre-hide all the extraneous elements
    IView::Probe(mBadgeLabel)->SetVisibility(IView::GONE);

    // Additional adjustments to make Dialog work for Search
    ITextView::Probe(mSearchAutoComplete)->GetImeOptions(&mSearchAutoCompleteImeOptions);
    return NOERROR;
}

ECode SearchDialog::Show(
    /* [in] */ const String& initialQuery,
    /* [in] */ Boolean selectInitialQuery,
    /* [in] */ IComponentName* componentName,
    /* [in] */ IBundle* appSearchData,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean success = DoShow(initialQuery, selectInitialQuery, componentName, appSearchData);
    if (success) {
        // Display the drop down as soon as possible instead of waiting for the rest of the
        // pending UI stuff to get done, so that things appear faster to the user.
        mSearchAutoComplete->ShowDropDownAfterLayout();
    }
    *result = success;
    return NOERROR;
}

Boolean SearchDialog::DoShow(
    /* [in] */ const String& initialQuery,
    /* [in] */ Boolean selectInitialQuery,
    /* [in] */ IComponentName* componentName,
    /* [in] */ IBundle* appSearchData)
{
    // set up the searchable and show the dialog
    if (!Show(componentName, appSearchData)) {
        return FALSE;
    }

    // finally, load the user's initial text (which may trigger suggestions)
    SetUserQuery(initialQuery);
    if (selectInitialQuery) {
        IEditText::Probe(mSearchAutoComplete)->SelectAll();
    }

    return TRUE;
}

Boolean SearchDialog::Show(
    /* [in] */ IComponentName* componentName,
    /* [in] */ IBundle* appSearchData)
{
    if (DBG) {
        Logger::D(TAG, "show(%s, %s)",
            TO_CSTR(componentName),
            TO_CSTR(appSearchData));
    }

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&obj);
    AutoPtr<ISearchManager> searchManager = ISearchManager::Probe(obj);

    // Try to get the searchable info for the provided component.
    mSearchable = NULL;
    searchManager->GetSearchableInfo(componentName, (ISearchableInfo**)&mSearchable);

    if (mSearchable == NULL) {
        return FALSE;
    }

    mLaunchComponent = componentName;
    mAppSearchData = appSearchData;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mActivityContext = NULL;
    mSearchable->GetActivityContext(ctx, (IContext**)&mActivityContext);

    // show the dialog. this will call onStart().
    Boolean bval;
    IsShowing(&bval);
    if (!bval) {
        // Recreate the search bar view every time the dialog is shown, to get rid
        // of any bad state in the AutoCompleteTextView etc
        CreateContentView();
        mSearchView->SetSearchableInfo(mSearchable);
        mSearchView->SetAppSearchData(mAppSearchData);

        Show();
    }
    UpdateUI();

    return TRUE;
}

ECode SearchDialog::OnStart()
{
    Dialog::OnStart();

    // Register a listener for configuration change events.
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IIntent> stickyIntent;
    return ctx->RegisterReceiver(mConfChangeListener, filter, (IIntent**)&stickyIntent);
}

ECode SearchDialog::OnStop()
{
    Dialog::OnStop();

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    ctx->UnregisterReceiver(mConfChangeListener);

    // dump extra memory we're hanging on to
    mLaunchComponent = NULL;
    mAppSearchData = NULL;
    mSearchable = NULL;
    mUserQuery = NULL;
    return NOERROR;
}

ECode SearchDialog::SetWorking(
    /* [in] */ Boolean working)
{
    mWorkingSpinner->SetAlpha(working ? 255 : 0);
    Boolean prev;
    mWorkingSpinner->SetVisible(working, FALSE, &prev);
    return mWorkingSpinner->InvalidateSelf();
}

ECode SearchDialog::OnSaveInstanceState(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result  = NULL;

    Boolean bval;
    IsShowing(&bval);
    if (!bval) return NOERROR;

    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);

    // setup info so I can recreate this particular search
    bundle->PutParcelable(INSTANCE_KEY_COMPONENT, IParcelable::Probe(mLaunchComponent));
    bundle->PutBundle(INSTANCE_KEY_APPDATA, mAppSearchData);
    bundle->PutString(INSTANCE_KEY_USER_QUERY, mUserQuery);

    *result = bundle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SearchDialog::OnRestoreInstanceState(
    /* [in] */ IBundle* savedInstanceState)
{
    if (savedInstanceState == NULL) return NOERROR;

    AutoPtr<IParcelable> p;
    savedInstanceState->GetParcelable(INSTANCE_KEY_COMPONENT, (IParcelable**)&p);
    AutoPtr<IComponentName> launchComponent = IComponentName::Probe(p);
    AutoPtr<IBundle> appSearchData;
    savedInstanceState->GetBundle(INSTANCE_KEY_APPDATA, (IBundle**)&appSearchData);
    String userQuery;
    savedInstanceState->GetString(INSTANCE_KEY_USER_QUERY, &userQuery);

    // show the dialog.
    if (!DoShow(userQuery, FALSE, launchComponent, appSearchData)) {
        // for some reason, we couldn't re-instantiate
        return NOERROR;
    }
    return NOERROR;
}

ECode SearchDialog::OnConfigurationChanged()
{
    Boolean bval;
    if (mSearchable != NULL && (IsShowing(&bval), bval)) {
        // Redraw (resources may have changed)
        UpdateSearchAppIcon();
        UpdateSearchBadge();
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        if (IsLandscapeMode(ctx)) {
            mSearchAutoComplete->EnsureImeVisible(TRUE);
        }
    }
    return NOERROR;
}

Boolean SearchDialog::IsLandscapeMode(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> configuration;
    res->GetConfiguration((IConfiguration**)&configuration);
    Int32 orientation;
    configuration->GetOrientation(&orientation);
    return orientation == IConfiguration::ORIENTATION_LANDSCAPE;
}

void SearchDialog::UpdateUI()
{
    if (mSearchable != NULL) {
        mDecor->SetVisibility(IView::VISIBLE);
        UpdateSearchAutoComplete();
        UpdateSearchAppIcon();
        UpdateSearchBadge();

        // In order to properly configure the input method (if one is being used), we
        // need to let it know if we'll be providing suggestions.  Although it would be
        // difficult/expensive to know if every last detail has been configured properly, we
        // can at least see if a suggestions provider has been configured, and use that
        // as our trigger.
        Int32 inputType;
        mSearchable->GetInputType(&inputType);
        // We only touch this if the input type is set up for text (which it almost certainly
        // should be, in the case of search!)
        if ((inputType & IInputType::TYPE_MASK_CLASS) == IInputType::TYPE_CLASS_TEXT) {
            // The existence of a suggestions authority is the proxy for "suggestions
            // are available here"
            inputType &= ~IInputType::TYPE_TEXT_FLAG_AUTO_COMPLETE;
            String authority;
            mSearchable->GetSuggestAuthority(&authority);
            if (!authority.IsNull()) {
                inputType |= IInputType::TYPE_TEXT_FLAG_AUTO_COMPLETE;
            }
        }
        ITextView* textView = ITextView::Probe(mSearchAutoComplete);
        textView->SetInputType(inputType);
        mSearchable->GetImeOptions(&mSearchAutoCompleteImeOptions);
        textView->SetImeOptions(mSearchAutoCompleteImeOptions);

        // If the search dialog is going to show a voice search button, then don't let
        // the soft keyboard display a microphone button if it would have otherwise.
        Boolean bval;
        mSearchable->GetVoiceSearchEnabled(&bval);
        if (bval) {
            textView->SetPrivateImeOptions(IME_OPTION_NO_MICROPHONE);
        }
        else {
            textView->SetPrivateImeOptions(String(NULL));
        }
    }
}

void SearchDialog::UpdateSearchAutoComplete()
{
    // we Dismiss the entire dialog instead
    mSearchAutoComplete->SetDropDownDismissedOnCompletion(FALSE);
    mSearchAutoComplete->SetForceIgnoreOutsideTouch(FALSE);
}

void SearchDialog::UpdateSearchAppIcon()
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IPackageManager> pm;
    ctx->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IDrawable> icon;
    // try {
    AutoPtr<IActivityInfo> info;
    pm->GetActivityInfo(mLaunchComponent, 0, (IActivityInfo**)&info);
    AutoPtr<IApplicationInfo> ai;
    IComponentInfo::Probe(info)->GetApplicationInfo((IApplicationInfo**)&ai);
    ECode ec = pm->GetApplicationIcon(ai, (IDrawable**)&icon);
    if (DBG) Logger::D(TAG, "Using app-specific icon");
    // } catch (NameNotFoundException e) {
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        pm->GetDefaultActivityIcon((IDrawable**)&icon);
        Logger::W(TAG, "%s not found, using generic app icon",
            TO_CSTR(mLaunchComponent));
    // }
    }

    mAppIcon->SetImageDrawable(icon);
    IView::Probe(mAppIcon)->SetVisibility(IView::VISIBLE);
    Int32 l, t, r, b;
    mSearchPlate->GetPadding(&l, &t, &r, &b);
    mSearchPlate->SetPadding(SEARCH_PLATE_LEFT_PADDING_NON_GLOBAL, t, r, b);
}

void SearchDialog::UpdateSearchBadge()
{
    // assume both hidden
    Int32 visibility = IView::GONE;
    AutoPtr<IDrawable> icon;
    AutoPtr<ICharSequence> text;

    // optionally show one or the other.
    Boolean bval;
    mSearchable->UseBadgeIcon(&bval);
    if (bval) {
        Int32 id;
        mSearchable->GetIconId(&id);
        mActivityContext->GetDrawable(id, (IDrawable**)&icon);
        visibility = IView::VISIBLE;
        // if (DBG) Logger::D(TAG, "Using badge icon: " + mSearchable->GetIconId());
    }
    else if (mSearchable->UseBadgeLabel(&bval), bval) {
        AutoPtr<IResources> res;
        mActivityContext->GetResources((IResources**)&res);
        Int32 id;
        mSearchable->GetLabelId(&id);
        res->GetText(id, (ICharSequence**)&text);
        visibility = IView::VISIBLE;
        // if (DBG) Logger::D(TAG, "Using badge label: " + mSearchable->GetLabelId());
    }

    mBadgeLabel->SetCompoundDrawablesWithIntrinsicBounds(icon, NULL, NULL, NULL);
    mBadgeLabel->SetText(text);
    IView::Probe(mBadgeLabel)->SetVisibility(visibility);
}

ECode SearchDialog::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // cancel if the drop-down is not showing and the touch event was outside the search plate
    Boolean bval;
    mSearchAutoComplete->IsPopupShowing(&bval);
    if (!bval && IsOutOfBounds(mSearchPlate, event)) {
        if (DBG) Logger::D(TAG, "Pop-up not showing and outside of search plate.");
        Cancel();
        *result = TRUE;
        return NOERROR;
    }
    // Let Dialog handle events outside the window while the pop-up is showing.
    return Dialog::OnTouchEvent(event, result);
}

Boolean SearchDialog::IsOutOfBounds(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event)
{
    Float fx,fy;
    event->GetX(&fx);
    event->GetY(&fy);
    Int32 x = (Int32)fx;
    Int32 y = (Int32)fy;
    AutoPtr<IViewConfiguration> vc = ViewConfiguration::Get(mContext);
    Int32 slop;
    vc->GetScaledWindowTouchSlop(&slop);
    Int32 w, h;
    v->GetWidth(&w);
    v->GetHeight(&h);
    return (x < -slop) || (y < -slop)
        || (x > (w + slop)) || (y > (h + slop));
}

ECode SearchDialog::Hide()
{
    Boolean show;
    IsShowing(&show);
    if (!show) return NOERROR;

    // We made sure the IME was displayed, so also make sure it is closed
    // when we go away.
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IInterface> obj;
    ctx->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    AutoPtr<IInputMethodManager> imm = IInputMethodManager::Probe(obj);
    if (imm != NULL) {
        AutoPtr<IWindow> window;
        GetWindow((IWindow**)&window);
        AutoPtr<IView> view;
        window->GetDecorView((IView**)&view);
        AutoPtr<IBinder> token;
        view->GetWindowToken((IBinder**)&token);
        Boolean bval;
        imm->HideSoftInputFromWindow(token, 0, &bval);
    }

    Dialog::Hide();
    return NOERROR;
}

ECode SearchDialog::LaunchQuerySearch()
{
    LaunchQuerySearch(IKeyEvent::KEYCODE_UNKNOWN, String(NULL));
    return NOERROR;
}

void SearchDialog::LaunchQuerySearch(
    /* [in] */ Int32 actionKey,
    /* [in] */ const String& actionMsg)
{
    AutoPtr<ICharSequence> csq;
    ITextView::Probe(mSearchAutoComplete)->GetText((ICharSequence**)&csq);
    String query = Object::ToString(csq);
    String action = IIntent::ACTION_SEARCH;
    String nullStr;
    AutoPtr<IIntent> intent = CreateIntent(action, NULL, nullStr, query, actionKey, actionMsg);
    LaunchIntent(intent);
}

void SearchDialog::LaunchIntent(
    /* [in] */ IIntent* intent)
{
    if (intent == NULL) {
        return;
    }
    // Logger::D(TAG, "launching " + intent);
    // try {
        // If the intent was created from a suggestion, it will always have an explicit
        // component here.
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    ctx->StartActivity(intent);

    // If the search switches to a different activity,
    // SearchDialogWrapper#performActivityResuming
    // will handle hiding the dialog when the next activity starts, but for
    // real in-app search, we still need to Dismiss the dialog.
    Dismiss();
    // } catch (RuntimeException ex) {
    //     Log.e(TAG, "Failed launch activity: " + intent, ex);
    // }
}

ECode SearchDialog::SetListSelection(
    /* [in] */ Int32 index)
{
    return mSearchAutoComplete->SetListSelection(index);
}

AutoPtr<IIntent> SearchDialog::CreateIntent(
    /* [in] */ const String& action,
    /* [in] */ IUri* data,
    /* [in] */ const String& extraData,
    /* [in] */ const String& query,
    /* [in] */ Int32 actionKey,
    /* [in] */ const String& actionMsg)
{
    // Now build the Intent
    AutoPtr<IIntent> intent;
    CIntent::New(action, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    // We need CLEAR_TOP to avoid reusing an old task that has other activities
    // on top of the one we want. We don't want to do this in in-app search though,
    // as it can be destructive to the activity stack.
    if (data != NULL) {
        intent->SetData(data);
    }
    intent->PutExtra(ISearchManager::USER_QUERY, mUserQuery);
    if (query != NULL) {
        intent->PutExtra(ISearchManager::QUERY, query);
    }
    if (extraData != NULL) {
        intent->PutExtra(ISearchManager::EXTRA_DATA_KEY, extraData);
    }
    if (mAppSearchData != NULL) {
        intent->PutExtra(ISearchManager::APP_DATA, mAppSearchData);
    }
    if (actionKey != IKeyEvent::KEYCODE_UNKNOWN) {
        intent->PutExtra(ISearchManager::ACTION_KEY, actionKey);
        intent->PutExtra(ISearchManager::ACTION_MSG, actionMsg);
    }
    AutoPtr<IComponentName> cn;
    mSearchable->GetSearchActivity((IComponentName**)&cn);
    intent->SetComponent(cn);
    return intent;
}

Boolean SearchDialog::IsEmpty(
    /* [in] */ IAutoCompleteTextView* actv)
{
    AutoPtr<ICharSequence> text;
    ITextView::Probe(actv)->GetText((ICharSequence**)&text);
    return TextUtils::GetTrimmedLength(text) == 0;
}

ECode SearchDialog::OnBackPressed()
{
    // If the input method is covering the search dialog completely,
    // e->G. in landscape mode with no hard keyboard, Dismiss just the input method
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IInterface> obj;
    ctx->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    AutoPtr<IInputMethodManager> imm = IInputMethodManager::Probe(obj);
    if (imm != NULL) {
        Boolean bval;
        imm->IsFullscreenMode(&bval);
        if (bval) {
            AutoPtr<IWindow> window;
            GetWindow((IWindow**)&window);
            AutoPtr<IView> view;
            window->GetDecorView((IView**)&view);
            AutoPtr<IBinder> token;
            view->GetWindowToken((IBinder**)&token);
            imm->HideSoftInputFromWindow(token, 0, &bval);
            if (bval) {
                return NOERROR;
            }
        }
    }

    // Close search dialog
    Cancel();
    return NOERROR;
}

Boolean SearchDialog::OnClosePressed()
{
    // Dismiss the dialog if close button is pressed when there's no query text
    if (IsEmpty(mSearchAutoComplete)) {
        Dismiss();
        return TRUE;
    }

    return FALSE;
}

void SearchDialog::SetUserQuery(
    /* [in] */ const String& inQuery)
{
    String query(inQuery);
    if (query == NULL) {
        query = "";
    }
    mUserQuery = query;
    ITextView::Probe(mSearchAutoComplete)->SetText(CoreUtils::Convert(query));
    IEditText::Probe(mSearchAutoComplete)->SetSelection(query.GetLength());
}

} // namespace App
} // namespace Droid
} // namespace Elastos
