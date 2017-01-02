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

#ifndef __ELASTOS_DROID_APP_ACTIVITY_TRANSITION_COORDINATOR_H__
#define __ELASTOS_DROID_APP_ACTIVITY_TRANSITION_COORDINATOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.View.h"
#define HASH_FOR_VIEW
#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/os/ResultReceiver.h"
#include "elastos/droid/transition/Transition.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ResultReceiver;
using Elastos::Droid::Transition::Transition;
using Elastos::Droid::Transition::ITransition;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::Widget::ImageViewScaleType;
using Elastos::Droid::Transition::Transition;
using Elastos::Droid::Transition::IEpicenterCallback;
using Elastos::Droid::Utility::IArrayMap;

using Elastos::Utility::ICollection;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Base class for ExitTransitionCoordinator and EnterTransitionCoordinator, classes
 * that manage activity transitions and the communications coordinating them between
 * Activities. The ExitTransitionCoordinator is created in the
 * ActivityOptions#makeSceneTransitionAnimation. The EnterTransitionCoordinator
 * is created by ActivityOptions#createEnterActivityTransition by Activity when the window is
 * attached.
 *
 * Typical startActivity goes like this:
 * 1) ExitTransitionCoordinator created with ActivityOptions#makeSceneTransitionAnimation
 * 2) Activity#startActivity called and that calls startExit() through
 *    ActivityOptions#dispatchStartExit
 *    - Exit transition starts by setting transitioning Views to INVISIBLE
 * 3) Launched Activity starts, creating an EnterTransitionCoordinator.
 *    - The Window is made translucent
 *    - The Window background alpha is set to 0
 *    - The transitioning views are made INVISIBLE
 *    - MSG_SET_LISTENER is sent back to the ExitTransitionCoordinator.
 * 4) The shared element transition completes.
 *    - MSG_TAKE_SHARED_ELEMENTS is sent to the EnterTransitionCoordinator
 * 5) The MSG_TAKE_SHARED_ELEMENTS is received by the EnterTransitionCoordinator.
 *    - Shared elements are made VISIBLE
 *    - Shared elements positions and size are set to match the end state of the calling
 *      Activity.
 *    - The shared element transition is started
 *    - If the window allows overlapping transitions, the views transition is started by setting
 *      the entering Views to VISIBLE and the background alpha is animated to opaque.
 *    - MSG_HIDE_SHARED_ELEMENTS is sent to the ExitTransitionCoordinator
 * 6) MSG_HIDE_SHARED_ELEMENTS is received by the ExitTransitionCoordinator
 *    - The shared elements are made INVISIBLE
 * 7) The exit transition completes in the calling Activity.
 *    - MSG_EXIT_TRANSITION_COMPLETE is sent to the EnterTransitionCoordinator.
 * 8) The MSG_EXIT_TRANSITION_COMPLETE is received by the EnterTransitionCoordinator.
 *    - If the window doesn't allow overlapping enter transitions, the enter transition is started
 *      by setting entering views to VISIBLE and the background is animated to opaque.
 * 9) The background opacity animation completes.
 *    - The window is made opaque
 * 10) The calling Activity gets an onStop() call
 *    - onActivityStopped() is called and all exited Views are made VISIBLE.
 *
 * Typical finishAfterTransition goes like this:
 * 1) finishAfterTransition() creates an ExitTransitionCoordinator and calls startExit()
 *    - The Window start transitioning to Translucent with a new ActivityOptions.
 *    - If no background exists, a black background is substituted
 *    - The shared elements in the scene are matched against those shared elements
 *      that were sent by comparing the names.
 *    - The exit transition is started by setting Views to INVISIBLE.
 * 2) The ActivityOptions is received by the Activity and an EnterTransitionCoordinator is created.
 *    - All transitioning views are made VISIBLE to reverse what was done when onActivityStopped()
 *      was called
 * 3) The Window is made translucent and a callback is received
 *    - The background alpha is animated to 0
 * 4) The background alpha animation completes
 * 5) The shared element transition completes
 *    - After both 4 & 5 complete, MSG_TAKE_SHARED_ELEMENTS is sent to the
 *      EnterTransitionCoordinator
 * 6) MSG_TAKE_SHARED_ELEMENTS is received by EnterTransitionCoordinator
 *    - Shared elements are made VISIBLE
 *    - Shared elements positions and size are set to match the end state of the calling
 *      Activity.
 *    - The shared element transition is started
 *    - If the window allows overlapping transitions, the views transition is started by setting
 *      the entering Views to VISIBLE.
 *    - MSG_HIDE_SHARED_ELEMENTS is sent to the ExitTransitionCoordinator
 * 7) MSG_HIDE_SHARED_ELEMENTS is received by the ExitTransitionCoordinator
 *    - The shared elements are made INVISIBLE
 * 8) The exit transition completes in the finishing Activity.
 *    - MSG_EXIT_TRANSITION_COMPLETE is sent to the EnterTransitionCoordinator.
 *    - finish() is called on the exiting Activity
 * 9) The MSG_EXIT_TRANSITION_COMPLETE is received by the EnterTransitionCoordinator.
 *    - If the window doesn't allow overlapping enter transitions, the enter transition is started
 *      by setting entering views to VISIBLE.
 */
