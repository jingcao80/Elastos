
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/app/FragmentManagerImpl.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/app/SharedElementCallback.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/transition/TransitionInflater.h"
#include "elastos/droid/transition/CTransitionSet.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Content::EIID_IComponentCallbacks;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Transition::TransitionInflater;
using Elastos::Droid::Transition::ITransitionInflater;
using Elastos::Droid::Transition::CTransitionSet;
using Elastos::Droid::Transition::ITransitionSet;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::EIID_IViewOnCreateContextMenuListener;
using Elastos::Droid::R;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::IClassLoader;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Slogger;


namespace Elastos {
namespace Droid {
namespace App {

//============================================================================
// FragmentContainerLocal
//============================================================================
class FragmentContainerLocal
    : public Object
    , public IFragmentContainer
{
public:
    CAR_INTERFACE_DECL()

    FragmentContainerLocal(
        /* [in] */ Fragment* host)
        : mHost(host)
    {}

    ECode FindViewById(
        /* [in] */ Int32 id,
        /* [out] */ IView** view)
    {
        VALIDATE_NOT_NULL(view)
        *view = NULL;

        if (mHost->mView == NULL) {
//            throw new IllegalStateException("Fragment does not have a view");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        return mHost->mView->FindViewById(id, view);
    }

    ECode HasView(
        /* [out] */ Boolean* result)
    {
        VALIDATE_NOT_NULL(result)
        *result = (mHost->mView != NULL);
        return NOERROR;
    }

public:
    Fragment* mHost;
};

CAR_INTERFACE_IMPL(FragmentContainerLocal, Object, IFragmentContainer)

//===================================================================
// FragmentState
//===================================================================

CAR_INTERFACE_IMPL_2(FragmentState, Object, IFragmentState, IParcelable)

FragmentState::FragmentState()
    : mIndex(0)
    , mFromLayout(FALSE)
    , mFragmentId(0)
    , mContainerId(0)
    , mRetainInstance(FALSE)
    , mDetached(FALSE)
{}

FragmentState::~FragmentState()
{}

ECode FragmentState::constructor()
{
    return NOERROR;
}

ECode FragmentState::constructor(
    /* [in] */ IFragment* frag)
{
//     mClassName = frag.getClass().getName();
    mClassName = String("Fragment");
    frag->GetIndex(&mIndex);
    frag->GetFromLayout(&mFromLayout);
    frag->GetFragmentId(&mFragmentId);
    frag->GetContainerId(&mContainerId);
    frag->GetTag(&mTag);
    frag->GetRetainInstance(&mRetainInstance);
    frag->IsDetached(&mDetached);
    frag->GetArguments((IBundle**)&mArguments);
    return NOERROR;
}

ECode FragmentState::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    FAIL_RETURN(in->ReadString(&mClassName));
    FAIL_RETURN(in->ReadInt32(&mIndex));
    Int32 value;
    FAIL_RETURN(in->ReadInt32(&value))
    mFromLayout = value != 0;
    FAIL_RETURN(in->ReadInt32(&mFragmentId));
    FAIL_RETURN(in->ReadInt32(&mContainerId));
    FAIL_RETURN(in->ReadString(&mTag));
    FAIL_RETURN(in->ReadInt32(&value))
    mRetainInstance = value != 0;
    FAIL_RETURN(in->ReadInt32(&value))
    mDetached = value != 0;
    FAIL_RETURN(in->ReadInterfacePtr((Handle32*)(IBundle**)&mArguments));
    FAIL_RETURN(in->ReadInterfacePtr((Handle32*)(IBundle**)&mSavedFragmentState));
    return NOERROR;
}

ECode FragmentState::Instantiate(
    /* [in] */ IActivity* activity,
    /* [in] */ IFragment* parent,
    /* [out] */ IFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    *fragment = NULL;
    VALIDATE_NOT_NULL(activity);
    VALIDATE_NOT_NULL(parent);

    if (mInstance != NULL) {
        *fragment = mInstance;
        REFCOUNT_ADD(*fragment);
        return NOERROR;
    }

    if (mArguments != NULL) {
        AutoPtr<IClassLoader> classLoader;
        // IContext::Probe(activity)->GetClassLoader((IClassLoader**)&classLoader);
        mArguments->SetClassLoader(classLoader);
    }

    Fragment::Instantiate(IContext::Probe(activity), mClassName, mArguments, (IFragment**)&mInstance);

    if (mSavedFragmentState != NULL) {
        AutoPtr<IClassLoader> classLoader;
        // IContext::Probe(activity)->GetClassLoader((IClassLoader**)&classLoader);
        mSavedFragmentState->SetClassLoader(classLoader);
        mInstance->SetSavedFragmentState(mSavedFragmentState);
    }

    AutoPtr<Activity> act = (Activity*)activity;
    mInstance->SetIndex(mIndex, parent);
    mInstance->SetFromLayout(mFromLayout);
    mInstance->SetRestored(TRUE);
    mInstance->SetFragmentId(mFragmentId);
    mInstance->SetContainerId(mContainerId);
    mInstance->SetTag(mTag);
    mInstance->SetRetainInstance(mRetainInstance);
    mInstance->SetDetached(mDetached);
    // mInstance->SetFragmentManager(act->mFragments);
    if (FragmentManagerImpl::DEBUG) Slogger::V(FragmentManagerImpl::TAG,
            "Instantiated fragment %p", mInstance.Get());

    *fragment = mInstance;
    REFCOUNT_ADD(*fragment);
    return NOERROR;
}

ECode FragmentState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteString(mClassName));
    FAIL_RETURN(dest->WriteInt32(mIndex));
    FAIL_RETURN(dest->WriteInt32(mFromLayout ? 1 : 0));
    FAIL_RETURN(dest->WriteInt32(mFragmentId));
    FAIL_RETURN(dest->WriteInt32(mContainerId));
    FAIL_RETURN(dest->WriteString(mTag));
    FAIL_RETURN(dest->WriteInt32(mRetainInstance ? 1 : 0));
    FAIL_RETURN(dest->WriteInt32(mDetached ? 1 : 0));
    FAIL_RETURN(dest->WriteInterfacePtr(mArguments));
    FAIL_RETURN(dest->WriteInterfacePtr(mSavedFragmentState));
    return NOERROR;
}

ECode FragmentState::GetClassName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mClassName;
    return NOERROR;
}

ECode FragmentState::GetIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIndex;
    return NOERROR;
}

ECode FragmentState::IsFromLayout(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFromLayout;
    return NOERROR;
}

ECode FragmentState::GetFragmentId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFragmentId;
    return NOERROR;
}

ECode FragmentState::GetContainerId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mContainerId;
    return NOERROR;
}

ECode FragmentState::GetTag(
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag)
    *tag = mTag;
    return NOERROR;
}

ECode FragmentState::IsRetainInstance(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRetainInstance;
    return NOERROR;
}

ECode FragmentState::IsDetached(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDetached;
    return NOERROR;
}

ECode FragmentState::GetArguments(
    /* [out] */ IBundle** arguments)
{
    VALIDATE_NOT_NULL(arguments)
    *arguments = mArguments;
    REFCOUNT_ADD(*arguments)
    return NOERROR;
}

