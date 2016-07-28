#include "elastos/droid/app/CSearchManager.h"
#include "elastos/droid/app/CUiModeManager.h"
#include "elastos/droid/app/CSearchDialog.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/net/CUriBuilder.h"
#include "elastos/core/StringUtils.h"

using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::CUriBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace App {

AutoPtr<IISearchManager> CSearchManager::mService;

CAR_INTERFACE_IMPL_3(CSearchManager, Object, ISearchManager, \
    IDialogInterfaceOnDismissListener, IDialogInterfaceOnCancelListener)

CAR_OBJECT_IMPL(CSearchManager)

CSearchManager::CSearchManager()
{
}

CSearchManager::~CSearchManager()
{
}

ECode CSearchManager::constructor(
    /* [in] */ IContext *context,
    /* [in] */ IHandler *handler)
{
    mContext = context;
    mHandler = handler;

    if (mService == NULL) {
        AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::SEARCH_SERVICE);
        mService = IISearchManager::Probe(obj);
    }
    return NOERROR;
}

ECode CSearchManager::StartSearch(
    /* [in] */ const String &initialQuery,
    /* [in] */ Boolean selectInitialQuery,
    /* [in] */ IComponentName* launchActivity,
    /* [in] */ IBundle* appSearchData,
    /* [in] */ Boolean globalSearch)
{
    return StartSearch(initialQuery, selectInitialQuery, launchActivity,
            appSearchData, globalSearch, NULL);
}

ECode CSearchManager::StartSearch(
    /* [in] */ const String &initialQuery,
    /* [in] */ Boolean selectInitialQuery,
    /* [in] */ IComponentName* launchActivity,
    /* [in] */ IBundle* appSearchData,
    /* [in] */ Boolean globalSearch,
    /* [in] */ IRect *sourceBounds)
{
    if (globalSearch) {
        StartGlobalSearch(initialQuery, selectInitialQuery, appSearchData, sourceBounds);
        return NOERROR;
    }

    AutoPtr<IUiModeManager> uiModeManager;
    CUiModeManager::New((IUiModeManager**)&uiModeManager);
    // Don't show search dialog on televisions.
    Int32 type;
    uiModeManager->GetCurrentModeType(&type);
    if (type != IConfiguration::UI_MODE_TYPE_TELEVISION) {
        EnsureSearchDialog();

        Boolean bval;
        mSearchDialog->Show(initialQuery, selectInitialQuery,
            launchActivity, appSearchData, &bval);
    }
    return NOERROR;
}

ECode CSearchManager::GetGlobalSearchActivities(
    /* [out] */ IList** apps)
{
    //try {
        return mService->GetGlobalSearchActivities(apps);
    //} catch (RemoteException ex) {
    //    Log.e(TAG, "getGlobalSearchActivities() failed: " + ex);
    //    return null;
    //}
}

ECode CSearchManager::GetGlobalSearchActivity(
    /* [out] */ IComponentName** name)
{
    //try {
    VALIDATE_NOT_NULL(name)
    *name = NULL;
    if (mService) {
        return mService->GetGlobalSearchActivity(name);
    }
    return NOERROR;
    //} catch (RemoteException ex) {
    //    Log.e(TAG, "getGlobalSearchActivity() failed: " + ex);
    //    return null;
    //}
}

ECode CSearchManager::GetWebSearchActivity(
    /* [out] */ IComponentName** name)
{
    //try {
        return mService->GetWebSearchActivity(name);
    //} catch (RemoteException ex) {
    //    Log.e(TAG, "getWebSearchActivity() failed: " + ex);
    //    return null;
    //}
}

ECode CSearchManager::TriggerSearch(
    /* [in] */ const String &query,
    /* [in] */ IComponentName* launchActivity,
    /* [in] */ IBundle* appSearchData)
{
    if (query == NULL || TextUtils::GetTrimmedLength(query) == 0) {
        //Log.w(TAG, "triggerSearch called with empty query, ignoring.");
        return NOERROR;
    }
    StartSearch(query, false, launchActivity, appSearchData, false);
    mSearchDialog->LaunchQuerySearch();
    return NOERROR;
}

