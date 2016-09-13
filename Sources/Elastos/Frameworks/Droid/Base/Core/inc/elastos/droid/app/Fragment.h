#ifndef __ELASTOS_DROID_APP_FRAGMENT_H__
#define __ELASTOS_DROID_APP_FRAGMENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IContextMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IContextMenuInfo;
using Elastos::Droid::View::IViewOnCreateContextMenuListener;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Transition::ITransition;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentCallbacks;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::ITypedArray;

using Elastos::Core::ICharSequence;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::IHashMap;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace App {

class FragmentState
    : public Object
    , public IFragmentState
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    FragmentState();

    virtual ~FragmentState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IFragment* frag);

    CARAPI Instantiate(
        /* [in] */ IActivity* activity,
        /* [in] */ IFragment* parent,
        /* [out] */ IFragment** fragment);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetClassName(
        /* [out] */ String* name);

    CARAPI GetIndex(
        /* [out] */ Int32* result);

    CARAPI IsFromLayout(
        /* [out] */ Boolean* result);

    CARAPI GetFragmentId(
        /* [out] */ Int32* result);

    CARAPI GetContainerId(
        /* [out] */ Int32* result);

    CARAPI GetTag(
        /* [out] */ String* tag);

    CARAPI IsRetainInstance(
        /* [out] */ Boolean* result);

    CARAPI IsDetached(
        /* [out] */ Boolean* result);

    CARAPI GetArguments(
        /* [out] */ IBundle** arguments);

    CARAPI GetSavedFragmentState(
        /* [out] */ IBundle** state);

    CARAPI GetInstance(
        /* [out] */ IFragment** fragment);
public:
    String mClassName;
    Int32 mIndex;
    Boolean mFromLayout;
    Int32 mFragmentId;
    Int32 mContainerId;
    String mTag;
    Boolean mRetainInstance;
    Boolean mDetached;
    AutoPtr<IBundle> mArguments;

    AutoPtr<IBundle> mSavedFragmentState;

    AutoPtr<IFragment> mInstance;
};