ECode FragmentState::GetSavedFragmentState(
    /* [out] */ IBundle** state)
{
    VALIDATE_NOT_NULL(state)
    *state = mSavedFragmentState;
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode FragmentState::GetInstance(
    /* [out] */ IFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment)
    *fragment = mInstance;
    REFCOUNT_ADD(*fragment)
    return NOERROR;
}

//===================================================================
// Fragment
//===================================================================

AutoPtr<ITransition> InitUSE_DEFAULT_TRANSITION()
{
    AutoPtr<ITransition> transit;
    CTransitionSet::New((ITransition**)&transit);
    return transit;
}

const AutoPtr<ITransition> Fragment::USE_DEFAULT_TRANSITION = InitUSE_DEFAULT_TRANSITION();

HashMap<String, AutoPtr<IClassInfo> > Fragment::sClassMap;

CAR_INTERFACE_IMPL_4(Fragment, Object, IFragment, IComponentCallbacks, IComponentCallbacks2, IViewOnCreateContextMenuListener)

Fragment::Fragment()
    : mState(INITIALIZING)
    , mStateAfterAnimating(0)
    , mIndex(-1)
    , mTargetIndex(-1)
    , mTargetRequestCode(0)
    , mAdded(FALSE)
    , mRemoving(FALSE)
    , mResumed(FALSE)
    , mFromLayout(FALSE)
    , mInLayout(FALSE)
    , mRestored(FALSE)
    , mBackStackNesting(0)
    , mFragmentId(0)
    , mContainerId(0)
    , mHidden(FALSE)
    , mDetached(FALSE)
    , mRetainInstance(FALSE)
    , mRetaining(FALSE)
    , mHasMenu(FALSE)
    , mMenuVisible(TRUE)
    , mCalled(FALSE)
    , mNextAnim(0)
    , mDeferStart(FALSE)
    , mUserVisibleHint(TRUE)
    , mLoadersStarted(FALSE)
    , mCheckedForLoaderManager(FALSE)
    , mAllowReturnTransitionOverlap(TRUE)
    , mAllowEnterTransitionOverlap(TRUE)
{}

Fragment::~Fragment()
{}

ECode Fragment::constructor()
{
    return NOERROR;
}


ECode Fragment::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode Fragment::SetState(
    /* [in] */ Int32 state)
{
    mState = state;
    return NOERROR;
}

ECode Fragment::GetAnimatingAway(
    /* [out] */ IAnimator** animator)
{
    VALIDATE_NOT_NULL(animator);
    *animator = mAnimatingAway;
    REFCOUNT_ADD(*animator);
    return NOERROR;
}

ECode Fragment::SetAnimatingAway(
    /* [in] */ IAnimator* animator)
{
    mAnimatingAway = animator;
    return NOERROR;
}

ECode Fragment::GetStateAfterAnimating(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mStateAfterAnimating;
    return NOERROR;
}

ECode Fragment::SetStateAfterAnimating(
    /* [in] */ Int32 state)
{
    mStateAfterAnimating = state;
    return NOERROR;
}

ECode Fragment::GetSavedFragmentState(
    /* [out] */ IBundle** fState)
{
    VALIDATE_NOT_NULL(fState);
    *fState = mSavedFragmentState;
    REFCOUNT_ADD(*fState);
    return NOERROR;
}

ECode Fragment::SetSavedFragmentState(
    /* [in] */ IBundle* fState)
{
    mSavedFragmentState = fState;
    return NOERROR;
}

ECode Fragment::GetSavedViewState(
    /* [out] */ ISparseArray** viewState)
{
    VALIDATE_NOT_NULL(viewState);
    *viewState = mSavedViewState;
    REFCOUNT_ADD(*viewState);
    return NOERROR;
}

ECode Fragment::SetSavedViewState(
    /* [in] */ ISparseArray* viewState)
{
    mSavedViewState = viewState;
    return NOERROR;
}

ECode Fragment::GetIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mIndex;
    return NOERROR;
}

ECode Fragment::GetWho(
    /* [out] */ String* who)
{
    VALIDATE_NOT_NULL(who);
    *who = mWho;
    return NOERROR;
}

ECode Fragment::SetWho(
    /* [in] */ const String& who)
{
    mWho = who;
    return NOERROR;
}

ECode Fragment::GetTarget(
    /* [out] */ IFragment** target)
{
    VALIDATE_NOT_NULL(target);
    *target = mTarget;
    REFCOUNT_ADD(*target);
    return NOERROR;
}

ECode Fragment::SetTarget(
    /* [in] */ IFragment* target)
{
    mTarget = target;
    return NOERROR;
}

ECode Fragment::GetTargetIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mTargetIndex;
    return NOERROR;
}

ECode Fragment::SetTargetIndex(
    /* [in] */ Int32 index)
{
    mTargetIndex = index;
    return NOERROR;
}

ECode Fragment::SetTargetRequestCode(
    /* [in] */ Int32 code)
{
    mTargetRequestCode = code;
    return NOERROR;
}

ECode Fragment::GetAdded(
    /* [out] */ Boolean* added)
{
    VALIDATE_NOT_NULL(added);
    *added = mAdded;
    return NOERROR;
}

ECode Fragment::SetAdded(
    /* [in] */ Boolean added)
{
    mAdded = added;
    return NOERROR;
}

ECode Fragment::SetRemoving(
    /* [in] */ Boolean removing)
{
    mRemoving = removing;
    return NOERROR;
}

ECode Fragment::SetResumed(
    /* [in] */ Boolean resumed)
{
    mResumed = resumed;
    return NOERROR;
}

ECode Fragment::GetFromLayout(
    /* [out] */ Boolean* fLayout)
{
    VALIDATE_NOT_NULL(fLayout);
    *fLayout = mFromLayout;
    return NOERROR;
}

ECode Fragment::SetFromLayout(
    /* [in] */ Boolean fLayout)
{
    mFromLayout = fLayout;
    return NOERROR;
}

ECode Fragment::SetInLayout(
    /* [in] */ Boolean inLayout)
{
    mInLayout = inLayout;
    return NOERROR;
}

ECode Fragment::GetRestored(
    /* [out] */ Boolean* restored)
{
    VALIDATE_NOT_NULL(restored);
    *restored = mRestored;
    return NOERROR;
}

ECode Fragment::SetRestored(
    /* [in] */ Boolean restored)
{
    mRestored = restored;
    return NOERROR;
}

ECode Fragment::GetBackStackNesting(
    /* [out] */ Int32* bsNesting)
{
    VALIDATE_NOT_NULL(bsNesting);
    *bsNesting = mBackStackNesting;
    return NOERROR;
}

ECode Fragment::SetBackStackNesting(
    /* [in] */ Int32 bsNesting)
{
    mBackStackNesting = bsNesting;
    return NOERROR;
}

ECode Fragment::SetFragmentManager(
    /* [in] */ IFragmentManagerImpl* fManager)
{
    mFragmentManager = fManager;
    return NOERROR;
}

ECode Fragment::SetActivity(
    /* [in] */ IActivity* activity)
{
    mActivity = activity;
    return NOERROR;
}

ECode Fragment::SetParentFragment(
    /* [in] */ IFragment* pFragment)
{
    mParentFragment = pFragment;
    return NOERROR;
}

ECode Fragment::GetFragmentId(
    /* [out] */ Int32* fid)
{
    VALIDATE_NOT_NULL(fid);
    *fid = mFragmentId;
    return NOERROR;
}

