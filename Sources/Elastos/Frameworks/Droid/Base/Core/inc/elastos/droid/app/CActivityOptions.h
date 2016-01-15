
#ifndef __ELASTOS_DROID_APP_CACTIVITYOPTIONS_H__
#define __ELASTOS_DROID_APP_CACTIVITYOPTIONS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_CActivityOptions.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IRemoteCallback;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindow;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace App {

class AnimationStartedListenerRunnable
    : public Runnable
{
public:
    AnimationStartedListenerRunnable(
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener);

    virtual CARAPI Run();

private:
    AutoPtr<IActivityOptionsOnAnimationStartedListener> mListener;
};

CarClass(CActivityOptions)
    , public Object
    , public IActivityOptions
{
private:
    class AnimationStartedListener
        : public Object
        , public IRemoteCallback
    {
    public:
        CAR_INTERFACE_DECL()

        AnimationStartedListener(
            /* [in] */ IHandler* handler,
            /* [in] */ IActivityOptionsOnAnimationStartedListener* listener);

        CARAPI SendResult(
            /* [in] */ IBundle* data);

        CARAPI ToString(
            /* [out]*/ String* str);

    private:
        AutoPtr<IHandler> mHandler;
        AutoPtr<IActivityOptionsOnAnimationStartedListener> mListener;
    };
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActivityOptions();

    virtual ~CActivityOptions();

    CARAPI constructor();

    /** @hide */
    CARAPI constructor(
        /* [in] */ IBundle* opts);

    CARAPI GetPackageName(
        /* [out] */ String* pkgName);

    CARAPI SetPackageName(
        /* [in] */ const String& pkgName);

    /** @hide */
    CARAPI GetAnimationType(
        /* [out] */ Int32* type);

    CARAPI SetAnimationType(
        /* [in] */ Int32 type);

    /** @hide */
    CARAPI GetCustomEnterResId(
        /* [out] */ Int32* id);

    CARAPI SetCustomEnterResId(
        /* [in] */ Int32 id);

    /** @hide */
    CARAPI GetCustomExitResId(
        /* [out] */ Int32* id);

    CARAPI SetCustomExitResId(
        /* [in] */ Int32 id);

    /** @hide */
    CARAPI GetThumbnail(
        /* [out] */ IBitmap** thumbnail);

    CARAPI SetThumbnail(
        /* [in] */ IBitmap* thumbnail);

    /** @hide */
    CARAPI GetStartX(
        /* [out] */ Int32* x);

    CARAPI SetStartX(
        /* [in] */ Int32 x);

    /** @hide */
    CARAPI GetStartY(
        /* [out] */ Int32* y);

    CARAPI SetStartY(
        /* [in] */ Int32 y);

    /** @hide */
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    CARAPI SetWidth(
        /* [in] */ Int32 width);

    /** @hide */
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    CARAPI SetHeight(
        /* [in] */ Int32 height);

    /** @hide */
    CARAPI GetOnAnimationStartListener(
        /* [out] */ IRemoteCallback** cb);

    CARAPI SetOnAnimationStartListener(
        /* [in] */ IRemoteCallback* cb);

    CARAPI GetExitCoordinatorKey(
        /* [out] */ Int32* key);

    /** @hide */
    CARAPI Abort();

    /** @hide */
    CARAPI IsReturning(
        /* [out] */ Boolean* bval);

    /** @hide */
    CARAPI GetSharedElementNames(
        /* [out] */ IArrayList** list);

    /** @hide */
    CARAPI GetResultReceiver(
        /* [out] */ IResultReceiver** receiver);

    /** @hide */
    CARAPI GetResultCode(
        /* [out] */ Int32* code);

    /** @hide */
    CARAPI GetResultData(
        /* [out] */ IIntent** intent);

    /**
     * Update the current values in this ActivityOptions from those supplied
     * in <var>otherOptions</var>.  Any values
     * defined in <var>otherOptions</var> replace those in the base options.
     */
    CARAPI Update(
        /* [in] */ IActivityOptions* otherOptions);

    /**
     * Returns the created options as a Bundle, which can be passed to
     * {@link android.content.Context#startActivity(android.content.Intent, android.os.Bundle)
     * Context.startActivity(Intent, Bundle)} and related methods.
     * Note that the returned Bundle is still owned by the ActivityOptions
     * object; you must not modify it, but can supply it to the startActivity
     * methods that take an options Bundle.
     */
    CARAPI ToBundle(
        /* [out] */ IBundle** bundle);

public:
    /**
     * Create an ActivityOptions specifying a custom animation to run when
     * the activity is displayed.
     *
     * @param context Who is defining this.  This is the application that the
     * animation resources will be loaded from.
     * @param enterResId A resource ID of the animation resource to use for
     * the incoming activity.  Use 0 for no animation.
     * @param exitResId A resource ID of the animation resource to use for
     * the outgoing activity.  Use 0 for no animation.
     * @return Returns a new ActivityOptions object that you can use to
     * supply these options as the options Bundle when starting an activity.
     */
    static CARAPI MakeCustomAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Int32 enterResId,
        /* [in] */ Int32 exitResId,
        /* [out] */ IActivityOptions** options);

    /**
     * Create an ActivityOptions specifying a custom animation to run when
     * the activity is displayed.
     *
     * @param context Who is defining this.  This is the application that the
     * animation resources will be loaded from.
     * @param enterResId A resource ID of the animation resource to use for
     * the incoming activity.  Use 0 for no animation.
     * @param exitResId A resource ID of the animation resource to use for
     * the outgoing activity.  Use 0 for no animation.
     * @param handler If <var>listener</var> is non-null this must be a valid
     * Handler on which to dispatch the callback; otherwise it should be null.
     * @param listener Optional OnAnimationStartedListener to find out when the
     * requested animation has started running.  If for some reason the animation
     * is not executed, the callback will happen immediately.
     * @return Returns a new ActivityOptions object that you can use to
     * supply these options as the options Bundle when starting an activity.
     * @hide
     */
    static CARAPI MakeCustomAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Int32 enterResId,
        /* [in] */ Int32 exitResId,
        /* [in] */ IHandler* handler,
        /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
        /* [out] */ IActivityOptions** options);

    /**
     * Create an ActivityOptions specifying an animation where the new
     * activity is scaled from a small originating area of the screen to
     * its final full representation.
     *
     * <p>If the Intent this is being used with has not set its
     * android.content.Intent#setSourceBounds Intent.setSourceBounds,
     * those bounds will be filled in for you based on the initial
     * bounds passed in here.
     *
     * @sa android.content.Intent#setSourceBounds Intent.setSourceBounds
     *
     * @param source The View that the new activity is animating from.  This
     * defines the coordinate space for <var>startX</var> and <var>startY</var>.
     * @param startX The x starting location of the new activity, relative to <var>source</var>.
     * @param startY The y starting location of the activity, relative to <var>source</var>.
     * @param startWidth The initial width of the new activity.
     * @param startHeight The initial height of the new activity.
     * @return Returns a new ActivityOptions object that you can use to
     * supply these options as the options Bundle when starting an activity.
     */
    static CARAPI MakeScaleUpAnimation(
        /* [in] */ IView* source,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 startWidth,
        /* [in] */ Int32 startHeight,
        /* [out] */ IActivityOptions** options);

    /**
     * Create an ActivityOptions specifying an animation where a thumbnail
     * is scaled from a given position to the new activity window that is
     * being started.
     *
     * <p>If the Intent this is being used with has not set its
     * android.content.Intent#setSourceBounds Intent.setSourceBounds,
     * those bounds will be filled in for you based on the initial
     * thumbnail location and size provided here.
     *
     * @sa android.content.Intent#setSourceBounds Intent.setSourceBounds
     *
     * @param source The View that this thumbnail is animating from.  This
     * defines the coordinate space for <var>startX</var> and <var>startY</var>.
     * @param thumbnail The bitmap that will be shown as the initial thumbnail
     * of the animation.
     * @param startX The x starting location of the bitmap, relative to <var>source</var>.
     * @param startY The y starting location of the bitmap, relative to <var>source</var>.
     * @return Returns a new ActivityOptions object that you can use to
     * supply these options as the options Bundle when starting an activity.
     */
    static CARAPI MakeThumbnailScaleUpAnimation(
        /* [in] */ IView* source,
        /* [in] */ IBitmap* thumbnail,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [out] */ IActivityOptions** options);

    /**
     * Create an ActivityOptions specifying an animation where a thumbnail
     * is scaled from a given position to the new activity window that is
     * being started.
     *
     * @param source The View that this thumbnail is animating from.  This
     * defines the coordinate space for <var>startX</var> and <var>startY</var>.
     * @param thumbnail The bitmap that will be shown as the initial thumbnail
     * of the animation.
     * @param startX The x starting location of the bitmap, relative to <var>source</var>.
     * @param startY The y starting location of the bitmap, relative to <var>source</var>.
     * @param listener Optional OnAnimationStartedListener to find out when the
     * requested animation has started running.  If for some reason the animation
     * is not executed, the callback will happen immediately.
     * @return Returns a new ActivityOptions object that you can use to
     * supply these options as the options Bundle when starting an activity.
     * @hide
     */
    static CARAPI MakeThumbnailScaleUpAnimation(
        /* [in] */ IView* source,
        /* [in] */ IBitmap* thumbnail,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
        /* [out] */ IActivityOptions** options);

    /**
     * Create an ActivityOptions specifying an animation where an activity window
     * is scaled from a given position to a thumbnail at a specified location.
     *
     * @param source The View that this thumbnail is animating to.  This
     * defines the coordinate space for <var>startX</var> and <var>startY</var>.
     * @param thumbnail The bitmap that will be shown as the final thumbnail
     * of the animation.
     * @param startX The x end location of the bitmap, relative to <var>source</var>.
     * @param startY The y end location of the bitmap, relative to <var>source</var>.
     * @param listener Optional OnAnimationStartedListener to find out when the
     * requested animation has started running.  If for some reason the animation
     * is not executed, the callback will happen immediately.
     * @return Returns a new ActivityOptions object that you can use to
     * supply these options as the options Bundle when starting an activity.
     * @hide
     */
    static CARAPI MakeThumbnailScaleDownAnimation(
        /* [in] */ IView* source,
        /* [in] */ IBitmap* thumbnail,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
        /* [out] */ IActivityOptions** options);

    /** @hide */
    static CARAPI Abort(
        /* [in] */ IBundle* options);

    CARAPI SetOnAnimationStartedListener(
        /* [in] */ IHandler* handler,
        /* [in] */ IActivityOptionsOnAnimationStartedListener* listener);

    /**
     * Create an ActivityOptions specifying an animation where the new activity
     * window and a thumbnail is aspect-scaled to a new location.
     *
     * @param source The View that this thumbnail is animating from.  This
     * defines the coordinate space for <var>startX</var> and <var>startY</var>.
     * @param thumbnail The bitmap that will be shown as the initial thumbnail
     * of the animation.
     * @param startX The x starting location of the bitmap, relative to <var>source</var>.
     * @param startY The y starting location of the bitmap, relative to <var>source</var>.
     * @param listener Optional OnAnimationStartedListener to find out when the
     * requested animation has started running.  If for some reason the animation
     * is not executed, the callback will happen immediately.
     * @return Returns a new ActivityOptions object that you can use to
     * supply these options as the options Bundle when starting an activity.
     * @hide
     */
    static CARAPI MakeThumbnailAspectScaleUpAnimation(
        /* [in] */ IView* source,
        /* [in] */ IBitmap* thumbnail,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 targetWidth,
        /* [in] */ Int32 targetHeight,
        /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
        /* [out] */ IActivityOptions** options);

    /**
     * Create an ActivityOptions specifying an animation where the new activity
     * window and a thumbnail is aspect-scaled to a new location.
     *
     * @param source The View that this thumbnail is animating to.  This
     * defines the coordinate space for <var>startX</var> and <var>startY</var>.
     * @param thumbnail The bitmap that will be shown as the final thumbnail
     * of the animation.
     * @param startX The x end location of the bitmap, relative to <var>source</var>.
     * @param startY The y end location of the bitmap, relative to <var>source</var>.
     * @param listener Optional OnAnimationStartedListener to find out when the
     * requested animation has started running.  If for some reason the animation
     * is not executed, the callback will happen immediately.
     * @return Returns a new ActivityOptions object that you can use to
     * supply these options as the options Bundle when starting an activity.
     * @hide
     */
    static CARAPI MakeThumbnailAspectScaleDownAnimation(
        /* [in] */ IView* source,
        /* [in] */ IBitmap* thumbnail,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 targetWidth,
        /* [in] */ Int32 targetHeight,
        /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
        /* [out] */ IActivityOptions** options);


    /**
     * Create an ActivityOptions to transition between Activities using cross-Activity scene
     * animations. This method carries the position of one shared element to the started Activity.
     * The position of <code>sharedElement</code> will be used as the epicenter for the
     * exit Transition. The position of the shared element in the launched Activity will be the
     * epicenter of its entering Transition.
     *
     * <p>This requires {@link android.view.Window#FEATURE_ACTIVITY_TRANSITIONS} to be
     * enabled on the calling Activity to cause an exit transition. The same must be in
     * the called Activity to get an entering transition.</p>
     * @param activity The Activity whose window contains the shared elements.
     * @param sharedElement The View to transition to the started Activity.
     * @param sharedElementName The shared element name as used in the target Activity. This
     *                          must not be null.
     * @return Returns a new ActivityOptions object that you can use to
     *         supply these options as the options Bundle when starting an activity.
     * @see android.transition.Transition#setEpicenterCallback(
     *          android.transition.Transition.EpicenterCallback)
     */
    static CARAPI MakeSceneTransitionAnimation(
        /* [in] */ IActivity* activity,
        /* [in] */ IView* sharedElement,
        /* [in] */ const String& sharedElementName,
        /* [out] */ IActivityOptions** options);

    /**
     * Create an ActivityOptions to transition between Activities using cross-Activity scene
     * animations. This method carries the position of multiple shared elements to the started
     * Activity. The position of the first element in sharedElements
     * will be used as the epicenter for the exit Transition. The position of the associated
     * shared element in the launched Activity will be the epicenter of its entering Transition.
     *
     * <p>This requires {@link android.view.Window#FEATURE_ACTIVITY_TRANSITIONS} to be
     * enabled on the calling Activity to cause an exit transition. The same must be in
     * the called Activity to get an entering transition.</p>
     * @param activity The Activity whose window contains the shared elements.
     * @param sharedElements The names of the shared elements to transfer to the called
     *                       Activity and their associated Views. The Views must each have
     *                       a unique shared element name.
     * @return Returns a new ActivityOptions object that you can use to
     *         supply these options as the options Bundle when starting an activity.
     * @see android.transition.Transition#setEpicenterCallback(
     *          android.transition.Transition.EpicenterCallback)
     */
    static CARAPI MakeSceneTransitionAnimation(
        /* [in] */ IActivity* activity,
        /* [in] */ ArrayOf<IPair*>* pairs,
        /* [out] */ IActivityOptions** options);

    /** @hide */
    static CARAPI MakeSceneTransitionAnimation(
        /* [in] */ IActivity* activity,
        /* [in] */ IExitTransitionCoordinator* exitCoordinator,
        /* [in] */ IArrayList* sharedElementNames,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* resultData,
        /* [out] */ IActivityOptions** options);

    /**
     * If set along with Intent.FLAG_ACTIVITY_NEW_DOCUMENT then the task being launched will not be
     * presented to the user but will instead be only available through the recents task list.
     * In addition, the new task wil be affiliated with the launching activity's task.
     * Affiliated tasks are grouped together in the recents task list.
     *
     * <p>This behavior is not supported for activities with {@link
     * android.R.styleable#AndroidManifestActivity_launchMode launchMode} values of
     * <code>singleInstance</code> or <code>singleTask</code>.
     */
    static CARAPI MakeTaskLaunchBehind(
        /* [out] */ IActivityOptions** options);

    /** @hide */
    CARAPI GetLaunchTaskBehind(
        /* [out] */ Boolean* bebind);

    /**
     * Return the filtered options only meant to be seen by the target activity itself
     * @hide
     */
    CARAPI ForTargetActivity(
        /* [out] */ IActivityOptions** options);