class ECO_PUBLIC Fragment
    : public Object
    , public IFragment
    , public IComponentCallbacks
    , public IComponentCallbacks2
    , public IViewOnCreateContextMenuListener
{
private:
    class ECO_LOCAL ViewCreateContextMenuListener
        : public Object
        , public IViewOnCreateContextMenuListener
    {
    public:
        CAR_INTERFACE_DECL()

        ViewCreateContextMenuListener(
            /* [in] */ Fragment* host);

        CARAPI OnCreateContextMenu(
            /* [in] */ IContextMenu* menu,
            /* [in] */ IView* v,
            /* [in] */ IContextMenuInfo* menuInfo);

    private:
        Fragment* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    Fragment();

    virtual ~Fragment();

    CARAPI constructor();

    CARAPI GetState(
        /* [out] */ Int32* state);

    CARAPI SetState(
        /* [in] */ Int32 state);

    CARAPI GetAnimatingAway(
        /* [out] */ IAnimator** animator);

    CARAPI SetAnimatingAway(
        /* [in] */ IAnimator* animator);

    CARAPI GetStateAfterAnimating(
        /* [out] */ Int32* state);

    CARAPI SetStateAfterAnimating(
        /* [in] */ Int32 state);

    CARAPI GetSavedFragmentState(
        /* [out] */ IBundle** fState);

    CARAPI SetSavedFragmentState(
        /* [in] */ IBundle* fState);

    CARAPI GetSavedViewState(
        /* [out] */ ISparseArray** viewState);

    CARAPI SetSavedViewState(
        /* [in] */ ISparseArray* viewState);

    CARAPI GetIndex(
        /* [out] */ Int32* index);

    CARAPI GetWho(
        /* [out] */ String* who);

    CARAPI SetWho(
        /* [in] */ const String& who);

    CARAPI GetTarget(
        /* [out] */ IFragment** target);

    CARAPI SetTarget(
        /* [in] */ IFragment* target);

    CARAPI GetTargetIndex(
        /* [out] */ Int32* index);

    CARAPI SetTargetIndex(
        /* [in] */ Int32 index);

    CARAPI SetTargetRequestCode(
        /* [in] */ Int32 code);

    CARAPI GetAdded(
        /* [out] */ Boolean* added);

    CARAPI SetAdded(
        /* [in] */ Boolean added);

    CARAPI SetRemoving(
        /* [in] */ Boolean removing);

    CARAPI SetResumed(
        /* [in] */ Boolean resumed);

    CARAPI GetFromLayout(
        /* [out] */ Boolean* fLayout);

    CARAPI SetFromLayout(
        /* [in] */ Boolean fLayout);

    CARAPI SetInLayout(
        /* [in] */ Boolean inLayout);

    CARAPI GetRestored(
        /* [out] */ Boolean* restored);

    CARAPI SetRestored(
        /* [in] */ Boolean restored);

    CARAPI GetBackStackNesting(
        /* [out] */ Int32* bsNesting);

    CARAPI SetBackStackNesting(
        /* [in] */ Int32 bsNesting);

    CARAPI SetFragmentManager(
        /* [in] */ IFragmentManagerImpl* fManager);

    CARAPI SetActivity(
        /* [in] */ IActivity* activity);

    CARAPI SetParentFragment(
        /* [in] */ IFragment* pFragment);

    CARAPI GetFragmentId(
        /* [out] */ Int32* fid);

    CARAPI SetFragmentId(
        /* [in] */ Int32 fid);

    CARAPI GetContainerId(
        /* [out] */ Int32* cid);

    CARAPI SetContainerId(
        /* [in] */ Int32 cid);

    CARAPI SetTag(
        /* [in] */ const String& tag);

    CARAPI SetHidden(
        /* [in] */ Boolean hidden);

    CARAPI SetDetached(
        /* [in] */ Boolean detached);

    CARAPI GetRetaining(
        /* [out] */ Boolean* retaining);

    CARAPI SetRetaining(
        /* [in] */ Boolean retaining);

    CARAPI GetHasMenu(
        /* [out] */ Boolean* hasMenu);

    CARAPI SetHasMenu(
        /* [in] */ Boolean hasMenu);

    CARAPI GetMenuVisible(
        /* [out] */ Boolean* visible);

    CARAPI SetMenuVisible(
        /* [in] */ Boolean visible);

    CARAPI GetCalled(
        /* [out] */ Boolean* called);

    CARAPI SetCalled(
        /* [in] */ Boolean called);

    CARAPI GetNextAnim(
        /* [out] */ Int32* anim);

    CARAPI SetNextAnim(
        /* [in] */ Int32 anim);

    CARAPI GetContainer(
        /* [out] */ IViewGroup** container);

    CARAPI SetContainer(
        /* [in] */ IViewGroup* container);

    CARAPI SetView(
        /* [in] */ IView* view);

    CARAPI GetDeferStart(
        /* [out] */ Boolean* start);

    CARAPI SetDeferStart(
        /* [in] */ Boolean start);

    CARAPI SetLoaderManager(
        /* [in] */ ILoaderManagerImpl* lManager);

    CARAPI GetLoaderManagerValue(
        /* [out] */ ILoaderManagerImpl** lManager);

    CARAPI GetLoadersStarted(
        /* [out] */ Boolean* started);

    CARAPI SetLoadersStarted(
        /* [in] */ Boolean started);

    CARAPI GetCheckedForLoaderManager(
        /* [out] */ Boolean* cfManager);

    CARAPI SetCheckedForLoaderManager(
        /* [in] */ Boolean cfManager);

    static CARAPI Instantiate(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname,
        /* [out] */ IFragment** fragment);

    static CARAPI Instantiate(
        /* [in] */ IContext* context,
        /* [in] */ const String& fname,
        /* [in] */ IBundle* args,
        /* [out] */ IFragment** fragment);

    CARAPI RestoreViewState(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI SetIndex(
        /* [in] */ Int32 index,
        /* [in] */ IFragment* parent);

    CARAPI IsInBackStack(
        /* [out] */ Boolean* inbs);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* equal);

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI GetId(
        /* [out] */ Int32* id);

    CARAPI GetTag(
        /* [out] */ String* tag);

    CARAPI SetArguments(
        /* [in] */ IBundle* args);

    CARAPI GetArguments(
        /* [out] */ IBundle** args);

    CARAPI SetInitialSavedState(
        /* [in] */ IFragmentSavedState* state);

    CARAPI SetTargetFragment(
        /* [in] */ IFragment* fragment,
        /* [in] */ Int32 requestCode);

    CARAPI GetTargetFragment(
        /* [out] */ IFragment** fragment);

    CARAPI GetTargetRequestCode(
        /* [out] */ Int32* code);

    CARAPI GetActivity(
        /* [out] */ IActivity** activity);

    CARAPI GetResources(
        /* [out] */ IResources** resources);

    CARAPI GetText(
        /* [in] */ Int32 resId,
        /* [out] */ ICharSequence** text);

    CARAPI GetString(
        /* [in] */ Int32 resId,
        /* [out] */ String* string);

    CARAPI GetString(
        /* [in] */ Int32 resId,
        /* [in] */ ArrayOf<IInterface*>* formatArgs,
        /* [out] */ String* string);

    CARAPI GetFragmentManager(
        /* [out] */ IFragmentManager** manager);

    CARAPI GetChildFragmentManager(
        /* [out] */ IFragmentManager** manager);

    CARAPI GetParentFragment(
        /* [out] */ IFragment** fragment);

    CARAPI IsAdded(
        /* [out] */ Boolean* added);

    CARAPI IsDetached(
        /* [out] */ Boolean* detached);

    CARAPI IsRemoving(
        /* [out] */ Boolean* removing);

    CARAPI IsInLayout(
        /* [out] */ Boolean* inlayout);

    CARAPI IsResumed(
        /* [out] */ Boolean* resumed);

    CARAPI IsVisible(
        /* [out] */ Boolean* visible);

    CARAPI IsHidden(
        /* [out] */ Boolean* hidden);

    CARAPI OnHiddenChanged(
        /* [in] */ Boolean hidden);

    CARAPI SetRetainInstance(
        /* [in] */ Boolean retain);

    CARAPI GetRetainInstance(
        /* [out] */ Boolean* retainInstance);

    CARAPI SetHasOptionsMenu(
        /* [in] */ Boolean hasMenu);

    CARAPI SetMenuVisibility(
        /* [in] */ Boolean menuVisible);

    CARAPI SetUserVisibleHint(
        /* [in] */ Boolean isVisibleToUser);

    CARAPI GetUserVisibleHint(
        /* [out] */ Boolean* hint);

    CARAPI GetLoaderManager(
        /* [out] */ ILoaderManager** manager);

    CARAPI StartActivity(
        /* [in] */ IIntent* intent);

    CARAPI StartActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options);

    CARAPI StartActivityForResult(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode);

    CARAPI StartActivityForResult(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options);

    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    CARAPI GetLayoutInflater(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ ILayoutInflater** inflater);

    CARAPI OnInflate(
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnInflate(
        /* [in] */ IActivity* activity,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnAttach(
        /* [in] */ IActivity* activity);

    CARAPI OnCreateAnimator(
        /* [in] */ Int32 transit,
        /* [in] */ Boolean enter,
        /* [in] */ Int32 nextAnim,
        /* [out] */ IAnimator** animator);

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    CARAPI OnViewCreated(
        /* [in] */ IView* view,
        /* [in] */ IBundle* savedInstanceState);

    CARAPI GetView(
        /* [out] */ IView** view);

    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnViewStateRestored(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnLowMemory();

    CARAPI OnTrimMemory(
        /* [in] */ Int32 level);

    CARAPI OnDestroyView();

    CARAPI OnDestroy();

    CARAPI InitState();

    CARAPI OnDetach();

    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu);

    CARAPI OnDestroyOptionsMenu();

    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* selected);

    CARAPI OnOptionsMenuClosed(
        /* [in] */ IMenu* menu);

    CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuInfo* menuInfo);

    CARAPI RegisterForContextMenu(
        /* [in] */ IView* view);

    CARAPI UnregisterForContextMenu(
        /* [in] */ IView* view);

    CARAPI OnContextItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* selected);

    /**
     * When custom transitions are used with Fragments, the enter transition callback
     * is called when this Fragment is attached or detached when not popping the back stack.
     *
     * @param callback Used to manipulate the shared element transitions on this Fragment
     *                 when added not as a pop from the back stack.
     */
    CARAPI SetEnterSharedElementCallback(
        /* [in] */ ISharedElementCallback* callback);

    /**
     * @hide
     */
    CARAPI SetEnterSharedElementTransitionCallback(
        /* [in] */ ISharedElementCallback* callback);

    /**
     * When custom transitions are used with Fragments, the exit transition callback
     * is called when this Fragment is attached or detached when popping the back stack.
     *
     * @param callback Used to manipulate the shared element transitions on this Fragment
     *                 when added as a pop from the back stack.
     */
    CARAPI SetExitSharedElementCallback(
        /* [in] */ ISharedElementCallback* callback);

    /**
     * @hide
     */
    CARAPI SetExitSharedElementTransitionCallback(
        /* [in] */ ISharedElementCallback* callback);

    /**
     * Sets the Transition that will be used to move Views into the initial scene. The entering
     * Views will be those that are regular Views or ViewGroups that have
     * {@link ViewGroup#isTransitionGroup} return true. Typical Transitions will extend
     * {@link android.transition.Visibility} as entering is governed by changing visibility from
     * {@link View#INVISIBLE} to {@link View#VISIBLE}. If <code>transition</code> is null,
     * entering Views will remain unaffected.
     *
     * @param transition The Transition to use to move Views into the initial Scene.
     * @attr ref android.R.styleable#Fragment_fragmentEnterTransition
     */
    CARAPI SetEnterTransition(
        /* [in] */ ITransition* transition);

    /**
     * Returns the Transition that will be used to move Views into the initial scene. The entering
     * Views will be those that are regular Views or ViewGroups that have
     * {@link ViewGroup#isTransitionGroup} return true. Typical Transitions will extend
     * {@link android.transition.Visibility} as entering is governed by changing visibility from
     * {@link View#INVISIBLE} to {@link View#VISIBLE}.
     *
     * @return the Transition to use to move Views into the initial Scene.
     * @attr ref android.R.styleable#Fragment_fragmentEnterTransition
     */
    CARAPI GetEnterTransition(
        /* [out] */ ITransition** transition);

    /**
     * Sets the Transition that will be used to move Views out of the scene when the Fragment is
     * preparing to be removed, hidden, or detached because of popping the back stack. The exiting
     * Views will be those that are regular Views or ViewGroups that have
     * {@link ViewGroup#isTransitionGroup} return true. Typical Transitions will extend
     * {@link android.transition.Visibility} as entering is governed by changing visibility from
     * {@link View#VISIBLE} to {@link View#INVISIBLE}. If <code>transition</code> is null,
     * entering Views will remain unaffected. If nothing is set, the default will be to
     * use the same value as set in {@link #setEnterTransition(android.transition.Transition)}.
     *
     * @param transition The Transition to use to move Views out of the Scene when the Fragment
     *                   is preparing to close.
     * @attr ref android.R.styleable#Fragment_fragmentExitTransition
     */
    CARAPI SetReturnTransition(
        /* [in] */ ITransition* transition);

    /**
     * Returns the Transition that will be used to move Views out of the scene when the Fragment is
     * preparing to be removed, hidden, or detached because of popping the back stack. The exiting
     * Views will be those that are regular Views or ViewGroups that have
     * {@link ViewGroup#isTransitionGroup} return true. Typical Transitions will extend
     * {@link android.transition.Visibility} as entering is governed by changing visibility from
     * {@link View#VISIBLE} to {@link View#INVISIBLE}. If <code>transition</code> is null,
     * entering Views will remain unaffected.
     *
     * @return the Transition to use to move Views out of the Scene when the Fragment
     *         is preparing to close.
     * @attr ref android.R.styleable#Fragment_fragmentExitTransition
     */
    CARAPI GetReturnTransition(
        /* [out] */ ITransition** transition);

    /**
     * Sets the Transition that will be used to move Views out of the scene when the
     * fragment is removed, hidden, or detached when not popping the back stack.
     * The exiting Views will be those that are regular Views or ViewGroups that
     * have {@link ViewGroup#isTransitionGroup} return true. Typical Transitions will extend
     * {@link android.transition.Visibility} as exiting is governed by changing visibility
     * from {@link View#VISIBLE} to {@link View#INVISIBLE}. If transition is null, the views will
     * remain unaffected.
     *
     * @param transition The Transition to use to move Views out of the Scene when the Fragment
     *                   is being closed not due to popping the back stack.
     * @attr ref android.R.styleable#Fragment_fragmentExitTransition
     */
    CARAPI SetExitTransition(
        /* [in] */ ITransition* transition);

    /**
     * Returns the Transition that will be used to move Views out of the scene when the
     * fragment is removed, hidden, or detached when not popping the back stack.
     * The exiting Views will be those that are regular Views or ViewGroups that
     * have {@link ViewGroup#isTransitionGroup} return true. Typical Transitions will extend
     * {@link android.transition.Visibility} as exiting is governed by changing visibility
     * from {@link View#VISIBLE} to {@link View#INVISIBLE}. If transition is null, the views will
     * remain unaffected.
     *
     * @return the Transition to use to move Views out of the Scene when the Fragment
     *         is being closed not due to popping the back stack.
     * @attr ref android.R.styleable#Fragment_fragmentExitTransition
     */
    CARAPI GetExitTransition(
        /* [out] */ ITransition** transition);

    /**
     * Sets the Transition that will be used to move Views in to the scene when returning due
     * to popping a back stack. The entering Views will be those that are regular Views
     * or ViewGroups that have {@link ViewGroup#isTransitionGroup} return true. Typical Transitions
     * will extend {@link android.transition.Visibility} as exiting is governed by changing
     * visibility from {@link View#VISIBLE} to {@link View#INVISIBLE}. If transition is null,
     * the views will remain unaffected. If nothing is set, the default will be to use the same
     * transition as {@link #setExitTransition(android.transition.Transition)}.
     *
     * @param transition The Transition to use to move Views into the scene when reentering from a
     *                   previously-started Activity.
     * @attr ref android.R.styleable#Fragment_fragmentReenterTransition
     */
    CARAPI SetReenterTransition(
        /* [in] */ ITransition* transition);

    /**
     * Returns the Transition that will be used to move Views in to the scene when returning due
     * to popping a back stack. The entering Views will be those that are regular Views
     * or ViewGroups that have {@link ViewGroup#isTransitionGroup} return true. Typical Transitions
     * will extend {@link android.transition.Visibility} as exiting is governed by changing
     * visibility from {@link View#VISIBLE} to {@link View#INVISIBLE}. If transition is null,
     * the views will remain unaffected. If nothing is set, the default will be to use the same
     * transition as {@link #setExitTransition(android.transition.Transition)}.
     *
     * @return the Transition to use to move Views into the scene when reentering from a
     *                   previously-started Activity.
     * @attr ref android.R.styleable#Fragment_fragmentReenterTransition
     */
    CARAPI GetReenterTransition(
        /* [out] */ ITransition** transition);

    /**
     * Sets the Transition that will be used for shared elements transferred into the content
     * Scene. Typical Transitions will affect size and location, such as
     * {@link android.transition.ChangeBounds}. A null
     * value will cause transferred shared elements to blink to the final position.
     *
     * @param transition The Transition to use for shared elements transferred into the content
     *                   Scene.
     * @attr ref android.R.styleable#Fragment_fragmentSharedElementEnterTransition
     */
    CARAPI SetSharedElementEnterTransition(
        /* [in] */ ITransition* transition);

    /**
     * Returns the Transition that will be used for shared elements transferred into the content
     * Scene. Typical Transitions will affect size and location, such as
     * {@link android.transition.ChangeBounds}. A null
     * value will cause transferred shared elements to blink to the final position.
     *
     * @return The Transition to use for shared elements transferred into the content
     *                   Scene.
     * @attr ref android.R.styleable#Fragment_fragmentSharedElementEnterTransition
     */
    CARAPI GetSharedElementEnterTransition(
        /* [out] */ ITransition** transition);

    /**
     * Sets the Transition that will be used for shared elements transferred back during a
     * pop of the back stack. This Transition acts in the leaving Fragment.
     * Typical Transitions will affect size and location, such as
     * {@link android.transition.ChangeBounds}. A null
     * value will cause transferred shared elements to blink to the final position.
     * If no value is set, the default will be to use the same value as
     * {@link #setSharedElementEnterTransition(android.transition.Transition)}.
     *
     * @param transition The Transition to use for shared elements transferred out of the content
     *                   Scene.
     * @attr ref android.R.styleable#Fragment_fragmentSharedElementReturnTransition
     */
    CARAPI SetSharedElementReturnTransition(
        /* [in] */ ITransition* transition);

    /**
     * Return the Transition that will be used for shared elements transferred back during a
     * pop of the back stack. This Transition acts in the leaving Fragment.
     * Typical Transitions will affect size and location, such as
     * {@link android.transition.ChangeBounds}. A null
     * value will cause transferred shared elements to blink to the final position.
     * If no value is set, the default will be to use the same value as
     * {@link #setSharedElementEnterTransition(android.transition.Transition)}.
     *
     * @return The Transition to use for shared elements transferred out of the content
     *                   Scene.
     * @attr ref android.R.styleable#Fragment_fragmentSharedElementReturnTransition
     */
    CARAPI GetSharedElementReturnTransition(
        /* [out] */ ITransition** transition);

    /**
     * Sets whether the the exit transition and enter transition overlap or not.
     * When true, the enter transition will start as soon as possible. When false, the
     * enter transition will wait until the exit transition completes before starting.
     *
     * @param allow true to start the enter transition when possible or false to
     *              wait until the exiting transition completes.
     * @attr ref android.R.styleable#Fragment_fragmentAllowEnterTransitionOverlap
     */
    CARAPI SetAllowEnterTransitionOverlap(
        /* [in] */ Boolean allow);

    /**
     * Returns whether the the exit transition and enter transition overlap or not.
     * When true, the enter transition will start as soon as possible. When false, the
     * enter transition will wait until the exit transition completes before starting.
     *
     * @return true when the enter transition should start as soon as possible or false to
     * when it should wait until the exiting transition completes.
     * @attr ref android.R.styleable#Fragment_fragmentAllowEnterTransitionOverlap
     */
    CARAPI GetAllowEnterTransitionOverlap(
        /* [out] */ Boolean* allow);

    /**
     * Sets whether the the return transition and reenter transition overlap or not.
     * When true, the reenter transition will start as soon as possible. When false, the
     * reenter transition will wait until the return transition completes before starting.
     *
     * @param allow true to start the reenter transition when possible or false to wait until the
     *              return transition completes.
     * @attr ref android.R.styleable#Fragment_fragmentAllowReturnTransitionOverlap
     */
    CARAPI SetAllowReturnTransitionOverlap(
        /* [in] */ Boolean allow);

    /**
     * Returns whether the the return transition and reenter transition overlap or not.
     * When true, the reenter transition will start as soon as possible. When false, the
     * reenter transition will wait until the return transition completes before starting.
     *
     * @return true to start the reenter transition when possible or false to wait until the
     *         return transition completes.
     * @attr ref android.R.styleable#Fragment_fragmentAllowReturnTransitionOverlap
     */
    CARAPI GetAllowReturnTransitionOverlap(
        /* [out] */ Boolean* allow);

    CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

    CARAPI FindFragmentByWho(
        /* [in] */ const String& who,
        /* [out] */ IFragment** fragment);

    CARAPI_(void) InstantiateChildFragmentManager();

    CARAPI PerformCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI PerformCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** v);

    CARAPI PerformActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI PerformStart();

    CARAPI PerformResume();

    CARAPI PerformConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI PerformLowMemory();

    CARAPI PerformTrimMemory(
        /* [in] */ Int32 level);

    CARAPI PerformCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater,
        /* [out] */ Boolean* result);

    CARAPI PerformPrepareOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    CARAPI PerformOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* selected);

    CARAPI PerformContextItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* selected);

    CARAPI PerformOptionsMenuClosed(
        /* [in] */ IMenu* menu);

    CARAPI PerformSaveInstanceState(
        /* [in] */ IBundle* outState);

    CARAPI PerformPause();

    CARAPI PerformStop();

    CARAPI PerformDestroyView();

    CARAPI PerformDestroy();

    static AutoPtr<ITransition> LoadTransition(
        /* [in] */ IContext* context,
        /* [in] */ ITypedArray* typedArray,
        /* [in] */ ITransition* currentValue,
        /* [in] */ ITransition* defaultValue,
        /* [in] */ Int32 id);