ECode Fragment::SetFragmentId(
    /* [in] */ Int32 fid)
{
    mFragmentId = fid;
    return NOERROR;
}

ECode Fragment::GetContainerId(
    /* [out] */ Int32* cid)
{
    VALIDATE_NOT_NULL(cid);
    *cid = mContainerId;
    return NOERROR;
}

ECode Fragment::SetContainerId(
    /* [in] */ Int32 cid)
{
    mContainerId = cid;
    return NOERROR;
}

ECode Fragment::SetTag(
    /* [in] */ const String& tag)
{
    mTag = tag;
    return NOERROR;
}

ECode Fragment::SetHidden(
    /* [in] */ Boolean hidden)
{
    mHidden = hidden;
    return NOERROR;
}

ECode Fragment::SetDetached(
    /* [in] */ Boolean detached)
{
    mDetached = detached;
    return NOERROR;
}

ECode Fragment::GetRetaining(
    /* [out] */ Boolean* retaining)
{
    VALIDATE_NOT_NULL(retaining);
    *retaining = mRetaining;
    return NOERROR;
}

ECode Fragment::SetRetaining(
    /* [in] */ Boolean retaining)
{
    mRetaining = retaining;
    return NOERROR;
}

ECode Fragment::GetHasMenu(
    /* [out] */ Boolean* hasMenu)
{
    VALIDATE_NOT_NULL(hasMenu);
    *hasMenu = mHasMenu;
    return NOERROR;
}

ECode Fragment::SetHasMenu(
    /* [in] */ Boolean hasMenu)
{
    mHasMenu = hasMenu;
    return NOERROR;
}

ECode Fragment::GetMenuVisible(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);
    *visible = mMenuVisible;
    return NOERROR;
}

ECode Fragment::SetMenuVisible(
    /* [in] */ Boolean visible)
{
    mMenuVisible = visible;
    return NOERROR;
}

ECode Fragment::GetCalled(
    /* [out] */ Boolean* called)
{
    VALIDATE_NOT_NULL(called);
    *called = mCalled;
    return NOERROR;
}

ECode Fragment::SetCalled(
    /* [in] */ Boolean called)
{
    mCalled = called;
    return NOERROR;
}

ECode Fragment::GetNextAnim(
    /* [out] */ Int32* anim)
{
    VALIDATE_NOT_NULL(anim);
    *anim = mNextAnim;
    return NOERROR;
}

ECode Fragment::SetNextAnim(
    /* [in] */ Int32 anim)
{
    mNextAnim = anim;
    return NOERROR;
}

ECode Fragment::GetContainer(
    /* [out] */ IViewGroup** container)
{
    VALIDATE_NOT_NULL(container);
    *container = mContainer;
    REFCOUNT_ADD(*container);
    return NOERROR;
}

ECode Fragment::SetContainer(
    /* [in] */ IViewGroup* container)
{
    mContainer = container;
    return NOERROR;
}

ECode Fragment::SetView(
    /* [in] */ IView* view)
{
    mView = view;
    return NOERROR;
}

ECode Fragment::GetDeferStart(
    /* [out] */ Boolean* start)
{
    VALIDATE_NOT_NULL(start);
    *start = mDeferStart;
    return NOERROR;
}

ECode Fragment::SetDeferStart(
    /* [in] */ Boolean start)
{
    mDeferStart = start;
    return NOERROR;
}

ECode Fragment::SetLoaderManager(
    /* [in] */ ILoaderManagerImpl* lManager)
{
    mLoaderManager = lManager;
    return NOERROR;
}

ECode Fragment::GetLoaderManagerValue(
    /* [out] */ ILoaderManagerImpl** lManager)
{
    VALIDATE_NOT_NULL(lManager);
    *lManager = mLoaderManager;
    REFCOUNT_ADD(*lManager);
    return NOERROR;
}

ECode Fragment::GetLoadersStarted(
    /* [out] */ Boolean* started)
{
    VALIDATE_NOT_NULL(started);
    *started = mLoadersStarted;
    return NOERROR;
}

ECode Fragment::SetLoadersStarted(
    /* [in] */ Boolean started)
{
    mLoadersStarted = started;
    return NOERROR;
}

ECode Fragment::GetCheckedForLoaderManager(
    /* [out] */ Boolean* cfManager)
{
    VALIDATE_NOT_NULL(cfManager);
    *cfManager = mCheckedForLoaderManager;
    return NOERROR;
}

ECode Fragment::SetCheckedForLoaderManager(
    /* [in] */ Boolean cfManager)
{
    mCheckedForLoaderManager = cfManager;
    return NOERROR;
}

ECode Fragment::Instantiate(
    /* [in] */ IContext* context,
    /* [in] */ const String& fname,
    /* [out] */ IFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    return Instantiate(context, fname, NULL, fragment);
}

ECode Fragment::Instantiate(
    /* [in] */ IContext* context,
    /* [in] */ const String& fname,
    /* [in] */ IBundle* args,
    /* [out] */ IFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    *fragment = NULL;
//     try {
    AutoPtr<IClassInfo> clazz;
    HashMap<String, AutoPtr<IClassInfo> >::Iterator it = sClassMap.Find(fname);
    if (it == sClassMap.End()) {
//             // Class not found in the cache, see if it's real, and try to add it
        AutoPtr<IClassLoader> classLoader;
        // context->GetClassLoader((IClassLoader**)&classLoader);
        classLoader->LoadClass(fname, (IClassInfo**)&clazz);
        // if (!Fragment.class.isAssignableFrom(clazz)) {
        //     throw new InstantiationException("Trying to instantiate a class " + fname
        //             + " that is not a Fragment", new ClassCastException());
        // }

        sClassMap[fname] = clazz;
    }
    else {
        clazz = it->mSecond;
    }

    AutoPtr<IInterface> obj;
    FAIL_RETURN(clazz->CreateObject((IInterface**)&obj));
    AutoPtr<IFragment> f = IFragment::Probe(obj);
    if (args != NULL) {
        AutoPtr<IClassLoader> cLoader;
//TODO:
//            args.setClassLoader(f.getClass().getClassLoader());
        f->SetArguments(args);
    }
    *fragment = f;
    REFCOUNT_ADD(*fragment);
    return NOERROR;
//     } catch (ClassNotFoundException e) {
//         throw new InstantiationException("Unable to instantiate fragment " + fname
//                 + ": make sure class name exists, is public, and has an"
//                 + " empty constructor that is public", e);
//     } catch (java.lang.InstantiationException e) {
//         throw new InstantiationException("Unable to instantiate fragment " + fname
//                 + ": make sure class name exists, is public, and has an"
//                 + " empty constructor that is public", e);
//     } catch (IllegalAccessException e) {
//         throw new InstantiationException("Unable to instantiate fragment " + fname
//                 + ": make sure class name exists, is public, and has an"
//                 + " empty constructor that is public", e);
//     }
}

ECode Fragment::RestoreViewState(
    /* [in] */ IBundle* savedInstanceState)
{
    if (mSavedViewState != NULL) {
        mView->RestoreHierarchyState(mSavedViewState);
        mSavedViewState = NULL;
    }
    mCalled = FALSE;
    OnViewStateRestored(savedInstanceState);
    if (!mCalled) {
        Slogger::E("Fragment", "Fragment did not call through to super::OnViewStateRestored()");
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }
    return NOERROR;
}