ECode CSearchManager::StopSearch()
{
    if (mSearchDialog != NULL) {
        IDialogInterface::Probe(mSearchDialog)->Cancel();
    }
    return NOERROR;
}

ECode CSearchManager::IsVisible(
    /* [out] */ Boolean *visible)
{
    VALIDATE_NOT_NULL(visible)
    *visible = FALSE;

    if(mSearchDialog != NULL)  {
        IDialog::Probe(mSearchDialog)->IsShowing(visible);
    }
    return NOERROR;
}

ECode CSearchManager::SetOnDismissListener(
    /* [in] */ ISearchManagerOnDismissListener *listener)
{
    mDismissListener = listener;
    return NOERROR;
}

ECode CSearchManager::SetOnCancelListener(
    /* [in] */ ISearchManagerOnCancelListener *listener)
{
    mCancelListener = listener;
    return NOERROR;
}

ECode CSearchManager::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    if (mCancelListener != NULL) {
        mCancelListener->OnCancel();
    }
    return NOERROR;
}

ECode CSearchManager::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    if (mDismissListener != NULL) {
        mDismissListener->OnDismiss();
    }
    return NOERROR;
}

ECode CSearchManager::GetSearchableInfo(
    /* [in] */ IComponentName* componentName,
    /* [out] */ ISearchableInfo **info)
{
    VALIDATE_NOT_NULL(info)
    //try {
        return mService->GetSearchableInfo(componentName, info);
    //} catch (RemoteException ex) {
    //    Log.e(TAG, "getSearchableInfo() failed: " + ex);
    //    return null;
    //}
}

ECode CSearchManager::GetSuggestions(
    /* [in] */ ISearchableInfo *searchable,
    /* [in] */ const String &query,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    return GetSuggestions(searchable, query, -1, cursor);
}

ECode CSearchManager::GetSuggestions(
    /* [in] */ ISearchableInfo *searchable,
    /* [in] */ const String &query,
    /* [in] */ Int32 limit,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = NULL;

    if (searchable == NULL) {
        return NOERROR;
    }

    String authority;
    searchable->GetSuggestAuthority(&authority);
    if (authority == NULL) {
        return NOERROR;
    }

    AutoPtr<IUriBuilder> uriBuilder;
    CUriBuilder::New((IUriBuilder**)&uriBuilder);
    uriBuilder->Scheme(IContentResolver::SCHEME_CONTENT);
    uriBuilder->Authority(authority);
    String empty("");
    uriBuilder->Query(empty);  // TODO: Remove, workaround for a bug in Uri.writeToParcel()
    uriBuilder->Fragment(empty);  // TODO: Remove, workaround for a bug in Uri.writeToParcel()

    // if content path provided, insert it now
    String contentPath;
    searchable->GetSuggestPath(&contentPath);
    if (contentPath != NULL) {
        uriBuilder->AppendEncodedPath(contentPath);
    }

    // append standard suggestion query path
    uriBuilder->AppendPath(ISearchManager::SUGGEST_URI_PATH_QUERY);

    // get the query selection, may be NULL
    String selection;
    searchable->GetSuggestSelection(&selection);
    // inject query, either as selection args or inline
    AutoPtr<ArrayOf<String> > selArgs;
    if (selection != NULL) {    // use selection if provided
        selArgs = ArrayOf<String>::Alloc(1);
        selArgs->Set(0, query);
    }
    else {                    // no selection, use REST pattern
        uriBuilder->AppendPath(query);
    }

    if (limit > 0) {
        uriBuilder->AppendQueryParameter(
            ISearchManager::SUGGEST_PARAMETER_LIMIT,
            StringUtils::ToString(limit));
    }

    AutoPtr<IUri> uri;
    uriBuilder->Build((IUri**)&uri);

    // finally, make the query
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ArrayOf<String> > projections;
    return cr->Query(uri, projections, selection, selArgs, String(NULL), cursor);
}

ECode CSearchManager::GetSearchablesInGlobalSearch(
    /* [out] */ IList** info)
{
    // try {
        return mService->GetSearchablesInGlobalSearch(info);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "getSearchablesInGlobalSearch() failed: " + e);
    //     return null;
    // }
}