public:
    static const AutoPtr<ITransition> USE_DEFAULT_TRANSITION;// = new TransitionSet();;

    Int32 mState;

    // Non-null if the fragment's view hierarchy is currently animating away,
    // meaning we need to wait a bit on completely destroying it.  This is the
    // animation that is running.
    AutoPtr<IAnimator> mAnimatingAway;

    // If mAnimatingAway != null, this is the state we should move to once the
    // animation is done.
    Int32 mStateAfterAnimating;

    // When instantiated from saved state, this is the saved state.
    AutoPtr<IBundle> mSavedFragmentState;
    AutoPtr<ISparseArray> mSavedViewState;

    // Index into active fragment array.
    Int32 mIndex;

    // Internal unique name for this fragment;
    String mWho;

    // Construction arguments;
    AutoPtr<IBundle> mArguments;

    // Target fragment.
    AutoPtr<IFragment> mTarget;

    // For use when retaining a fragment: this is the index of the last mTarget.
    Int32 mTargetIndex;

    // Target request code.
    Int32 mTargetRequestCode;

    // True if the fragment is in the list of added fragments.
    Boolean mAdded;

    // If set this fragment is being removed from its activity.
    Boolean mRemoving;

    // True if the fragment is in the resumed state.
    Boolean mResumed;

    // Set to true if this fragment was instantiated from a layout file.
    Boolean mFromLayout;

    // Set to true when the view has actually been inflated in its layout.
    Boolean mInLayout;

    // True if this fragment has been restored from previously saved state.
    Boolean mRestored;

    // Number of active back stack entries this fragment is in.
    Int32 mBackStackNesting;

    // The fragment manager we are associated with.  Set as soon as the
    // fragment is used in a transaction; cleared after it has been removed
    // from all transactions.
    AutoPtr<IFragmentManagerImpl> mFragmentManager;

    // Activity this fragment is attached to.
    IActivity* mActivity;

    // Private fragment manager for child fragments inside of this one.
    AutoPtr<IFragmentManagerImpl> mChildFragmentManager;

    // If this Fragment is contained in another Fragment, this is that container.
    IFragment* mParentFragment;

    // The optional identifier for this fragment -- either the container ID if it
    // was dynamically added to the view hierarchy, or the ID supplied in
    // layout.
    Int32 mFragmentId;

    // When a fragment is being dynamically added to the view hierarchy, this
    // is the identifier of the parent container it is being added to.
    Int32 mContainerId;

    // The optional named tag for this fragment -- usually used to find
    // fragments that are not part of the layout.
    String mTag;

    // Set to true when the app has requested that this fragment be hidden
    // from the user.
    Boolean mHidden;

    // Set to true when the app has requested that this fragment be detached.
    Boolean mDetached;

    // If set this fragment would like its instance retained across
    // configuration changes.
    Boolean mRetainInstance;

    // If set this fragment is being retained across the current config change.
    Boolean mRetaining;

    // If set this fragment has menu items to contribute.
    Boolean mHasMenu;

    // Set to true to allow the fragment's menu to be shown.
    Boolean mMenuVisible;

    // Used to verify that subclasses call through to super class.
    Boolean mCalled;

    // If app has requested a specific animation, this is the one to use.
    Int32 mNextAnim;

    // The parent container of the fragment after dynamically added to UI.
    AutoPtr<IViewGroup> mContainer;

    // The View generated for this fragment.
    AutoPtr<IView> mView;

    // Whether this fragment should defer starting until after other fragments
    // have been started and their loaders are finished.
    Boolean mDeferStart;

    // Hint provided by the app that this fragment is currently visible to the user.
    Boolean mUserVisibleHint;

    AutoPtr<ILoaderManagerImpl> mLoaderManager;
    Boolean mLoadersStarted;
    Boolean mCheckedForLoaderManager;

    AutoPtr<ITransition> mEnterTransition;// = null;
    AutoPtr<ITransition> mReturnTransition;// = USE_DEFAULT_TRANSITION;
    AutoPtr<ITransition> mExitTransition;// = null;
    AutoPtr<ITransition> mReenterTransition;// = USE_DEFAULT_TRANSITION;
    AutoPtr<ITransition> mSharedElementEnterTransition;// = null;
    AutoPtr<ITransition> mSharedElementReturnTransition;// = USE_DEFAULT_TRANSITION;
    Boolean mAllowReturnTransitionOverlap;
    Boolean mAllowEnterTransitionOverlap;

    AutoPtr<ISharedElementCallback> mEnterTransitionCallback;// = SharedElementCallback.NULL_CALLBACK;
    AutoPtr<ISharedElementCallback> mExitTransitionCallback;// = SharedElementCallback.NULL_CALLBACK;

protected:
    AutoPtr<IViewOnCreateContextMenuListener> mViewCreateContextMenuListener;

private:
    ECO_LOCAL static HashMap<String, AutoPtr<IClassInfo> > sClassMap;

#if defined(_DEBUG)
     Boolean mIsConstructed;
#endif
};

} // namespace App
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::App::FragmentState, IInterface)

#endif //__ELASTOS_DROID_APP_FRAGMENT_H__