ECode Fragment::SetIndex(
    /* [in] */ Int32 index,
    /* [in] */ IFragment* parent)
{
    mIndex = index;
    if (parent != NULL) {
        String who;
        parent->GetWho(&who);
        StringBuilder sb(who);
        sb += ":";
        sb += mIndex;
        mWho =  sb.ToString();
    } else {
        StringBuilder sb("android:fragment:");
        sb += mIndex;
        mWho = sb.ToString();
    }
    return NOERROR;
}

Fragment::IsInBackStack(
    /* [out] */ Boolean* inbs)
{
    VALIDATE_NOT_NULL(inbs);
    *inbs = mBackStackNesting > 0;
    return NOERROR;
}

ECode Fragment::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* equal)
{
    return Object::Equals(o, equal);
}

ECode Fragment::GetHashCode(
    /* [out] */ Int32* code)
{
    return Object::GetHashCode(code);
}

ECode Fragment::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string)

    StringBuilder sb(128);
//     DebugUtils.buildShortClassTag(this, sb);
    if (mIndex >= 0) {
        sb.Append(" #");
        sb.Append(mIndex);
    }
    if (mFragmentId != 0) {
        sb.Append(" id=0x");
        sb.Append(StringUtils::ToHexString(mFragmentId));
    }
    if (mTag != NULL) {
        sb.Append(" ");
        sb.Append(mTag);
    }
    sb.Append("}");
    *string = sb.ToString();
    return NOERROR;
}

ECode Fragment::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mFragmentId;
    return NOERROR;
}

ECode Fragment::GetTag(
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag);

    *tag = mTag;
    return NOERROR;
}

ECode Fragment::SetArguments(
    /* [in] */ IBundle* args)
{
    if (mIndex >= 0) {
        Slogger::E("Fragment", "Fragment already active");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mArguments = args;
    return NOERROR;
}

ECode Fragment::GetArguments(
    /* [out] */ IBundle** args)
{
    VALIDATE_NOT_NULL(args);

    *args = mArguments;
    REFCOUNT_ADD(*args);
    return NOERROR;
}

ECode Fragment::SetInitialSavedState(
    /* [in] */ IFragmentSavedState* state)
{
    if (mIndex >= 0) {
        Slogger::E("Fragment", "Fragment already active");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IBundle> fState;
    mSavedFragmentState = state != NULL && (state->GetState((IBundle**)&fState), fState != NULL)
            ? fState : NULL;
    return NOERROR;
}

ECode Fragment::SetTargetFragment(
    /* [in] */ IFragment* fragment,
    /* [in] */ Int32 requestCode)
{
    mTarget = fragment;
    mTargetRequestCode = requestCode;
    return NOERROR;
}

ECode Fragment::GetTargetFragment(
    /* [out] */ IFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);

    *fragment = mTarget;
    REFCOUNT_ADD(*fragment);
    return NOERROR;
}

ECode Fragment::GetTargetRequestCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    *code = mTargetRequestCode;
    return NOERROR;
}

ECode Fragment::GetActivity(
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity);

    *activity = mActivity;
    REFCOUNT_ADD(*activity);
    return NOERROR;
}

ECode Fragment::GetResources(
    /* [out] */ IResources** resources)
{
    VALIDATE_NOT_NULL(resources);

    if (mActivity == NULL) {
//         throw new IllegalStateException("Fragment " + this + " not attached to Activity");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    IContext::Probe(mActivity)->GetResources(resources);
    return NOERROR;
}

ECode Fragment::GetText(
    /* [in] */ Int32 resId,
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetText(resId, text);
    return NOERROR;
}

ECode Fragment::GetString(
    /* [in] */ Int32 resId,
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetString(resId, string);
    return NOERROR;
}

ECode Fragment::GetString(
    /* [in] */ Int32 resId,
    /* [in] */ ArrayOf<IInterface*>* formatArgs,
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetString(resId, formatArgs, string);
    return NOERROR;
}

ECode Fragment::GetFragmentManager(
    /* [out] */ IFragmentManager** manager)
{
    VALIDATE_NOT_NULL(manager);

    *manager = IFragmentManager::Probe(mFragmentManager);
    REFCOUNT_ADD(*manager);
    return NOERROR;
}

ECode Fragment::GetChildFragmentManager(
    /* [out] */ IFragmentManager** manager)
{
    VALIDATE_NOT_NULL(manager);

    if (mChildFragmentManager == NULL) {
        InstantiateChildFragmentManager();
        if (mState >= IFragment::RESUMED) {
            mChildFragmentManager->DispatchResume();
        } else if (mState >= IFragment::STARTED) {
            mChildFragmentManager->DispatchStart();
        } else if (mState >= IFragment::ACTIVITY_CREATED) {
            mChildFragmentManager->DispatchActivityCreated();
        } else if (mState >= IFragment::CREATED) {
            mChildFragmentManager->DispatchCreate();
        }
    }
    *manager = IFragmentManager::Probe(mChildFragmentManager);
    REFCOUNT_ADD(*manager);
    return NOERROR;
}

ECode Fragment::GetParentFragment(
    /* [out] */ IFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);

    *fragment = mParentFragment;
    REFCOUNT_ADD(*fragment);
    return NOERROR;
}

ECode Fragment::IsAdded(
    /* [out] */ Boolean* added)
{
    VALIDATE_NOT_NULL(added);

    *added = mActivity != NULL && mAdded;
    return NOERROR;
}

ECode Fragment::IsDetached(
    /* [out] */ Boolean* detached)
{
    VALIDATE_NOT_NULL(detached);

    *detached = mDetached;
    return NOERROR;
}

ECode Fragment::IsRemoving(
    /* [out] */ Boolean* removing)
{
    VALIDATE_NOT_NULL(removing);

    *removing = mRemoving;
    return NOERROR;
}

ECode Fragment::IsInLayout(
    /* [out] */ Boolean* inlayout)
{
    VALIDATE_NOT_NULL(inlayout);
    *inlayout = mInLayout;
    return NOERROR;
}

ECode Fragment::IsResumed(
    /* [out] */ Boolean* resumed)
{
    VALIDATE_NOT_NULL(resumed);

    *resumed = mResumed;
    return NOERROR;
}

ECode Fragment::IsVisible(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);

    Boolean added, hidden;
    IsAdded(&added);
    IsHidden(&hidden);
    AutoPtr<IBinder> token;
    mView->GetWindowToken((IBinder**)&token);
    Int32 visibility;
    mView->GetVisibility(&visibility);
    *visible = added && !hidden && mView != NULL
            && token != NULL && visibility == IView::VISIBLE;
    return NOERROR;
}

ECode Fragment::IsHidden(
    /* [out] */ Boolean* hidden)
{
    VALIDATE_NOT_NULL(hidden);
    *hidden = mHidden;
    return NOERROR;
}

ECode Fragment::OnHiddenChanged(
    /* [in] */ Boolean hidden)
{
    return NOERROR;
}