class ActivityTransitionCoordinator
    : public ResultReceiver
    , public IActivityTransitionCoordinator
{
public:
    class SharedElementOriginalState
        : public Object
        , public ISharedElementOriginalState
    {
    public:
        CAR_INTERFACE_DECL()

        SharedElementOriginalState();

        CARAPI ToString(
            /* [out] */ String* str);

        Int32 mLeft;
        Int32 mTop;
        Int32 mRight;
        Int32 mBottom;
        Int32 mMeasuredWidth;
        Int32 mMeasuredHeight;
        ImageViewScaleType mScaleType;
        AutoPtr<IMatrix> mMatrix;
        Float mTranslationZ;
        Float mElevation;
    };

protected:
    class ContinueTransitionListener
        : public Elastos::Droid::Transition::Transition::TransitionListenerAdapter
    {
    public:
        ContinueTransitionListener(
            /* [in] */ ActivityTransitionCoordinator* host);

        CARAPI OnTransitionStart(
            /* [in] */ ITransition* transition);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        ActivityTransitionCoordinator* mHost;
    };

    class DecorViewOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        DecorViewOnPreDrawListener(
            /* [in] */ ActivityTransitionCoordinator* host,
            /* [in] */ IView* decorView,
            /* [in] */ IArrayList* snapshots);

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        ActivityTransitionCoordinator* mHost;
        AutoPtr<IView> mDecorView;
        AutoPtr<IArrayList> mSnapshots;
    };

    class GhostVisibilityOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        GhostVisibilityOnPreDrawListener(
            /* [in] */ ActivityTransitionCoordinator* host,
            /* [in] */ IView* decorView,
            /* [in] */ Int32 visibility);

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        ActivityTransitionCoordinator* mHost;
        AutoPtr<IView> mDecorView;
        Int32 mVisibility;
    };

private:
    class FixedEpicenterCallback
        : public Object
        , public IEpicenterCallback
    {
    public:
        CAR_INTERFACE_DECL()

        void SetEpicenter(
            /* [in] */ IRect* epicenter);

        CARAPI OnGetEpicenter(
            /* [in] */ ITransition* transition,
            /* [out] */ IRect** rect);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        AutoPtr<IRect> mEpicenter;
    };

    class GhostViewListeners
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        GhostViewListeners(
            /* [in] */ IView* view,
            /* [in] */ IView* parent,
            /* [in] */ IViewGroup* decor);

        AutoPtr<IView> GetView();

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        AutoPtr<IView> mView;
        AutoPtr<IViewGroup> mDecor;
        AutoPtr<IView> mParent;
        AutoPtr<IMatrix> mMatrix;// = new Matrix();
    };

