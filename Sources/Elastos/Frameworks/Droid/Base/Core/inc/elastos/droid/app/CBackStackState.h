#ifndef __ELASTOS_DROID_APP_CBACKSTACKSTATE_H__
#define __ELASTOS_DROID_APP_CBACKSTACKSTATE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Transition.h"
#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_App_CBackStackState.h"
#include <elastos/core/Object.h>
#include "elastos/droid/app/FragmentTransaction.h"

using Elastos::Droid::Transition::ITransition;
using Elastos::Droid::Transition::IEpicenterCallback;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::IPair;

using Elastos::Core::IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace App {

class FragmentManagerImpl;

CarClass(CBackStackState)
    , public Object
    , public IBackStackState
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBackStackState();

    virtual ~CBackStackState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IFragmentManagerImpl* fm,
        /* [in] */ IBackStackRecord* bse);

    CARAPI Instantiate(
        /* [in] */ IFragmentManagerImpl* fm,
        /* [out] */ IBackStackRecord** record);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

public:
    AutoPtr< ArrayOf<Int32> > mOps;
    Int32 mTransition;
    Int32 mTransitionStyle;
    String mName;
    Int32 mIndex;
    Int32 mBreadCrumbTitleRes;
    AutoPtr<ICharSequence> mBreadCrumbTitleText;
    Int32 mBreadCrumbShortTitleRes;
    AutoPtr<ICharSequence> mBreadCrumbShortTitleText;

    AutoPtr<IArrayList> mSharedElementSourceNames; //ArrayList<String>
    AutoPtr<IArrayList> mSharedElementTargetNames; //ArrayList<String>
};

/**
 * @hide Entry of an operation on the fragment back stack.
 */
class BackStackRecord
    : public FragmentTransaction
    , public IBackStackRecord
    , public IRunnable
    , public IFragmentManagerBackStackEntry
{
private:
    class AddTargetsOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("BackStackRecord::AddTargetsOnPreDrawListener")

        AddTargetsOnPreDrawListener(
            /* [in] */ BackStackRecord* host,
            /* [in] */ IBackStackRecordTransitionState* state,
            /* [in] */ ITransition* enterTransition,
            /* [in] */ ITransition* sharedElementTransition,
            /* [in] */ ITransition* overallTransition,
            /* [in] */ IView* container,
            /* [in] */ IFragment* inFragment,
            /* [in] */ IFragment* outFragment,
            /* [in] */ IArrayList* /*<View>*/ hiddenFragmentViews,
            /* [in] */ Boolean isBack,
            /* [in] */ IArrayList* /*<View>*/ sharedElementTargets,
            /* [in] */ IArrayList* /*<View>*/ enteringViews);

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

    private:
        BackStackRecord* mHost;
        AutoPtr<IBackStackRecordTransitionState> mState;
        AutoPtr<ITransition> mEnterTransition;
        AutoPtr<ITransition> mSharedElementTransition;
        AutoPtr<ITransition> mOverallTransition;
        AutoPtr<IView> mContainer;
        AutoPtr<IFragment> mInFragment;
        AutoPtr<IFragment> mOutFragment;
        AutoPtr<IArrayList> /*<View>*/ mHiddenFragmentViews;
        Boolean mIsBack;
        AutoPtr<IArrayList> /*<View>*/ mSharedElementTargets;
        AutoPtr<IArrayList> /*<View>*/ mEnteringViews;
    };

    class RemoveTargetsOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("BackStackRecord::RemoveTargetsOnPreDrawListener")

        RemoveTargetsOnPreDrawListener(
            /* [in] */ BackStackRecord* host,
            /* [in] */ IViewGroup* sceneRoot,
            /* [in] */ IView* nonExistingView,
            /* [in] */ ITransition* enterTransition,
            /* [in] */ IArrayList* /*<View>*/ enteringViews,
            /* [in] */ ITransition* exitTransition,
            /* [in] */ IArrayList* /*<View>*/ exitingViews,
            /* [in] */ ITransition* sharedElementTransition,
            /* [in] */ IArrayList* /*<View>*/ sharedElementTargets,
            /* [in] */ ITransition* overallTransition,
            /* [in] */ IArrayList* /*<View>*/ hiddenViews);

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

    private:
        BackStackRecord* mHost;
        AutoPtr<IViewGroup> mSceneRoot;
        AutoPtr<IView> mNonExistingView;
        AutoPtr<ITransition> mEnterTransition;
        AutoPtr<IArrayList> /*<View>*/ mEnteringViews;
        AutoPtr<ITransition> mExitTransition;
        AutoPtr<IArrayList> /*<View>*/ mExitingViews;
        AutoPtr<ITransition> mSharedElementTransition;
        AutoPtr<IArrayList> /*<View>*/ mSharedElementTargets;
        AutoPtr<ITransition> mOverallTransition;
        AutoPtr<IArrayList> /*<View>*/ mHiddenViews;
    };

    class EpicenterCallback
        : public Object
        , public IEpicenterCallback
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("BackStackRecord::EpicenterCallback")

        EpicenterCallback(
            /* [in] */ IRect* epicenter);

        CARAPI OnGetEpicenter(
            /* [in] */ ITransition* transition,
            /* [out] */ IRect** result);

    private:
        AutoPtr<IRect> mEpicenter;
    };


    class SharedElementEpicenterCallback
        : public Object
        , public IEpicenterCallback
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("BackStackRecord::SharedElementEpicenterCallback")

        SharedElementEpicenterCallback(
            /* [in] */ IBackStackRecordTransitionState* state);

        CARAPI OnGetEpicenter(
            /* [in] */ ITransition* transition,
            /* [out] */ IRect** result);

    private:
        AutoPtr<IBackStackRecordTransitionState> mState;
        AutoPtr<IRect> mEpicenter;
    };