ECode Fragment::SetRetainInstance(
    /* [in] */ Boolean retain)
{
    if (retain && mParentFragment != NULL) {
//         throw new IllegalStateException(
//                 "Can't retain fragements that are nested in other fragments");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mRetainInstance = retain;
    return NOERROR;
}

ECode Fragment::GetRetainInstance(
    /* [out] */ Boolean* retainInstance)
{
    VALIDATE_NOT_NULL(retainInstance);
    *retainInstance = mRetainInstance;
    return NOERROR;
}

ECode Fragment::SetHasOptionsMenu(
    /* [in] */ Boolean hasMenu)
{
    if (mHasMenu != hasMenu) {
        mHasMenu = hasMenu;
        Boolean added, hidden;
        IsAdded(&added);
        IsHidden(&hidden);
        if (added && !hidden) {
            IFragmentManager::Probe(mFragmentManager)->InvalidateOptionsMenu();
        }
    }
    return NOERROR;
}

ECode Fragment::SetMenuVisibility(
    /* [in] */ Boolean menuVisible)
{
    if (mMenuVisible != menuVisible) {
        mMenuVisible = menuVisible;
        Boolean added, hidden;
        IsAdded(&added);
        IsHidden(&hidden);
        if (mHasMenu && added && !hidden) {
            IFragmentManager::Probe(mFragmentManager)->InvalidateOptionsMenu();
        }
    }
    return NOERROR;
}

ECode Fragment::SetUserVisibleHint(
    /* [in] */ Boolean isVisibleToUser)
{
    if (!mUserVisibleHint && isVisibleToUser && mState < IFragment::STARTED) {
        mFragmentManager->PerformPendingDeferredStart(this);
    }
    mUserVisibleHint = isVisibleToUser;
    mDeferStart = !isVisibleToUser;
    return NOERROR;
}

ECode Fragment::GetUserVisibleHint(
    /* [out] */ Boolean* hint)
{
    VALIDATE_NOT_NULL(hint);
    *hint = mUserVisibleHint;
    return NOERROR;
}

ECode Fragment::GetLoaderManager(
    /* [out] */ ILoaderManager** manager)
{
    VALIDATE_NOT_NULL(manager);

    if (mLoaderManager != NULL) {
        *manager = ILoaderManager::Probe(mLoaderManager);
        REFCOUNT_ADD(*manager);
    }
    if (mActivity == NULL) {
        Slogger::E("Fragment", "Fragment  not attached to Activity");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mCheckedForLoaderManager = TRUE;
    AutoPtr<Activity> act = (Activity*)mActivity.Get();;
    mLoaderManager = act->GetLoaderManager(mWho, mLoadersStarted, TRUE);
    *manager = ILoaderManager::Probe(mLoaderManager);
    REFCOUNT_ADD(*manager);
    return NOERROR;
}

ECode Fragment::StartActivity(
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent);

    StartActivity(intent, NULL);
    return NOERROR;
}

ECode Fragment::StartActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options)
{
    VALIDATE_NOT_NULL(intent);
    VALIDATE_NOT_NULL(options);

    if (mActivity == NULL) {
        Slogger::E("Fragment", "Fragment  not attached to Activity");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (options != NULL) {
        mActivity->StartActivityFromFragment(this, intent, -1, options);
    } else {
        // Note we want to go through this call for compatibility with
        // applications that may have overridden the method.
        mActivity->StartActivityFromFragment(this, intent, -1);
    }
    return NOERROR;
}

ECode Fragment::StartActivityForResult(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode)
{
    return StartActivityForResult(intent, requestCode, NULL);
}

ECode Fragment::StartActivityForResult(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options)
{
    if (mActivity == NULL) {
        Slogger::E("Fragment", "Fragment  not attached to Activity");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (options != NULL) {
        mActivity->StartActivityFromFragment(this, intent, requestCode, options);
    } else {
        // Note we want to go through this call for compatibility with
        // applications that may have overridden the method.
        mActivity->StartActivityFromFragment(this, intent, requestCode, options);
    }
    return NOERROR;
}

ECode Fragment::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    return NOERROR;
}

ECode Fragment::GetLayoutInflater(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater);

    // Newer platform versions use the child fragment manager's LayoutInflaterFactory.
    AutoPtr<IApplicationInfo> ai;
    IContext::Probe(mActivity)->GetApplicationInfo((IApplicationInfo**)&ai);
    Int32 skdVer;
    ai->GetTargetSdkVersion(&skdVer);
    if (skdVer >= Build::VERSION_CODES::LOLLIPOP) {
        AutoPtr<ILayoutInflater> li, result;
        mActivity->GetLayoutInflater((ILayoutInflater**)&li);
        li->CloneInContext(IContext::Probe(mActivity), (ILayoutInflater**)&result);
        AutoPtr<IFragmentManager> tmp;
        GetChildFragmentManager((IFragmentManager**)&tmp); // Init if needed; use raw implementation below.
        AutoPtr<ILayoutInflaterFactory2> fact;
        ((FragmentManagerImpl*)mChildFragmentManager.Get())->GetLayoutInflaterFactory((ILayoutInflaterFactory2**)&fact);
        result->SetPrivateFactory(fact);
        *inflater = result;
        REFCOUNT_ADD(*inflater)
        return NOERROR;
    }

    return mActivity->GetLayoutInflater(inflater);
}

ECode Fragment::OnInflate(
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IBundle* savedInstanceState)
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnInflate(
    /* [in] */ IActivity* activity,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IBundle* savedInstanceState)
{
    OnInflate(attrs, savedInstanceState);
    mCalled = TRUE;

    IContext* ctx = IContext::Probe(activity);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::Fragment),
            ArraySize(R::styleable::Fragment));
    AutoPtr<ITypedArray> a;
    ctx->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);

    mEnterTransition = LoadTransition(ctx, a, mEnterTransition, NULL,
            R::styleable::Fragment_fragmentEnterTransition);
    mReturnTransition = LoadTransition(ctx, a, mReturnTransition, USE_DEFAULT_TRANSITION,
            R::styleable::Fragment_fragmentReturnTransition);
    mExitTransition = LoadTransition(ctx, a, mExitTransition, NULL,
            R::styleable::Fragment_fragmentExitTransition);
    mReenterTransition = LoadTransition(ctx, a, mReenterTransition, USE_DEFAULT_TRANSITION,
            R::styleable::Fragment_fragmentReenterTransition);
    mSharedElementEnterTransition = LoadTransition(ctx, a, mSharedElementEnterTransition,
            NULL, R::styleable::Fragment_fragmentSharedElementEnterTransition);
    mSharedElementReturnTransition = LoadTransition(ctx, a, mSharedElementReturnTransition,
            USE_DEFAULT_TRANSITION,
            R::styleable::Fragment_fragmentSharedElementReturnTransition);
    // if (mAllowEnterTransitionOverlap == NULL)
    {
        a->GetBoolean(
            R::styleable::Fragment_fragmentAllowEnterTransitionOverlap, TRUE, &mAllowEnterTransitionOverlap);
    }
    // if (mAllowReturnTransitionOverlap == NULL)
    {
        a->GetBoolean(
            R::styleable::Fragment_fragmentAllowReturnTransitionOverlap, TRUE, &mAllowReturnTransitionOverlap);
    }

    a->Recycle();
    return NOERROR;
}

ECode Fragment::OnAttach(
    /* [in] */ IActivity* activity)
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnCreateAnimator(
    /* [in] */ Int32 transit,
    /* [in] */ Boolean enter,
    /* [in] */ Int32 nextAnim,
    /* [out] */ IAnimator** animator)
{
    VALIDATE_NOT_NULL(animator);
    *animator = NULL;
    return NOERROR;
}