public:
    CAR_INTERFACE_DECL()

    ActivityTransitionCoordinator();

    virtual ~ActivityTransitionCoordinator();

    CARAPI constructor(
        /* [in] */ IWindow* window,
        /* [in] */ IArrayList* allSharedElementNames, //ArrayList<String>
        /* [in] */ ISharedElementCallback* listener,
        /* [in] */ Boolean isReturning);

    CARAPI GetDecor(
        /* [out] */ IViewGroup** vg);

    CARAPI SetEpicenter(
        /* [in] */ IView* view);

    CARAPI GetAcceptedNames(
        /* [out] */ IArrayList** list); //ArrayList<String>

    CARAPI GetMappedNames(
        /* [out] */ IArrayList** list); //ArrayList<String>

    CARAPI CopyMappedViews(
        /* [out] */ IArrayList** list); //ArrayList<View>

    CARAPI GetAllSharedElementNames(
        /* [out] */ IArrayList** list); //ArrayList<String>

    static CARAPI_(Boolean) IsInTransitionGroup(
        /* [in] */ IViewParent* viewParent,
        /* [in] */ IViewGroup* decor);

protected:
    CARAPI ViewsReady(
        /* [in] */ IArrayMap* sharedElements); //ArrayMap<String, View>

    CARAPI StripOffscreenViews();

    CARAPI GetWindow(
        /* [out] */ IWindow** window);

    /**
     * Sets the transition epicenter to the position of the first shared element.
     */
    CARAPI SetEpicenter();

    CARAPI_(AutoPtr<ITransition>) SetTargets(
        /* [in] */ ITransition* transition,
        /* [in] */ Boolean add);

    CARAPI_(AutoPtr<ITransition>) ConfigureTransition(
        /* [in] */ ITransition* transition,
        /* [in] */ Boolean includeTransitioningViews);

    static CARAPI_(AutoPtr<ITransition>) MergeTransitions(
        /* [in] */ ITransition* transition1,
        /* [in] */ ITransition* transition2);

    CARAPI_(AutoPtr<IArrayMap>) MapSharedElements( //ArrayMap<String, View>
        /* [in] */ IArrayList* accepted, //ArrayList<String>
        /* [in] */ IArrayList* localViews); //ArrayList<View>

    CARAPI SetResultReceiver(
        /* [in] */ IResultReceiver* resultReceiver);

    virtual CARAPI_(AutoPtr<ITransition>) GetViewsTransition() = 0;

    CARAPI GetSharedElementParentMatrix(
        /* [in] */ IView* view,
        /* [in] */ IMatrix* matrix);

    CARAPI_(AutoPtr<IArrayList>) SetSharedElementState(
        /* [in] */ IBundle* sharedElementState,
        /* [in] */ IArrayList* snapshots); //ArrayList<View>

    CARAPI NotifySharedElementEnd(
        /* [in] */ IArrayList* snapshots); //ArrayList<View>

    CARAPI ScheduleSetSharedElementEnd(
        /* [in] */ IArrayList* snapshots); //ArrayList<View>

    CARAPI_(AutoPtr<IArrayList>) CreateSnapshots( //ArrayList<View>
        /* [in] */ IBundle* state,
        /* [in] */ ICollection* names); //Collection<String>

    static CARAPI SetOriginalSharedElementState(
        /* [in] */ IArrayList* sharedElements, //ArrayList<View>
        /* [in] */ IArrayList* originalState); //ArrayList<SharedElementOriginalState>

    CARAPI_(AutoPtr<IBundle>) CaptureSharedElementState();

    CARAPI ClearState();

    CARAPI_(Int64) GetFadeDuration();

    CARAPI HideViews(
        /* [in] */ IArrayList* views); //ArrayList<View>

    CARAPI ShowViews(
        /* [in] */ IArrayList* views, //ArrayList<View>
        /* [in] */ Boolean setTransitionAlpha);

    /**
     * Captures placement information for Views with a shared element name for
     * Activity Transitions.
     *
     * @param view           The View to capture the placement information for.
     * @param name           The shared element name in the target Activity to apply the placement
     *                       information for.
     * @param transitionArgs Bundle to store shared element placement information.
     * @param tempBounds     A temporary Rect for capturing the current location of views.
     */
    CARAPI CaptureSharedElementState(
        /* [in] */ IView* view,
        /* [in] */ const String& name,
        /* [in] */ IBundle* transitionArgs,
        /* [in] */ IMatrix* tempMatrix,
        /* [in] */ IRectF* tempBounds);

    CARAPI StartTransition(
        /* [in] */ IRunnable* runnable);

    CARAPI TransitionStarted();

    CARAPI MoveSharedElementsToOverlay();

    CARAPI_(Boolean) MoveSharedElementWithParent();

    virtual CARAPI MoveSharedElementsFromOverlay();

    CARAPI SetGhostVisibility(
        /* [in] */ Int32 visibility);

    CARAPI ScheduleGhostVisibilityChange(
        /* [in] */ Int32 visibility);

    CARAPI ToString(
        /* [out] */ String* str);