public:
    class Op : public Object
    {
    public:
        Op()
            : mCmd(0)
            , mEnterAnim(0)
            , mExitAnim(0)
            , mPopEnterAnim(0)
            , mPopExitAnim(0)
        {}

    public:
        AutoPtr<Op> mNext;
        AutoPtr<Op> mPrev;
        Int32 mCmd;
        AutoPtr<IFragment> mFragment;
        Int32 mEnterAnim;
        Int32 mExitAnim;
        Int32 mPopEnterAnim;
        Int32 mPopExitAnim;
        AutoPtr<IList> mRemoved; //List<IFragment>
    };

public:
    CAR_INTERFACE_DECL()

    BackStackRecord();

    virtual ~BackStackRecord();

    CARAPI constructor(
        /* [in] */ IFragmentManagerImpl* manager);

    CARAPI ToString(
        /* [out] */ String* str);

    virtual CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

    virtual CARAPI GetId(
        /* [out] */ Int32* id);

    virtual CARAPI GetBreadCrumbTitleRes(
        /* [out] */ Int32* titleRes);

    virtual CARAPI GetBreadCrumbShortTitleRes(
        /* [out] */ Int32* shortTitleRes);

    virtual CARAPI GetBreadCrumbTitle(
        /* [out] */ ICharSequence** title);

    virtual CARAPI GetBreadCrumbShortTitle(
        /* [out] */ ICharSequence** shortTitle);

    virtual CARAPI Add(
        /* [in] */ IFragment* fragment,
        /* [in] */ const String& tag);

    virtual CARAPI Add(
        /* [in] */ Int32 containerViewId,
        /* [in] */ IFragment* fragment);

    virtual CARAPI Add(
        /* [in] */ Int32 containerViewId,
        /* [in] */ IFragment* fragment,
        /* [in] */ const String& tag);

    virtual CARAPI Replace(
        /* [in] */ Int32 containerViewId,
        /* [in] */ IFragment* fragment);

    virtual CARAPI Replace(
        /* [in] */ Int32 containerViewId,
        /* [in] */ IFragment* fragment,
        /* [in] */ const String& tag);

    virtual CARAPI Remove(
        /* [in] */ IFragment* fragment);

    virtual CARAPI Hide(
        /* [in] */ IFragment* fragment);

    virtual CARAPI Show(
        /* [in] */ IFragment* fragment);

    virtual CARAPI Detach(
        /* [in] */ IFragment* fragment);

    virtual CARAPI Attach(
        /* [in] */ IFragment* fragment);

    virtual CARAPI SetCustomAnimations(
        /* [in] */ Int32 enter,
        /* [in] */ Int32 exit);

    virtual CARAPI SetCustomAnimations(
        /* [in] */ Int32 enter,
        /* [in] */ Int32 exit,
        /* [in] */ Int32 popEnter,
        /* [in] */ Int32 popExit);

    virtual CARAPI SetTransition(
        /* [in] */ Int32 transition);

    //@Override
    CARAPI AddSharedElement(
        /* [in] */ IView* sharedElement,
        /* [in] */ const String& name);

    /** TODO: remove this */
    //@Override
    CARAPI SetSharedElement(
        /* [in] */ IView* sharedElement,
        /* [in] */ const String& name);

    /** TODO: remove this */
    //@Override
    CARAPI SetSharedElements(
        /* [in] */ ArrayOf<IPair*>* sharedElements);//* Pair<View, String>... sharedElements*/

    virtual CARAPI SetTransitionStyle(
        /* [in] */ Int32 styleRes);

    virtual CARAPI AddToBackStack(
        /* [in] */ const String& name);

    virtual CARAPI IsAddToBackStackAllowed(
        /* [out] */ Boolean* allowed);

    virtual CARAPI DisallowAddToBackStack();

    virtual CARAPI SetBreadCrumbTitle(
        /* [in] */ Int32 res);

    virtual CARAPI SetBreadCrumbTitle(
        /* [in] */ ICharSequence* text);

    virtual CARAPI SetBreadCrumbShortTitle(
        /* [in] */ Int32 res);

    virtual CARAPI SetBreadCrumbShortTitle(
        /* [in] */ ICharSequence* text);

    virtual CARAPI Commit(
        /* [out] */ Int32* identifier);

    virtual CARAPI CommitAllowingStateLoss(
        /* [out] */ Int32* res);

    virtual CARAPI Run();

    virtual CARAPI PopFromBackStack(
        /* [in] */ Boolean doStateMove,
        /* [in] */ IBackStackRecordTransitionState* state,
        /* [in] */ IHashMap* firstOutFragments, //SparseArray<Fragment>
        /* [in] */ IHashMap* lastInFragments, //SparseArray<Fragment>
        /* [out] */ IBackStackRecordTransitionState** result);

    virtual CARAPI GetName(
        /* [out] */ String* name);

    virtual CARAPI GetTransition(
        /* [out] */ Int32* trans);

    virtual CARAPI GetTransitionStyle(
        /* [out] */ Int32* transStyle);

    virtual CARAPI IsEmpty(
        /* [out] */ Boolean* empty);