ECode Fragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;
    return NOERROR;
}

ECode Fragment::OnViewCreated(
    /* [in] */ IView* view,
    /* [in] */ IBundle* savedInstanceState)
{
    return NOERROR;
}

ECode Fragment::GetView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode Fragment::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnViewStateRestored(
    /* [in] */ IBundle* savedInstanceState)
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnStart()
{
    mCalled = TRUE;

    if (!mLoadersStarted) {
        mLoadersStarted = TRUE;
        if (!mCheckedForLoaderManager) {
            mCheckedForLoaderManager = TRUE;
            AutoPtr<Activity> act = (Activity*)mActivity.Get();;
            mLoaderManager = act->GetLoaderManager(mWho, mLoadersStarted, FALSE);
        }
        if (mLoaderManager != NULL) {
            assert(0 && "TODO"); //((LoaderManagerImpl*)mLoaderManager.Get())->DoStart();
        }
    }
    return NOERROR;
}

ECode Fragment::OnResume()
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    return NOERROR;
}

ECode Fragment::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnPause()
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnStop()
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnLowMemory()
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnTrimMemory(
        /* [in] */ Int32 level)
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnDestroyView()
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnDestroy()
{
    mCalled = TRUE;
    //Log.v("foo", "onDestroy: mCheckedForLoaderManager=" + mCheckedForLoaderManager
    //        + " mLoaderManager=" + mLoaderManager);
    if (!mCheckedForLoaderManager) {
        mCheckedForLoaderManager = TRUE;
        AutoPtr<Activity> act = (Activity*)mActivity.Get();;
        mLoaderManager = act->GetLoaderManager(mWho, mLoadersStarted, FALSE);
    }
    if (mLoaderManager != NULL) {
        assert(0 && "TODO"); //((LoaderManagerImpl*)mLoaderManager.Get())->DoStart();
    }
    return NOERROR;
}

ECode Fragment::InitState()
{
    mIndex = -1;
    mWho = NULL;
    mAdded = FALSE;
    mRemoving = FALSE;
    mResumed = FALSE;
    mFromLayout = FALSE;
    mInLayout = FALSE;
    mRestored = FALSE;
    mBackStackNesting = 0;
    mFragmentManager = NULL;
    mChildFragmentManager = NULL;
    mActivity = NULL;
    mFragmentId = 0;
    mContainerId = 0;
    mTag = NULL;
    mHidden = FALSE;
    mDetached = FALSE;
    mRetaining = FALSE;
    mLoaderManager = NULL;
    mLoadersStarted = FALSE;
    mCheckedForLoaderManager = FALSE;
    return NOERROR;
}

ECode Fragment::OnDetach()
{
    mCalled = TRUE;
    return NOERROR;
}

ECode Fragment::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater)
{
    return NOERROR;
}

ECode Fragment::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu)
{
    return NOERROR;
}

ECode Fragment::OnDestroyOptionsMenu()
{
    return NOERROR;
}

ECode Fragment::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* selected)
{
    VALIDATE_NOT_NULL(selected);
    *selected = FALSE;
    return NOERROR;
}

ECode Fragment::OnOptionsMenuClosed(
    /* [in] */ IMenu* menu)
{
    return NOERROR;
}

ECode Fragment::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    activity->OnCreateContextMenu(menu, v, menuInfo);
    return NOERROR;
}

ECode Fragment::RegisterForContextMenu(
    /* [in] */ IView* view)
{
    view->SetOnCreateContextMenuListener(
        this);
    return NOERROR;
}

ECode Fragment::UnregisterForContextMenu(
    /* [in] */ IView* view)
{
    view->SetOnCreateContextMenuListener(NULL);
    return NOERROR;
}

ECode Fragment::OnContextItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* selected)
{
    VALIDATE_NOT_NULL(selected);
    *selected = FALSE;
    return NOERROR;
}

ECode Fragment::SetEnterSharedElementCallback(
    /* [in] */ ISharedElementCallback* callback)
{
    mEnterTransitionCallback = callback;
    if (mEnterTransitionCallback == NULL) {
        mEnterTransitionCallback = SharedElementCallback::NULL_CALLBACK;
    }
    return NOERROR;
}

ECode Fragment::SetEnterSharedElementTransitionCallback(
    /* [in] */ ISharedElementCallback* callback)
{
    return SetEnterSharedElementCallback(callback);
}

ECode Fragment::SetExitSharedElementCallback(
    /* [in] */ ISharedElementCallback* callback)
{
    mExitTransitionCallback = callback;
    if (mExitTransitionCallback == NULL) {
        mExitTransitionCallback = SharedElementCallback::NULL_CALLBACK;
    }

    return NOERROR;
}

ECode Fragment::SetExitSharedElementTransitionCallback(
    /* [in] */ ISharedElementCallback* callback)
{
    return SetExitSharedElementCallback(callback);
}

ECode Fragment::SetEnterTransition(
    /* [in] */ ITransition* transition)
{
    mEnterTransition = transition;
    return NOERROR;
}

ECode Fragment::GetEnterTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition)
    *transition = mEnterTransition;
    REFCOUNT_ADD(*transition)
    return NOERROR;
}

ECode Fragment::SetReturnTransition(
    /* [in] */ ITransition* transition)
{
    mReturnTransition = transition;
    return NOERROR;
}

ECode Fragment::GetReturnTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition)
    if (mReturnTransition == USE_DEFAULT_TRANSITION) {
        return GetEnterTransition(transition);
    }
    else {
        *transition = mReturnTransition;
        REFCOUNT_ADD(*transition)
    }
    return NOERROR;
}


ECode Fragment::SetExitTransition(
    /* [in] */ ITransition* transition)
{
    mExitTransition = transition;
    return NOERROR;
}

ECode Fragment::GetExitTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition)
    *transition = mExitTransition;
    REFCOUNT_ADD(*transition)
    return NOERROR;
}

ECode Fragment::SetReenterTransition(
    /* [in] */ ITransition* transition)
{
    mReenterTransition = transition;
    return NOERROR;
}

ECode Fragment::GetReenterTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition)
    if (mReenterTransition == USE_DEFAULT_TRANSITION) {
        return GetExitTransition(transition);
    }
    else {
        *transition = mReenterTransition;
        REFCOUNT_ADD(*transition)
    }
    return NOERROR;
}

ECode Fragment::SetSharedElementEnterTransition(
    /* [in] */ ITransition* transition)
{
    mSharedElementEnterTransition = transition;
    return NOERROR;
}

ECode Fragment::GetSharedElementEnterTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition)
    *transition = mSharedElementEnterTransition;
    REFCOUNT_ADD(*transition)
    return NOERROR;
}

ECode Fragment::SetSharedElementReturnTransition(
    /* [in] */ ITransition* transition)
{
    mSharedElementReturnTransition = transition;
    return NOERROR;
}

ECode Fragment::GetSharedElementReturnTransition(
    /* [out] */ ITransition** transition)
{
    VALIDATE_NOT_NULL(transition)
    if (mSharedElementReturnTransition == USE_DEFAULT_TRANSITION) {
        return GetSharedElementEnterTransition(transition);
    }
    else {
        *transition = mSharedElementReturnTransition;
        REFCOUNT_ADD(*transition)
    }
    return NOERROR;
}