ECode CSearchManager::GetAssistIntent(
    /* [in] */ IContext *context,
    /* [in] */ Boolean inclContext,
    /* [out] */ IIntent **intent)
{
    return GetAssistIntent(context, inclContext, UserHandle::GetMyUserId(), intent);
}

ECode CSearchManager::GetAssistIntent(
    /* [in] */ IContext *context,
    /* [in] */ Boolean inclContext,
    /* [in] */ Int32 userHandle,
    /* [out] */ IIntent **intent)
{
    VALIDATE_NOT_NULL(intent);
    *intent = NULL;

    //try {
    if (mService == NULL) {
        return NOERROR;
    }

    AutoPtr<IComponentName> comp;
    mService->GetAssistIntent(userHandle, (IComponentName**)&comp);
    if (comp == NULL) {
        return NOERROR;
    }
    AutoPtr<IIntent> temp;
    CIntent::New(IIntent::ACTION_ASSIST, (IIntent**)&temp);
    temp->SetComponent(comp);

    if (inclContext) {
        AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
        AutoPtr<IBundle> extras;
        am->GetAssistContextExtras(0, (IBundle**)&extras);
        if (extras != NULL) {
            temp->ReplaceExtras(extras);
        }
    }

    *intent = temp;
    REFCOUNT_ADD(*intent);
    //} catch (RemoteException re) {
    //    Log.e(TAG, "getAssistIntent() failed: " + re);
    //    return null;
    //}
    return NOERROR;
}

ECode CSearchManager::LaunchAssistAction(
    /* [in] */ Int32 requestType,
    /* [in] */ const String& hint,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mService == NULL) {
        return NOERROR;
    }
    // try {
        return mService->LaunchAssistAction(requestType, hint, userHandle, result);
    // } catch (RemoteException re) {
    //     Log.e(TAG, "launchAssistAction() failed: " + re);
    //     return false;
    // }
}

void CSearchManager::EnsureSearchDialog()
{
    if (mSearchDialog == NULL) {
        CSearchDialog::New(mContext, this, (ISearchDialog**)&mSearchDialog);
        IDialog* dialog = IDialog::Probe(mSearchDialog);
        dialog->SetOnCancelListener(this);
        dialog->SetOnDismissListener(this);
    }
}

void CSearchManager::StartGlobalSearch(
    /* [in] */ const String& initialQuery,
    /* [in] */ Boolean selectInitialQuery,
    /* [in] */ IBundle* inAppSearchData,
    /* [in] */ IRect* sourceBounds)
{
    AutoPtr<IComponentName> globalSearchActivity;
    GetGlobalSearchActivity((IComponentName**)&globalSearchActivity);
    if (globalSearchActivity == NULL) {
        //Log.w(TAG, "No global search activity found.");
        return;
    }
    AutoPtr<IBundle> appSearchData = inAppSearchData;
    AutoPtr<IIntent> intent;
    CIntent::New(ISearchManager::INTENT_ACTION_GLOBAL_SEARCH, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    intent->SetComponent(globalSearchActivity);
    // Make sure that we have a Bundle to put source in
    if (appSearchData == NULL) {
        CBundle::New((IBundle**)&appSearchData);
    }
    else {
        AutoPtr<IBundle> tmp = appSearchData;
        appSearchData = NULL;
        CBundle::New(tmp, (IBundle**)&appSearchData);
    }
    // Set source to package name of app that starts global search, if not set already.
    Boolean b;
    String key("source");
    appSearchData->ContainsKey(key, &b);
    if (!b) {
        String name;
        mContext->GetPackageName(&name);
        appSearchData->PutString(key, name);
    }
    intent->PutExtra(ISearchManager::APP_DATA, appSearchData);
    if (!TextUtils::IsEmpty(initialQuery)) {
        intent->PutExtra(ISearchManager::QUERY, initialQuery);
    }
    if (selectInitialQuery) {
        intent->PutExtra(ISearchManager::EXTRA_SELECT_QUERY, selectInitialQuery);
    }
    intent->SetSourceBounds(sourceBounds);
    //try {
        //if (DBG) Log.d(TAG, "Starting global search: " + intent.toUri(0));
        mContext->StartActivity(intent);
    //} catch (ActivityNotFoundException ex) {
    //    Log.e(TAG, "Global search activity not found: " + globalSearchActivity);
    //}
}

}
}
}
