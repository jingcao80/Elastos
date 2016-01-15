
#ifndef __ELASTOS_DROID_APP_INSTRUMENTATION_H__
#define __ELASTOS_DROID_APP_INSTRUMENTATION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IApplication;
using Elastos::Droid::App::IInstrumentationActivityMonitor;
using Elastos::Droid::App::IInstrumentationActivityResult;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IPersistableBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IIdleHandler;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;


using Elastos::Core::IClassLoader;
using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;
using Elastos::Core::Thread;
using Elastos::Core::IThread;

namespace Elastos {
namespace Droid {
namespace App {

class ECO_PUBLIC Instrumentation
    : public Object
    , public IInstrumentation
{
private:
    class ECO_LOCAL ActivityWaiter
        : public Object
    {
    public:
        ActivityWaiter(
            /* [in] */ IIntent* intent)
            : mIntent(intent)
        {}

    public:
        AutoPtr<IIntent> mIntent;
        AutoPtr<IActivity> mActivity;
    };

    class ECO_LOCAL MenuRunnable
        : public Runnable
    {
    public:
        MenuRunnable(
            /* [in] */ IActivity* activity,
            /* [in] */ Int32 identifier,
            /* [in] */ Int32 flags);

        CARAPI Run();

    private:
        AutoPtr<IActivity> mActivity;
        Int32 mIdentifier;
        Int32 mFlags;
        Boolean mReturnValue;

        friend class Instrumentation;
    };

    class ECO_LOCAL ContextMenuRunnable
        : public Runnable
    {
    public:
        ContextMenuRunnable(
            /* [in] */ IActivity* activity,
            /* [in] */ Int32 identifier,
            /* [in] */ Int32 flags);

        CARAPI Run();

    private:
        AutoPtr<IActivity> mActivity;
        Int32 mIdentifier;
        Int32 mFlags;
        Boolean mReturnValue;

        friend class Instrumentation;
    };

    class ECO_LOCAL InstrumentationThread
        : public Thread
    {
    public:
        InstrumentationThread(
            /* [in] */ const String& name,
            /* [in] */ Instrumentation* host);

        CARAPI Run();

    private:
        Instrumentation* mHost;
    };

    class ECO_LOCAL EmptyRunnable
        : public Runnable
    {
    public:
        CARAPI Run();
    };

    class ECO_LOCAL SyncRunnable
        : public Runnable
    {
    public:
        SyncRunnable(
            /* [in] */ IRunnable* target);

        CARAPI Run();

        CARAPI_(void) WaitForComplete();

    private:
        AutoPtr<IRunnable> mTarget;
        Boolean mComplete;
    };

    class ECO_LOCAL ActivityGoing
        : public Object
        , public IIdleHandler
    {
    public:
        CAR_INTERFACE_DECL()

        ActivityGoing(
            /* [in] */ ActivityWaiter* waiter,
            /* [in] */ Instrumentation* host);

        CARAPI QueueIdle(
            /* [out] */ Boolean* res);

    private:
        AutoPtr<ActivityWaiter> mWaiter;
        Instrumentation* mHost;
    };

    class ECO_LOCAL Idler
        : public Object
        , public IIdleHandler
    {
    public:
        CAR_INTERFACE_DECL()

        Idler(
            /* [in] */ IRunnable* callback);

        CARAPI QueueIdle(
            /* [out] */ Boolean* res);

        CARAPI_(void) WaitForIdle();

    private:
        AutoPtr<IRunnable> mCallback;
        Boolean mIdle;
    };

    class ECO_LOCAL BlockPhoneCallRunnable
        : public Runnable
    {
    public:
        BlockPhoneCallRunnable(
            /* [in] */ IContext* con)
            : mContext(con)
        {}

        CARAPI Run();

    private:
        AutoPtr<IContext> mContext;
    };

public:
    CAR_INTERFACE_DECL()

    Instrumentation();

    virtual ~Instrumentation();

    CARAPI constructor();

    /**
     * Called when the instrumentation is starting, before any application code
     * has been loaded.  Usually this will be implemented to simply call
     * {@link #start} to begin the instrumentation thread, which will then
     * continue execution in {@link #onStart}.
     *
     * <p>If you do not need your own thread -- that is you are writing your
     * instrumentation to be completely asynchronous (returning to the event
     * loop so that the application can run), you can simply begin your
     * instrumentation here, for example call {@link Context#startActivity} to
     * begin the appropriate first activity of the application.
     *
     * @param arguments Any additional arguments that were supplied when the
     *                  instrumentation was started.
     */
    virtual CARAPI OnCreate(
        /* [in] */ IBundle* arguments);

    /**
     * Create and start a new thread in which to run instrumentation.  This new
     * thread will call to {@link #onStart} where you can implement the
     * instrumentation.
     */
    virtual CARAPI Start();

    /**
     * Method where the instrumentation thread enters execution.  This allows
     * you to run your instrumentation code in a separate thread than the
     * application, so that it can perform blocking operation such as
     * {@link #sendKeySync} or {@link #startActivitySync}.
     *
     * <p>You will typically want to call finish() when this function is done,
     * to end your instrumentation.
     */
    virtual CARAPI OnStart();

    /**
     * This is called whenever the system captures an unhandled exception that
     * was thrown by the application.  The default implementation simply
     * returns false, allowing normal system handling of the exception to take
     * place.
     *
     * @param obj The client object that generated the exception.  May be an
     *            Application, Activity, BroadcastReceiver, Service, or null.
     * @param e The exception that was thrown.
     *
     * @return To allow normal system exception process to occur, return false.
     *         If true is returned, the system will proceed as if the exception
     *         didn't happen.
     */
    virtual CARAPI OnException(
        /* [in] */ IInterface* obj,
        /* [in] */ ECode e,
        /* [out] */ Boolean* result);

    /**
     * Provide a status report about the application.
     *
     * @param resultCode Current success/failure of instrumentation.
     * @param results Any results to send back to the code that started the instrumentation.
     */
    virtual CARAPI SendStatus(
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    /**
     * Terminate instrumentation of the application.  This will cause the
     * application process to exit, removing this instrumentation from the next
     * time the application is started.
     *
     * @param resultCode Overall success/failure of instrumentation.
     * @param results Any results to send back to the code that started the
     *                instrumentation.
     */
    virtual CARAPI Finish(
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    virtual CARAPI SetAutomaticPerformanceSnapshots();

    virtual CARAPI StartPerformanceSnapshot();

    virtual CARAPI EndPerformanceSnapshot();

       /**
     * Called when the instrumented application is stopping, after all of the
     * normal application cleanup has occurred.
     */
    virtual CARAPI OnDestroy();

    /**
     * Return the Context of this instrumentation's package.  Note that this is
     * often different than the Context of the application being
     * instrumentated, since the instrumentation code often lives is a
     * different package than that of the application it is running against.
     * See {@link #getTargetContext} to retrieve a Context for the target
     * application.
     *
     * @return The instrumentation's package context.
     *
     * @see #getTargetContext
     */
    virtual CARAPI GetContext(
        /* [out] */ IContext** context);

    /**
     * Returns complete component name of this instrumentation.
     *
     * @return Returns the complete component name for this instrumentation.
     */
    virtual CARAPI GetComponentName(
        /* [out] */ IComponentName** component);

    /**
     * Return a Context for the target application being instrumented.  Note
     * that this is often different than the Context of the instrumentation
     * code, since the instrumentation code often lives is a different package
     * than that of the application it is running against. See
     * {@link #getContext} to retrieve a Context for the instrumentation code.
     *
     * @return A Context in the target application.
     *
     * @see #getContext
     */
    virtual CARAPI GetTargetContext(
        /* [out] */ IContext** context);

    /**
     * Check whether this instrumentation was started with profiling enabled.
     *
     * @return Returns true if profiling was enabled when starting, else false.
     */
    virtual CARAPI IsProfiling(
        /* [out] */ Boolean* isProfiling);

    /**
     * This method will start profiling if isProfiling() returns true. You should
     * only call this method if you set the handleProfiling attribute in the
     * manifest file for this Instrumentation to true.
     */
    virtual CARAPI StartProfiling();

    /**
     * Stops profiling if isProfiling() returns true.
     */
    virtual CARAPI StopProfiling();

    /**
     * Force the global system in or out of touch mode.  This can be used if
     * your instrumentation relies on the UI being in one more or the other
     * when it starts.
     *
     * @param inTouch Set to true to be in touch mode, false to be in
     * focus mode.
     */
    virtual CARAPI SetInTouchMode(
        /* [in] */ Boolean inTouch);

    /**
     * Schedule a callback for when the application's main thread goes idle
     * (has no more events to process).
     *
     * @param recipient Called the next time the thread's message queue is
     *                  idle.
     */
    virtual CARAPI WaitForIdle(
        /* [in] */ IRunnable* recipient);

    /**
     * Synchronously wait for the application to be idle.  Can not be called
     * from the main application thread -- use {@link #start} to execute
     * instrumentation in its own thread.
     */
    virtual CARAPI WaitForIdleSync();

    /**
     * Execute a call on the application's main thread, blocking until it is
     * complete.  Useful for doing things that are not thread-safe, such as
     * looking at or modifying the view hierarchy.
     *
     * @param runner The code to run on the main thread.
     */
    virtual CARAPI RunOnMainSync(
        /* [in] */ IRunnable* runner);

    /**
     * Start a new activity and wait for it to begin running before returning.
     * In addition to being synchronous, this method as some semantic
     * differences from the standard {@link Context#startActivity} call: the
     * activity component is resolved before talking with the activity manager
     * (its class name is specified in the Intent that this method ultimately
     * starts), and it does not allow you to start activities that run in a
     * different process.  In addition, if the given Intent resolves to
     * multiple activities, instead of displaying a dialog for the user to
     * select an activity, an exception will be thrown.
     *
     * <p>The function returns as soon as the activity goes idle following the
     * call to its {@link Activity#onCreate}.  Generally this means it has gone
     * through the full initialization including {@link Activity#onResume} and
     * drawn and displayed its initial window.
     *
     * @param intent Description of the activity to start.
     *
     * @see Context#startActivity
     */
    virtual CARAPI StartActivitySync(
        /* [in] */ IIntent* intent,
        /* [out] */ IActivity** activity);

    /**
     * Add a new {@link ActivityMonitor} that will be checked whenever an
     * activity is started.  The monitor is added
     * after any existing ones; the monitor will be hit only if none of the
     * existing monitors can themselves handle the Intent.
     *
     * @param monitor The new ActivityMonitor to see.
     *
     * @see #addMonitor(IntentFilter, ActivityResult, boolean)
     * @see #checkMonitorHit
     */
    virtual CARAPI AddMonitor(
        /* [in] */ IInstrumentationActivityMonitor* monitor);

    /**
     * A convenience wrapper for {@link #addMonitor(ActivityMonitor)} that
     * creates an intent filter matching {@link ActivityMonitor} for you and
     * returns it.
     *
     * @param filter The set of intents this monitor is responsible for.
     * @param result A canned result to return if the monitor is hit; can
     *               be null.
     * @param block Controls whether the monitor should block the activity
     *              start (returning its canned result) or let the call
     *              proceed.
     *
     * @return The newly created and added activity monitor.
     *
     * @see #addMonitor(ActivityMonitor)
     * @see #checkMonitorHit
     */
    virtual CARAPI AddMonitor(
        /* [in] */ IIntentFilter* filter,
        /* [in] */ IInstrumentationActivityResult* result,
        /* [in] */ Boolean block,
        /* [out] */ IInstrumentationActivityMonitor** monitor);

    /**
     * A convenience wrapper for {@link #addMonitor(ActivityMonitor)} that
     * creates a class matching {@link ActivityMonitor} for you and returns it.
     *
     * @param cls The activity class this monitor is responsible for.
     * @param result A canned result to return if the monitor is hit; can
     *               be null.
     * @param block Controls whether the monitor should block the activity
     *              start (returning its canned result) or let the call
     *              proceed.
     *
     * @return The newly created and added activity monitor.
     *
     * @see #addMonitor(ActivityMonitor)
     * @see #checkMonitorHit
     */
    virtual CARAPI AddMonitor(
        /* [in] */ const String& cls,
        /* [in] */ IInstrumentationActivityResult* result,
        /* [in] */ Boolean block,
        /* [out] */ IInstrumentationActivityMonitor** monitor);

    /**
     * Test whether an existing {@link ActivityMonitor} has been hit.  If the
     * monitor has been hit at least <var>minHits</var> times, then it will be
     * removed from the activity monitor list and true returned.  Otherwise it
     * is left as-is and false is returned.
     *
     * @param monitor The ActivityMonitor to check.
     * @param minHits The minimum number of hits required.
     *
     * @return True if the hit count has been reached, else false.
     *
     * @see #addMonitor
     */
    virtual CARAPI CheckMonitorHit(
        /* [in] */ IInstrumentationActivityMonitor* monitor,
        /* [in] */ Int32 minHits,
        /* [out] */ Boolean* reached);

    /**
     * Wait for an existing {@link ActivityMonitor} to be hit.  Once the
     * monitor has been hit, it is removed from the activity monitor list and
     * the first created Activity object that matched it is returned.
     *
     * @param monitor The ActivityMonitor to wait for.
     *
     * @return The Activity object that matched the monitor.
     */
    virtual CARAPI WaitForMonitor(
        /* [in] */ IInstrumentationActivityMonitor* monitor,
        /* [out] */ IActivity** activity);

    /**
     * Wait for an existing {@link ActivityMonitor} to be hit till the timeout
     * expires.  Once the monitor has been hit, it is removed from the activity
     * monitor list and the first created Activity object that matched it is
     * returned.  If the timeout expires, a null object is returned.
     *
     * @param monitor The ActivityMonitor to wait for.
     * @param timeOut The timeout value in secs.
     *
     * @return The Activity object that matched the monitor.
     */
    virtual CARAPI WaitForMonitorWithTimeout(
        /* [in] */ IInstrumentationActivityMonitor* monitor,
        /* [in] */ Int64 timeOut,
        /* [out] */ IActivity** activity);

    /**
     * Remove an {@link ActivityMonitor} that was previously added with
     * {@link #addMonitor}.
     *
     * @param monitor The monitor to remove.
     *
     * @see #addMonitor
     */
    virtual CARAPI RemoveMonitor(
        /* [in] */ IInstrumentationActivityMonitor* monitor);

    /**
     * Execute a particular menu item.
     *
     * @param targetActivity The activity in question.
     * @param id The identifier associated with the menu item.
     * @param flag Additional flags, if any.
     * @return Whether the invocation was successful (for example, it could be
     *         false if item is disabled).
     */
    virtual CARAPI InvokeMenuActionSync(
        /* [in] */ IActivity* targetActivity,
        /* [in] */ Int32 id,
        /* [in] */ Int32 flag,
        /* [out] */ Boolean* success);

    /**
     * Show the context menu for the currently focused view and executes a
     * particular context menu item.
     *
     * @param targetActivity The activity in question.
     * @param id The identifier associated with the context menu item.
     * @param flag Additional flags, if any.
     * @return Whether the invocation was successful (for example, it could be
     *         false if item is disabled).
     */
    virtual CARAPI InvokeContextMenuAction(
        /* [in] */ IActivity* targetActivity,
        /* [in] */ Int32 id,
        /* [in] */ Int32 flag,
        /* [out] */ Boolean* success);

    /**
     * Sends the key events corresponding to the text to the app being
     * instrumented.
     *
     * @param text The text to be sent.
     */
    virtual CARAPI SendStringSync(
        /* [in] */ const String& text);

    /**
     * Send a key event to the currently focused window/view and wait for it to
     * be processed.  Finished at some point after the recipient has returned
     * from its event processing, though it may <em>not</em> have completely
     * finished reacting from the event -- for example, if it needs to update
     * its display as a result, it may still be in the process of doing that.
     *
     * @param event The event to send to the current focus.
     */
    virtual CARAPI SendKeySync(
        /* [in] */ IKeyEvent* event);

    /**
     * Sends an up and down key event sync to the currently focused window.
     *
     * @param key The integer keycode for the event.
     */
    virtual CARAPI SendKeyDownUpSync(
        /* [in] */ Int32 key);

    /**
     * Higher-level method for sending both the down and up key events for a
     * particular character key code.  Equivalent to creating both KeyEvent
     * objects by hand and calling {@link #sendKeySync}.  The event appears
     * as if it came from keyboard 0, the built in one.
     *
     * @param keyCode The key code of the character to send.
     */
    virtual CARAPI SendCharacterSync(
        /* [in] */ Int32 keyCode);

    /**
     * Dispatch a pointer event. Finished at some point after the recipient has
     * returned from its event processing, though it may <em>not</em> have
     * completely finished reacting from the event -- for example, if it needs
     * to update its display as a result, it may still be in the process of
     * doing that.
     *
     * @param event A motion event describing the pointer action.  (As noted in
     * {@link MotionEvent#obtain(long, long, int, float, float, int)}, be sure to use
     * {@link SystemClock#uptimeMillis()} as the timebase.
     */
    virtual CARAPI SendPointerSync(
        /* [in] */ IMotionEvent* event);

    /**
     * Dispatch a trackball event. Finished at some point after the recipient has
     * returned from its event processing, though it may <em>not</em> have
     * completely finished reacting from the event -- for example, if it needs
     * to update its display as a result, it may still be in the process of
     * doing that.
     *
     * @param event A motion event describing the trackball action.  (As noted in
     * {@link MotionEvent#obtain(long, long, int, float, float, int)}, be sure to use
     * {@link SystemClock#uptimeMillis()} as the timebase.
     */
    virtual CARAPI SendTrackballEventSync(
        /* [in] */ IMotionEvent* event);

    virtual CARAPI NewApplication(
        /* [in] */ IClassLoader* cl,
        /* [in] */ const String& className,
        /* [in] */ IContext* ctx,
        /* [out] */ IApplication** app);

    static CARAPI NewApplication(
        /* [in] */ IClassInfo* cls,
        /* [in] */ IContext* context,
        /* [out] */ IApplication** app);

    static CARAPI NewApplication(
        /* [in] */ const ClassID& clsid,
        /* [in] */ IContext* context,
        /* [out] */ IApplication** app);

    virtual CARAPI CallApplicationOnCreate(
        /* [in] */ IApplication* app);

    /**
     * Perform instantiation of the process's {@link Activity} object.  The
     * default implementation provides the normal system behavior.
     *
     * @param cl The ClassLoader with which to instantiate the object.
     * @param className The name of the class implementing the Activity
     *                  object.
     * @param intent The Intent object that specified the activity class being
     *               instantiated.
     *
     * @return The newly instantiated Activity object.
     */
    virtual CARAPI NewActivity(
        /* [in] */ IClassLoader* cl,
        /* [in] */ const String& className,
        /* [in] */ IIntent* intent,
        /* [out] */ IActivity** activity);

    /**
     * Perform instantiation of an {@link Activity} object.  This method is intended for use with
     * unit tests, such as android.test.ActivityUnitTestCase.  The activity will be useable
     * locally but will be missing some of the linkages necessary for use within the sytem.
     *
     * @param clazz The Class of the desired Activity
     * @param context The base context for the activity to use
     * @param token The token for this activity to communicate with
     * @param application The application object (if any)
     * @param intent The intent that started this Activity
     * @param info ActivityInfo from the manifest
     * @param title The title, typically retrieved from the ActivityInfo record
     * @param parent The parent Activity (if any)
     * @param id The embedded Id (if any)
     * @param lastNonConfigurationInstance Arbitrary object that will be
     * available via {@link Activity#getLastNonConfigurationInstance()
     * Activity.getLastNonConfigurationInstance()}.
     * @return Returns the instantiated activity
     * @throws InstantiationException
     * @throws IllegalAccessException
     */
    virtual CARAPI NewActivity(
        /* [in] */ IClassInfo* clazz,
        /* [in] */ IContext* context,
        /* [in] */ IBinder* token,
        /* [in] */ IApplication* application,
        /* [in] */ IIntent* intent,
        /* [in] */ IActivityInfo* info,
        /* [in] */ ICharSequence* title,
        /* [in] */ IActivity* parent,
        /* [in] */ const String& id,
        /* [in] */ IActivityNonConfigurationInstances* lastNonConfigurationInstance,
        /* [out] */ IActivity** activity);

    CARAPI PrePerformCreate(
        /* [in] */ IActivity* activity);

    CARAPI PostPerformCreate(
        /* [in] */ IActivity* activity);

    /**
     * Perform calling of an activity's {@link Activity#onCreate}
     * method.  The default implementation simply calls through to that method.
     *
     * @param activity The activity being created.
     * @param icicle The previously frozen state (or null) to pass through to onCreate().
     */
    CARAPI CallActivityOnCreate(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* icicle);

    /**
     * Perform calling of an activity's {@link Activity#onCreate}
     * method.  The default implementation simply calls through to that method.
     *  @param activity The activity being created.
     * @param icicle The previously frozen state (or null) to pass through to
     * @param persistentState The previously persisted state (or null)
     */
    CARAPI CallActivityOnCreate(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* icicle,
        /* [in] */ IPersistableBundle* persistentState);

    virtual CARAPI CallActivityOnDestroy(
        /* [in] */ IActivity* activity);

    virtual CARAPI CallActivityOnRestoreInstanceState(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Perform calling of an activity's {@link Activity#onRestoreInstanceState}
     * method.  The default implementation simply calls through to that method.
     *
     * @param activity The activity being restored.
     * @param savedInstanceState The previously saved state being restored.
     * @param persistentState The previously persisted state (or null)
     */
    CARAPI CallActivityOnRestoreInstanceState(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* savedInstanceState,
        /* [in] */ IPersistableBundle* persistentState);

    /**
     * Perform calling of an activity's {@link Activity#onPostCreate} method.
     * The default implementation simply calls through to that method.
     *
     * @param activity The activity being created.
     * @param icicle The previously frozen state (or null) to pass through to
     *               onPostCreate().
     */
    virtual CARAPI CallActivityOnPostCreate(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* icicle);

    /**
     * Perform calling of an activity's {@link Activity#onPostCreate} method.
     * The default implementation simply calls through to that method.
     *
     * @param activity The activity being created.
     * @param icicle The previously frozen state (or null) to pass through to
     *               onPostCreate().
     */
    virtual CARAPI CallActivityOnPostCreate(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* icicle,
        /* [in] */ IPersistableBundle* persistentState);

    virtual CARAPI CallActivityOnNewIntent(
        /* [in] */ IActivity* activity,
        /* [in] */ IIntent* intent);

    virtual CARAPI CallActivityOnStart(
        /* [in] */ IActivity* activity);

    virtual CARAPI CallActivityOnRestart(
        /* [in] */ IActivity* activity);

    virtual CARAPI CallActivityOnResume(
        /* [in] */ IActivity* activity);

    virtual CARAPI CallActivityOnStop(
        /* [in] */ IActivity* activity);

    virtual CARAPI CallActivityOnSaveInstanceState(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* outState);

    /**
     * Perform calling of an activity's {@link Activity#onSaveInstanceState}
     * method.  The default implementation simply calls through to that method.
     *  @param activity The activity being saved.
     * @param outState The bundle to pass to the call.
     * @param outPersistentState The persistent bundle to pass to the call.
     */
    virtual CARAPI CallActivityOnSaveInstanceState(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* outState,
        /* [in] */ IPersistableBundle* outPersistentState);

    virtual CARAPI CallActivityOnPause(
        /* [in] */ IActivity* activity);

    virtual CARAPI CallActivityOnUserLeaving(
        /* [in] */ IActivity* activity);

    /*
     * Starts allocation counting. This triggers a gc and resets the counts.
     */
    virtual CARAPI StartAllocCounting();

    /*
     * Stops allocation counting.
     */
    virtual CARAPI StopAllocCounting();

    /**
     * Returns a bundle with the current results from the allocation counting.
     */
    virtual CARAPI GetAllocCounts(
        /* [out] */ IBundle** bundle);

    /**
     * Returns a bundle with the counts for various binder counts for this process. Currently the only two that are
     * reported are the number of send and the number of received transactions.
     */
    virtual CARAPI GetBinderCounts(
        /* [out] */ IBundle** bundle);

    /**
     * Execute a startActivity call made by the application.  The default
     * implementation takes care of updating any active {@link ActivityMonitor}
     * objects and dispatches this call to the system activity manager; you can
     * override this to watch for the application to start an activity, and
     * modify what happens when it does.
     *
     * <p>This method returns an {@link ActivityResult} object, which you can
     * use when intercepting application calls to avoid performing the start
     * activity action but still return the result the application is
     * expecting.  To do this, override this method to catch the call to start
     * activity so that it returns a new ActivityResult containing the results
     * you would like the application to see, and don't call up to the super
     * class.  Note that an application is only expecting a result if
     * <var>requestCode</var> is &gt;= 0.
     *
     * <p>This method throws {@link android.content.ActivityNotFoundException}
     * if there was no Activity found to run the given Intent.
     *
     * @param who The Context from which the activity is being started.
     * @param contextThread The main thread of the Context from which the activity
     *                      is being started.
     * @param token Internal token identifying to the system who is starting
     *              the activity; may be null.
     * @param target Which activity is performing the start (and thus receiving
     *               any result); may be null if this call is not being made
     *               from an activity.
     * @param intent The actual Intent to start.
     * @param requestCode Identifier for this request's result; less than zero
     *                    if the caller is not expecting a result.
     * @param options Addition options.
     *
     * @return To force the return of a particular result, return an
     *         ActivityResult object containing the desired data; otherwise
     *         return null.  The default implementation always returns null.
     *
     * @throws android.content.ActivityNotFoundException
     *
     * @see Activity#startActivity(Intent)
     * @see Activity#startActivityForResult(Intent, int)
     * @see Activity#startActivityFromChild
     *
     * {@hide}
     */
    virtual CARAPI ExecStartActivity(
        /* [in] */ IContext* who,
        /* [in] */ IBinder* contextThread,
        /* [in] */ IBinder* token,
        /* [in] */ IActivity* target,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options,
        /* [out] */ IInstrumentationActivityResult** result);

    /**
     * Like {@link #execStartActivity(Context, IBinder, IBinder, Activity, Intent, int)},
     * but accepts an array of activities to be started.  Note that active
     * {@link ActivityMonitor} objects only match against the first activity in
     * the array.
     *
     * {@hide}
     */
    virtual CARAPI ExecStartActivities(
        /* [in] */ IContext* who,
        /* [in] */ IBinder* contextThread,
        /* [in] */ IBinder* token,
        /* [in] */ IActivity* target,
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ IBundle* options);

    /**
     * Like {@link #execStartActivity(Context, IBinder, IBinder, Activity, Intent, int)},
     * but accepts an array of activities to be started.  Note that active
     * {@link ActivityMonitor} objects only match against the first activity in
     * the array.
     *
     * {@hide}
     */
    virtual CARAPI ExecStartActivitiesAsUser(
        /* [in] */ IContext* who,
        /* [in] */ IBinder* contextThread,
        /* [in] */ IBinder* token,
        /* [in] */ IActivity* target,
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId);

    /**
     * Like {@link #execStartActivity(Context, IBinder, IBinder, Activity, Intent, int)},
     * but for calls from a {#link Fragment}.
     *
     * @param who The Context from which the activity is being started.
     * @param contextThread The main thread of the Context from which the activity
     *                      is being started.
     * @param token Internal token identifying to the system who is starting
     *              the activity; may be null.
     * @param target Which fragment is performing the start (and thus receiving
     *               any result).
     * @param intent The actual Intent to start.
     * @param requestCode Identifier for this request's result; less than zero
     *                    if the caller is not expecting a result.
     *
     * @return To force the return of a particular result, return an
     *         ActivityResult object containing the desired data; otherwise
     *         return null.  The default implementation always returns null.
     *
     * @throws android.content.ActivityNotFoundException
     *
     * @see Activity#startActivity(Intent)
     * @see Activity#startActivityForResult(Intent, int)
     * @see Activity#startActivityFromChild
     *
     * {@hide}
     */
    virtual CARAPI ExecStartActivity(
        /* [in] */ IContext* who,
        /* [in] */ IBinder* contextThread,
        /* [in] */ IBinder* token,
        /* [in] */ IFragment* target,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options,
        /* [out] */ IInstrumentationActivityResult** activityResult);

    /**
     * Like {@link #execStartActivity(Context, IBinder, IBinder, Activity, Intent, int)},
     * but for starting as a particular user.
     *
     * @param who The Context from which the activity is being started.
     * @param contextThread The main thread of the Context from which the activity
     *                      is being started.
     * @param token Internal token identifying to the system who is starting
     *              the activity; may be null.
     * @param target Which fragment is performing the start (and thus receiving
     *               any result).
     * @param intent The actual Intent to start.
     * @param requestCode Identifier for this request's result; less than zero
     *                    if the caller is not expecting a result.
     *
     * @return To force the return of a particular result, return an
     *         ActivityResult object containing the desired data; otherwise
     *         return null.  The default implementation always returns null.
     *
     * @throws android.content.ActivityNotFoundException
     *
     * @see Activity#startActivity(Intent)
     * @see Activity#startActivityForResult(Intent, int)
     * @see Activity#startActivityFromChild
     *
     * {@hide}
     */
    virtual CARAPI ExecStartActivity(
        /* [in] */ IContext* who,
        /* [in] */ IBinder* contextThread,
        /* [in] */ IBinder* token,
        /* [in] */ IActivity* target,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options,
        /* [in] */ IUserHandle* user,
        /* [out] */ IInstrumentationActivityResult** activityResult);

    /**
     * Special version!
     * @hide
     */
    CARAPI ExecStartActivityAsCaller(
        /* [in] */ IContext* who,
        /* [in] */ IBinder* contextThread,
        /* [in] */ IBinder* token,
        /* [in] */ IActivity* target,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ IInstrumentationActivityResult** activityResult);

    /**
     * Special version!
     * @hide
     */
    CARAPI ExecStartActivityFromAppTask(
        /* [in] */ IContext* who,
        /* [in] */ IBinder* contextThread,
        /* [in] */ IIAppTask* appTask,
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options);

    virtual CARAPI Init(
        /* [in] */ IActivityThread* thread,
        /* [in] */ IContext* instrContext,
        /* [in] */ IContext* appContext,
        /* [in] */ IComponentName* component,
        /* [in] */ IInstrumentationWatcher* watcher,
        /* [in] */ IIUiAutomationConnection* uiAutomationConnection);

    static CARAPI CheckStartActivityResult(
        /* [in] */ Int32 res,
        /* [in] */ IIntent* intent);

    /**
     * Gets the {@link UiAutomation} instance.
     * <p>
     * <strong>Note:</strong> The APIs exposed via the returned {@link UiAutomation}
     * work across application boundaries while the APIs exposed by the instrumentation
     * do not. For example, {@link Instrumentation#sendPointerSync(MotionEvent)} will
     * not allow you to inject the event in an app different from the instrumentation
     * target, while {@link UiAutomation#injectInputEvent(android.view.InputEvent, boolean)}
     * will work regardless of the current application.
     * </p>
     * <p>
     * A typical test case should be using either the {@link UiAutomation} or
     * {@link Instrumentation} APIs. Using both APIs at the same time is not
     * a mistake by itself but a client has to be aware of the APIs limitations.
     * </p>
     * @return The UI automation instance.
     *
     * @see UiAutomation
     */
    CARAPI GetUiAutomation(
        /* [out] */ IUiAutomation** ua);

private:
    ECO_LOCAL CARAPI_(void) AddValue(
        /* [in] */ const String& key,
        /* [in] */ Int32 value,
        /* [in] */ IBundle* results);

    ECO_LOCAL CARAPI ValidateNotAppThread();

private:
    ECO_LOCAL const static String TAG;

    Object mSync;
    IActivityThread* mThread;
    AutoPtr<IMessageQueue> mMessageQueue;
    AutoPtr<IContext> mInstrContext;
    AutoPtr<IContext> mAppContext;
    AutoPtr<IComponentName> mComponent;
    AutoPtr<IThread> mRunner;
    AutoPtr< List<AutoPtr<ActivityWaiter> > > mWaitingActivities;
    AutoPtr< List<AutoPtr<IInstrumentationActivityMonitor> > > mActivityMonitors;
    AutoPtr<IInstrumentationWatcher> mWatcher;
    AutoPtr<IIUiAutomationConnection> mUiAutomationConnection;
    Boolean mAutomaticPerformanceSnapshots;
   // AutoPtr<IPerformanceCollector> mPerformanceCollector;
    AutoPtr<IBundle> mPerfMetrics;

    AutoPtr<IUiAutomation> mUiAutomation;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __Instrumentation_H__