ECode Fragment::SetAllowEnterTransitionOverlap(
    /* [in] */ Boolean allow)
{
    mAllowEnterTransitionOverlap = allow;
    return NOERROR;
}

ECode Fragment::GetAllowEnterTransitionOverlap(
    /* [out] */ Boolean* allow)
{
    return mAllowEnterTransitionOverlap;
}

ECode Fragment::SetAllowReturnTransitionOverlap(
    /* [in] */ Boolean allow)
{
    mAllowReturnTransitionOverlap = allow;
    return NOERROR;
}

ECode Fragment::GetAllowReturnTransitionOverlap(
    /* [out] */ Boolean* allow)
{
    VALIDATE_NOT_NULL(allow)
    *allow = mAllowReturnTransitionOverlap;
    return NOERROR;
}

ECode Fragment::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    writer->Print(prefix); writer->Print(String("mFragmentId=#"));
//             writer.print(Integer.toHexString(mFragmentId));
    writer->Print(String(" mContainerId=#"));
//             writer.print(Integer.toHexString(mContainerId));
    writer->Print(String(" mTag=")); writer->Println(mTag);
    writer->Print(prefix); writer->Print(String("mState="));
    writer->Print(mState);
    writer->Print(String(" mIndex=")); writer->Print(mIndex);
    writer->Print(String(" mWho=")); writer->Print(mWho);
    writer->Print(String(" mBackStackNesting="));
    writer->Println(mBackStackNesting);
    writer->Print(prefix); writer->Print(String("mAdded="));
    writer->Print(mAdded);
        writer->Print(String(" mRemoving=")); writer->Print(mRemoving);
        writer->Print(String(" mResumed=")); writer->Print(mResumed);
        writer->Print(String(" mFromLayout=")); writer->Print(mFromLayout);
        writer->Print(String(" mInLayout=")); writer->Println(mInLayout);
    writer->Print(prefix); writer->Print(String("mHidden="));
    writer->Print(mHidden);
        writer->Print(String(" mDetached=")); writer->Print(mDetached);
        writer->Print(String(" mMenuVisible=")); writer->Print(mMenuVisible);
        writer->Print(String(" mHasMenu=")); writer->Println(mHasMenu);
    writer->Print(prefix); writer->Print(String("mRetainInstance="));
    writer->Print(mRetainInstance);
        writer->Print(String(" mRetaining=")); writer->Print(mRetaining);
        writer->Print(String(" mUserVisibleHint=")); writer->Println(mUserVisibleHint);
    if (mFragmentManager != NULL) {
        writer->Print(prefix); writer->Print(String("mFragmentManager="));
                writer->Println((IInterface*)mFragmentManager);
    }
    if (mActivity != NULL) {
        writer->Print(prefix); writer->Print(String("mActivity="));
                writer->Println((IInterface*)mActivity);
    }
    if (mParentFragment != NULL) {
        writer->Print(prefix); writer->Print(String("mParentFragment="));
                writer->Println((IInterface*)mParentFragment);
    }
    if (mArguments != NULL) {
        writer->Print(prefix); writer->Print(String("mArguments="));
        writer->Println((IInterface*)mArguments);
    }
    if (mSavedFragmentState != NULL) {
        writer->Print(prefix); writer->Print(String("mSavedFragmentState="));
                writer->Println((IInterface*)mSavedFragmentState);
    }
    if (mSavedViewState != NULL) {
        writer->Print(prefix); writer->Print(String("mSavedViewState="));
                writer->Println((IInterface*)mSavedViewState);
    }
    if (mTarget != NULL) {
        writer->Print(prefix); writer->Print(String("mTarget="));
        writer->Print((IInterface*)mTarget);
                writer->Print(String(" mTargetRequestCode="));
                writer->Println(mTargetRequestCode);
    }
    if (mNextAnim != 0) {
        writer->Print(prefix); writer->Print(String("mNextAnim="));
        writer->Println(mNextAnim);
    }
    if (mContainer != NULL) {
        writer->Print(prefix); writer->Print(String("mContainer="));
        writer->Println((IInterface*)mContainer);
    }
    if (mView != NULL) {
        writer->Print(prefix); writer->Print(String("mView="));
        writer->Println((IInterface*)mView);
    }
    if (mAnimatingAway != NULL) {
        writer->Print(prefix); writer->Print(String("mAnimatingAway="));
        writer->Println((IInterface*)mAnimatingAway);
        writer->Print(prefix); writer->Print(String("mStateAfterAnimating="));
            writer->Println(mStateAfterAnimating);
    }
    if (mLoaderManager != NULL) {
        writer->Print(prefix); writer->Println(String("Loader Manager:"));
        ILoaderManager::Probe(mLoaderManager)->Dump(prefix + "  ", fd, writer, args);
    }
    if (mChildFragmentManager != NULL) {
        StringBuilder sb("Child ");
        sb += mChildFragmentManager;
        sb += ":";
        writer->Print(prefix); writer->Println(sb.ToString());
        IFragmentManager::Probe(mChildFragmentManager)->Dump(prefix + "  ", fd, writer, args);
    }
    return NOERROR;
}

Fragment::FindFragmentByWho(
    /* [in] */ const String& who,
    /* [out] */ IFragment** f)
{
    VALIDATE_NOT_NULL(f);
    if (who.Equals(mWho)) {
        *f = this;
        REFCOUNT_ADD(*f);
        return NOERROR;
    }
    if (mChildFragmentManager != NULL) {
        AutoPtr<IFragment> fragment;
        mChildFragmentManager->FindFragmentByWho(who, (IFragment**)&fragment);
        *f = fragment;
        REFCOUNT_ADD(*f);
        return NOERROR;
    }
    *f = NULL;
    return NOERROR;
}

void Fragment::InstantiateChildFragmentManager()
{
    mChildFragmentManager = new FragmentManagerImpl();
    AutoPtr<FragmentContainerLocal> fc = new FragmentContainerLocal(this);
    mChildFragmentManager->AttachActivity(mActivity, IFragmentContainer::Probe(fc), this);
}

ECode Fragment::PerformCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->NoteStateNotSaved();
    }
    mCalled = FALSE;
    OnCreate(savedInstanceState);
    if (!mCalled) {
        Slogger::E("Fragment", "Fragment  did not call through to super::OnCreate()");
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }
    if (savedInstanceState != NULL) {
        AutoPtr<IParcelable> p;
        savedInstanceState->GetParcelable(Activity::FRAGMENTS_TAG, (IParcelable**)&p);
        if (p != NULL) {
            if (mChildFragmentManager == NULL) {
                InstantiateChildFragmentManager();
            }
            mChildFragmentManager->RestoreAllState(p, NULL);
            mChildFragmentManager->DispatchCreate();
        }
    }
    return NOERROR;
}

ECode Fragment::PerformCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** v)
{
    VALIDATE_NOT_NULL(v);
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->NoteStateNotSaved();
    }
    AutoPtr<IView> view;
    OnCreateView(inflater, container, savedInstanceState, (IView**)&view);
    *v = view;
    REFCOUNT_ADD(*v);
    return NOERROR;
}

