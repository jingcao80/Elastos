
#ifndef __ELASTOS_DROID_APP_ACTIVITY_H__
#define __ELASTOS_DROID_APP_ACTIVITY_H__

#include "elastos/droid/view/ContextThemeWrapper.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Os::IPersistableBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IComponentCallbacks;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Transition::IScene;
using Elastos::Droid::Transition::ITransitionManager;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::ContextThemeWrapper;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::View::IContextMenu;
using Elastos::Droid::View::IContextMenuInfo;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnCreateContextMenuListener;
using Elastos::Droid::View::ILayoutInflaterFactory;
using Elastos::Droid::View::ILayoutInflaterFactory2;
using Elastos::Droid::View::IOnWindowDismissedCallback;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::Widget::IToolbar;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Media::Session::IMediaController;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Internal::App::IIVoiceInteractor;

using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IHashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace App {

class ECO_PUBLIC Activity
    : public ContextThemeWrapper
    , public IActivity
    , public ILayoutInflaterFactory
    , public ILayoutInflaterFactory2
    , public IWindowCallback
    , public IKeyEventCallback
    , public IViewOnCreateContextMenuListener
    , public IComponentCallbacks
    , public IComponentCallbacks2
    , public IOnWindowDismissedCallback
{
private:
    class ECO_LOCAL FragmentContainerLocal
        : public Object
        , public IFragmentContainer
    {
    public:
        CAR_INTERFACE_DECL()

        FragmentContainerLocal(
            /* [in] */ Activity* host);

        CARAPI FindViewById(
            /* [in] */ Int32 id,
            /* [out] */ IView** view);

        CARAPI HasView(
            /* [out] */ Boolean* hasView);

    private:
        Activity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    Activity();

    virtual ~Activity();

    CARAPI constructor();

    /** Return the intent that started this activity. */
    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

    /**
     * Change the intent returned by {@link #getIntent}.  This holds a
     * reference to the given intent; it does not copy it.  Often used in
     * conjunction with {@link #onNewIntent}.
     *
     * @param newIntent The new Intent object to return from getIntent
     *
     * @see #getIntent
     * @see #onNewIntent
     */
    CARAPI SetIntent(
        /* [in] */ IIntent* newIntent);

    /** Return the application that owns this activity. */
    CARAPI GetApplication(
        /* [out] */ IApplication** application);

    CARAPI_(AutoPtr<IApplication>) GetApplication();

    /** Is this activity embedded inside of another activity? */
    CARAPI IsChild(
        /* [out] */ Boolean* isChild);

    /** Return the parent activity if this view is an embedded child. */
    CARAPI GetParent(
        /* [out] */ IActivity** parent);

    /** Retrieve the window manager for showing custom windows. */
    CARAPI GetWindowManager(
        /* [out] */ IWindowManager** mgr);

    /** Retrieve the window manager for showing custom windows. */
    CARAPI_(AutoPtr<IWindowManager>) GetWindowManager();

    /**
     * Retrieve the current {@link android.view.Window} for the activity.
     * This can be used to directly access parts of the Window API that
     * are not available through Activity/Screen.
     *
     * @return Window The current window, or null if the activity is not
     *         visual.
     */
    CARAPI GetWindow(
        /* [out] */ IWindow** window);

    CARAPI_(AutoPtr<IWindow>) GetWindow();

    /**
     * Return the LoaderManager for this fragment, creating it if needed.
     */
    CARAPI GetLoaderManager(
        /* [out] */ ILoaderManager** manager);

    CARAPI_(AutoPtr<ILoaderManagerImpl>) GetLoaderManager(
        /* [in] */ const String& who,
        /* [in] */ Boolean started,
        /* [in] */ Boolean create);

    /**
     * Calls {@link android.view.Window#getCurrentFocus} on the
     * Window of this Activity to return the currently focused view.
     *
     * @return View The current View with focus or null.
     *
     * @see #getWindow
     * @see android.view.Window#getCurrentFocus
     */
    CARAPI GetCurrentFocus(
        /* [out] */ IView** focus);

    /**
     * Generate a new thumbnail for this activity.  This method is called before
     * pausing the activity, and should draw into <var>outBitmap</var> the
     * imagery for the desired thumbnail in the dimensions of that bitmap.  It
     * can use the given <var>canvas</var>, which is configured to draw into the
     * bitmap, for rendering if desired.
     *
     * <p>The default implementation returns fails and does not draw a thumbnail;
     * this will result in the platform creating its own thumbnail if needed.
     *
     * @param outBitmap The bitmap to contain the thumbnail.
     * @param canvas Can be used to render into the bitmap.
     *
     * @return Return true if you have drawn into the bitmap; otherwise after
     *         you return it will be filled with a default thumbnail.
     *
     * @see #onCreateDescription
     * @see #onSaveInstanceState
     * @see #onPause
     */
    CARAPI OnCreateThumbnail(
        /* [in] */ IBitmap* outBitmap,
        /* [in] */ ICanvas* canvas,
        /* [out] */ Boolean* value);

    /**
     * Generate a new description for this activity.  This method is called
     * before pausing the activity and can, if desired, return some textual
     * description of its current state to be displayed to the user.
     *
     * <p>The default implementation returns null, which will cause you to
     * inherit the description from the previous activity.  If all activities
     * return null, generally the label of the top activity will be used as the
     * description.
     *
     * @return A description of what the user is doing.  It should be short and
     *         sweet (only a few words).
     *
     * @see #onCreateThumbnail
     * @see #onSaveInstanceState
     * @see #onPause
     */
    CARAPI OnCreateDescription(
        /* [out] */ ICharSequence** description);

    /**
     * This is called when the user is requesting an assist, to build a full
     * {@link Intent#ACTION_ASSIST} Intent with all of the context of the current
     * application.  You can override this method to place into the bundle anything
     * you would like to appear in the {@link Intent#EXTRA_ASSIST_CONTEXT} part
     * of the assist Intent.  The default implementation does nothing.
     *
     * <p>This function will be called after any global assist callbacks that had
     * been registered with {@link Application#registerOnProvideAssistDataListener
     * Application.registerOnProvideAssistDataListener}.
     */
    CARAPI OnProvideAssistData(
        /* [out] */ IBundle* data);

    /**
     * Report to the system that your app is now fully drawn, purely for diagnostic
     * purposes (calling it does not impact the visible behavior of the activity).
     * This is only used to help instrument application launch times, so that the
     * app can report when it is fully in a usable state; without this, the only thing
     * the system itself can determine is the point at which the activity's window
     * is <em>first</em> drawn and displayed.  To participate in app launch time
     * measurement, you should always call this method after first launch (when
     * {@link #onCreate(android.os.Bundle)} is called), at the point where you have
     * entirely drawn your UI and populated with all of the significant data.  You
     * can safely call this method any time after first launch as well, in which case
     * it will simply be ignored.
     */
    CARAPI ReportFullyDrawn();

    /**
     * Called by the system when the device configuration changes while your
     * activity is running.  Note that this will <em>only</em> be called if
     * you have selected configurations you would like to handle with the
     * {@link android.R.attr#configChanges} attribute in your manifest.  If
     * any configuration change occurs that is not selected to be reported
     * by that attribute, then instead of reporting it the system will stop
     * and restart the activity (to have it launched with the new
     * configuration).
     *
     * <p>At the time that this function has been called, your Resources
     * object will have been updated to return resource values matching the
     * new configuration.
     *
     * @param newConfig The new device configuration.
     */
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    /**
     * If this activity is being destroyed because it can not handle a
     * configuration parameter being changed (and thus its
     * {@link #onConfigurationChanged(Configuration)} method is
     * <em>not</em> being called), then you can use this method to discover
     * the set of changes that have occurred while in the process of being
     * destroyed.  Note that there is no guarantee that these will be
     * accurate (other changes could have happened at any time), so you should
     * only use this as an optimization hint.
     *
     * @return Returns a bit field of the configuration parameters that are
     * changing, as defined by the {@link android.content.res.Configuration}
     * class.
     */
    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* config);

    /**
     * Retrieve the non-configuration instance data that was previously
     * returned by {@link #onRetainNonConfigurationInstance()}.  This will
     * be available from the initial {@link #onCreate} and
     * {@link #onStart} calls to the new instance, allowing you to extract
     * any useful dynamic state from the previous instance.
     *
     * <p>Note that the data you retrieve here should <em>only</em> be used
     * as an optimization for handling configuration changes.  You should always
     * be able to handle getting a null pointer back, and an activity must
     * still be able to restore itself to its previous state (through the
     * normal {@link #onSaveInstanceState(Bundle)} mechanism) even if this
     * function returns null.
     *
     * @return Returns the object previously returned by
     * {@link #onRetainNonConfigurationInstance()}.
     *
     * @deprecated Use the new {@link Fragment} API
     * {@link Fragment#setRetainInstance(boolean)} instead; this is also
     * available on older platforms through the Android compatibility package.
     */
    //@Deprecated
    CARAPI GetLastNonConfigurationInstance(
        /* [out] */ IInterface** instance);

    /**
     * Called by the system, as part of destroying an
     * activity due to a configuration change, when it is known that a new
     * instance will immediately be created for the new configuration.  You
     * can return any object you like here, including the activity instance
     * itself, which can later be retrieved by calling
     * {@link #getLastNonConfigurationInstance()} in the new activity
     * instance.
     *
     * <em>If you are targeting {@link android.os.Build.VERSION_CODES#HONEYCOMB}
     * or later, consider instead using a {@link Fragment} with
     * {@link Fragment#setRetainInstance(boolean)
     * Fragment.setRetainInstance(boolean}.</em>
     *
     * <p>This function is called purely as an optimization, and you must
     * not rely on it being called.  When it is called, a number of guarantees
     * will be made to help optimize configuration switching:
     * <ul>
     * <li> The function will be called between {@link #onStop} and
     * {@link #onDestroy}.
     * <li> A new instance of the activity will <em>always</em> be immediately
     * created after this one's {@link #onDestroy()} is called.  In particular,
     * <em>no</em> messages will be dispatched during this time (when the returned
     * object does not have an activity to be associated with).
     * <li> The object you return here will <em>always</em> be available from
     * the {@link #getLastNonConfigurationInstance()} method of the following
     * activity instance as described there.
     * </ul>
     *
     * <p>These guarantees are designed so that an activity can use this API
     * to propagate extensive state from the old to new activity instance, from
     * loaded bitmaps, to network connections, to evenly actively running
     * threads.  Note that you should <em>not</em> propagate any data that
     * may change based on the configuration, including any data loaded from
     * resources such as strings, layouts, or drawables.
     *
     * <p>The guarantee of no message handling during the switch to the next
     * activity simplifies use with active objects.  For example if your retained
     * state is an {@link android.os.AsyncTask} you are guaranteed that its
     * call back functions (like {@link android.os.AsyncTask#onPostExecute}) will
     * not be called from the call here until you execute the next instance's
     * {@link #onCreate(Bundle)}.  (Note however that there is of course no such
     * guarantee for {@link android.os.AsyncTask#doInBackground} since that is
     * running in a separate thread.)
     *
     * @return Return any Object holding the desired state to propagate to the
     * next activity instance.
     *
     * @deprecated Use the new {@link Fragment} API
     * {@link Fragment#setRetainInstance(boolean)} instead; this is also
     * available on older platforms through the Android compatibility package.
     */
    CARAPI OnRetainNonConfigurationInstance(
        /* [out] */ IInterface** object);

    /**
     * Retrieve the non-configuration instance data that was previously
     * returned by {@link #onRetainNonConfigurationChildInstances()}.  This will
     * be available from the initial {@link #onCreate} and
     * {@link #onStart} calls to the new instance, allowing you to extract
     * any useful dynamic state from the previous instance.
     *
     * <p>Note that the data you retrieve here should <em>only</em> be used
     * as an optimization for handling configuration changes.  You should always
     * be able to handle getting a null pointer back, and an activity must
     * still be able to restore itself to its previous state (through the
     * normal {@link #onSaveInstanceState(Bundle)} mechanism) even if this
     * function returns null.
     *
     * @return Returns the object previously returned by
     * {@link #onRetainNonConfigurationChildInstances()}
     */
    CARAPI GetLastNonConfigurationChildInstances(
        /* [out] */ IHashMap** data);

    /**
     * This method is similar to {@link #onRetainNonConfigurationInstance()} except that
     * it should return either a mapping from  child activity id strings to arbitrary objects,
     * or null.  This method is intended to be used by Activity framework subclasses that control a
     * set of child activities, such as ActivityGroup.  The same guarantees and restrictions apply
     * as for {@link #onRetainNonConfigurationInstance()}.  The default implementation returns null.
     */
    CARAPI OnRetainNonConfigurationChildInstances(
        /* [out] */ IHashMap** objectStringMap);

    CARAPI RetainNonConfigurationInstances(
        /* [out] */ IActivityNonConfigurationInstances** instance);

    CARAPI OnLowMemory();

    CARAPI OnTrimMemory(
        /* [in] */ Int32 level);

    /**
     * Return the FragmentManager for interacting with fragments associated
     * with this activity.
     */
    CARAPI GetFragmentManager(
        /* [out] */ IFragmentManager** fmanager);

    CARAPI InvalidateFragment(
        /* [in] */ const String& who);

    /**
     * Called when a Fragment is being attached to this activity, immediately
     * after the call to its {@link Fragment#onAttach Fragment.onAttach()}
     * method and before {@link Fragment#onCreate Fragment.onCreate()}.
     */
    CARAPI OnAttachFragment(
        /* [in] */ IFragment* fragment);

    /**
     * Wrapper around
     * {@link ContentResolver#query(android.net.Uri , String[], String, String[], String)}
     * that gives the resulting {@link Cursor} to call
     * {@link #startManagingCursor} so that the activity will manage its
     * lifecycle for you.
     *
     * <em>If you are targeting {@link android.os.Build.VERSION_CODES#HONEYCOMB}
     * or later, consider instead using {@link LoaderManager} instead, available
     * via {@link #getLoaderManager()}.</em>
     *
     * <p><strong>Warning:</strong> Do not call {@link Cursor#close()} on a cursor obtained using
     * this method, because the activity will do that for you at the appropriate time. However, if
     * you call {@link #stopManagingCursor} on a cursor from a managed query, the system <em>will
     * not</em> automatically close the cursor and, in that case, you must call
     * {@link Cursor#close()}.</p>
     *
     * @param uri The URI of the content provider to query.
     * @param projection List of columns to return.
     * @param selection SQL WHERE clause.
     * @param sortOrder SQL ORDER BY clause.
     *
     * @return The Cursor that was returned by query().
     *
     * @see ContentResolver#query(android.net.Uri , String[], String, String[], String)
     * @see #startManagingCursor
     * @hide
     *
     * @deprecated Use {@link CursorLoader} instead.
     */
    //@Deprecated
    CARAPI ManagedQuery(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    /**
     * Wrapper around
     * {@link ContentResolver#query(android.net.Uri , String[], String, String[], String)}
     * that gives the resulting {@link Cursor} to call
     * {@link #startManagingCursor} so that the activity will manage its
     * lifecycle for you.
     *
     * <em>If you are targeting {@link android.os.Build.VERSION_CODES#HONEYCOMB}
     * or later, consider instead using {@link LoaderManager} instead, available
     * via {@link #getLoaderManager()}.</em>
     *
     * <p><strong>Warning:</strong> Do not call {@link Cursor#close()} on a cursor obtained using
     * this method, because the activity will do that for you at the appropriate time. However, if
     * you call {@link #stopManagingCursor} on a cursor from a managed query, the system <em>will
     * not</em> automatically close the cursor and, in that case, you must call
     * {@link Cursor#close()}.</p>
     *
     * @param uri The URI of the content provider to query.
     * @param projection List of columns to return.
     * @param selection SQL WHERE clause.
     * @param selectionArgs The arguments to selection, if any ?s are pesent
     * @param sortOrder SQL ORDER BY clause.
     *
     * @return The Cursor that was returned by query().
     *
     * @see ContentResolver#query(android.net.Uri , String[], String, String[], String)
     * @see #startManagingCursor
     *
     * @deprecated Use {@link CursorLoader} instead.
     */
    //@Deprecated
    CARAPI ManagedQuery(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    /**
     * This method allows the activity to take care of managing the given
     * {@link Cursor}'s lifecycle for you based on the activity's lifecycle.
     * That is, when the activity is stopped it will automatically call
     * {@link Cursor#deactivate} on the given Cursor, and when it is later restarted
     * it will call {@link Cursor#requery} for you.  When the activity is
     * destroyed, all managed Cursors will be closed automatically.
     *
     * <em>If you are targeting {@link android.os.Build.VERSION_CODES#HONEYCOMB}
     * or later, consider instead using {@link LoaderManager} instead, available
     * via {@link #getLoaderManager()}.</em>
     *
     * <p><strong>Warning:</strong> Do not call {@link Cursor#close()} on cursor obtained from
     * {@link #managedQuery}, because the activity will do that for you at the appropriate time.
     * However, if you call {@link #stopManagingCursor} on a cursor from a managed query, the system
     * <em>will not</em> automatically close the cursor and, in that case, you must call
     * {@link Cursor#close()}.</p>
     *
     * @param c The Cursor to be managed.
     *
     * @see #managedQuery(android.net.Uri , String[], String, String[], String)
     * @see #stopManagingCursor
     *
     * @deprecated Use the new {@link android.content.CursorLoader} class with
     * {@link LoaderManager} instead; this is also
     * available on older platforms through the Android compatibility package.
     */
    //@Deprecated
    CARAPI StartManagingCursor(
        /* [in] */ ICursor* c);

    /**
     * Given a Cursor that was previously given to
     * {@link #startManagingCursor}, stop the activity's management of that
     * cursor.
     *
     * <p><strong>Warning:</strong> After calling this method on a cursor from a managed query,
     * the system <em>will not</em> automatically close the cursor and you must call
     * {@link Cursor#close()}.</p>
     *
     * @param c The Cursor that was being managed.
     *
     * @see #startManagingCursor
     *
     * @deprecated Use the new {@link android.content.CursorLoader} class with
     * {@link LoaderManager} instead; this is also
     * available on older platforms through the Android compatibility package.
     */
    //@Deprecated
    CARAPI StopManagingCursor(
        /* [in] */ ICursor* c);

    /**
     * @deprecated As of {@link android.os.Build.VERSION_CODES#GINGERBREAD}
     * this is a no-op.
     * @hide
     */
    //@Deprecated
    CARAPI SetPersistent(
        /* [in] */ Boolean isPersistent);

    /**
     * Finds a view that was identified by the id attribute from the XML that
     * was processed in {@link #onCreate}.
     *
     * @return The view if found or null otherwise.
     */
    CARAPI FindViewById(
        /* [in] */ Int32 id,
        /* [out] */ IView** view);

    CARAPI_(AutoPtr<IView>) FindViewById(
        /* [in] */ Int32 id);

    /**
     * Set a {@link android.widget.Toolbar Toolbar} to act as the {@link ActionBar} for this
     * Activity window.
     *
     * <p>When set to a non-null value the {@link #getActionBar()} method will return
     * an {@link ActionBar} object that can be used to control the given toolbar as if it were
     * a traditional window decor action bar. The toolbar's menu will be populated with the
     * Activity's options menu and the navigation button will be wired through the standard
     * {@link android.R.id#home home} menu select action.</p>
     *
     * <p>In order to use a Toolbar within the Activity's window content the application
     * must not request the window feature {@link Window#FEATURE_ACTION_BAR FEATURE_ACTION_BAR}.</p>
     *
     * @param toolbar Toolbar to set as the Activity's action bar
     */
    CARAPI SetActionBar(
        /* [in] */ IToolbar* actionbar);

    /**
     * Retrieve a reference to this activity's ActionBar.
     *
     * @return The Activity's ActionBar, or null if it does not have one.
     */
    CARAPI GetActionBar(
        /* [out] */ IActionBar** actionbar);

    CARAPI_(AutoPtr<IActionBar>) GetActionBar();

    /**
     * Set the activity content from a layout resource.  The resource will be
     * inflated, adding all top-level views to the activity.
     *
     * @param layoutResID Resource ID to be inflated.
     */
    CARAPI SetContentView(
        /* [in] */ Int32 layoutResID);

    /**
     * Set the activity content to an explicit view.  This view is placed
     * directly into the activity's view hierarchy.  It can itself be a complex
     * view hierarhcy.
     *
     * @param view The desired content to display.
     */
    CARAPI SetContentView(
        /* [in] */ IView* view);

    /**
     * Set the activity content to an explicit view.  This view is placed
     * directly into the activity's view hierarchy.  It can itself be a complex
     * view hierarhcy.
     *
     * @param view The desired content to display.
     * @param params Layout parameters for the view.
     */
    CARAPI SetContentView(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams* params);

    CARAPI AddContentView(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams* params);

    /**
     * Retrieve the {@link TransitionManager} responsible for default transitions in this window.
     * Requires {@link Window#FEATURE_CONTENT_TRANSITIONS}.
     *
     * <p>This method will return non-null after content has been initialized (e.g. by using
     * {@link #setContentView}) if {@link Window#FEATURE_CONTENT_TRANSITIONS} has been granted.</p>
     *
     * @return This window's content TransitionManager or null if none is set.
     */
    CARAPI GetContentTransitionManager(
        /* [out] */ ITransitionManager** tm);

    /**
     * Set the {@link TransitionManager} to use for default transitions in this window.
     * Requires {@link Window#FEATURE_CONTENT_TRANSITIONS}.
     *
     * @param tm The TransitionManager to use for scene changes.
     */
    CARAPI SetContentTransitionManager(
        /* [in] */ ITransitionManager* tm);

    /**
     * Retrieve the {@link Scene} representing this window's current content.
     * Requires {@link Window#FEATURE_CONTENT_TRANSITIONS}.
     *
     * <p>This method will return null if the current content is not represented by a Scene.</p>
     *
     * @return Current Scene being shown or null
     */
    CARAPI GetContentScene(
        /* [out] */ IScene** scene);

    CARAPI SetFinishOnTouchOutside(
        /* [in] */ Boolean finish);

    /**
     * Select the default key handling for this activity.  This controls what
     * will happen to key events that are not otherwise handled.  The default
     * mode ({@link #DEFAULT_KEYS_DISABLE}) will simply drop them on the
     * floor. Other modes allow you to launch the dialer
     * ({@link #DEFAULT_KEYS_DIALER}), execute a shortcut in your options
     * menu without requiring the menu key be held down
     * ({@link #DEFAULT_KEYS_SHORTCUT}), or launch a search ({@link #DEFAULT_KEYS_SEARCH_LOCAL}
     * and {@link #DEFAULT_KEYS_SEARCH_GLOBAL}).
     *
     * <p>Note that the mode selected here does not impact the default
     * handling of system keys, such as the "back" and "menu" keys, and your
     * activity and its views always get a first chance to receive and handle
     * all application keys.
     *
     * @param mode The desired default key mode constant.
     *
     * @see #DEFAULT_KEYS_DISABLE
     * @see #DEFAULT_KEYS_DIALER
     * @see #DEFAULT_KEYS_SHORTCUT
     * @see #DEFAULT_KEYS_SEARCH_LOCAL
     * @see #DEFAULT_KEYS_SEARCH_GLOBAL
     * @see #onKeyDown
     */
    CARAPI SetDefaultKeyMode(
        /* [in] */  Int32 mode);

    /**
     * Called when a key was pressed down and not handled by any of the views
     * inside of the activity. So, for example, key presses while the cursor
     * is inside a TextView will not trigger the event (unless it is a navigation
     * to another object) because TextView handles its own key presses.
     *
     * <p>If the focused view didn't want this event, this method is called.
     *
     * <p>The default implementation takes care of {@link KeyEvent#KEYCODE_BACK}
     * by calling {@link #onBackPressed()}, though the behavior varies based
     * on the application compatibility mode: for
     * {@link android.os.Build.VERSION_CODES#ECLAIR} or later applications,
     * it will set up the dispatch to call {@link #onKeyUp} where the action
     * will be performed; for earlier applications, it will perform the
     * action immediately in on-down, as those versions of the platform
     * behaved.
     *
     * <p>Other additional default key handling may be performed
     * if configured with {@link #setDefaultKeyMode}.
     *
     * @return Return <code>true</code> to prevent this event from being propagated
     * further, or <code>false</code> to indicate that you have not handled
     * this event and it should continue to be propagated.
     * @see #onKeyUp
     * @see android.view.KeyEvent
     */
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Default implementation of {@link KeyEvent.Callback#onKeyLongPress(int, KeyEvent)
     * KeyEvent.Callback.onKeyLongPress()}: always returns false (doesn't handle
     * the event).
     */
    CARAPI OnKeyLongPress(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Called when a key was released and not handled by any of the views
     * inside of the activity. So, for example, key presses while the cursor
     * is inside a TextView will not trigger the event (unless it is a navigation
     * to another object) because TextView handles its own key presses.
     *
     * <p>The default implementation handles KEYCODE_BACK to stop the activity
     * and go back.
     *
     * @return Return <code>true</code> to prevent this event from being propagated
     * further, or <code>false</code> to indicate that you have not handled
     * this event and it should continue to be propagated.
     * @see #onKeyDown
     * @see KeyEvent
     */
    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Default implementation of {@link KeyEvent.Callback#onKeyMultiple(int, int, KeyEvent)
     * KeyEvent.Callback.onKeyMultiple()}: always returns false (doesn't handle
     * the event).
     */
    CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Called when the activity has detected the user's press of the back
     * key.  The default implementation simply finishes the current activity,
     * but you can override this to do whatever you want.
     */
    CARAPI OnBackPressed();

    /**
     * Called when a key shortcut event is not handled by any of the views in the Activity.
     * Override this method to implement global key shortcuts for the Activity.
     * Key shortcuts can also be implemented by setting the
     * {@link MenuItem#setShortcut(char, char) shortcut} property of menu items.
     *
     * @param keyCode The value in event.getKeyCode().
     * @param event Description of the key event.
     * @return True if the key shortcut was handled.
     */
    CARAPI OnKeyShortcut(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* value);

    /**
     * Called when a touch screen event was not handled by any of the views
     * under it.  This is most useful to process touch events that happen
     * outside of your window bounds, where there is no view to receive it.
     *
     * @param event The touch screen event being processed.
     *
     * @return Return true if you have consumed the event, false if you haven't.
     * The default implementation always returns false.
     */
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    /**
     * Called when the trackball was moved and not handled by any of the
     * views inside of the activity.  So, for example, if the trackball moves
     * while focus is on a button, you will receive a call here because
     * buttons do not normally do anything with trackball events.  The call
     * here happens <em>before</em> trackball movements are converted to
     * DPAD key events, which then get sent back to the view hierarchy, and
     * will be processed at the point for things like focus navigation.
     *
     * @param event The trackball event being processed.
     *
     * @return Return true if you have consumed the event, false if you haven't.
     * The default implementation always returns false.
     */
    CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    /**
     * Called when a generic motion event was not handled by any of the
     * views inside of the activity.
     * <p>
     * Generic motion events describe joystick movements, mouse hovers, track pad
     * touches, scroll wheel movements and other input events.  The
     * {@link MotionEvent#getSource() source} of the motion event specifies
     * the class of input that was received.  Implementations of this method
     * must examine the bits in the source before processing the event.
     * The following code example shows how this is done.
     * </p><p>
     * Generic motion events with source class
     * {@link android.view.InputDevice#SOURCE_CLASS_POINTER}
     * are delivered to the view under the pointer.  All other generic motion events are
     * delivered to the focused view.
     * </p><p>
     * See {@link View#onGenericMotionEvent(MotionEvent)} for an example of how to
     * handle this event.
     * </p>
     *
     * @param event The generic motion event being processed.
     *
     * @return Return true if you have consumed the event, false if you haven't.
     * The default implementation always returns false.
     */
    CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Called as part of the activity lifecycle when an activity is about to go
     * into the background as the result of user choice.  For example, when the
     * user presses the Home key, {@link #onUserLeaveHint} will be called, but
     * when an incoming phone call causes the in-call Activity to be automatically
     * brought to the foreground, {@link #onUserLeaveHint} will not be called on
     * the activity being interrupted.  In cases when it is invoked, this method
     * is called right before the activity's {@link #onPause} callback.
     *
     * <p>This callback and {@link #onUserInteraction} are intended to help
     * activities manage status bar notifications intelligently; specifically,
     * for helping activities determine the proper time to cancel a notfication.
     *
     * @see #onUserInteraction()
     */
    virtual CARAPI OnUserLeaveHint();

    /**
     * Called whenever a key, touch, or trackball event is dispatched to the
     * activity.  Implement this method if you wish to know that the user has
     * interacted with the device in some way while your activity is running.
     * This callback and {@link #onUserLeaveHint} are intended to help
     * activities manage status bar notifications intelligently; specifically,
     * for helping activities determine the proper time to cancel a notfication.
     *
     * <p>All calls to your activity's {@link #onUserLeaveHint} callback will
     * be accompanied by calls to {@link #onUserInteraction}.  This
     * ensures that your activity will be told of relevant user activity such
     * as pulling down the notification pane and touching an item there.
     *
     * <p>Note that this callback will be invoked for the touch down action
     * that begins a touch gesture, but may not be invoked for the touch-moved
     * and touch-up actions that follow.
     *
     * @see #onUserLeaveHint()
     */
    virtual CARAPI OnUserInteraction();

    CARAPI OnWindowAttributesChanged(
        /* [in] */ IWindowManagerLayoutParams* attrs);

    CARAPI OnContentChanged();

    /**
     * Called when the current {@link Window} of the activity gains or loses
     * focus.  This is the best indicator of whether this activity is visible
     * to the user.  The default implementation clears the key tracking
     * state, so should always be called.
     *
     * <p>Note that this provides information about global focus state, which
     * is managed independently of activity lifecycles.  As such, while focus
     * changes will generally have some relation to lifecycle changes (an
     * activity that is stopped will not generally get window focus), you
     * should not rely on any particular order between the callbacks here and
     * those in the other lifecycle methods such as {@link #onResume}.
     *
     * <p>As a general rule, however, a resumed activity will have window
     * focus...  unless it has displayed other dialogs or popups that take
     * input focus, in which case the activity itself will not have focus
     * when the other windows have it.  Likewise, the system may display
     * system-level windows (such as the status bar notification panel or
     * a system alert) which will temporarily take window input focus without
     * pausing the foreground activity.
     *
     * @param hasFocus Whether the window of this activity has focus.
     *
     * @see #hasWindowFocus()
     * @see #onResume
     * @see View#onWindowFocusChanged(boolean)
     */
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

    /**
     * Called when the main window associated with the activity has been
     * attached to the window manager.
     * See {@link View#onAttachedToWindow() View.onAttachedToWindow()}
     * for more information.
     * @see View#onAttachedToWindow
     */
    CARAPI OnAttachedToWindow();

    /**
     * Called when the main window associated with the activity has been
     * detached from the window manager.
     * See {@link View#onDetachedFromWindow() View.onDetachedFromWindow()}
     * for more information.
     * @see View#onDetachedFromWindow
     */
    CARAPI OnDetachedFromWindow();

    /**
     * Returns true if this activity's <em>main</em> window currently has window focus.
     * Note that this is not the same as the view itself having focus.
     *
     * @return True if this activity's main window currently has window focus.
     *
     * @see #onWindowAttributesChanged(android.view.WindowManager.LayoutParams)
     */
    CARAPI HasWindowFocus(
        /* [out] */ Boolean* hasFocus);

    /**
     * Called when the main window associated with the activity has been dismissed.
     * @hide
     */
    CARAPI OnWindowDismissed();

    /**
     * Called to process key events.  You can override this to intercept all
     * key events before they are dispatched to the window.  Be sure to call
     * this implementation for key events that should be handled normally.
     *
     * @param event The key event.
     *
     * @return boolean Return true if this event was consumed.
     */
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* isConsumed);

    /**
     * Called to process a key shortcut event.
     * You can override this to intercept all key shortcut events before they are
     * dispatched to the window.  Be sure to call this implementation for key shortcut
     * events that should be handled normally.
     *
     * @param event The key shortcut event.
     * @return True if this event was consumed.
     */
    CARAPI DispatchKeyShortcutEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Called to process touch screen events.  You can override this to
     * intercept all touch screen events before they are dispatched to the
     * window.  Be sure to call this implementation for touch screen events
     * that should be handled normally.
     *
     * @param ev The touch screen event.
     *
     * @return boolean Return true if this event was consumed.
     */
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* isConsumed);

    /**
     * Called to process trackball events.  You can override this to
     * intercept all trackball events before they are dispatched to the
     * window.  Be sure to call this implementation for trackball events
     * that should be handled normally.
     *
     * @param ev The trackball event.
     *
     * @return boolean Return true if this event was consumed.
     */
    CARAPI DispatchTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* isConsumed);

    /**
     * Called to process generic motion events.  You can override this to
     * intercept all generic motion events before they are dispatched to the
     * window.  Be sure to call this implementation for generic motion events
     * that should be handled normally.
     *
     * @param ev The generic motion event.
     *
     * @return boolean Return true if this event was consumed.
     */
    CARAPI DispatchGenericMotionEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* isCompleted);

    /**
     * Default implementation of
     * {@link android.view.Window.Callback#onCreatePanelView}
     * for activities. This
     * simply returns null so that all panel sub-windows will have the default
     * menu behavior.
     */
    CARAPI OnCreatePanelView(
        /* [in] */ Int32 featureId,
        /* [out] */ IView** view);

    /**
     * Default implementation of
     * {@link android.view.Window.Callback#onCreatePanelMenu}
     * for activities.  This calls through to the new
     * {@link #onCreateOptionsMenu} method for the
     * {@link android.view.Window#FEATURE_OPTIONS_PANEL} panel,
     * so that subclasses of Activity don't need to deal with feature codes.
     */
    CARAPI OnCreatePanelMenu(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* allowToShow);

    /**
     * Default implementation of
     * {@link android.view.Window.Callback#onPreparePanel}
     * for activities.  This
     * calls through to the new {@link #onPrepareOptionsMenu} method for the
     * {@link android.view.Window#FEATURE_OPTIONS_PANEL}
     * panel, so that subclasses of
     * Activity don't need to deal with feature codes.
     */
    CARAPI OnPreparePanel(
        /* [in] */ Int32 featureId,
        /* [in] */ IView* view,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* allowToShow);

    /**
     * {@inheritDoc}
     *
     * @return The default implementation returns true.
     */
    CARAPI OnMenuOpened(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* allowToOpen);

    /**
     * Default implementation of
     * {@link android.view.Window.Callback#onMenuItemSelected}
     * for activities.  This calls through to the new
     * {@link #onOptionsItemSelected} method for the
     * {@link android.view.Window#FEATURE_OPTIONS_PANEL}
     * panel, so that subclasses of
     * Activity don't need to deal with feature codes.
     */
    CARAPI OnMenuItemSelected(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* toFinish);

    /**
     * Default implementation of
     * {@link android.view.Window.Callback#onPanelClosed(int, Menu)} for
     * activities. This calls through to {@link #onOptionsMenuClosed(Menu)}
     * method for the {@link android.view.Window#FEATURE_OPTIONS_PANEL} panel,
     * so that subclasses of Activity don't need to deal with feature codes.
     * For context menus ({@link Window#FEATURE_CONTEXT_MENU}), the
     * {@link #onContextMenuClosed(Menu)} will be called.
     */
    CARAPI OnPanelClosed(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu);

    /**
     * Declare that the options menu has changed, so should be recreated.
     * The {@link #onCreateOptionsMenu(Menu)} method will be called the next
     * time it needs to be displayed.
     */
    CARAPI InvalidateOptionsMenu();

    /**
     * Initialize the contents of the Activity's standard options menu.  You
     * should place your menu items in to <var>menu</var>.
     *
     * <p>This is only called once, the first time the options menu is
     * displayed.  To update the menu every time it is displayed, see
     * {@link #onPrepareOptionsMenu}.
     *
     * <p>The default implementation populates the menu with standard system
     * menu items.  These are placed in the {@link Menu#CATEGORY_SYSTEM} group so that
     * they will be correctly ordered with application-defined menu items.
     * Deriving classes should always call through to the base implementation.
     *
     * <p>You can safely hold on to <var>menu</var> (and any items created
     * from it), making modifications to it as desired, until the next
     * time onCreateOptionsMenu() is called.
     *
     * <p>When you add items to the menu, you can implement the Activity's
     * {@link #onOptionsItemSelected} method to handle them there.
     *
     * @param menu The options menu in which you place your items.
     *
     * @return You must return true for the menu to be displayed;
     *         if you return false it will not be shown.
     *
     * @see #onPrepareOptionsMenu
     * @see #onOptionsItemSelected
     */
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* allowToShow);

    /**
     * Prepare the Screen's standard options menu to be displayed.  This is
     * called right before the menu is shown, every time it is shown.  You can
     * use this method to efficiently enable/disable items or otherwise
     * dynamically modify the contents.
     *
     * <p>The default implementation updates the system menu items based on the
     * activity's state.  Deriving classes should always call through to the
     * base class implementation.
     *
     * @param menu The options menu as last shown or first initialized by
     *             onCreateOptionsMenu().
     *
     * @return You must return true for the menu to be displayed;
     *         if you return false it will not be shown.
     *
     * @see #onCreateOptionsMenu
     */
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* res);

    /**
     * This hook is called whenever an item in your options menu is selected.
     * The default implementation simply returns false to have the normal
     * processing happen (calling the item's Runnable or sending a message to
     * its Handler as appropriate).  You can use this method for any items
     * for which you would like to do processing without those other
     * facilities.
     *
     * <p>Derived classes should call through to the base class for it to
     * perform the default menu handling.</p>
     *
     * @param item The menu item that was selected.
     *
     * @return boolean Return false to allow normal menu processing to
     *         proceed, true to consume it here.
     *
     * @see #onCreateOptionsMenu
     */
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* res);

    /**
     * This method is called whenever the user chooses to navigate Up within your application's
     * activity hierarchy from the action bar.
     *
     * <p>If the attribute {@link android.R.attr#parentActivityName parentActivityName}
     * was specified in the manifest for this activity or an activity-alias to it,
     * default Up navigation will be handled automatically. If any activity
     * along the parent chain requires extra Intent arguments, the Activity subclass
     * should override the method {@link #onPrepareNavigateUpTaskStack(TaskStackBuilder)}
     * to supply those arguments.</p>
     *
     * <p>See <a href="{@docRoot}guide/topics/fundamentals/tasks-and-back-stack.html">Tasks and Back Stack</a>
     * from the developer guide and <a href="{@docRoot}design/patterns/navigation.html">Navigation</a>
     * from the design guide for more information about navigating within your app.</p>
     *
     * <p>See the {@link TaskStackBuilder} class and the Activity methods
     * {@link #getParentActivityIntent()}, {@link #shouldUpRecreateTask(Intent)}, and
     * {@link #navigateUpTo(Intent)} for help implementing custom Up navigation.
     * The AppNavigation sample application in the Android SDK is also available for reference.</p>
     *
     * @return true if Up navigation completed successfully and this Activity was finished,
     *         false otherwise.
     */
    CARAPI OnNavigateUp(
        /* [out] */ Boolean* result);

    /**
     * This is called when a child activity of this one attempts to navigate up.
     * The default implementation simply calls onNavigateUp() on this activity (the parent).
     *
     * @param child The activity making the call.
     */
    CARAPI OnNavigateUpFromChild(
        /* [in] */ IActivity* child,
        /* [out] */ Boolean* result);

    /**
     * Define the synthetic task stack that will be generated during Up navigation from
     * a different task.
     *
     * <p>The default implementation of this method adds the parent chain of this activity
     * as specified in the manifest to the supplied {@link TaskStackBuilder}. Applications
     * may choose to override this method to construct the desired task stack in a different
     * way.</p>
     *
     * <p>This method will be invoked by the default implementation of {@link #onNavigateUp()}
     * if {@link #shouldUpRecreateTask(Intent)} returns true when supplied with the intent
     * returned by {@link #getParentActivityIntent()}.</p>
     *
     * <p>Applications that wish to supply extra Intent parameters to the parent stack defined
     * by the manifest should override {@link #onPrepareNavigateUpTaskStack(TaskStackBuilder)}.</p>
     *
     * @param builder An empty TaskStackBuilder - the application should add intents representing
     *                the desired task stack
     */
    CARAPI OnCreateNavigateUpTaskStack(
        /* [in] */ ITaskStackBuilder* builder);

    /**
     * Prepare the synthetic task stack that will be generated during Up navigation
     * from a different task.
     *
     * <p>This method receives the {@link TaskStackBuilder} with the constructed series of
     * Intents as generated by {@link #onCreateNavigateUpTaskStack(TaskStackBuilder)}.
     * If any extra data should be added to these intents before launching the new task,
     * the application should override this method and add that data here.</p>
     *
     * @param builder A TaskStackBuilder that has been populated with Intents by
     *                onCreateNavigateUpTaskStack.
     */
    CARAPI OnPrepareNavigateUpTaskStack(
        /* [in] */ ITaskStackBuilder* builder);

    /**
     * This hook is called whenever the options menu is being closed (either by the user canceling
     * the menu with the back/menu button, or when an item is selected).
     *
     * @param menu The options menu as last shown or first initialized by
     *             onCreateOptionsMenu().
     */
    CARAPI OnOptionsMenuClosed(
        /* [in] */ IMenu* menu);

    /**
     * Programmatically opens the options menu. If the options menu is already
     * open, this method does nothing.
     */
    CARAPI OpenOptionsMenu();

    /**
     * Progammatically closes the options menu. If the options menu is already
     * closed, this method does nothing.
     */
    CARAPI CloseOptionsMenu();

    /**
     * Called when a context menu for the {@code view} is about to be shown.
     * Unlike {@link #onCreateOptionsMenu(Menu)}, this will be called every
     * time the context menu is about to be shown and should be populated for
     * the view (or item inside the view for {@link AdapterView} subclasses,
     * this can be found in the {@code menuInfo})).
     * <p>
     * Use {@link #onContextItemSelected(android.view.MenuItem)} to know when an
     * item has been selected.
     * <p>
     * It is not safe to hold onto the context menu after this method returns.
     */
    CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuInfo* menuInfo);

    /**
     * Registers a context menu to be shown for the given view (multiple views
     * can show the context menu). This method will set the
     * {@link OnCreateContextMenuListener} on the view to this activity, so
     * {@link #onCreateContextMenu(ContextMenu, View, ContextMenuInfo)} will be
     * called when it is time to show the context menu.
     *
     * @see #unregisterForContextMenu(View)
     * @param view The view that should show a context menu.
     */
    CARAPI RegisterForContextMenu(
        /* [in] */ IView* view);

    /**
     * Prevents a context menu to be shown for the given view. This method will remove the
     * {@link OnCreateContextMenuListener} on the view.
     *
     * @see #registerForContextMenu(View)
     * @param view The view that should stop showing a context menu.
     */
    CARAPI UnregisterForContextMenu(
        /* [in] */ IView* view);

    /**
     * Programmatically opens the context menu for a particular {@code view}.
     * The {@code view} should have been added via
     * {@link #registerForContextMenu(View)}.
     *
     * @param view The view to show the context menu for.
     */
    CARAPI OpenContextMenu(
        /* [in] */ IView* view);

    /**
     * Programmatically closes the most recently opened context menu, if showing.
     */
    CARAPI CloseContextMenu();

    /**
     * This hook is called whenever an item in a context menu is selected. The
     * default implementation simply returns false to have the normal processing
     * happen (calling the item's Runnable or sending a message to its Handler
     * as appropriate). You can use this method for any items for which you
     * would like to do processing without those other facilities.
     * <p>
     * Use {@link MenuItem#getMenuInfo()} to get extra information set by the
     * View that added this menu item.
     * <p>
     * Derived classes should call through to the base class for it to perform
     * the default menu handling.
     *
     * @param item The context menu item that was selected.
     * @return boolean Return false to allow normal context menu processing to
     *         proceed, true to consume it here.
     */
    CARAPI OnContextItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* res);

    /**
     * This hook is called whenever the context menu is being closed (either by
     * the user canceling the menu with the back/menu button, or when an item is
     * selected).
     *
     * @param menu The context menu that is being closed.
     */
    CARAPI OnContextMenuClosed(
        /* [in] */ IMenu* menu);

    /**
     * Simple version of {@link #showDialog(int, Bundle)} that does not
     * take any arguments.  Simply calls {@link #showDialog(int, Bundle)}
     * with null arguments.
     *
     * @deprecated Use the new {@link DialogFragment} class with
     * {@link FragmentManager} instead; this is also
     * available on older platforms through the Android compatibility package.
     */
    //@Deprecated
    CARAPI ShowDialog(
        /* [in] */ Int32 id);

    /**
     * Show a dialog managed by this activity.  A call to {@link #onCreateDialog(int, Bundle)}
     * will be made with the same id the first time this is called for a given
     * id.  From thereafter, the dialog will be automatically saved and restored.
     *
     * <em>If you are targeting {@link android.os.Build.VERSION_CODES#HONEYCOMB}
     * or later, consider instead using a {@link DialogFragment} instead.</em>
     *
     * <p>Each time a dialog is shown, {@link #onPrepareDialog(int, Dialog, Bundle)} will
     * be made to provide an opportunity to do any timely preparation.
     *
     * @param id The id of the managed dialog.
     * @param args Arguments to pass through to the dialog.  These will be saved
     * and restored for you.  Note that if the dialog is already created,
     * {@link #onCreateDialog(int, Bundle)} will not be called with the new
     * arguments but {@link #onPrepareDialog(int, Dialog, Bundle)} will be.
     * If you need to rebuild the dialog, call {@link #removeDialog(int)} first.
     * @return Returns true if the Dialog was created; false is returned if
     * it is not created because {@link #onCreateDialog(int, Bundle)} returns false.
     *
     * @see Dialog
     * @see #onCreateDialog(int, Bundle)
     * @see #onPrepareDialog(int, Dialog, Bundle)
     * @see #dismissDialog(int)
     * @see #removeDialog(int)
     *
     * @deprecated Use the new {@link DialogFragment} class with
     * {@link FragmentManager} instead; this is also
     * available on older platforms through the Android compatibility package.
     */
    //@Deprecated
    CARAPI ShowDialog(
        /* [in] */ Int32 id,
        /* [in] */ IBundle* args,
        /* [out] */ Boolean* res);

    /**
     * Dismiss a dialog that was previously shown via {@link #showDialog(int)}.
     *
     * @param id The id of the managed dialog.
     *
     * @throws IllegalArgumentException if the id was not previously shown via
     *   {@link #showDialog(int)}.
     *
     * @see #onCreateDialog(int, Bundle)
     * @see #onPrepareDialog(int, Dialog, Bundle)
     * @see #showDialog(int)
     * @see #removeDialog(int)
     *
     * @deprecated Use the new {@link DialogFragment} class with
     * {@link FragmentManager} instead; this is also
     * available on older platforms through the Android compatibility package.
     */
    //@Deprecated
    CARAPI DismissDialog(
        /* [in] */ Int32 id);

    /**
     * Removes any internal references to a dialog managed by this Activity.
     * If the dialog is showing, it will dismiss it as part of the clean up.
     *
     * <p>This can be useful if you know that you will never show a dialog again and
     * want to avoid the overhead of saving and restoring it in the future.
     *
     * <p>As of {@link android.os.Build.VERSION_CODES#GINGERBREAD}, this function
     * will not throw an exception if you try to remove an ID that does not
     * currently have an associated dialog.</p>
     *
     * @param id The id of the managed dialog.
     *
     * @see #onCreateDialog(int, Bundle)
     * @see #onPrepareDialog(int, Dialog, Bundle)
     * @see #showDialog(int)
     * @see #dismissDialog(int)
     *
     * @deprecated Use the new {@link DialogFragment} class with
     * {@link FragmentManager} instead; this is also
     * available on older platforms through the Android compatibility package.
     */
    //@Deprecated
    CARAPI RemoveDialog(
        /* [in] */ Int32 id);

    /**
     * This hook is called when the user signals the desire to start a search.
     *
     * <p>You can use this function as a simple way to launch the search UI, in response to a
     * menu item, search button, or other widgets within your activity. Unless overidden,
     * calling this function is the same as calling
     * {@link #startSearch startSearch(null, false, null, false)}, which launches
     * search for the current activity as specified in its manifest, see {@link SearchManager}.
     *
     * <p>You can override this function to force global search, e.g. in response to a dedicated
     * search key, or to block search entirely (by simply returning false).
     *
     * @return Returns {@code true} if search launched, and {@code false} if activity blocks it.
     *         The default implementation always returns {@code true}.
     *
     * @see android.app.SearchManager
     */
    CARAPI OnSearchRequested(
        /* [out] */ Boolean* isLaunched);

    /**
     * This hook is called to launch the search UI.
     *
     * <p>It is typically called from onSearchRequested(), either directly from
     * Activity.onSearchRequested() or from an overridden version in any given
     * Activity.  If your goal is simply to activate search, it is preferred to call
     * onSearchRequested(), which may have been overriden elsewhere in your Activity.  If your goal
     * is to inject specific data such as context data, it is preferred to <i>override</i>
     * onSearchRequested(), so that any callers to it will benefit from the override.
     *
     * @param initialQuery Any non-null non-empty string will be inserted as
     * pre-entered text in the search query box.
     * @param selectInitialQuery If true, the intial query will be preselected, which means that
     * any further typing will replace it.  This is useful for cases where an entire pre-formed
     * query is being inserted.  If false, the selection point will be placed at the end of the
     * inserted query.  This is useful when the inserted query is text that the user entered,
     * and the user would expect to be able to keep typing.  <i>This parameter is only meaningful
     * if initialQuery is a non-empty string.</i>
     * @param appSearchData An application can insert application-specific
     * context here, in order to improve quality or specificity of its own
     * searches.  This data will be returned with SEARCH intent(s).  Null if
     * no extra data is required.
     * @param globalSearch If false, this will only launch the search that has been specifically
     * defined by the application (which is usually defined as a local search).  If no default
     * search is defined in the current application or activity, global search will be launched.
     * If true, this will always launch a platform-global (e.g. web-based) search instead.
     *
     * @see android.app.SearchManager
     * @see #onSearchRequested
     */
    CARAPI StartSearch(
        /* [in] */ const String& initialQuery,
        /* [in] */ Boolean selectInitialQuery,
        /* [in] */ IBundle* appSearchData,
        /* [in] */ Boolean globalSearch);

    /**
     * Similar to {@link #startSearch}, but actually fires off the search query after invoking
     * the search dialog.  Made available for testing purposes.
     *
     * @param query The query to trigger.  If empty, the request will be ignored.
     * @param appSearchData An application can insert application-specific
     * context here, in order to improve quality or specificity of its own
     * searches.  This data will be returned with SEARCH intent(s).  Null if
     * no extra data is required.
     */
    CARAPI TriggerSearch(
        /* [in] */ const String& query,
        /* [in] */ IBundle* appSearchData);

    /**
     * Request that key events come to this activity. Use this if your
     * activity has no views with focus, but the activity still wants
     * a chance to process key events.
     *
     * @see android.view.Window#takeKeyEvents
     */
    CARAPI TakeKeyEvents(
        /* [in] */ Boolean get);

    /**
     * Enable extended window features.  This is a convenience for calling
     * {@link android.view.Window#requestFeature getWindow().requestFeature()}.
     *
     * @param featureId The desired feature as defined in
     *                  {@link android.view.Window}.
     * @return Returns true if the requested feature is supported and now
     *         enabled.
     *
     * @see android.view.Window#requestFeature
     */
    CARAPI RequestWindowFeature(
        /* [in] */ Int32 featureId,
        /* [out] */ Boolean* result);

    /**
     * Convenience for calling
     * {@link android.view.Window#setFeatureDrawableResource}.
     */
    CARAPI SetFeatureDrawableResource(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 resId);

    /**
     * Convenience for calling
     * {@link android.view.Window#setFeatureDrawableUri}.
     */
    CARAPI SetFeatureDrawableUri(
        /* [in] */ Int32 featureId,
        /* [in] */ IUri* uri);

    /**
     * Convenience for calling
     * {@link android.view.Window#setFeatureDrawable(int, Drawable)}.
     */
    CARAPI SetFeatureDrawable(
        /* [in] */ Int32 featureId,
        /* [in] */ IDrawable* drawable);

    /**
     * Convenience for calling
     * {@link android.view.Window#setFeatureDrawableAlpha}.
     */
    CARAPI SetFeatureDrawableAlpha(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 alpha);

    /**
     * Convenience for calling
     * {@link android.view.Window#getLayoutInflater}.
     */
    CARAPI GetLayoutInflater(
        /* [out] */ ILayoutInflater** inflater);

    /**
     * Returns a {@link MenuInflater} with this context.
     */
    CARAPI GetMenuInflater(
        /* [out] */ IMenuInflater** menuInflater);

    /**
     * Same as calling {@link #startActivityForResult(Intent, int, Bundle)}
     * with no options.
     *
     * @param intent The intent to start.
     * @param requestCode If >= 0, this code will be returned in
     *                    onActivityResult() when the activity exits.
     *
     * @throws android.content.ActivityNotFoundException
     *
     * @see #startActivity
     */
    CARAPI StartActivityForResult(
        /* [in] */ IIntent *pIntent,
        /* [in] */ Int32 requestCode);

    /**
     * Launch an activity for which you would like a result when it finished.
     * When this activity exits, your
     * onActivityResult() method will be called with the given requestCode.
     * Using a negative requestCode is the same as calling
     * {@link #startActivity} (the activity is not launched as a sub-activity).
     *
     * <p>Note that this method should only be used with Intent protocols
     * that are defined to return a result.  In other protocols (such as
     * {@link Intent#ACTION_MAIN} or {@link Intent#ACTION_VIEW}), you may
     * not get the result when you expect.  For example, if the activity you
     * are launching uses the singleTask launch mode, it will not run in your
     * task and thus you will immediately receive a cancel result.
     *
     * <p>As a special case, if you call startActivityForResult() with a requestCode
     * >= 0 during the initial onCreate(Bundle savedInstanceState)/onResume() of your
     * activity, then your window will not be displayed until a result is
     * returned back from the started activity.  This is to avoid visible
     * flickering when redirecting to another activity.
     *
     * <p>This method throws {@link android.content.ActivityNotFoundException}
     * if there was no Activity found to run the given Intent.
     *
     * @param intent The intent to start.
     * @param requestCode If >= 0, this code will be returned in
     *                    onActivityResult() when the activity exits.
     * @param options Additional options for how the Activity should be started.
     * See {@link android.content.Context#startActivity(Intent, Bundle)
     * Context.startActivity(Intent, Bundle)} for more details.
     *
     * @throws android.content.ActivityNotFoundException
     *
     * @see #startActivity
     */
    CARAPI StartActivityForResult(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options);

    /**
     * Same as calling {@link #startIntentSenderForResult(IntentSender, int,
     * Intent, int, int, int, Bundle)} with no options.
     *
     * @param intent The IntentSender to launch.
     * @param requestCode If >= 0, this code will be returned in
     *                    onActivityResult() when the activity exits.
     * @param fillInIntent If non-null, this will be provided as the
     * intent parameter to {@link IntentSender#sendIntent}.
     * @param flagsMask Intent flags in the original IntentSender that you
     * would like to change.
     * @param flagsValues Desired values for any bits set in
     * <var>flagsMask</var>
     * @param extraFlags Always set to 0.
     */
    CARAPI StartIntentSenderForResult(
        /* [in] */ IIntentSender* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ Int32 extraFlags);

    /**
     * Like {@link #startActivityForResult(Intent, int)}, but allowing you
     * to use a IntentSender to describe the activity to be started.  If
     * the IntentSender is for an activity, that activity will be started
     * as if you had called the regular {@link #startActivityForResult(Intent, int)}
     * here; otherwise, its associated action will be executed (such as
     * sending a broadcast) as if you had called
     * {@link IntentSender#sendIntent IntentSender.sendIntent} on it.
     *
     * @param intent The IntentSender to launch.
     * @param requestCode If >= 0, this code will be returned in
     *                    onActivityResult() when the activity exits.
     * @param fillInIntent If non-null, this will be provided as the
     * intent parameter to {@link IntentSender#sendIntent}.
     * @param flagsMask Intent flags in the original IntentSender that you
     * would like to change.
     * @param flagsValues Desired values for any bits set in
     * <var>flagsMask</var>
     * @param extraFlags Always set to 0.
     * @param options Additional options for how the Activity should be started.
     * See {@link android.content.Context#startActivity(Intent, Bundle)
     * Context.startActivity(Intent, Bundle)} for more details.  If options
     * have also been supplied by the IntentSender, options given here will
     * override any that conflict with those given by the IntentSender.
     */
    CARAPI StartIntentSenderForResult(
        /* [in] */ IIntentSender* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ Int32 extraFlags,
        /* [in] */ IBundle* options);

    /**
     * @hide Implement to provide correct calling token.
     */
    CARAPI StartActivityForResultAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IUserHandle* user);

    /**
     * @hide Implement to provide correct calling token.
     */
    CARAPI StartActivityForResultAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options,
        /* [in] */ IUserHandle* user);

    /**
     * Same as calling {@link #startActivityIfNeeded(Intent, int, Bundle)}
     * with no options.
     *
     * @param intent The intent to start.
     * @param requestCode If >= 0, this code will be returned in
     *         onActivityResult() when the activity exits, as described in
     *         {@link #startActivityForResult}.
     *
     * @return If a new activity was launched then true is returned; otherwise
     *         false is returned and you must handle the Intent yourself.
     *
     * @see #startActivity
     * @see #startActivityForResult
     */
    CARAPI StartActivityIfNeeded(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [out] */ Boolean* started);

    /**
     * A special variation to launch an activity only if a new activity
     * instance is needed to handle the given Intent.  In other words, this is
     * just like {@link #startActivityForResult(Intent, int)} except: if you are
     * using the {@link Intent#FLAG_ACTIVITY_SINGLE_TOP} flag, or
     * singleTask or singleTop
     * {@link android.R.styleable#AndroidManifestActivity_launchMode launchMode},
     * and the activity
     * that handles <var>intent</var> is the same as your currently running
     * activity, then a new instance is not needed.  In this case, instead of
     * the normal behavior of calling {@link #onNewIntent} this function will
     * return and you can handle the Intent yourself.
     *
     * <p>This function can only be called from a top-level activity; if it is
     * called from a child activity, a runtime exception will be thrown.
     *
     * @param intent The intent to start.
     * @param requestCode If >= 0, this code will be returned in
     *         onActivityResult() when the activity exits, as described in
     *         {@link #startActivityForResult}.
     * @param options Additional options for how the Activity should be started.
     * See {@link android.content.Context#startActivity(Intent, Bundle)
     * Context.startActivity(Intent, Bundle)} for more details.
     *
     * @return If a new activity was launched then true is returned; otherwise
     *         false is returned and you must handle the Intent yourself.
     *
     * @see #startActivity
     * @see #startActivityForResult
     */
    CARAPI  StartActivityIfNeeded(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options,
        /* [out] */ Boolean* started);

    /**
     * Same as calling {@link #startNextMatchingActivity(Intent, Bundle)} with
     * no options.
     *
     * @param intent The intent to dispatch to the next activity.  For
     * correct behavior, this must be the same as the Intent that started
     * your own activity; the only changes you can make are to the extras
     * inside of it.
     *
     * @return Returns a boolean indicating whether there was another Activity
     * to start: true if there was a next activity to start, false if there
     * wasn't.  In general, if true is returned you will then want to call
     * finish() on yourself.
     */
    CARAPI StartNextMatchingActivity(
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* started);

    /**
     * Special version of starting an activity, for use when you are replacing
     * other activity components.  You can use this to hand the Intent off
     * to the next Activity that can handle it.  You typically call this in
     * {@link #onCreate} with the Intent returned by {@link #getIntent}.
     *
     * @param intent The intent to dispatch to the next activity.  For
     * correct behavior, this must be the same as the Intent that started
     * your own activity; the only changes you can make are to the extras
     * inside of it.
     * @param options Additional options for how the Activity should be started.
     * See {@link android.content.Context#startActivity(Intent, Bundle)
     * Context.startActivity(Intent, Bundle)} for more details.
     *
     * @return Returns a boolean indicating whether there was another Activity
     * to start: true if there was a next activity to start, false if there
     * wasn't.  In general, if true is returned you will then want to call
     * finish() on yourself.
     */
    CARAPI StartNextMatchingActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options,
        /* [out] */ Boolean* started);

    /**
     * Same as calling {@link #startActivityFromChild(Activity, Intent, int, Bundle)}
     * with no options.
     *
     * @param child The activity making the call.
     * @param intent The intent to start.
     * @param requestCode Reply request code.  < 0 if reply is not requested.
     *
     * @throws android.content.ActivityNotFoundException
     *
     * @see #startActivity
     * @see #startActivityForResult
     */
    CARAPI StartActivityFromChild(
        /* [in] */ IActivity* child,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode);

    /**
     * This is called when a child activity of this one calls its
     * {@link #startActivity} or {@link #startActivityForResult} method.
     *
     * <p>This method throws {@link android.content.ActivityNotFoundException}
     * if there was no Activity found to run the given Intent.
     *
     * @param child The activity making the call.
     * @param intent The intent to start.
     * @param requestCode Reply request code.  < 0 if reply is not requested.
     * @param options Additional options for how the Activity should be started.
     * See {@link android.content.Context#startActivity(Intent, Bundle)
     * Context.startActivity(Intent, Bundle)} for more details.
     *
     * @throws android.content.ActivityNotFoundException
     *
     * @see #startActivity
     * @see #startActivityForResult
     */
    CARAPI StartActivityFromChild(
        /* [in] */ IActivity* child,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options);

    /**
     * Same as calling {@link #startActivityFromFragment(Fragment, Intent, int, Bundle)}
     * with no options.
     *
     * @param fragment The fragment making the call.
     * @param intent The intent to start.
     * @param requestCode Reply request code.  < 0 if reply is not requested.
     *
     * @throws android.content.ActivityNotFoundException
     *
     * @see Fragment#startActivity
     * @see Fragment#startActivityForResult
     */
    CARAPI StartActivityFromFragment(
        /* [in] */ IFragment* fragment,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode);

    /**
     * This is called when a Fragment in this activity calls its
     * {@link Fragment#startActivity} or {@link Fragment#startActivityForResult}
     * method.
     *
     * <p>This method throws {@link android.content.ActivityNotFoundException}
     * if there was no Activity found to run the given Intent.
     *
     * @param fragment The fragment making the call.
     * @param intent The intent to start.
     * @param requestCode Reply request code.  < 0 if reply is not requested.
     * @param options Additional options for how the Activity should be started.
     * See {@link android.content.Context#startActivity(Intent, Bundle)
     * Context.startActivity(Intent, Bundle)} for more details.
     *
     * @throws android.content.ActivityNotFoundException
     *
     * @see Fragment#startActivity
     * @see Fragment#startActivityForResult
     */
    CARAPI StartActivityFromFragment(
        /* [in] */ IFragment* fragment,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options);

    /**
     * Same as calling {@link #startIntentSenderFromChild(Activity, IntentSender,
     * int, Intent, int, int, int, Bundle)} with no options.
     */
    CARAPI StartIntentSenderFromChild(
        /* [in] */ IActivity* child,
        /* [in] */ IIntentSender* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ Int32 extraFlags);

    /**
     * Like {@link #startActivityFromChild(Activity, Intent, int)}, but
     * taking a IntentSender; see
     * {@link #startIntentSenderForResult(IntentSender, int, Intent, int, int, int)}
     * for more information.
     */
    CARAPI StartIntentSenderFromChild(
        /* [in] */ IActivity* child,
        /* [in] */ IIntentSender* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ Int32 extraFlags,
        /* [in] */ IBundle* options);

    /**
     * Call immediately after one of the flavors of {@link #startActivity(Intent)}
     * or {@link #finish} to specify an explicit transition animation to
     * perform next.
     *
     * <p>As of {@link android.os.Build.VERSION_CODES#JELLY_BEAN} an alternative
     * to using this with starting activities is to supply the desired animation
     * information through a {@link ActivityOptions} bundle to
     * {@link #startActivity(Intent, Bundle) or a related function.  This allows
     * you to specify a custom animation even when starting an activity from
     * outside the context of the current top activity.
     *
     * @param enterAnim A resource ID of the animation resource to use for
     * the incoming activity.  Use 0 for no animation.
     * @param exitAnim A resource ID of the animation resource to use for
     * the outgoing activity.  Use 0 for no animation.
     */
    CARAPI OverridePendingTransition(
        /* [in] */ Int32 enterAnim,
        /* [in] */ Int32 exitAnim);

    /**
     * Call this to set the result that your activity will return to its
     * caller.
     *
     * @param resultCode The result code to propagate back to the originating
     *                   activity, often RESULT_CANCELED or RESULT_OK
     *
     * @see #RESULT_CANCELED
     * @see #RESULT_OK
     * @see #RESULT_FIRST_USER
     * @see #setResult(int, Intent)
     */
    CARAPI SetResult(
        /* [in] */ Int32 resultCode);

    /**
     * Call this to set the result that your activity will return to its
     * caller.
     *
     * <p>As of {@link android.os.Build.VERSION_CODES#GINGERBREAD}, the Intent
     * you supply here can have {@link Intent#FLAG_GRANT_READ_URI_PERMISSION
     * Intent.FLAG_GRANT_READ_URI_PERMISSION} and/or {@link Intent#FLAG_GRANT_WRITE_URI_PERMISSION
     * Intent.FLAG_GRANT_WRITE_URI_PERMISSION} set.  This will grant the
     * Activity receiving the result access to the specific URIs in the Intent.
     * Access will remain until the Activity has finished (it will remain across the hosting
     * process being killed and other temporary destruction) and will be added
     * to any existing set of URI permissions it already holds.
     *
     * @param resultCode The result code to propagate back to the originating
     *                   activity, often RESULT_CANCELED or RESULT_OK
     * @param data The data to propagate back to the originating activity.
     *
     * @see #RESULT_CANCELED
     * @see #RESULT_OK
     * @see #RESULT_FIRST_USER
     * @see #setResult(int)
     */
    CARAPI SetResult(
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    /**
     * Return the name of the package that invoked this activity.  This is who
     * the data in {@link #setResult setResult()} will be sent to.  You can
     * use this information to validate that the recipient is allowed to
     * receive the data.
     *
     * <p>Note: if the calling activity is not expecting a result (that is it
     * did not use the {@link #startActivityForResult}
     * form that includes a request code), then the calling package will be
     * null.
     *
     * @return The package of the activity that will receive your
     *         reply, or null if none.
     */
    CARAPI GetCallingPackage(
        /* [out] */ String* package);

    /**
     * Return the name of the activity that invoked this activity.  This is
     * who the data in {@link #setResult setResult()} will be sent to.  You
     * can use this information to validate that the recipient is allowed to
     * receive the data.
     *
     * <p>Note: if the calling activity is not expecting a result (that is it
     * did not use the {@link #startActivityForResult}
     * form that includes a request code), then the calling package will be
     * null.
     *
     * @return String The full name of the activity that will receive your
     *         reply, or null if none.
     */
    CARAPI GetCallingActivity(
        /* [out] */ IComponentName** name);

    /**
     * Control whether this activity's main window is visible.  This is intended
     * only for the special case of an activity that is not going to show a
     * UI itself, but can't just finish prior to onResume() because it needs
     * to wait for a service binding or such.  Setting this to false allows
     * you to prevent your UI from being shown during that time.
     *
     * <p>The default value for this is taken from the
     * {@link android.R.attr#windowNoDisplay} attribute of the activity's theme.
     */
    CARAPI SetVisible(
        /* [in] */ Boolean visible);

    CARAPI MakeVisible();

    /**
     * Check to see whether this activity is in the process of finishing,
     * either because you called {@link #finish} on it or someone else
     * has requested that it finished.  This is often used in
     * {@link #onPause} to determine whether the activity is simply pausing or
     * completely finishing.
     *
     * @return If the activity is finishing, returns true; else returns false.
     *
     * @see #finish
     */
    CARAPI IsFinishing(
        /* [out] */ Boolean* finished);

    /**
     * Returns true if the final {@link #onDestroy()} call has been made
     * on the Activity, so this instance is now dead.
     */
    CARAPI IsDestroyed(
        /* [out] */ Boolean* destroyed);

    /**
     * Check to see whether this activity is in the process of being destroyed in order to be
     * recreated with a new configuration. This is often used in
     * {@link #onStop} to determine whether the state needs to be cleaned up or will be passed
     * on to the next instance of the activity via {@link #onRetainNonConfigurationInstance()}.
     *
     * @return If the activity is being torn down in order to be recreated with a new configuration,
     * returns true; else returns false.
     */
    CARAPI IsChangingConfigurations(
        /* [out] */ Boolean* changed);

    /**
     * Cause this Activity to be recreated with a new instance.  This results
     * in essentially the same flow as when the Activity is created due to
     * a configuration change -- the current instance will go through its
     * lifecycle to {@link #onDestroy} and a new instance then created after it.
     */
    CARAPI Recreate();

    /**
     * Call this when your activity is done and should be closed.  The
     * ActivityResult is propagated back to whoever launched you via
     * onActivityResult().
     */
    CARAPI Finish();

    /**
     * Finish this activity as well as all activities immediately below it
     * in the current task that have the same affinity.  This is typically
     * used when an application can be launched on to another task (such as
     * from an ACTION_VIEW of a content type it understands) and the user
     * has used the up navigation to switch out of the current task and in
     * to its own task.  In this case, if the user has navigated down into
     * any other activities of the second application, all of those should
     * be removed from the original task as part of the task switch.
     *
     * <p>Note that this finish does <em>not</em> allow you to deliver results
     * to the previous activity, and an exception will be thrown if you are trying
     * to do so.</p>
     */
    CARAPI FinishAffinity();

    /**
     * This is called when a child activity of this one calls its
     * {@link #finish} method.  The default implementation simply calls
     * finish() on this activity (the parent), finishing the entire group.
     *
     * @param child The activity making the call.
     *
     * @see #finish
     */
    CARAPI FinishFromChild(
        /* [in] */ IActivity* child);

    /**
     * Reverses the Activity Scene entry Transition and triggers the calling Activity
     * to reverse its exit Transition. When the exit Transition completes,
     * {@link #finish()} is called. If no entry Transition was used, finish() is called
     * immediately and the Activity exit Transition is run.
     * @see android.app.ActivityOptions#makeSceneTransitionAnimation(Activity, android.util.Pair[])
     */
    CARAPI FinishAfterTransition();

    /**
     * Force finish another activity that you had previously started with
     * {@link #startActivityForResult}.
     *
     * @param requestCode The request code of the activity that you had
     *                    given to startActivityForResult().  If there are multiple
     *                    activities started with this request code, they
     *                    will all be finished.
     */
    CARAPI FinishActivity(
        /* [in] */ Int32 requestCode);

    /**
     * This is called when a child activity of this one calls its
     * finishActivity().
     *
     * @param child The activity making the call.
     * @param requestCode Request code that had been used to start the
     *                    activity.
     */
    CARAPI FinishActivityFromChild(
        /* [in] */ IActivity* child,
        /* [in] */ Int32 requestCode);

    /**
     * Call this when your activity is done and should be closed and the task should be completely
     * removed as a part of finishing the Activity.
     */
    CARAPI FinishAndRemoveTask();

    /**
     * Ask that the local app instance of this activity be released to free up its memory.
     * This is asking for the activity to be destroyed, but does <b>not</b> finish the activity --
     * a new instance of the activity will later be re-created if needed due to the user
     * navigating back to it.
     *
     * @return Returns true if the activity was in a state that it has started the process
     * of destroying its current instance; returns false if for any reason this could not
     * be done: it is currently visible to the user, it is already being destroyed, it is
     * being finished, it hasn't yet saved its state, etc.
     */
    CARAPI ReleaseInstance(
        /* [out] */ Boolean* result);

    /**
     * Called when an activity you launched with an activity transition exposes this
     * Activity through a returning activity transition, giving you the resultCode
     * and any additional data from it. This method will only be called if the activity
     * set a result code other than {@link #RESULT_CANCELED} and it supports activity
     * transitions with {@link Window#FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * <p>The purpose of this function is to let the called Activity send a hint about
     * its state so that this underlying Activity can prepare to be exposed. A call to
     * this method does not guarantee that the called Activity has or will be exiting soon.
     * It only indicates that it will expose this Activity's Window and it has
     * some data to pass to prepare it.</p>
     *
     * @param resultCode The integer result code returned by the child activity
     *                   through its setResult().
     * @param data An Intent, which can return result data to the caller
     *               (various data can be attached to Intent "extras").
     */
    CARAPI OnActivityReenter(
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    /**
     * Called when an activity you launched exits, giving you the requestCode
     * you started it with, the resultCode it returned, and any additional
     * data from it.  The <var>resultCode</var> will be
     * {@link #RESULT_CANCELED} if the activity explicitly returned that,
     * didn't return any result, or crashed during its operation.
     *
     * <p>You will receive this call immediately before onResume() when your
     * activity is re-starting.
     *
     * @param requestCode The integer request code originally supplied to
     *                    startActivityForResult(), allowing you to identify who this
     *                    result came from.
     * @param resultCode The integer result code returned by the child activity
     *                   through its setResult().
     * @param data An Intent, which can return result data to the caller
     *               (various data can be attached to Intent "extras").
     *
     * @see #startActivityForResult
     * @see #createPendingResult
     * @see #setResult(int)
     */
    virtual CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    /**
     * Create a new PendingIntent object which you can hand to others
     * for them to use to send result data back to your
     * {@link #onActivityResult} callback.  The created object will be either
     * one-shot (becoming invalid after a result is sent back) or multiple
     * (allowing any number of results to be sent through it).
     *
     * @param requestCode Private request code for the sender that will be
     * associated with the result data when it is returned.  The sender can not
     * modify this value, allowing you to identify incoming results.
     * @param data Default data to supply in the result, which may be modified
     * by the sender.
     * @param flags May be {@link PendingIntent#FLAG_ONE_SHOT PendingIntent.FLAG_ONE_SHOT},
     * {@link PendingIntent#FLAG_NO_CREATE PendingIntent.FLAG_NO_CREATE},
     * {@link PendingIntent#FLAG_CANCEL_CURRENT PendingIntent.FLAG_CANCEL_CURRENT},
     * {@link PendingIntent#FLAG_UPDATE_CURRENT PendingIntent.FLAG_UPDATE_CURRENT},
     * or any of the flags as supported by
     * {@link Intent#fillIn Intent.fillIn()} to control which unspecified parts
     * of the intent that can be supplied when the actual send happens.
     *
     * @return Returns an existing or new PendingIntent matching the given
     * parameters.  May return null only if
     * {@link PendingIntent#FLAG_NO_CREATE PendingIntent.FLAG_NO_CREATE} has been
     * supplied.
     *
     * @see PendingIntent
     */
    CARAPI CreatePendingResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ IIntent* data,
        /* [in] */ Int32 flags,
        /* [out] */ IPendingIntent** pIntent);

    /**
     * Change the desired orientation of this activity.  If the activity
     * is currently in the foreground or otherwise impacting the screen
     * orientation, the screen will immediately be changed (possibly causing
     * the activity to be restarted). Otherwise, this will be used the next
     * time the activity is visible.
     *
     * @param requestedOrientation An orientation constant as used in
     * {@link ActivityInfo#screenOrientation ActivityInfo.screenOrientation}.
     */
    CARAPI SetRequestedOrientation(
        /* [int] */ Int32 requestedOrientation);

    /**
     * Return the current requested orientation of the activity.  This will
     * either be the orientation requested in its component's manifest, or
     * the last requested orientation given to
     * {@link #setRequestedOrientation(int)}.
     *
     * @return Returns an orientation constant as used in
     * {@link ActivityInfo#screenOrientation ActivityInfo.screenOrientation}.
     */
    CARAPI GetRequestedOrientation(
        /* [out] */ Int32* orientation);

    /**
     * Return the identifier of the task this activity is in.  This identifier
     * will remain the same for the lifetime of the activity.
     *
     * @return Task identifier, an opaque integer.
     */
    CARAPI GetTaskId(
        /* [out] */ Int32* id);

    /**
     * Return whether this activity is the root of a task.  The root is the
     * first activity in a task.
     *
     * @return True if this is the root activity, else false.
     */
    CARAPI IsTaskRoot(
        /* [out] */ Boolean* isRoot);

    /**
     * Move the task containing this activity to the back of the activity
     * stack.  The activity's order within the task is unchanged.
     *
     * @param nonRoot If false then this only works if the activity is the root
     *                of a task; if true it will work for any activity in
     *                a task.
     *
     * @return If the task was moved (or it was already at the
     *         back) true is returned, else false.
     */
    CARAPI MoveTaskToBack(
        /* [in] */ Boolean nonRoot,
        /* [out] */ Boolean* succeed);

    /**
     * Returns class name for this activity with the package prefix removed.
     * This is the default name used to read and write settings.
     *
     * @return The local class name.
     */
    CARAPI GetLocalClassName(
        /* [out] */ String* className);

    /**
     * Returns complete component name of this activity.
     *
     * @return Returns the complete component name for this activity
     */
    virtual CARAPI GetComponentName(
        /* [out] */ IComponentName** name);

    /**
     * Retrieve a {@link SharedPreferences} object for accessing preferences
     * that are private to this activity.  This simply calls the underlying
     * {@link #getSharedPreferences(String, int)} method by passing in this activity's
     * class name as the preferences name.
     *
     * @param mode Operating mode.  Use {@link #MODE_PRIVATE} for the default
     *             operation, {@link #MODE_WORLD_READABLE} and
     *             {@link #MODE_WORLD_WRITEABLE} to control permissions.
     *
     * @return Returns the single SharedPreferences instance that can be used
     *         to retrieve and modify the preference values.
     */
    CARAPI GetPreferences(
        /* [in] */ Int32 mode,
        /* [out] */ ISharedPreferences** perferences);

    /**
     * Change the title associated with this activity.  If this is a
     * top-level activity, the title for its window will change.  If it
     * is an embedded activity, the parent can do whatever it wants
     * with it.
     */
    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    /**
     * Change the title associated with this activity.  If this is a
     * top-level activity, the title for its window will change.  If it
     * is an embedded activity, the parent can do whatever it wants
     * with it.
     */
    CARAPI SetTitle(
        /* [in] */ Int32 titleId);

    CARAPI SetTitleColor(
        /* [in] */ Int32 textColor);

    CARAPI GetTitle(
        /* [out] */ ICharSequence** title);

    CARAPI GetTitleColor(
        /* [out] */ Int32* textColor);

    /*protected*/
    virtual CARAPI OnTitleChanged(
        /* [in] */ ICharSequence* title,
        /* [in] */ Int32 color);

    virtual CARAPI OnChildTitleChanged(
        /* [in] */ IActivity* childActivity,
        /* [in] */ ICharSequence* title);

    /**
     * Sets information describing the task with this activity for presentation inside the Recents
     * System UI. When {@link ActivityManager#getRecentTasks} is called, the activities of each task
     * are traversed in order from the topmost activity to the bottommost. The traversal continues
     * for each property until a suitable value is found. For each task the taskDescription will be
     * returned in {@link android.app.ActivityManager.TaskDescription}.
     *
     * @see ActivityManager#getRecentTasks
     * @see android.app.ActivityManager.TaskDescription
     *
     * @param taskDescription The TaskDescription properties that describe the task with this activity
     */
    CARAPI SetTaskDescription(
        /* [in] */ IActivityManagerTaskDescription* taskDescription);

    /**
     * Sets the visibility of the progress bar in the title.
     * <p>
     * In order for the progress bar to be shown, the feature must be requested
     * via {@link #requestWindowFeature(int)}.
     *
     * @param visible Whether to show the progress bars in the title.
     */
    CARAPI SetProgressBarVisibility(
        /* [in] */ Boolean visible);

    /**
      * Sets the visibility of the indeterminate progress bar in the title.
      * <p>
      * In order for the progress bar to be shown, the feature must be requested
      * via {@link #requestWindowFeature(int)}.
      *
      * @param visible Whether to show the progress bars in the title.
      */
    CARAPI SetProgressBarIndeterminateVisibility(
        /* [in] */ Boolean visible);

    /**
     * Sets whether the horizontal progress bar in the title should be indeterminate (the circular
     * is always indeterminate).
     * <p>
     * In order for the progress bar to be shown, the feature must be requested
     * via {@link #requestWindowFeature(int)}.
     *
     * @param indeterminate Whether the horizontal progress bar should be indeterminate.
     */
    CARAPI SetProgressBarIndeterminate(
        /* [in] */ Boolean visible);

    /**
     * Sets the progress for the progress bars in the title.
     * <p>
     * In order for the progress bar to be shown, the feature must be requested
     * via {@link #requestWindowFeature(int)}.
     *
     * @param progress The progress for the progress bar. Valid ranges are from
     *            0 to 10000 (both inclusive). If 10000 is given, the progress
     *            bar will be completely filled and will fade out.
     */
    CARAPI SetProgress(
        /* [in] */ Int32 progress);

    /**
     * Sets the secondary progress for the progress bar in the title. This
     * progress is drawn between the primary progress (set via
     * {@link #setProgress(int)} and the background. It can be ideal for media
     * scenarios such as showing the buffering progress while the default
     * progress shows the play progress.
     * <p>
     * In order for the progress bar to be shown, the feature must be requested
     * via {@link #requestWindowFeature(int)}.
     *
     * @param secondaryProgress The secondary progress for the progress bar. Valid ranges are from
     *            0 to 10000 (both inclusive).
     */
    CARAPI SetSecondaryProgress(
        /* [in] */ Int32 secondaryProgress);

    /**
    * Suggests an audio stream whose volume should be changed by the hardware
    * volume controls.
    * <p>
    * The suggested audio stream will be tied to the window of this Activity.
    * If the Activity is switched, the stream set here is no longer the
    * suggested stream. The client does not need to save and restore the old
    * suggested stream value in onPause and onResume.
    *
    * @param streamType The type of the audio stream whose volume should be
    *        changed by the hardware volume controls. It is not guaranteed that
    *        the hardware volume controls will always change this stream's
    *        volume (for example, if a call is in progress, its stream's volume
    *        may be changed instead). To reset back to the default, use
    *        {@link AudioManager#USE_DEFAULT_STREAM_TYPE}.
    */
    CARAPI SetVolumeControlStream(
        /* [in] */ Int32 streamType);

    /**
     * Gets the suggested audio stream whose volume should be changed by the
     * harwdare volume controls.
     *
     * @return The suggested audio stream type whose volume should be changed by
     *         the hardware volume controls.
     * @see #setVolumeControlStream(int)
     */
    CARAPI GetVolumeControlStream(
        /* [out] */ Int32* type);

    /**
     * Sets a {@link MediaController} to send media keys and volume changes to.
     * <p>
     * The controller will be tied to the window of this Activity. Media key and
     * volume events which are received while the Activity is in the foreground
     * will be forwarded to the controller and used to invoke transport controls
     * or adjust the volume. This may be used instead of or in addition to
     * {@link #setVolumeControlStream} to affect a specific session instead of a
     * specific stream.
     * <p>
     * It is not guaranteed that the hardware volume controls will always change
     * this session's volume (for example, if a call is in progress, its
     * stream's volume may be changed instead). To reset back to the default use
     * null as the controller.
     *
     * @param controller The controller for the session which should receive
     *            media keys and volume changes.
     */
    CARAPI SetMediaController(
        /* [in] */ IMediaController* controller);

    /**
     * Gets the controller which should be receiving media key and volume events
     * while this activity is in the foreground.
     *
     * @return The controller which should receive events.
     * @see #setMediaController(android.media.session.MediaController)
     */
    CARAPI GetMediaController(
        /* [out] */ IMediaController** mc);

    /**
     * Runs the specified action on the UI thread. If the current thread is the UI
     * thread, then the action is executed immediately. If the current thread is
     * not the UI thread, the action is posted to the event queue of the UI thread.
     *
     * @param action the action to run on the UI thread
     */
    CARAPI RunOnUiThread(
        /* [in] */ IRunnable* action);

    /**
     * Standard implementation of
     * {@link android.view.LayoutInflater.Factory#onCreateView} used when
     * inflating with the LayoutInflater returned by {@link #getSystemService}.
     * This implementation does nothing and is for
     * pre-{@link android.os.Build.VERSION_CODES#HONEYCOMB} apps.  Newer apps
     * should use {@link #onCreateView(View, String, Context, AttributeSet)}.
     *
     * @see android.view.LayoutInflater#createView
     * @see android.view.Window#getLayoutInflater
     */
    CARAPI OnCreateView(
        /* [in] */ const String& name,
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IView** view);

    /**
     * Standard implementation of
     * {@link android.view.LayoutInflater.Factory2#onCreateView(View, String, Context, AttributeSet)}
     * used when inflating with the LayoutInflater returned by {@link #getSystemService}.
     * This implementation handles <fragment> tags to embed fragments inside
     * of the activity.
     *
     * @see android.view.LayoutInflater#createView
     * @see android.view.Window#getLayoutInflater
     */
    CARAPI OnCreateView(
        /* [in] */ IView* parent,
        /* [in] */ const String& name,
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IView** view);

    /**
     * Print the Activity's state into the given stream.  This gets invoked if
     * you run "adb shell dumpsys activity &lt;activity_component_name&gt;".
     *
     * @param prefix Desired prefix to prepend at each line of output.
     * @param fd The raw file descriptor that the dump is being sent to.
     * @param writer The PrintWriter to which you should dump your state.  This will be
     * closed for you after you return.
     * @param args additional arguments to the dump request.
     */
    CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

    /**
     * Bit indicating that this activity is "immersive" and should not be
     * interrupted by notifications if possible.
     *
     * This value is initially set by the manifest property
     * <code>android:immersive</code> but may be changed at runtime by
     * {@link #setImmersive}.
     *
     * @see android.content.pm.ActivityInfo#FLAG_IMMERSIVE
     * @hide
     */
    CARAPI IsImmersive(
        /* [out] */ Boolean* isImmersive);

    /**
     * Convert a translucent themed Activity {@link android.R.attr#windowIsTranslucent} to a
     * fullscreen opaque Activity.
     * <p>
     * Call this whenever the background of a translucent Activity has changed to become opaque.
     * Doing so will allow the {@link android.view.Surface} of the Activity behind to be released.
     * <p>
     * This call has no effect on non-translucent activities or on activities with the
     * {@link android.R.attr#windowIsFloating} attribute.
     *
     * @see #convertToTranslucent(android.app.Activity.TranslucentConversionListener,
     * ActivityOptions)
     * @see TranslucentConversionListener
     *
     * @hide
     */
    CARAPI ConvertFromTranslucent();

    /**
     * Convert a translucent themed Activity {@link android.R.attr#windowIsTranslucent} back from
     * opaque to translucent following a call to {@link #convertFromTranslucent()}.
     * <p>
     * Calling this allows the Activity behind this one to be seen again. Once all such Activities
     * have been redrawn {@link TranslucentConversionListener#onTranslucentConversionComplete} will
     * be called indicating that it is safe to make this activity translucent again. Until
     * {@link TranslucentConversionListener#onTranslucentConversionComplete} is called the image
     * behind the frontmost Activity will be indeterminate.
     * <p>
     * This call has no effect on non-translucent activities or on activities with the
     * {@link android.R.attr#windowIsFloating} attribute.
     *
     * @param callback the method to call when all visible Activities behind this one have been
     * drawn and it is safe to make this Activity translucent again.
     * @param options activity options delivered to the activity below this one. The options
     * are retrieved using {@link #getActivityOptions}.
     * @return <code>true</code> if Window was opaque and will become translucent or
     * <code>false</code> if window was translucent and no change needed to be made.
     *
     * @see #convertFromTranslucent()
     * @see TranslucentConversionListener
     *
     * @hide
     */
    CARAPI ConvertToTranslucent(
        /* [in] */ ITranslucentConversionListener* callback,
        /* [in] */ IActivityOptions* options,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI OnTranslucentConversionComplete(
        /* [in] */ Boolean drawComplete);

    /** @hide */
    CARAPI OnNewActivityOptions(
        /* [in] */ IActivityOptions* options);

    /**
     * Retrieve the ActivityOptions passed in from the launching activity or passed back
     * from an activity launched by this activity in its call to {@link
     * #convertToTranslucent(TranslucentConversionListener, ActivityOptions)}
     *
     * @return The ActivityOptions passed to {@link #convertToTranslucent}.
     * @hide
     */
    CARAPI GetActivityOptions(
        /* [out] */ IActivityOptions** options);

    /**
     * Activities that want to remain visible behind a translucent activity above them must call
     * this method anytime between the start of {@link #onResume()} and the return from
     * {@link #onPause()}. If this call is successful then the activity will remain visible after
     * {@link #onPause()} is called, and is allowed to continue playing media in the background.
     *
     * <p>The actions of this call are reset each time that this activity is brought to the
     * front. That is, every time {@link #onResume()} is called the activity will be assumed
     * to not have requested visible behind. Therefore, if you want this activity to continue to
     * be visible in the background you must call this method again.
     *
     * <p>Only fullscreen opaque activities may make this call. I.e. this call is a nop
     * for dialog and translucent activities.
     *
     * <p>Under all circumstances, the activity must stop playing and release resources prior to or
     * within a call to {@link #onVisibleBehindCanceled()} or if this call returns false.
     *
     * <p>False will be returned any time this method is called between the return of onPause and
     *      the next call to onResume.
     *
     * @param visible true to notify the system that the activity wishes to be visible behind other
     *                translucent activities, false to indicate otherwise. Resources must be
     *                released when passing false to this method.
     * @return the resulting visibiity state. If true the activity will remain visible beyond
     *      {@link #onPause()} if the next activity is translucent or not fullscreen. If false
     *      then the activity may not count on being visible behind other translucent activities,
     *      and must stop any media playback and release resources.
     *      Returning false may occur in lieu of a call to {@link #onVisibleBehindCanceled()} so
     *      the return value must be checked.
     *
     * @see #onVisibleBehindCanceled()
     * @see #onBackgroundVisibleBehindChanged(boolean)
     */
    CARAPI RequestVisibleBehind(
        /* [in] */ Boolean visible,
        /* [out] */ Boolean* result);

    /**
     * Called when a translucent activity over this activity is becoming opaque or another
     * activity is being launched. Activities that override this method must call
     * <code>super.onVisibleBehindCanceled()</code> or a SuperNotCalledException will be thrown.
     *
     * <p>When this method is called the activity has 500 msec to release any resources it may be
     * using while visible in the background.
     * If the activity has not returned from this method in 500 msec the system will destroy
     * the activity and kill the process in order to recover the resources for another
     * process. Otherwise {@link #onStop()} will be called following return.
     *
     * @see #requestVisibleBehind(boolean)
     * @see #onBackgroundVisibleBehindChanged(boolean)
     */
    CARAPI OnVisibleBehindCanceled();

    /**
     * Translucent activities may call this to determine if there is an activity below them that
     * is currently set to be visible in the background.
     *
     * @return true if an activity below is set to visible according to the most recent call to
     * {@link #requestVisibleBehind(boolean)}, false otherwise.
     *
     * @see #requestVisibleBehind(boolean)
     * @see #onVisibleBehindCanceled()
     * @see #onBackgroundVisibleBehindChanged(boolean)
     * @hide
     */
    CARAPI IsBackgroundVisibleBehind(
        /* [out] */ Boolean* result);

    /**
     * The topmost foreground activity will receive this call when the background visibility state
     * of the activity below it changes.
     *
     * This call may be a consequence of {@link #requestVisibleBehind(boolean)} or might be
     * due to a background activity finishing itself.
     *
     * @param visible true if a background activity is visible, false otherwise.
     *
     * @see #requestVisibleBehind(boolean)
     * @see #onVisibleBehindCanceled()
     * @hide
     */
    CARAPI OnBackgroundVisibleBehindChanged(
        /* [in] */ Boolean visible);

    /**
     * Activities cannot draw during the period that their windows are animating in. In order
     * to know when it is safe to begin drawing they can override this method which will be
     * called when the entering animation has completed.
     */
    CARAPI OnEnterAnimationComplete();

    /**
     * Adjust the current immersive mode setting.
     *
     * Note that changing this value will have no effect on the activity's
     * {@link android.content.pm.ActivityInfo} structure; that is, if
     * <code>android:immersive</code> is set to <code>true</code>
     * in the application's manifest entry for this activity, the {@link
     * android.content.pm.ActivityInfo#flags ActivityInfo.flags} member will
     * always have its {@link android.content.pm.ActivityInfo#FLAG_IMMERSIVE
     * FLAG_IMMERSIVE} bit set.
     *
     * @see #isImmersive
     * @see android.content.pm.ActivityInfo#FLAG_IMMERSIVE
     * @hide
     */
    CARAPI SetImmersive(
        /* [in] */ Boolean i);

    /**
     * Start an action mode.
     *
     * @param callback Callback that will manage lifecycle events for this context mode
     * @return The ContextMode that was started, or null if it was canceled
     *
     * @see ActionMode
     */
    CARAPI StartActionMode(
        /* [in] */ IActionModeCallback* cb,
        /* [out] */ IActionMode** mode);

    /**
     * Give the Activity a chance to control the UI for an action mode requested
     * by the system.
     *
     * <p>Note: If you are looking for a notification callback that an action mode
     * has been started for this activity, see {@link #onActionModeStarted(ActionMode)}.</p>
     *
     * @param callback The callback that should control the new action mode
     * @return The new action mode, or <code>null</code> if the activity does not want to
     *         provide special handling for this action mode. (It will be handled by the system.)
     */
    virtual CARAPI OnWindowStartingActionMode(
        /* [in] */ IActionModeCallback* cb,
        /* [out] */ IActionMode** mode);

    /**
     * Notifies the Activity that an action mode has been started.
     * Activity subclasses overriding this method should call the superclass implementation.
     *
     * @param mode The new action mode.
     */
    virtual CARAPI OnActionModeStarted(
        /* [in] */ IActionMode* mode);

    /**
     * Notifies the activity that an action mode has finished.
     * Activity subclasses overriding this method should call the superclass implementation.
     *
     * @param mode The action mode that just finished.
     */
    virtual CARAPI OnActionModeFinished(
        /* [in] */ IActionMode* mode);

    /**
     * Returns true if the app should recreate the task when navigating 'up' from this activity
     * by using targetIntent.
     *
     * <p>If this method returns false the app can trivially call
     * {@link #navigateUpTo(Intent)} using the same parameters to correctly perform
     * up navigation. If this method returns false, the app should synthesize a new task stack
     * by using {@link TaskStackBuilder} or another similar mechanism to perform up navigation.</p>
     *
     * @param targetIntent An intent representing the target destination for up navigation
     * @return true if navigating up should recreate a new task stack, false if the same task
     *         should be used for the destination
     */
    CARAPI ShouldUpRecreateTask(
        /* [in] */ IIntent* targetIntent,
        /* [out] */ Boolean* res);

    /**
     * Navigate from this activity to the activity specified by upIntent, finishing this activity
     * in the process. If the activity indicated by upIntent already exists in the task's history,
     * this activity and all others before the indicated activity in the history stack will be
     * finished.
     *
     * <p>If the indicated activity does not appear in the history stack, this will finish
     * each activity in this task until the root activity of the task is reached, resulting in
     * an "in-app home" behavior. This can be useful in apps with a complex navigation hierarchy
     * when an activity may be reached by a path not passing through a canonical parent
     * activity.</p>
     *
     * <p>This method should be used when performing up navigation from within the same task
     * as the destination. If up navigation should cross tasks in some cases, see
     * {@link #shouldUpRecreateTask(Intent)}.</p>
     *
     * @param upIntent An intent representing the target destination for up navigation
     *
     * @return true if up navigation successfully reached the activity indicated by upIntent and
     *         upIntent was delivered to it. false if an instance of the indicated activity could
     *         not be found and this activity was simply finished normally.
     */
    CARAPI NavigateUpTo(
        /* [in] */ IIntent* upIntent,
        /* [out] */ Boolean* success);

    /**
     * This is called when a child activity of this one calls its
     * {@link #navigateUpTo} method.  The default implementation simply calls
     * navigateUpTo(upIntent) on this activity (the parent).
     *
     * @param child The activity making the call.
     * @param upIntent An intent representing the target destination for up navigation
     *
     * @return true if up navigation successfully reached the activity indicated by upIntent and
     *         upIntent was delivered to it. false if an instance of the indicated activity could
     *         not be found and this activity was simply finished normally.
     */
    CARAPI NavigateUpToFromChild(
        /* [in] */ IActivity* child,
        /* [in] */ IIntent* upIntent,
        /* [out] */ Boolean* success);

    /**
     * When {@link android.app.ActivityOptions#makeSceneTransitionAnimation(Activity,
     * android.view.View, String)} was used to start an Activity, <var>callback</var>
     * will be called to handle shared elements on the <i>launched</i> Activity. This requires
     * {@link Window#FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @param callback Used to manipulate shared element transitions on the launched Activity.
     */
    CARAPI SetEnterSharedElementCallback(
        /* [in] */ ISharedElementCallback* callback);

    /**
     * When {@link android.app.ActivityOptions#makeSceneTransitionAnimation(Activity,
     * android.view.View, String)} was used to start an Activity, <var>callback</var>
     * will be called to handle shared elements on the <i>launching</i> Activity. Most
     * calls will only come when returning from the started Activity.
     * This requires {@link Window#FEATURE_ACTIVITY_TRANSITIONS}.
     *
     * @param callback Used to manipulate shared element transitions on the launching Activity.
     */
    CARAPI SetExitSharedElementCallback(
        /* [in] */ ISharedElementCallback* callback);

    /**
     * Postpone the entering activity transition when Activity was started with
     * {@link android.app.ActivityOptions#makeSceneTransitionAnimation(Activity,
     * android.util.Pair[])}.
     * <p>This method gives the Activity the ability to delay starting the entering and
     * shared element transitions until all data is loaded. Until then, the Activity won't
     * draw into its window, leaving the window transparent. This may also cause the
     * returning animation to be delayed until data is ready. This method should be
     * called in {@link #onCreate(android.os.Bundle)} or in
     * {@link #onActivityReenter(int, android.content.Intent)}.
     * {@link #startPostponedEnterTransition()} must be called to allow the Activity to
     * start the transitions. If the Activity did not use
     * {@link android.app.ActivityOptions#makeSceneTransitionAnimation(Activity,
     * android.util.Pair[])}, then this method does nothing.</p>
     */
    CARAPI PostponeEnterTransition();

    /**
     * Begin postponed transitions after {@link #postponeEnterTransition()} was called.
     * If postponeEnterTransition() was called, you must call startPostponedEnterTransition()
     * to have your Activity start drawing.
     */
    CARAPI StartPostponedEnterTransition();

    /**
     * Obtain an {@link Intent} that will launch an explicit target activity specified by
     * this activity's logical parent. The logical parent is named in the application's manifest
     * by the {@link android.R.attr#parentActivityName parentActivityName} attribute.
     * Activity subclasses may override this method to modify the Intent returned by
     * super.getParentActivityIntent() or to implement a different mechanism of retrieving
     * the parent intent entirely.
     *
     * @return a new Intent targeting the defined parent of this activity or null if
     *         there is no valid parent.
     */
    CARAPI GetParentActivityIntent(
        /* [out] */ IIntent** intent);

    /** @hide */
    CARAPI GetActivityToken(
        /* [out] */ IBinder** binder);

    /**
     * @hide
     */
    CARAPI IsResumed(
        /* [out] */ Boolean* isResumed);

public:
    CARAPI SetParent(
        /* [int] */ IActivity* parent);

    CARAPI Attach(
        /* [in] */ IContext* context,
        /* [in] */ IActivityThread* thread,
        /* [in] */ IInstrumentation* instr,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 ident,
        /* [in] */ IApplication* application,
        /* [in] */ IIntent* intent,
        /* [in] */ IActivityInfo* info,
        /* [in] */ ICharSequence* title,
        /* [in] */ IActivity* parent,
        /* [in] */ const String& id,
        /* [in] */ IActivityNonConfigurationInstances* lastNonConfigurationInstance,
        /* [in] */ IConfiguration* config,
        /* [in] */ IIVoiceInteractor* voiceInteractor);

    /**
     * The hook for {@link ActivityThread} to restore the state of this activity.
     *
     * Calls {@link #onSaveInstanceState(android.os.Bundle)} and
     * {@link #restoreManagedDialogs(android.os.Bundle)}.
     *
     * @param savedInstanceState contains the saved state
     */
    CARAPI PerformRestoreInstanceState(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * The hook for {@link ActivityThread} to restore the state of this activity.
     *
     * Calls {@link #onSaveInstanceState(android.os.Bundle)} and
     * {@link #restoreManagedDialogs(android.os.Bundle)}.
     *
     * @param savedInstanceState contains the saved state
     * @param persistentState contains the persistable saved state
     */
    CARAPI PerformRestoreInstanceState(
        /* [in] */ IBundle* savedInstanceState,
        /* [in] */ IPersistableBundle* persistentState);

    CARAPI GetID(
        /* [out] */ String* id);

    CARAPI GetToken(
        /* [out] */ IBinder** token);

    CARAPI Create(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI PostCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI Start();

    CARAPI Restart();

    CARAPI Resume();

    CARAPI Pause();

    CARAPI Stop();

    CARAPI Destroy();

    CARAPI DispatchActivityResult(
        /* [in] */ const String& who,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent *data);

    CARAPI DispatchNewIntent(
        /* [in] */ IIntent *intent);

    /**
     * Request to put this Activity in a mode where the user is locked to the
     * current task.
     *
     * This will prevent the user from launching other apps, going to settings,
     * or reaching the home screen.
     *
     * If {@link DevicePolicyManager#isLockTaskPermitted(String)} returns true
     * for this component then the app will go directly into Lock Task mode.  The user
     * will not be able to exit this mode until {@link Activity#stopLockTask()} is called.
     *
     * If {@link DevicePolicyManager#isLockTaskPermitted(String)} returns false
     * then the system will prompt the user with a dialog requesting permission to enter
     * this mode.  When entered through this method the user can exit at any time through
     * an action described by the request dialog.  Calling stopLockTask will also exit the
     * mode.
     */
    CARAPI StartLockTask();

    /**
     * Allow the user to switch away from the current task.
     *
     * Called to end the mode started by {@link Activity#startLockTask}. This
     * can only be called by activities that have successfully called
     * startLockTask previously.
     *
     * This will allow the user to exit this app and move onto other activities.
     */
    CARAPI StopLockTask();

    CARAPI SetCalled(
        /* [in] */ Boolean called);

    CARAPI IsCalled(
        /* [out] */ Boolean* called);

    CARAPI SetFinishing(
        /* [in] */ Boolean finished);

    CARAPI PerformCreateCommon();

    CARAPI PerformCreate(
        /* [in] */ IBundle* icicle);

    CARAPI PerformCreate(
        /* [in] */ IBundle* icicle,
        /* [in] */ IPersistableBundle* bundle);

    CARAPI PerformStart();

    CARAPI PerformRestart();

    CARAPI PerformResume();

    CARAPI PerformPause();

    CARAPI PerformUserLeaving();

    CARAPI PerformStop();

    CARAPI PerformDestroy();

    /**
     * The hook for {@link ActivityThread} to save the state of this activity.
     *
     * Calls {@link #onSaveInstanceState(android.os.Bundle)}
     * and {@link #saveManagedDialogs(android.os.Bundle)}.
     *
     * @param outState The bundle to save the state to.
     */
    CARAPI PerformSaveInstanceState(
        /* [in] */ IBundle* outState);

    /**
     * The hook for {@link ActivityThread} to save the state of this activity.
     *
     * Calls {@link #onSaveInstanceState(android.os.Bundle)}
     * and {@link #saveManagedDialogs(android.os.Bundle)}.
     *
     * @param outState The bundle to save the state to.
     * @param outPersistentState The bundle to save persistent state to.
     */
    CARAPI PerformSaveInstanceState(
        /* [in] */ IBundle* outState,
        /* [in] */ IPersistableBundle* outPersistentState);

    // internal use.
    CARAPI IsStartedActivity(
        /* [out] */ Boolean* isStartedActivity);

    CARAPI SetDecorView(
        /* [in] */ IView* decor);

    CARAPI GetDecorView(
        /* [out] */ IView** decor);

    CARAPI SetVisibleFromServer(
        /* [in] */ Boolean visible);

    CARAPI IsVisibleFromServer(
        /* [out] */ Boolean * visible);

    CARAPI IsVisibleFromClient(
        /* [out] */ Boolean* visible);

    CARAPI SetWindowAdded(
        /* [in] */ Boolean added);

    CARAPI IsWindowAdded(
        /* [out] */ Boolean* added);

    CARAPI SetConfigChangeFlags(
        /* [in] */ Int32 configChangeFlags);

    CARAPI GetConfigChangeFlags(
        /* [out] */ Int32* configChangeFlags);

    CARAPI  SetStartedActivity(
        /* [in] */ Boolean started);

    virtual CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    /**
     * This is the same as {@link #onSaveInstanceState} but is called for activities
     * created with the attribute {@link android.R.attr#persistableMode} set to
     * <code>persistAcrossReboots</code>. The {@link android.os.PersistableBundle} passed
     * in will be saved and presented in {@link #onCreate(Bundle, PersistableBundle)}
     * the first time that this activity is restarted following the next device reboot.
     *
     * @param outState Bundle in which to place your saved state.
     * @param outPersistentState State which will be saved across reboots.
     *
     * @see #onSaveInstanceState(Bundle)
     * @see #onCreate
     * @see #onRestoreInstanceState(Bundle, PersistableBundle)
     * @see #onPause
     */
    virtual CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState,
        /* [in] */ IPersistableBundle* outPersistentState);

    CARAPI SetResumed(
        /* [in] */ Boolean resumed);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(String) ToString();

    CARAPI StartActivity(
        /* [in] */ IIntent* intent);

    CARAPI StartActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options);

    /**
     * @hide Implement to provide correct calling token.
     */
    CARAPI StartActivityAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user);

    /**
     * @hide Implement to provide correct calling token.
     */
    CARAPI StartActivityAsUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options,
        /* [in] */ IUserHandle* user);

    /**
     * Start a new activity as if it was started by the activity that started our
     * current activity.  This is for the resolver and chooser activities, which operate
     * as intermediaries that dispatch their intent to the target the user selects -- to
     * do this, they must perform all security checks including permission grants as if
     * their launch had come from the original activity.
     * @hide
     */
    CARAPI StartActivityAsCaller(
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId);

    CARAPI StartActivities(
        /* [in] */ ArrayOf<IIntent*>* intents);

    CARAPI StartActivities(
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ IBundle* options);

    CARAPI StartActivitiesAsUser(
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ IBundle* options,
        /* [in] */ IUserHandle* userHandle);

    CARAPI StartIntentSender(
        /* [in] */ IIntentSender* intent,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ Int32 extraFlags);

    CARAPI StartIntentSender(
        /* [in] */ IIntentSender* intent,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ Int32 extraFlags,
        /* [in] */ IBundle* options);

    CARAPI GetSystemService(
        /* [in] */ const String& name,
        /* [out] */ IInterface** object);

public:
    // @Override
    virtual CARAPI OnApplyThemeResource(
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ Int32 resid,
        /* [in] */ Boolean first);


    /**
     * Called when the activity is starting.  This is where most initialization
     * should go: calling {@link #setContentView(int)} to inflate the
     * activity's UI, using {@link #findViewById} to programmatically interact
     * with widgets in the UI, calling
     * {@link #managedQuery(android.net.Uri , String[], String, String[], String)} to retrieve
     * cursors for data being displayed, etc.
     *
     * <p>You can call {@link #finish} from within this function, in
     * which case onDestroy() will be immediately called without any of the rest
     * of the activity lifecycle ({@link #onStart}, {@link #onResume},
     * {@link #onPause}, etc) executing.
     *
     * <p><em>Derived classes must call through to the super class's
     * implementation of this method.  If they do not, an exception will be
     * thrown.</em></p>
     *
     * @param savedInstanceState If the activity is being re-initialized after
     *     previously being shut down then this Bundle contains the data it most
     *     recently supplied in {@link #onSaveInstanceState}.  <b><i>Note: Otherwise it is null.</i></b>
     *
     * @see #onStart
     * @see #onSaveInstanceState
     * @see #onRestoreInstanceState
     * @see #onPostCreate
     */
    virtual CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Same as {@link #onCreate(android.os.Bundle)} but called for those activities created with
     * the attribute {@link android.R.attr#persistableMode} set to
     * <code>persistAcrossReboots</code>.
     *
     * @param savedInstanceState if the activity is being re-initialized after
     *     previously being shut down then this Bundle contains the data it most
     *     recently supplied in {@link #onSaveInstanceState}.
     *     <b><i>Note: Otherwise it is null.</i></b>
     * @param persistentState if the activity is being re-initialized after
     *     previously being shut down or powered off then this Bundle contains the data it most
     *     recently supplied to outPersistentState in {@link #onSaveInstanceState}.
     *     <b><i>Note: Otherwise it is null.</i></b>
     *
     * @see #onCreate(android.os.Bundle)
     * @see #onStart
     * @see #onSaveInstanceState
     * @see #onRestoreInstanceState
     * @see #onPostCreate
     */
    virtual CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState,
        /* [in] */ IPersistableBundle* persistentState);


    /**
     * This method is called after {@link #onStart} when the activity is
     * being re-initialized from a previously saved state, given here in
     * <var>savedInstanceState</var>.  Most implementations will simply use {@link #onCreate}
     * to restore their state, but it is sometimes convenient to do it here
     * after all of the initialization has been done or to allow subclasses to
     * decide whether to use your default implementation.  The default
     * implementation of this method performs a restore of any view state that
     * had previously been frozen by {@link #onSaveInstanceState}.
     *
     * <p>This method is called between {@link #onStart} and
     * {@link #onPostCreate}.
     *
     * @param savedInstanceState the data most recently supplied in {@link #onSaveInstanceState}.
     *
     * @see #onCreate
     * @see #onPostCreate
     * @see #onResume
     * @see #onSaveInstanceState
     */
    virtual CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * This is the same as {@link #onRestoreInstanceState(Bundle)} but is called for activities
     * created with the attribute {@link android.R.attr#persistableMode} set to
     * <code>persistAcrossReboots</code>. The {@link android.os.PersistableBundle} passed
     * came from the restored PersistableBundle first
     * saved in {@link #onSaveInstanceState(Bundle, PersistableBundle)}.
     *
     * <p>This method is called between {@link #onStart} and
     * {@link #onPostCreate}.
     *
     * <p>If this method is called {@link #onRestoreInstanceState(Bundle)} will not be called.
     *
     * @param savedInstanceState the data most recently supplied in {@link #onSaveInstanceState}.
     * @param persistentState the data most recently supplied in {@link #onSaveInstanceState}.
     *
     * @see #onRestoreInstanceState(Bundle)
     * @see #onCreate
     * @see #onPostCreate
     * @see #onResume
     * @see #onSaveInstanceState
     */
    virtual CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* savedInstanceState,
        /* [in] */ IPersistableBundle* persistentState);

    virtual CARAPI_(AutoPtr<IDialog>) CreateDialog(
        /* [in] */ Int32 dialogId,
        /* [in] */ IBundle* state,
        /* [in] */ IBundle* args);

    /**
     * Called when activity start-up is complete (after {@link #onStart}
     * and {@link #onRestoreInstanceState} have been called).  Applications will
     * generally not implement this method; it is intended for system
     * classes to do final initialization after application code has run.
     *
     * <p><em>Derived classes must call through to the super class's
     * implementation of this method.  If they do not, an exception will be
     * thrown.</em></p>
     *
     * @param savedInstanceState If the activity is being re-initialized after
     *     previously being shut down then this Bundle contains the data it most
     *     recently supplied in {@link #onSaveInstanceState}.  <b><i>Note: Otherwise it is null.</i></b>
     * @see #onCreate
     */
    virtual CARAPI OnPostCreate(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * This is the same as {@link #onPostCreate(Bundle)} but is called for activities
     * created with the attribute {@link android.R.attr#persistableMode} set to
     * <code>persistAcrossReboots</code>.
     *
     * @param savedInstanceState The data most recently supplied in {@link #onSaveInstanceState}
     * @param persistentState The data caming from the PersistableBundle first
     * saved in {@link #onSaveInstanceState(Bundle, PersistableBundle)}.
     *
     * @see #onCreate
     */
    virtual CARAPI OnPostCreate(
        /* [in] */ IBundle* savedInstanceState,
        /* [in] */ IPersistableBundle* persistentState);

    /**
     * Called after {@link #onCreate} &mdash; or after {@link #onRestart} when
     * the activity had been stopped, but is now again being displayed to the
     * user.  It will be followed by {@link #onResume}.
     *
     * <p><em>Derived classes must call through to the super class's
     * implementation of this method.  If they do not, an exception will be
     * thrown.</em></p>
     *
     * @see #onCreate
     * @see #onStop
     * @see #onResume
     */
    virtual CARAPI OnStart();

    /**
     * Called after {@link #onStop} when the current activity is being
     * re-displayed to the user (the user has navigated back to it).  It will
     * be followed by {@link #onStart} and then {@link #onResume}.
     *
     * <p>For activities that are using raw {@link Cursor} objects (instead of
     * creating them through
     * {@link #managedQuery(android.net.Uri , String[], String, String[], String)},
     * this is usually the place
     * where the cursor should be requeried (because you had deactivated it in
     * {@link #onStop}.
     *
     * <p><em>Derived classes must call through to the super class's
     * implementation of this method.  If they do not, an exception will be
     * thrown.</em></p>
     *
     * @see #onStop
     * @see #onStart
     * @see #onResume
     */
    virtual CARAPI OnRestart();

    /**
     * Called after {@link #onRestoreInstanceState}, {@link #onRestart}, or
     * {@link #onPause}, for your activity to start interacting with the user.
     * This is a good place to begin animations, open exclusive-access devices
     * (such as the camera), etc.
     *
     * <p>Keep in mind that onResume is not the best indicator that your activity
     * is visible to the user; a system window such as the keyguard may be in
     * front.  Use {@link #onWindowFocusChanged} to know for certain that your
     * activity is visible to the user (for example, to resume a game).
     *
     * <p><em>Derived classes must call through to the super class's
     * implementation of this method.  If they do not, an exception will be
     * thrown.</em></p>
     *
     * @see #onRestoreInstanceState
     * @see #onRestart
     * @see #onPostResume
     * @see #onPause
     */
    virtual CARAPI OnResume();

    virtual CARAPI OnPause();

    /**
     * Called when activity resume is complete (after {@link #onResume} has
     * been called). Applications will generally not implement this method;
     * it is intended for system classes to do final setup after application
     * resume code has run.
     *
     * <p><em>Derived classes must call through to the super class's
     * implementation of this method.  If they do not, an exception will be
     * thrown.</em></p>
     *
     * @see #onResume
     */
    virtual CARAPI OnPostResume();

    virtual CARAPI OnStop();

    virtual CARAPI OnDestroy();

    /**
     * @hide
     * Check whether this activity is running as part of a voice interaction with the user.
     * If true, it should perform its interaction with the user through the
     * {@link VoiceInteractor} returned by {@link #getVoiceInteractor}.
     */
    CARAPI IsVoiceInteraction(
        /* [out] */ Boolean* bval);

    /**
     * @hide
     * Retrieve the active {@link VoiceInteractor} that the user is going through to
     * interact with this activity.
     */
    CARAPI GetVoiceInteractor(
        /* [out] */ IVoiceInteractor** vi);

    /**
     * This is called for activities that set launchMode to "singleTop" in
     * their package, or if a client used the {@link Intent#FLAG_ACTIVITY_SINGLE_TOP}
     * flag when calling {@link #startActivity}.  In either case, when the
     * activity is re-launched while at the top of the activity stack instead
     * of a new instance of the activity being started, onNewIntent() will be
     * called on the existing instance with the Intent that was used to
     * re-launch it.
     *
     * <p>An activity will always be paused before receiving a new intent, so
     * you can count on {@link #onResume} being called after this method.
     *
     * <p>Note that {@link #getIntent} still returns the original Intent.  You
     * can use {@link #setIntent} to update it to this new Intent.
     *
     * @param intent The new intent that was started for the activity.
     *
     * @see #getIntent
     * @see #setIntent
     * @see #onResume
     */
    virtual CARAPI OnNewIntent(
        /* [in] */ IIntent *intent);

    /**
     * @deprecated Old no-arguments version of {@link #onCreateDialog(int, Bundle)}.
     */
    //@Deprecated
    virtual CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id);

    /**
     * Callback for creating dialogs that are managed (saved and restored) for you
     * by the activity.  The default implementation calls through to
     * {@link #onCreateDialog(int)} for compatibility.
     *
     * <em>If you are targeting {@link android.os.Build.VERSION_CODES#HONEYCOMB}
     * or later, consider instead using a {@link DialogFragment} instead.</em>
     *
     * <p>If you use {@link #showDialog(int)}, the activity will call through to
     * this method the first time, and hang onto it thereafter.  Any dialog
     * that is created by this method will automatically be saved and restored
     * for you, including whether it is showing.
     *
     * <p>If you would like the activity to manage saving and restoring dialogs
     * for you, you should override this method and handle any ids that are
     * passed to {@link #showDialog}.
     *
     * <p>If you would like an opportunity to prepare your dialog before it is shown,
     * override {@link #onPrepareDialog(int, Dialog, Bundle)}.
     *
     * @param id The id of the dialog.
     * @param args The dialog arguments provided to {@link #showDialog(int, Bundle)}.
     * @return The dialog.  If you return null, the dialog will not be created.
     *
     * @see #onPrepareDialog(int, Dialog, Bundle)
     * @see #showDialog(int, Bundle)
     * @see #dismissDialog(int)
     * @see #removeDialog(int)
     *
     * @deprecated Use the new {@link DialogFragment} class with
     * {@link FragmentManager} instead; this is also
     * available on older platforms through the Android compatibility package.
     */
    //@Deprecated
    virtual CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id,
        /* [in] */ IBundle* args);

    /**
     * @deprecated Old no-arguments version of
     * {@link #onPrepareDialog(int, Dialog, Bundle)}.
     */
    //@Deprecated
    virtual CARAPI_(void) OnPrepareDialog(
        /* [in] */ Int32 id,
        /* [in] */ IDialog* dialog);

    /**
     * Provides an opportunity to prepare a managed dialog before it is being
     * shown.  The default implementation calls through to
     * {@link #onPrepareDialog(int, Dialog)} for compatibility.
     *
     * <p>
     * Override this if you need to update a managed dialog based on the state
     * of the application each time it is shown. For example, a time picker
     * dialog might want to be updated with the current time. You should call
     * through to the superclass's implementation. The default implementation
     * will set this Activity as the owner activity on the Dialog.
     *
     * @param id The id of the managed dialog.
     * @param dialog The dialog.
     * @param args The dialog arguments provided to {@link #showDialog(int, Bundle)}.
     * @see #onCreateDialog(int, Bundle)
     * @see #showDialog(int)
     * @see #dismissDialog(int)
     * @see #removeDialog(int)
     *
     * @deprecated Use the new {@link DialogFragment} class with
     * {@link FragmentManager} instead; this is also
     * available on older platforms through the Android compatibility package.
     */
    //@Deprecated
    virtual CARAPI_(void) OnPrepareDialog(
        /* [in] */ Int32 id,
        /* [in] */ IDialog* dialog,
        /* [in] */ IBundle* args);

private:
    class ECO_LOCAL ManagedDialog
        : public Object
    {
    public:
        AutoPtr<IDialog> mDialog;
        AutoPtr<IBundle> mArgs;
    };

    class ECO_LOCAL ManagedCursor
        : public Object
    {
    friend class Activity;

    public:
        ManagedCursor(
            /* [in] */ ICursor* cursor)
            : mCursor(cursor)
            , mReleased(FALSE)
            , mUpdated(FALSE)
        {}

    private:
        AutoPtr<ICursor> mCursor;
        Boolean mReleased;
        Boolean mUpdated;
    };

private:
    ECO_LOCAL CARAPI InitializeTheme();

    /**
     * Restore the state of any saved managed dialogs.
     *
     * @param savedInstanceState The bundle to restore from.
     */
    ECO_LOCAL CARAPI RestoreManagedDialogs(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Save the state of any managed dialogs.
     *
     * @param outState place to store the saved state.
     */
    CARAPI SaveManagedDialogs(
        /* [in] */ IBundle* outState);

    ECO_LOCAL static CARAPI_(String) SavedDialogKeyFor(
        /* [in] */ Int32 key);

    ECO_LOCAL static CARAPI_(String) SavedDialogArgsKeyFor(
        /* [in] */ Int32 key);

    ECO_LOCAL CARAPI InitWindowDecorActionBar();

    ECO_LOCAL CARAPI StartIntentSenderForResultInner(
        /* [in] */ IIntentSender* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* options);

    ECO_LOCAL CARAPI EnsureSearchManager();

    ECO_LOCAL CARAPI DumpInner(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

    ECO_LOCAL CARAPI DumpViewHierarchy(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ IView* view);

    /**
     * Finishes the current activity and specifies whether to remove the task associated with this
     * activity.
     */
    ECO_LOCAL CARAPI Finish(
        /* [in] */ Boolean finishTask);


    /**
     * Indication of whether this is the highest level activity in this task. Can be used to
     * determine whether an activity launched by this activity was placed in the same task or
     * another task.
     *
     * @return true if this is the topmost, non-finishing activity in its task.
     */
    ECO_LOCAL CARAPI IsTopOfTask(
        /* [out] */ Boolean* top);

public:
    static const String FRAGMENTS_TAG;

    String mEmbeddedID;
    AutoPtr<IIntent> mIntent;
    /*package*/ AutoPtr<IActivityInfo> mActivityInfo;
    /*package*/ AutoPtr<IActivityThread> mMainThread;
    AutoPtr<IActivity> mParent;
    Boolean mCalled;
    Boolean mCheckedForLoaderManager;
    Boolean mLoadersStarted;
    /*package*/ Boolean mResumed;
    Boolean mStopped;
    Boolean mFinished;
    Boolean mStartedActivity;

    /** true if the activity is going through a transient pause */
    /*package*/ Boolean mTemporaryPause;
    /** true if the activity is being destroyed in order to recreate it with a new configuration */
    /*package*/ Boolean mChangingConfigurations;
    /*package*/
    Int32 mConfigChangeFlags;
    /*package*/ AutoPtr<IConfiguration> mCurrentConfig;
    /* package */ AutoPtr<IActivityNonConfigurationInstances> mLastNonConfigurationInstances;

    AutoPtr<IView> mDecor;
    // frome ContextThemeWrapper.java
    AutoPtr<ILayoutInflater> mInflater;
    Boolean mWindowAdded;
    Boolean mVisibleFromServer;
    Boolean mVisibleFromClient;
    /*package*/ AutoPtr<IActionBar> mActionBar;

    AutoPtr<IFragmentManagerImpl> mFragments;
    AutoPtr<IFragmentContainer> mContainer;

    AutoPtr<IArrayMap> mAllLoaderManagers;
    AutoPtr<ILoaderManagerImpl> mLoaderManager;

    // protected by synchronized(this)
    Int32 mResultCode;
    AutoPtr<IIntent> mResultData;
    AutoPtr<IHandler> mHandler;

    AutoPtr<ITranslucentConversionListener> mTranslucentCallback;
    Boolean mChangeCanvasToTranslucent;

    // Most recent call to requestVisibleBehind().
    Boolean mVisibleBehind;

    AutoPtr<IActivityTransitionState> mActivityTransitionState;// = new ActivityTransitionState();
    AutoPtr<ISharedElementCallback> mEnterTransitionListener;// = SharedElementCallback.NULL_CALLBACK;
    AutoPtr<ISharedElementCallback> mExitTransitionListener;// = SharedElementCallback.NULL_CALLBACK;

protected:
    AutoPtr<IBinder> mToken;

//protected static final int[] FOCUSED_STATE_SET = {com.android.internal.R.attr.state_focused};

private:
    ECO_LOCAL static const Boolean DEBUG_LIFECYCLE;

    ECO_LOCAL static const String WINDOW_HIERARCHY_TAG;
    ECO_LOCAL static const String SAVED_DIALOG_IDS_KEY;
    ECO_LOCAL static const String SAVED_DIALOGS_TAG;
    ECO_LOCAL static const String SAVED_DIALOG_KEY_PREFIX;
    ECO_LOCAL static const String SAVED_DIALOG_ARGS_KEY_PREFIX;

    HashMap<Int32, AutoPtr<ManagedDialog> > mManagedDialogs;
    typedef HashMap<Int32, AutoPtr<ManagedDialog> >::Iterator ManagedDialogHashMapIter;

    List<AutoPtr<ManagedCursor> > mManagedCursors;

    // set by the thread after the constructor and before onCreate(Bundle savedInstanceState) is called.
    AutoPtr<IInstrumentation> mInstrumentation;
    AutoPtr<IApplication> mApplication;

    Int32 mIdent;

    AutoPtr<IComponentName> mComponent;
    Boolean mDestroyed;
    Boolean mDoReportFullyDrawn;

    AutoPtr<ISearchManager> mSearchManager;
    AutoPtr<IMenuInflater> mMenuInflater;

    AutoPtr<IWindow> mWindow;
    AutoPtr<IWindowManager> mWindowManager;

    Boolean mEnableDefaultActionBarUp;

    AutoPtr<IVoiceInteractor> mVoiceInteractor;

    AutoPtr<ICharSequence> mTitle;
    Int32 mTitleColor;

    Boolean mTitleReady;

    Int32 mDefaultKeyMode;
    AutoPtr<ISpannableStringBuilder> mDefaultKeySsb;

    // form ContextThemeWrapper.java
    Int32 mThemeResource;
    AutoPtr<IResourcesTheme> mTheme;
    AutoPtr<IConfiguration> mOverrideConfiguration;
    AutoPtr<IResources> mResources;

     //@SuppressWarnings("unused")
    AutoPtr<IInterface> mInstanceTracker;

    AutoPtr<IThread> mUiThread;

    Object mManagedCursorsLock;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ACTIVITY_H__