protected:
    CARAPI_(void) Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ Boolean full);

    CARAPI_(void) AddOp(
        /* [in] */ Op* op);

    CARAPI_(void) BumpBackStackNesting(
        /* [in] */ Int32 amt);

    CARAPI_(Int32) CommitInternal(
        /* [in] */ Boolean allowStateLoss);

private:
     CARAPI DoAddOp(
        /* [in] */ Int32 containerViewId,
        /* [in] */ IFragment* fragment,
        /* [in] */ const String& tag,
        /* [in] */ Int32 opcmd);

    static CARAPI_(void) SetFirstOut(
        /* [in] */ IHashMap* fragments, //SparseArray<Fragment>
        /* [in] */ IFragment* fragment);

    CARAPI_(void) SetLastIn(
        /* [in] */ IHashMap* fragments, //SparseArray<Fragment>
        /* [in] */ IFragment* fragment);
    /**
     * Finds the first removed fragment and last added fragments when going forward.
     * If none of the fragments have transitions, then both lists will be empty.
     *
     * @param firstOutFragments The list of first fragments to be removed, keyed on the
     *                          container ID. This list will be modified by the method.
     * @param lastInFragments The list of last fragments to be added, keyed on the
     *                        container ID. This list will be modified by the method.
     */
    CARAPI_(void) CalculateFragments(
        /* [in] */ IHashMap* firstOutFragments, //SparseArray<Fragment>
        /* [in] */ IHashMap* lastInFragments); //SparseArray<Fragment>

    /**
     * Finds the first removed fragment and last added fragments when popping the back stack.
     * If none of the fragments have transitions, then both lists will be empty.
     *
     * @param firstOutFragments The list of first fragments to be removed, keyed on the
     *                          container ID. This list will be modified by the method.
     * @param lastInFragments The list of last fragments to be added, keyed on the
     *                        container ID. This list will be modified by the method.
     */
    CARAPI_(void) CalculateBackFragments(
        /* [in] */ IHashMap* firstOutFragments, //SparseArray<Fragment>
        /* [in] */ IHashMap* lastInFragments); //SparseArray<Fragment>

    /**
     * When custom fragment transitions are used, this sets up the state for each transition
     * and begins the transition. A different transition is started for each fragment container
     * and consists of up to 3 different transitions: the exit transition, a shared element
     * transition and an enter transition.
     *
     * <p>The exit transition operates against the leaf nodes of the first fragment
     * with a view that was removed. If no such fragment was removed, then no exit
     * transition is executed. The exit transition comes from the outgoing fragment.</p>
     *
     * <p>The enter transition operates against the last fragment that was added. If
     * that fragment does not have a view or no fragment was added, then no enter
     * transition is executed. The enter transition comes from the incoming fragment.</p>
     *
     * <p>The shared element transition operates against all views and comes either
     * from the outgoing fragment or the incoming fragment, depending on whether this
     * is going forward or popping the back stack. When going forward, the incoming
     * fragment's enter shared element transition is used, but when going back, the
     * outgoing fragment's return shared element transition is used. Shared element
     * transitions only operate if there is both an incoming and outgoing fragment.</p>
     *
     * @param firstOutFragments The list of first fragments to be removed, keyed on the
     *                          container ID.
     * @param lastInFragments The list of last fragments to be added, keyed on the
     *                        container ID.
     * @param isBack true if this is popping the back stack or false if this is a
     *               forward operation.
     * @return The TransitionState used to complete the operation of the transition
     * in {@link #setNameOverrides(android.app.BackStackRecord.TransitionState, java.util.ArrayList,
     * java.util.ArrayList)}.
     */
    AutoPtr<IBackStackRecordTransitionState> BeginTransition(
        /* [in] */ IHashMap* firstOutFragments, //SparseArray<Fragment>
        /* [in] */ IHashMap* lastInFragments, //SparseArray<Fragment>
        /* [in] */ Boolean isBack);

    static AutoPtr<ITransition> CloneTransition(
        /* [in] */ ITransition* transition);

    static AutoPtr<ITransition> GetEnterTransition(
        /* [in] */ IFragment* inFragment,
        /* [in] */ Boolean isBack);

    static AutoPtr<ITransition> GetExitTransition(
        /* [in] */ IFragment* outFragment,
        /* [in] */ Boolean isBack);

    static AutoPtr<ITransition> GetSharedElementTransition(
        /* [in] */ IFragment* inFragment,
        /* [in] */ IFragment* outFragment,
        /* [in] */ Boolean isBack);

    static AutoPtr<IArrayList>/*<View>*/ CaptureExitingViews(
        /* [in] */ ITransition* exitTransition,
        /* [in] */ IFragment* outFragment,
        /* [in] */ IArrayMap* namedViews); //  <String, View>

    AutoPtr<IArrayMap>/*<String, View>*/ RemapSharedElements(
        /* [in] */ IBackStackRecordTransitionState* state,
        /* [in] */ IFragment* outFragment,
        /* [in] */ Boolean isBack);

    /**
     * Prepares the enter transition by adding a non-existent view to the transition's target list
     * and setting it epicenter callback. By adding a non-existent view to the target list,
     * we can prevent any view from being targeted at the beginning of the transition.
     * We will add to the views before the end state of the transition is captured so that the
     * views will appear. At the start of the transition, we clear the list of targets so that
     * we can restore the state of the transition and use it again.
     *
     * <p>The shared element transition maps its shared elements immediately prior to
     * capturing the state of the Transition.</p>
     */
    AutoPtr<IArrayList>/*<View>*/ AddTransitionTargets(
        /* [in] */ IBackStackRecordTransitionState* state,
        /* [in] */ ITransition* enterTransition,
        /* [in] */ ITransition* sharedElementTransition,
        /* [in] */ ITransition* overallTransition,
        /* [in] */ IView* container,
        /* [in] */ IFragment* inFragment,
        /* [in] */ IFragment* outFragment,
        /* [in] */ IArrayList* /*<View>*/ hiddenFragmentViews,
        /* [in] */ Boolean isBack,
        /* [in] */ IArrayList* /*<View>*/ sharedElementTargets);

    void CallSharedElementEnd(
        /* [in] */ IBackStackRecordTransitionState* state,
        /* [in] */ IFragment* inFragment,
        /* [in] */ IFragment* outFragment,
        /* [in] */ Boolean isBack,
        /* [in] */ IArrayMap* /*<String, View>*/ namedViews);

    void SetEpicenterIn(
        /* [in] */ IArrayMap* /*<String, View>*/ namedViews,
        /* [in] */ IBackStackRecordTransitionState* state);

    AutoPtr<IArrayMap> /*<String, View>*/ MapSharedElementsIn(
        /* [in] */ IBackStackRecordTransitionState* state,
        /* [in] */ Boolean isBack,
        /* [in] */ IFragment* inFragment);

    static AutoPtr<ITransition> MergeTransitions(
        /* [in] */ ITransition* enterTransition,
        /* [in] */ ITransition* exitTransition,
        /* [in] */ ITransition* sharedElementTransition,
        /* [in] */ IFragment* inFragment,
        /* [in] */ Boolean isBack) ;

    /**
     * Configures custom transitions for a specific fragment container.
     *
     * @param containerId The container ID of the fragments to configure the transition for.
     * @param state The Transition State keeping track of the executing transitions.
     * @param firstOutFragments The list of first fragments to be removed, keyed on the
     *                          container ID.
     * @param lastInFragments The list of last fragments to be added, keyed on the
     *                        container ID.
     * @param isBack true if this is popping the back stack or false if this is a
     *               forward operation.
     */
    void ConfigureTransitions(int containerId,
        /* [in] */ IBackStackRecordTransitionState* state,
        /* [in] */ Boolean isBack,
        /* [in] */ IHashMap* firstOutFragments, //SparseArray<Fragment>
        /* [in] */ IHashMap* lastInFragments);//SparseArray<Fragment>

    /**
     * After the transition has started, remove all targets that we added to the transitions
     * so that the transitions are left in a clean state.
     */
    void RemoveTargetedViewsFromTransitions(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ IView* nonExistingView,
        /* [in] */ ITransition* enterTransition,
        /* [in] */ IArrayList* /*<View>*/ enteringViews,
        /* [in] */ ITransition* exitTransition,
        /* [in] */ IArrayList* /*<View>*/ exitingViews,
        /* [in] */ ITransition* sharedElementTransition,
        /* [in] */ IArrayList* /*<View>*/ sharedElementTargets,
        /* [in] */ ITransition* overallTransition,
        /* [in] */ IArrayList* /*<View>*/ hiddenViews);

    static void RemoveTargets(
        /* [in] */ ITransition* transition,
        /* [in] */ IArrayList* /*<View>*/ views);

    static void AddTargets(
        /* [in] */ ITransition* transition,
        /* [in] */ IArrayList* /*<View>*/ views);

    /**
     * Remaps a name-to-View map, substituting different names for keys.
     *
     * @param inMap A list of keys found in the map, in the order in toGoInMap
     * @param toGoInMap A list of keys to use for the new map, in the order of inMap
     * @param namedViews The current mapping
     * @return a new Map after it has been mapped with the new names as keys.
     */
    static AutoPtr<IArrayMap> /*<String, View>*/ RemapNames(
        /* [in] */ IArrayList* /* <String> */ inMap,
        /* [in] */ IArrayList* /* <String> */ toGoInMap,
        /* [in] */ IArrayMap* /* <String, View> */ namedViews);

    /**
     * Maps shared elements to views in the entering fragment.
     *
     * @param state The transition State as returned from {@link #beginTransition(
     * android.util.SparseArray, android.util.SparseArray, boolean)}.
     * @param inFragment The last fragment to be added.
     * @param isBack true if this is popping the back stack or false if this is a
     *               forward operation.
     */
    AutoPtr<IArrayMap> /*<String, View>*/ MapEnteringSharedElements(
        /* [in] */ IBackStackRecordTransitionState* state,
        /* [in] */ IFragment* inFragment,
        /* [in] */ Boolean isBack);

    void ExcludeHiddenFragments(
        /* [in] */ IArrayList* /* <View> */ hiddenFragmentViews,
        /* [in] */ Int32 containerId,
        /* [in] */ ITransition* transition);

    static void SetEpicenter(
        /* [in] */ ITransition* transition,
        /* [in] */ IView* view);

    void SetSharedElementEpicenter(
        /* [in] */ ITransition* transition,
        /* [in] */ IBackStackRecordTransitionState* state);

    static void SetNameOverride(
        /* [in] */ IArrayMap* /*<String, String>*/ overrides,
        /* [in] */ const String& source,
        /* [in] */ const String& target);

    static void SetNameOverride(
        /* [in] */ IArrayMap* /*<String, String>*/ overrides,
        /* [in] */ ICharSequence* source,
        /* [in] */ ICharSequence* target);

    static void SetNameOverrides(
        /* [in] */ IBackStackRecordTransitionState* state,
        /* [in] */ IArrayList* /*<String>*/ sourceNames,
        /* [in] */ IArrayList* /*<String>*/ targetNames);

    void SetBackNameOverrides(
        /* [in] */ IBackStackRecordTransitionState* state,
        /* [in] */ IArrayMap* /*<String, View>*/ namedViews,
        /* [in] */ Boolean isEnd);

    void SetNameOverrides(
        /* [in] */ IBackStackRecordTransitionState* state,
        /* [in] */ IArrayMap* /*<String, View>*/ namedViews,
        /* [in] */ Boolean isEnd);