ECode Fragment::PerformActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->NoteStateNotSaved();
    }
    mCalled = FALSE;
    OnActivityCreated(savedInstanceState);
    if (!mCalled) {
        Slogger::E("Fragment", "Fragment  did not call through to super::OnActivityCreated()");
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->DispatchActivityCreated();
    }
    return NOERROR;
}

ECode Fragment::PerformStart()
{
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->NoteStateNotSaved();
        Boolean hasexec;
        mChildFragmentManager->ExecPendingActions(&hasexec);
    }
    mCalled = FALSE;
    OnStart();
    if (!mCalled) {
        Slogger::E("Fragment", "Fragment  did not call through to super::OnStart()");
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->DispatchStart();
    }
    if (mLoaderManager != NULL) {
        assert(0 && "TODO");
        //((LoaderManagerImpl*)mLoaderManager.Get())->DoReportStart();
    }
    return NOERROR;
}

ECode Fragment::PerformResume()
{
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->NoteStateNotSaved();
        Boolean hasexec;
        mChildFragmentManager->ExecPendingActions(&hasexec);
    }
    mCalled = FALSE;
    OnResume();
    if (!mCalled) {
        Slogger::E("Fragment", "Fragment  did not call through to super::OnResume()");
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->DispatchResume();
        Boolean hasexec;
        mChildFragmentManager->ExecPendingActions(&hasexec);
    }
    return NOERROR;
}

ECode Fragment::PerformConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    OnConfigurationChanged(newConfig);
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->DispatchConfigurationChanged(newConfig);
    }
    return NOERROR;
}

ECode Fragment::PerformLowMemory()
{
    OnLowMemory();
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->DispatchLowMemory();
    }
    return NOERROR;
}

ECode Fragment::PerformTrimMemory(
    /* [in] */ Int32 level)
{
    OnTrimMemory(level);
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->DispatchTrimMemory(level);
    }
    return NOERROR;
}

ECode Fragment::PerformCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean show = FALSE;
    if (!mHidden) {
        if (mHasMenu && mMenuVisible) {
            show = TRUE;
            OnCreateOptionsMenu(menu, inflater);
        }
        if (mChildFragmentManager != NULL) {
            Boolean dispatch;
            mChildFragmentManager->DispatchCreateOptionsMenu(menu, inflater, &dispatch);
            show |= dispatch;
        }
    }
    *result = show;
    return NOERROR;
}

ECode Fragment::PerformPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean show = FALSE;
    if (!mHidden) {
        if (mHasMenu && mMenuVisible) {
            show = TRUE;
            OnPrepareOptionsMenu(menu);
        }
        if (mChildFragmentManager != NULL) {
            Boolean dispatch;
            mChildFragmentManager->DispatchPrepareOptionsMenu(menu, &dispatch);
            show |= dispatch;
        }
    }
    *result = show;
    return NOERROR;
}

ECode Fragment::PerformOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (!mHidden) {
        if (mHasMenu && mMenuVisible) {
            Boolean selected;
            OnOptionsItemSelected(item, &selected);
            if (selected) {
                *result = TRUE;
                return NOERROR;
            }
        }
        if (mChildFragmentManager != NULL) {
            Boolean itSelected;
            mChildFragmentManager->DispatchOptionsItemSelected(item, &itSelected);
            if (itSelected) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode Fragment::PerformContextItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (!mHidden) {
        Boolean selected;
        OnContextItemSelected(item, &selected);
        if (selected) {
            *result = TRUE;
            return NOERROR;
        }
        if (mChildFragmentManager != NULL) {
            Boolean itSelected;
            mChildFragmentManager->DispatchContextItemSelected(item, &itSelected);
            if (itSelected) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode Fragment::PerformOptionsMenuClosed(
    /* [in] */ IMenu* menu)
{
    if (!mHidden) {
        if (mHasMenu && mMenuVisible) {
            OnOptionsMenuClosed(menu);
        }
        if (mChildFragmentManager != NULL) {
            mChildFragmentManager->DispatchOptionsMenuClosed(menu);
        }
    }
    return NOERROR;
}

ECode Fragment::PerformSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    OnSaveInstanceState(outState);
    if (mChildFragmentManager != NULL) {
        AutoPtr<IParcelable> p;
        mChildFragmentManager->SaveAllState((IParcelable**)&p);
        if (p != NULL) {
            outState->PutParcelable(Activity::FRAGMENTS_TAG, p);
        }
    }
    return NOERROR;
}

ECode Fragment::PerformPause()
{
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->DispatchPause();
    }
    mCalled = FALSE;
    OnPause();
    if (!mCalled) {
        Slogger::E("Fragment", "Fragment did not call through to super::OnPause()");
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }
    return NOERROR;
}

ECode Fragment::PerformStop()
{
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->DispatchStop();
    }
    mCalled = FALSE;
    OnStop();
    if (!mCalled) {
        Slogger::E("Fragment", "Fragment did not call through to super::OnStop()");
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }

    if (mLoadersStarted) {
        mLoadersStarted = FALSE;
        AutoPtr<Activity> act = (Activity*)mActivity.Get();;
        if (!mCheckedForLoaderManager) {
            mCheckedForLoaderManager = TRUE;
            mLoaderManager = act->GetLoaderManager(mWho, mLoadersStarted, FALSE);
        }
        if (mLoaderManager != NULL) {
            assert(0 && "TODO");
            // if (mActivity == NULL || !act->mChangingConfigurations) {
            //     ILoaderManager::Probe(mLoaderManager)->DoStop();
            // }
            // else {
            //     ILoaderManager::Probe(mLoaderManager)->DoRetain();
            // }
        }
    }
    return NOERROR;
}

ECode Fragment::PerformDestroyView()
{
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->DispatchDestroyView();
    }
    mCalled = FALSE;
    OnDestroyView();
    if (!mCalled) {
        Slogger::E("Fragment", "Fragment did not call through to super::OnDestroyView()");
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }
    if (mLoaderManager != NULL) {
        assert(0 && "TODO");
        // ((LoaderManagerImpl*)mLoaderManager.Get())->DoReportNextStart();
    }
    return NOERROR;
}

ECode Fragment::PerformDestroy()
{
    if (mChildFragmentManager != NULL) {
        mChildFragmentManager->DispatchDestroy();
    }
    mCalled = FALSE;
    OnDestroy();
    if (!mCalled) {
        Slogger::E("Fragment", "Fragment did not call through to super::OnDestroy()");
        return E_SUPER_NOT_CALLED_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<ITransition> Fragment::LoadTransition(
    /* [in] */ IContext* context,
    /* [in] */ ITypedArray* typedArray,
    /* [in] */ ITransition* currentValue,
    /* [in] */ ITransition* defaultValue,
    /* [in] */ Int32 id)
{
    if (currentValue != defaultValue) {
        return currentValue;
    }

    Int32 transitionId;
    typedArray->GetResourceId(id, 0, &transitionId);
    AutoPtr<ITransition> transition = defaultValue;
    if (transitionId != 0 && transitionId != R::transition::no_transition) {
        AutoPtr<ITransitionInflater> inflater = TransitionInflater::From(context);
        transition = NULL;
        inflater->InflateTransition(transitionId, (ITransition**)&transition);

        ITransitionSet* ts = ITransitionSet::Probe(transition);
        if (ts != NULL) {
            Int32 count;
            ts->GetTransitionCount(&count);
            if (count == 0) {
                transition = NULL;
            }
        }
    }
    return transition;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