private:

    CARAPI SetSharedElementState(
        /* [in] */ IView* view,
        /* [in] */ const String& name,
        /* [in] */ IBundle* transitionArgs,
        /* [in] */ IMatrix* tempMatrix,
        /* [in] */ IRectF* tempRect,
        /* [in] */ ArrayOf<Int32>* decorLoc);

    static AutoPtr<SharedElementOriginalState> GetOldSharedElementState(
        /* [in] */ IView* view,
        /* [in] */ const String& name,
        /* [in] */ IBundle* transitionArgs);

    CARAPI ShowView(
        /* [in] */ IView* view,
        /* [in] */ Boolean setTransitionAlpha);

    static Int32 ScaleTypeToInt(
        /* [in] */ ImageViewScaleType scaleType);

protected:
    static const String KEY_SCREEN_LEFT;// = "shared_element:screenLeft";
    static const String KEY_SCREEN_TOP;// = "shared_element:screenTop";
    static const String KEY_SCREEN_RIGHT;// = "shared_element:screenRight";
    static const String KEY_SCREEN_BOTTOM;//= "shared_element:screenBottom";
    static const String KEY_TRANSLATION_Z;// = "shared_element:translationZ";
    static const String KEY_SNAPSHOT;// = "shared_element:bitmap";
    static const String KEY_SCALE_TYPE;// = "shared_element:scaleType";
    static const String KEY_IMAGE_MATRIX;// = "shared_element:imageMatrix";
    static const String KEY_ELEVATION;// = "shared_element:elevation";

    static const AutoPtr<ArrayOf<ImageViewScaleType> > SCALE_TYPE_VALUES;// = ImageView.ScaleType.values();

    /**
     * For Activity transitions, the called Activity's listener to receive calls
     * when transitions complete.
     */
    static const String KEY_REMOTE_RECEIVER;// = "android:remoteReceiver";

    AutoPtr<IArrayList> mAllSharedElementNames; //ArrayList<String>
    AutoPtr<IArrayList> mSharedElements;// = new ArrayList<View>();
    AutoPtr<IArrayList> mSharedElementNames;// = new ArrayList<String>();
    AutoPtr<IArrayList> mTransitioningViews;// = new ArrayList<View>();

    AutoPtr<ISharedElementCallback> mListener;
    AutoPtr<IResultReceiver> mResultReceiver;

    Boolean mIsReturning;

private:
    static const String TAG;// = "ActivityTransitionCoordinator";

    AutoPtr<IWindow> mWindow;

    AutoPtr<FixedEpicenterCallback> mEpicenterCallback;// = new FixedEpicenterCallback();
    AutoPtr<IRunnable> mPendingTransition;
    Boolean mIsStartingTransition;
    List<AutoPtr<GhostViewListeners> > mGhostViewListeners;
    HashMap<AutoPtr<IView>, Float> mOriginalAlphas;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ACTIVITY_TRANSITION_COORDINATOR_H__