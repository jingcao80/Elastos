#ifndef __ELASTOS_DROID_APP_CSEARCHMANAGER_H__
#define __ELASTOS_DROID_APP_CSEARCHMANAGER_H__

#include "_Elastos_Droid_App_CSearchManager.h"
#include <elastos/core/Object.h>
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IRect;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CSearchManager)
    , public Object
    , public ISearchManager
{
private:
    class InnerListener
        : public Object
        , public IDialogInterfaceOnDismissListener
        , public IDialogInterfaceOnCancelListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CSearchManager* host);
        /**
         * @deprecated This method is an obsolete internal implementation detail. Do not use.
         */
        //@Deprecated
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

        /**
         * @deprecated This method is an obsolete internal implementation detail. Do not use.
         */
        //@Deprecated
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);
    private:
        CSearchManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSearchManager();

    virtual ~CSearchManager();

    CARAPI constructor(
        /* [in] */ IContext * context,
        /* [in] */ IHandler * handler);

    /**
     * Launch search UI.
     *
     * <p>The search manager will open a search widget in an overlapping
     * window, and the underlying activity may be obscured.  The search
     * entry state will remain in effect until one of the following events:
     * <ul>
     * <li>The user completes the search.  In most cases this will launch
     * a search intent.</li>
     * <li>The user uses the back, home, or other keys to exit the search.</li>
     * <li>The application calls the {@link #stopSearch}
     * method, which will hide the search window and return focus to the
     * activity from which it was launched.</li>
     *
     * <p>Most applications will <i>not</i> use this interface to invoke search.
     * The primary method for invoking search is to call
     * {@link android.app.Activity#onSearchRequested Activity.onSearchRequested()} or
     * {@link android.app.Activity#startSearch Activity.startSearch()}.
     *
     * @param initialQuery A search string can be pre-entered here, but this
     * is typically null or empty.
     * @param selectInitialQuery If true, the intial query will be preselected, which means that
     * any further typing will replace it.  This is useful for cases where an entire pre-formed
     * query is being inserted.  If false, the selection point will be placed at the end of the
     * inserted query.  This is useful when the inserted query is text that the user entered,
     * and the user would expect to be able to keep typing.  <i>This parameter is only meaningful
     * if initialQuery is a non-empty string.</i>
     * @param launchActivity The ComponentName of the activity that has launched this search.
     * @param appSearchData An application can insert application-specific
     * context here, in order to improve quality or specificity of its own
     * searches.  This data will be returned with SEARCH intent(s).  Null if
     * no extra data is required.
     * @param globalSearch If false, this will only launch the search that has been specifically
     * defined by the application (which is usually defined as a local search).  If no default
     * search is defined in the current application or activity, global search will be launched.
     * If true, this will always launch a platform-global (e.g. web-based) search instead.
     *
     * @see android.app.Activity#onSearchRequested
     * @see #stopSearch
     */
    CARAPI StartSearch(
        /* [in] */ const String& initialQuery,
        /* [in] */ Boolean selectInitialQuery,
        /* [in] */ IComponentName* launchActivity,
        /* [in] */ IBundle* appSearchData,
        /* [in] */ Boolean globalSearch);

    /**
     * As {@link #startSearch(String, boolean, ComponentName, Bundle, boolean)} but including
     * source bounds for the global search intent.
     *
     * @hide
     */
    CARAPI StartSearch(
        /* [in] */ const String& initialQuery,
        /* [in] */ Boolean selectInitialQuery,
        /* [in] */ IComponentName* launchActivity,
        /* [in] */ IBundle* appSearchData,
        /* [in] */ Boolean globalSearch,
        /* [in] */ IRect* sourceBounds);

    /**
     * Returns a list of installed apps that handle the global search
     * intent.
     *
     * @hide
     */
    CARAPI GetGlobalSearchActivities(
        /* [out] */ IList** apps);

    /**
     * Gets the name of the global search activity.
     */
    CARAPI GetGlobalSearchActivity(
        /* [out] */ IComponentName** name);

    /**
     * Gets the name of the web search activity.
     *
     * @return The name of the default activity for web searches. This activity
     *         can be used to get web search suggestions. Returns {@code null} if
     *         there is no default web search activity.
     *
     * @hide
     */
    CARAPI GetWebSearchActivity(
        /* [out] */ IComponentName** name);

    /**
     * Similar to {@link #startSearch} but actually fires off the search query after invoking
     * the search dialog.  Made available for testing purposes.
     *
     * @param query The query to trigger.  If empty, request will be ignored.
     * @param launchActivity The ComponentName of the activity that has launched this search.
     * @param appSearchData An application can insert application-specific
     * context here, in order to improve quality or specificity of its own
     * searches.  This data will be returned with SEARCH intent(s).  Null if
     * no extra data is required.
     *
     * @see #startSearch
     */
    CARAPI TriggerSearch(
        /* [in] */ const String& query,
        /* [in] */ IComponentName* launchActivity,
        /* [in] */ IBundle* appSearchData);

    /**
     * Terminate search UI.
     *
     * <p>Typically the user will terminate the search UI by launching a
     * search or by canceling.  This function allows the underlying application
     * or activity to cancel the search prematurely (for any reason).
     *
     * <p>This function can be safely called at any time (even if no search is active.)
     *
     * @see #startSearch
     */
    CARAPI StopSearch();

    /**
     * Determine if the Search UI is currently displayed.
     *
     * This is provided primarily for application test purposes.
     *
     * @return Returns true if the search UI is currently displayed.
     *
     * @hide
     */
    CARAPI IsVisible(
        /* [out] */ Boolean* visible);

    /**
     * Set or clear the callback that will be invoked whenever the search UI is dismissed.
     *
     * @param listener The {@link OnDismissListener} to use, or null.
     */
    CARAPI SetOnDismissListener(
        /* [in] */ ISearchManagerOnDismissListener* listener);

    /**
     * Set or clear the callback that will be invoked whenever the search UI is canceled.
     *
     * @param listener The {@link OnCancelListener} to use, or null.
     */
    CARAPI SetOnCancelListener(
        /* [in] */ ISearchManagerOnCancelListener* listener);

    /**
     * @deprecated This method is an obsolete internal implementation detail. Do not use.
     */
    //@Deprecated
    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    /**
     * @deprecated This method is an obsolete internal implementation detail. Do not use.
     */
    //@Deprecated
    CARAPI OnDismiss(
        /* [in] */ IDialogInterface* dialog);

    /**
     * Gets information about a searchable activity.
     *
     * @param componentName The activity to get searchable information for.
     * @return Searchable information, or <code>null</code> if the activity does not
     *         exist, or is not searchable.
     */
    CARAPI GetSearchableInfo(
        /* [in] */ IComponentName* componentName,
        /* [out] */ ISearchableInfo** info);

    /**
     * Gets a cursor with search suggestions.
     *
     * @param searchable Information about how to get the suggestions.
     * @param query The search text entered (so far).
     * @return a cursor with suggestions, or <code>null</null> the suggestion query failed.
     *
     * @hide because SearchableInfo is not part of the API.
     */
    CARAPI GetSuggestions(
        /* [in] */ ISearchableInfo* searchable,
        /* [in] */ const String& query,
        /* [out] */ ICursor** cursor);

    /**
     * Gets a cursor with search suggestions.
     *
     * @param searchable Information about how to get the suggestions.
     * @param query The search text entered (so far).
     * @param limit The query limit to pass to the suggestion provider. This is advisory,
     *        the returned cursor may contain more rows. Pass {@code -1} for no limit.
     * @return a cursor with suggestions, or <code>null</null> the suggestion query failed.
     *
     * @hide because SearchableInfo is not part of the API.
     */
    CARAPI GetSuggestions(
        /* [in] */ ISearchableInfo* searchable,
        /* [in] */ const String& query,
        /* [in] */ Int32 limit,
        /* [out] */ ICursor** cursor);

    /**
     * Returns a list of the searchable activities that can be included in global search.
     *
     * @return a list containing searchable information for all searchable activities
     *         that have the <code>android:includeInGlobalSearch</code> attribute set
     *         in their searchable meta-data.
     */
    CARAPI GetSearchablesInGlobalSearch(
        /* [out] */ IList** info);

    /**
     * Gets an intent for launching installed assistant activity, or null if not available.
     * @return The assist intent.
     *
     * @hide
     */
    CARAPI GetAssistIntent(
        /* [in] */ IContext* context,
        /* [in] */ Boolean inclContext,
        /* [out] */ IIntent** intent);

    /**
     * Gets an intent for launching installed assistant activity, or null if not available.
     * @return The assist intent.
     *
     * @hide
     */
    CARAPI GetAssistIntent(
        /* [in] */ IContext* context,
        /* [in] */ Boolean inclContext,
        /* [in] */ Int32 userHandle,
        /* [out] */ IIntent** intent);

    CARAPI LaunchAssistAction(
        /* [in] */ Int32 requestType,
        /* [in] */ const String& hint,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

public:
    /**
     * Reference to the shared system search service.
     */
    static AutoPtr<IISearchManager> mService;

    IContext* mContext;

    // package private since they are used by the inner class SearchManagerCallback
    /* package */ AutoPtr<IHandler> mHandler;
    /* package */ AutoPtr<ISearchManagerOnDismissListener> mDismissListener;
    /* package */ AutoPtr<ISearchManagerOnCancelListener> mCancelListener;

    AutoPtr<ISearchDialog> mSearchDialog;

private:
    CARAPI_(void) EnsureSearchDialog();

    /**
     * Starts the global search activity.
     */
    CARAPI_(void) StartGlobalSearch(
        /* [in] */ const String& initialQuery,
        /* [in] */ Boolean selectInitialQuery,
        /* [in] */ IBundle* appSearchData,
        /* [in] */ IRect* sourceBounds);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CSEARCHMANAGER_H__