private:
    static CARAPI MakeThumbnailAnimation(
        /* [in] */ IView* source,
        /* [in] */ IBitmap* thumbnail,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
        /* [in] */ Boolean scaleUp,
        /* [out] */ IActivityOptions** options);

    static CARAPI MakeAspectScaledThumbnailAnimation(
        /* [in] */ IView* source,
        /* [in] */ IBitmap* thumbnail,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 targetWidth,
        /* [in] */ Int32 targetHeight,
        /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
        /* [in] */ Boolean scaleUp,
        /* [out] */ IActivityOptions** options);

private:
    static const String TAG;

    /**
     * For Activity transitions, the calling Activity's TransitionListener used to
     * notify the called Activity when the shared element and the exit transitions
     * complete.
     */
    static const String KEY_TRANSITION_COMPLETE_LISTENER;//= "android:transitionCompleteListener";

    static const String KEY_TRANSITION_IS_RETURNING;// = "android:transitionIsReturning";
    static const String KEY_TRANSITION_SHARED_ELEMENTS;// = "android:sharedElementNames";
    static const String KEY_RESULT_DATA;// = "android:resultData";
    static const String KEY_RESULT_CODE;// = "android:resultCode";
    static const String KEY_EXIT_COORDINATOR_INDEX;// = "android:exitCoordinatorIndex";


    String mPackageName;
    Int32 mAnimationType;
    Int32 mCustomEnterResId;
    Int32 mCustomExitResId;
    AutoPtr<IBitmap> mThumbnail;
    Int32 mStartX;
    Int32 mStartY;
    Int32 mWidth;
    Int32 mHeight;
    AutoPtr<IRemoteCallback> mAnimationStartedListener;
    AutoPtr<IResultReceiver> mTransitionReceiver;
    Boolean mIsReturning;
    AutoPtr<IArrayList> mSharedElementNames;
    AutoPtr<IIntent> mResultData;
    Int32 mResultCode;
    Int32 mExitCoordinatorIndex;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYOPTIONS_H__