public:
    friend class FragmentManagerImpl;

    static const String TAG;

    AutoPtr<FragmentManagerImpl> mManager;

    static const Int32 OP_NULL;
    static const Int32 OP_ADD;
    static const Int32 OP_REPLACE;
    static const Int32 OP_REMOVE;
    static const Int32 OP_HIDE;
    static const Int32 OP_SHOW;
    static const Int32 OP_DETACH;
    static const Int32 OP_ATTACH;

    AutoPtr<Op> mHead;
    AutoPtr<Op> mTail;
    Int32 mNumOp;
    Int32 mEnterAnim;
    Int32 mExitAnim;
    Int32 mPopEnterAnim;
    Int32 mPopExitAnim;
    Int32 mTransition;
    Int32 mTransitionStyle;
    Boolean mAddToBackStack;
    Boolean mAllowAddToBackStack;
    String mName;
    Boolean mCommitted;
    Int32 mIndex;

    Int32 mBreadCrumbTitleRes;
    AutoPtr<ICharSequence> mBreadCrumbTitleText;
    Int32 mBreadCrumbShortTitleRes;
    AutoPtr<ICharSequence> mBreadCrumbShortTitleText;

    AutoPtr<IArrayList> mSharedElementSourceNames; //ArrayList<String>
    AutoPtr<IArrayList> mSharedElementTargetNames; //ArrayList<String>

    friend class CBackStackState;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CBACKSTACKSTATE_H__
