
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/app/FragmentManagerImpl.h"
#include "elastos/droid/app/CFragmentSavedState.h"
#include "elastos/droid/app/CFragmentManagerState.h"
#include "elastos/droid/app/CBackStackRecord.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/app/CFragmentState.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/app/CBackStackState.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/animation/AnimatorInflater.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/etl/Algorithm.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::EIID_ILayoutInflaterFactory;
using Elastos::Droid::View::EIID_ILayoutInflaterFactory2;
using Elastos::Droid::Animation::AnimatorInflater;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Utility::CSparseArray;

using Elastos::Core::CInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CHashMap;

#define TO_ACTIVITY(obj) ((Activity*)obj)

namespace Elastos {
namespace Droid {
namespace App {

//=======================================================================
// ExecCommitRunnable
//=======================================================================

ECode ExecCommitRunnable::Run()
{
    Boolean executed;
    return mHost->ExecPendingActions(&executed);
}

//=======================================================================
// PopBackStackRunnable1
//=======================================================================
ECode PopBackStackRunnable1::Run()
{
    Boolean hasPopped;
    return mHost->PopBackStackState(TO_ACTIVITY(mHost->mActivity)->mHandler, String(NULL), -1, 0, &hasPopped);
}

//=======================================================================
// PopBackStackRunnable2
//=======================================================================
ECode PopBackStackRunnable2::Run()
{
    Boolean hasPopped;
    return mHost->PopBackStackState(TO_ACTIVITY(mHost->mActivity)->mHandler, mName, -1, mFlags, &hasPopped);
}

//=======================================================================
// PopBackStackRunnable3
//=======================================================================
ECode PopBackStackRunnable3::Run()
{
    Boolean hasPopped;
    return mHost->PopBackStackState(TO_ACTIVITY(mHost->mActivity)->mHandler, String(NULL), mId, mFlags, &hasPopped);
}

//=======================================================================
// AnimatorListener
//=======================================================================
AnimatorListener::AnimatorListener(
    /* [in] */ IViewGroup* container,
    /* [in] */ IView* view,
    /* [in] */ IFragment* fragment,
    /* [in] */ FragmentManagerImpl* host)
{
    mContainer = container;
    mView = view;
    mFragment = fragment;
    mHost = host;
}

ECode AnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mContainer->EndViewTransition(mView);
    AutoPtr<IAnimator> anim;
    mFragment->GetAnimatingAway((IAnimator**)&anim);
    if(anim) {
        mFragment->SetAnimatingAway(NULL);
        Int32 state;
        mFragment->GetStateAfterAnimating(&state);
        mHost->MoveToState(mFragment, state, 0, 0, FALSE);
    }
    return NOERROR;
}

//=======================================================================
// AnimatorListenerEx
//=======================================================================
AnimatorListenerEx::AnimatorListenerEx(
    /* [in] */ IFragment* fragment)
{
    mFragment = fragment;
}

ECode AnimatorListenerEx::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mFragment) {
        AutoPtr<IView> v;
        mFragment->GetView((IView**)&v);
        v->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

//=======================================================================
// FragmentManagerImpl
//=======================================================================

Boolean FragmentManagerImpl::DEBUG = FALSE;
const String FragmentManagerImpl::TAG("FragmentManagerImpl");

CAR_INTERFACE_IMPL_3(FragmentManagerImpl, FragmentManager, IFragmentManagerImpl, ILayoutInflaterFactory, ILayoutInflaterFactory2)

FragmentManagerImpl::FragmentManagerImpl()
    : mExecutingActions(FALSE)
    , mCurState(IFragment::INITIALIZING)
    , mActivity(NULL)
    , mNeedMenuInvalidate(FALSE)
    , mStateSaved(FALSE)
    , mDestroyed(FALSE)
    , mHavePendingDeferredStart(FALSE)
{
    mExecCommit = new ExecCommitRunnable(this);
}

FragmentManagerImpl::~FragmentManagerImpl()
{}

ECode FragmentManagerImpl::BeginTransaction(
    /* [out] */ IFragmentTransaction** transaction)
{
    return CBackStackRecord::New(this, transaction);
}

ECode FragmentManagerImpl::ExecutePendingTransactions(
    /* [out] */ Boolean* executed)
{
   return ExecPendingActions(executed);
}

ECode FragmentManagerImpl::PopBackStack()
{
   AutoPtr<IRunnable> runnable = new PopBackStackRunnable1(this);
   return EnqueueAction(runnable, FALSE);
}

ECode FragmentManagerImpl::PopBackStackImmediate(
    /* [out] */ Boolean* hasPopped)
{
   VALIDATE_NOT_NULL(hasPopped);
   *hasPopped = FALSE;

   FAIL_RETURN(CheckStateLoss());
   Boolean executed;
   ExecutePendingTransactions(&executed);
   return PopBackStackState(TO_ACTIVITY(mActivity)->mHandler, String(NULL), -1, 0, hasPopped);
}

ECode FragmentManagerImpl::PopBackStack(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags)
{
   AutoPtr<IRunnable> runnable = new PopBackStackRunnable2(this, name, flags);
   return EnqueueAction(runnable, FALSE);
}

ECode FragmentManagerImpl::PopBackStackImmediate(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* hasPopped)
{
    VALIDATE_NOT_NULL(hasPopped);
    *hasPopped = FALSE;

    FAIL_RETURN(CheckStateLoss());
    Boolean executed;
    ExecutePendingTransactions(&executed);
    return PopBackStackState(TO_ACTIVITY(mActivity)->mHandler, name, -1, flags, hasPopped);
}

ECode FragmentManagerImpl::PopBackStack(
    /* [in] */ Int32 id,
    /* [in] */ Int32 flags)
{
    if (id < 0) {
//         throw new IllegalArgumentException("Bad id: " + id);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IRunnable> runnable = new PopBackStackRunnable3(this, id, flags);
    EnqueueAction(runnable, FALSE);
    return NOERROR;
}

ECode FragmentManagerImpl::PopBackStackImmediate(
    /* [in] */ Int32 id,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* hasPopped)
{
    VALIDATE_NOT_NULL(hasPopped)
    *hasPopped = FALSE;

    FAIL_RETURN(CheckStateLoss());
    Boolean executed;
    ExecutePendingTransactions(&executed);
    if (id < 0) {
//         throw new IllegalArgumentException("Bad id: " + id);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return PopBackStackState(TO_ACTIVITY(mActivity)->mHandler, String(NULL), id, flags, hasPopped);
}

ECode FragmentManagerImpl::GetBackStackEntryCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 0;
    if (mBackStack != NULL) {
        mBackStack->GetSize(count);
    }
    return NOERROR;
}

ECode FragmentManagerImpl::GetBackStackEntryAt(
    /* [in] */ Int32 index,
    /* [out] */ IFragmentManagerBackStackEntry** bsEntry)
{
    VALIDATE_NOT_NULL(bsEntry);
    AutoPtr<IInterface> obj;
    mBackStack->Get(index, (IInterface**)&obj);
    *bsEntry = IFragmentManagerBackStackEntry::Probe(obj);
    REFCOUNT_ADD(*bsEntry)
    return NOERROR;
}

ECode FragmentManagerImpl::AddOnBackStackChangedListener(
    /* [in] */ IFragmentManagerOnBackStackChangedListener* listener)
{
    if (mBackStackChangeListeners == NULL) {
        CArrayList::New((IArrayList**)&mBackStackChangeListeners);
    }
    mBackStackChangeListeners->Add(listener);
    return NOERROR;
}

ECode FragmentManagerImpl::RemoveOnBackStackChangedListener(
    /* [in] */ IFragmentManagerOnBackStackChangedListener* listener)
{
    if (mBackStackChangeListeners != NULL) {
        mBackStackChangeListeners->Remove(listener);
    }
    return NOERROR;
}

ECode FragmentManagerImpl::PutFragment(
    /* [in] */ IBundle* bundle,
    /* [in] */ const String& key,
    /* [in] */ IFragment* fragment)
{
    Int32 index;
    fragment->GetIndex(&index);
    if (index < 0) {
        Logger::E(TAG, "%s is not currently in the FragmentManager", TO_CSTR(fragment));
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    bundle->PutInt32(key, index);
    return NOERROR;
}

ECode FragmentManagerImpl::GetFragment(
    /* [in] */ IBundle* bundle,
    /* [in] */ const String& key,
    /* [out] */ IFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    *fragment = NULL;

    Int32 index;
    bundle->GetInt32(key, -1, &index);
    if (index == -1) {
        *fragment = NULL;
        return NOERROR;
    }
    Int32 N;
    mActive->GetSize(&N);
    if (index >= N) {
        Logger::E(TAG, "Fragement no longer exists for key %s : index %d", key.string(), index);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IInterface> obj;
    mActive->Get(index, (IInterface**)&obj);
    IFragment* f = IFragment::Probe(obj);
    if (f == NULL) {
        Logger::E(TAG, "Fragement no longer exists for key %s : index %d", key.string(), index);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *fragment = f;
    REFCOUNT_ADD(*fragment);
    return NOERROR;
}

ECode FragmentManagerImpl::SaveFragmentInstanceState(
    /* [in] */ IFragment* fragment,
    /* [out] */ IFragmentSavedState** state)
{
    VALIDATE_NOT_NULL(state);
    *state = NULL;

    Int32 index;
    fragment->GetIndex(&index);
    if (index < 0) {
        Logger::E(TAG, "Fragment %s is not currently in the FragmentManager", TO_CSTR(fragment));
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Int32 fstate;
    fragment->GetState(&fstate);
    if (fstate > IFragment::INITIALIZING) {
        AutoPtr<IBundle> result;
        SaveFragmentBasicState(fragment, (IBundle**)&result);
        if (result != NULL) {
            return CFragmentSavedState::New(result, state);
        }
    }

    return NOERROR;
}

ECode FragmentManagerImpl::IsDestroyed(
    /* [out] */ Boolean* destroyed)
{
    VALIDATE_NOT_NULL(destroyed);
    *destroyed = mDestroyed;
    return NOERROR;
}

ECode FragmentManagerImpl::ToString(
    /* [out] */ String* s)
{
    VALIDATE_NOT_NULL(s);
    StringBuilder sb(128);
    sb += "FragmentManager{";
    sb += StringUtils::ToHexString((Int32)this);
    sb += " in ";
    if (mParent != NULL) {
//         DebugUtils.buildShortClassTag(mParent, sb);
    } else {
//         DebugUtils.buildShortClassTag(mActivity, sb);
    }
    sb += "}}";
    *s = sb.ToString();
    return NOERROR;
}

ECode FragmentManagerImpl::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    String innerPrefix = prefix + "    ";

    Int32 N;
    if (mActive != NULL) {
        mActive->GetSize(&N);
        if (N > 0) {
            writer->Print(prefix);
            writer->Print(String("Active Fragments in "));
            writer->Print(StringUtils::ToHexString((Int32)this));
            writer->Println(String(":"));
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                mActive->Get(i, (IInterface**)&obj);
                IFragment* f = IFragment::Probe(obj);
                writer->Print(prefix);
                writer->Print(String("  #"));
                writer->Print(i);
                writer->Print(String(": "));
                String fstring = Object::ToString(f);
                writer->Println(fstring);
                if (f != NULL) {
                    f->Dump(innerPrefix, fd, writer, args);
                }
            }
        }
    }

    if (mAdded != NULL) {
        mAdded->GetSize(&N);
        if (N > 0) {
            writer->Print(prefix);
            writer->Println(String("Added Fragments:"));
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                mAdded->Get(i, (IInterface**)&obj);
                IFragment* f = IFragment::Probe(obj);
                writer->Print(prefix);
                writer->Print(String("  #"));
                writer->Print(i);
                writer->Print(String(": "));
                String fstring = Object::ToString(f);
                writer->Println(fstring);
            }
        }
    }

   if (mCreatedMenus != NULL) {
        Int32 N;
        mCreatedMenus->GetSize(&N);
        if (N > 0) {
            writer->Print(prefix);
            writer->Println(String("Fragments Created Menus:"));
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                mCreatedMenus->Get(i, (IInterface**)&obj);
                IFragment* f = IFragment::Probe(obj);
                writer->Print(prefix);
                writer->Print(String("  #"));
                writer->Print(i);
                writer->Print(String(": "));
                writer->Println(Object::ToString(f));
            }
        }
    }

//TODO:
//     if (mBackStack != NULL) {
//         if (mBackStack->IsEmpty() == FALSE) {
//             writer->Print(prefix);
//             writer->Println("Back Stack:");
//             List<IBackStackRecord* >::Iterator it;
//             for (it = mBackStack->Begin(); it != mBackStack->End(); ++it) {
//                 AutoPtr<IBackStackRecord> bs = *it;
//                 writer->Print(prefix);
//                 writer->Print("  #");
//                 writer->Print(i);
//                 writer->Print(": ");
//                 String bsString;
//                 bs->ToString(&bsString);
//                 writer->Println(bsString);
//                 bs->Dump(innerPrefix, fd, writer, args);
//             }
//         }
//     }
//TODO:
//     {
//         Object::AutoLock lock(this);
//         if (mBackStackIndices != NULL) {
//             if (mBackStackIndices->Begin() > mBackStackIndices->End()) {
//                 writer->Print(prefix);
//                 writer->Println("Back Stack Indices:");
//                 List<IBackStackRecord* >::Iterator it;
//                 for (it = mBackStackIndices->Begin(); it != mBackStackIndices->End(); ++it) {
// //                 for (Int32 i=0; i<N; i++) {
//                     AutoPtr<IBackStackRecord> bs = *it;
//                     writer->Print(prefix);
//                     writer->Print("  #");
//                     writer->Print(i);
//                     writer->Print(": ");
//                     writer->Println(bs->Probe(EIID_IInterface);
//                 }
//             }
//         }

//         if (mAvailBackStackIndices != NULL && mAvailBackStackIndices->IsEmpty() == FALSE) {
//             writer->Print(prefix);
//             writer->Print("mAvailBackStackIndices: ");
// //            writer->Println(Arrays.toString(mAvailBackStackIndices.toArray()));
//         }
//     }

    if (mPendingActions != NULL) {
        mPendingActions->GetSize(&N);
        if (N > 0) {
            writer->Print(prefix);
            writer->Println(String("Pending Actions:"));
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                mPendingActions->Get(i, (IInterface**)&obj);
                IRunnable* r = IRunnable::Probe(obj);
                writer->Print(prefix);
                writer->Print(String("  #"));
                writer->Print(i);
                writer->Print(String(": "));
                writer->Println(r);
            }
        }
    }

    writer->Print(prefix);
    writer->Println(String("FragmentManager misc state:"));
    writer->Print(prefix);
    writer->Print(String("  mActivity="));
    writer->Println(mActivity->Probe(EIID_IInterface));
    writer->Print(prefix);
    writer->Print(String("  mContainer="));
    writer->Println(mContainer->Probe(EIID_IInterface));
    if (mParent != NULL) {
        writer->Print(prefix);
        writer->Print(String("  mParent="));
        writer->Println(mParent->Probe(EIID_IInterface));
    }
    writer->Print(prefix);
    writer->Print(String("  mCurState="));
    writer->Print(mCurState);
    writer->Print(String(" mStateSaved="));
    writer->Print(mStateSaved);
    writer->Print(String(" mDestroyed="));
    writer->Println(mDestroyed);
    if (mNeedMenuInvalidate) {
        writer->Print(prefix);
        writer->Print(String("  mNeedMenuInvalidate="));
        writer->Println(mNeedMenuInvalidate);
    }
    if (mNoTransactionsBecause != NULL) {
        writer->Print(prefix);
        writer->Print(String("  mNoTransactionsBecause="));
        writer->Println(mNoTransactionsBecause);
    }
    if (mAvailIndices != NULL && mAvailIndices->GetSize(&N), N > 0) {
        writer->Print(prefix);
        writer->Print(String("  mAvailIndices: "));
        // TODO:
//        writer.println(Arrays.toString(mAvailIndices.toArray()));
    }
    return NOERROR;
}

ECode FragmentManagerImpl::LoadAnimator(
    /* [in] */ IFragment* fragment,
    /* [in] */ Int32 transit,
    /* [in] */ Boolean enter,
    /* [in] */ Int32 transitionStyle,
    /* [out] */ IAnimator** animator)
{
    VALIDATE_NOT_NULL(animator);
    *animator = NULL;

    Int32 fanim;
    fragment->GetNextAnim(&fanim);
    AutoPtr<IAnimator> animObj;
    fragment->OnCreateAnimator(transit, enter, fanim, (IAnimator**)&animObj);
    if (animObj != NULL) {
        *animator = animObj;
        REFCOUNT_ADD(*animator);
        return NOERROR;
    }

    IContext* activityCtx = IContext::Probe(mActivity);
    if (fanim != 0) {
        AutoPtr<IAnimator> anim;
        AnimatorInflater::LoadAnimator(activityCtx, fanim, (IAnimator**)&anim);
        if (anim != NULL) {
            *animator = anim;
            REFCOUNT_ADD(*animator);
            return NOERROR;
        }
    }

    if (transit == 0) {
        return NOERROR;
    }

    Int32 styleIndex;
    TransitToStyleIndex(transit, enter, &styleIndex);
    if (styleIndex < 0) {
        return NOERROR;
    }

    AutoPtr<IWindow> window;
    mActivity->GetWindow((IWindow**)&window);
    if (transitionStyle == 0 && window != NULL) {
        AutoPtr<IWindowManagerLayoutParams> attr;
        window->GetAttributes((IWindowManagerLayoutParams**)&attr);
        Int32 wAnimations;
        attr->GetWindowAnimations(&wAnimations);
        transitionStyle = wAnimations;
    }
    if (transitionStyle == 0) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::FragmentAnimation);
    AutoPtr<ITypedArray> attrs;
    activityCtx->ObtainStyledAttributes(transitionStyle, attrIds, (ITypedArray**)&attrs);
    Int32 anim;
    attrs->GetResourceId(styleIndex, 0, &anim);
    attrs->Recycle();

    if (anim == 0) {
        return NOERROR;
    }
    AutoPtr<IAnimator> a;
    AnimatorInflater::LoadAnimator(activityCtx, anim, (IAnimator**)&a);
    *animator = a;
    REFCOUNT_ADD(*animator)
    return NOERROR;
}

ECode FragmentManagerImpl::PerformPendingDeferredStart(
    /* [in] */ IFragment* f)
{
    Boolean start;
    f->GetDeferStart(&start);
    if (start) {
        if (mExecutingActions) {
            // Wait until we're done executing our pending transactions
            mHavePendingDeferredStart = TRUE;
            return NOERROR;
        }
        f->SetDeferStart(FALSE);
        MoveToState(f, mCurState, 0, 0, FALSE);
    }
    return NOERROR;
}

ECode FragmentManagerImpl::MoveToState(
    /* [in] */ IFragment* f,
    /* [in] */ Int32 newState,
    /* [in] */ Int32 transit,
    /* [in] */ Int32 transitionStyle,
    /* [in] */ Boolean keepActive)
{
    Int32 state;
    f->GetState(&state);
    Boolean removing;
    f->IsRemoving(&removing);
    if (DEBUG && FALSE) Logger::V(TAG, "MoveToState: %s oldState=%d newState=%d mRemoving=%d",
        TO_CSTR(f), state, newState, removing /*" Callers=" + *Debug->GetCallers(5)*/);

    // Fragments that are not currently added will sit in the onCreate() state.
    Boolean added;
    f->GetAdded(&added);
    Boolean detached;
    f->IsDetached(&detached);
    if ((!added || detached) && newState > IFragment::CREATED) {
        newState = IFragment::CREATED;
    }
    if (removing && newState > state) {
        // While removing a fragment, we can't change it to a higher state.
        newState = state;
    }
    // Defer start if requested; don't allow it to move to STARTED or higher
    // if it's not already started.
    Boolean deferStart;
    f->GetDeferStart(&deferStart);
    if (deferStart && state < IFragment::STARTED && newState > IFragment::STOPPED) {
        newState = IFragment::STOPPED;
    }
    Boolean fromLayout;
    f->GetFromLayout(&fromLayout);
    Boolean inLayout;
    f->IsInLayout(&inLayout);
    AutoPtr<IAnimator> animatingAway;
    f->GetAnimatingAway((IAnimator**)&animatingAway);
    Int32 stateAfterAnimating;
    f->GetStateAfterAnimating(&stateAfterAnimating);
    AutoPtr<IBundle> savedFragmentState;
    f->GetSavedFragmentState((IBundle**)&savedFragmentState);
    if (state < newState) {
        // For fragments that are created from a layout, when restoring from
        // state we don't want to allow them to be created until they are
        // being reloaded from the layout.
        if (fromLayout && !inLayout) {
            return NOERROR;
        }
        if (animatingAway != NULL) {
            // The fragment is currently being animated...  but!  Now we
            // want to move our state back up.  Give up on waiting for the
            // animation, move to whatever the final state should be once
            // the animation is done, and then we can proceed from there.
            animatingAway = NULL;
            MoveToState(f, stateAfterAnimating, 0, 0, TRUE);
        }
        switch (state) {
            case IFragment::INITIALIZING:
            {
                if (DEBUG) Logger::V(TAG, "moveto CREATED: %s", TO_CSTR(f));
                if (savedFragmentState != NULL) {
                   AutoPtr<ISparseArray> parcelableMap;
                   savedFragmentState->GetSparseParcelableArray(
                        IFragmentManagerImpl::VIEW_STATE_TAG, (ISparseArray**)&parcelableMap);
                    f->SetSavedViewState(parcelableMap);
                    AutoPtr<IFragment> fragment;
                    GetFragment(savedFragmentState,
                        IFragmentManagerImpl::TARGET_STATE_TAG, (IFragment**)&fragment);
                    f->SetTarget(fragment);
                    if (fragment != NULL) {
                        Int32 value;
                        savedFragmentState->GetInt32(
                                IFragmentManagerImpl::TARGET_REQUEST_CODE_STATE_TAG, 0, &value);
                        f->SetTargetRequestCode(value);
                    }
                    Boolean value;
                    savedFragmentState->GetBoolean(
                            IFragmentManagerImpl::USER_VISIBLE_HINT_TAG, TRUE, &value);
                    f->SetUserVisibleHint(value);
                    if (!value) {
                        f->SetDeferStart(TRUE);
                        if (newState > IFragment::STOPPED) {
                            newState = IFragment::STOPPED;
                        }
                    }
                }
                f->SetActivity(mActivity);
                f->SetParentFragment(mParent);
                if (mParent != NULL) {
                    Fragment* fragment = (Fragment*)mParent.Get();
                    f->SetFragmentManager(fragment->mChildFragmentManager);
                }
                else {
                    f->SetFragmentManager(TO_ACTIVITY(mActivity)->mFragments);
                }
                f->SetCalled(FALSE);
                f->OnAttach(mActivity);
                Boolean called;
                f->GetCalled(&called);
                if (!called) {
                    Logger::E(TAG, "Fragment %s did not call through to super::OnAttach()", TO_CSTR(f));
                    return E_SUPER_NOT_CALLED_EXCEPTION;
                }
                AutoPtr<IFragment> parentFragment;
                f->GetParentFragment((IFragment**)&parentFragment);
                if (parentFragment == NULL) {
                    mActivity->OnAttachFragment(f);
                }
                Boolean retaining;
                f->GetRetaining(&retaining);
                if (!retaining) {
                    f->PerformCreate(savedFragmentState);
                }
                f->SetRetaining(FALSE);
                if (fromLayout) {
                    // For fragments that are part of the content view
                    // layout, we need to instantiate the view immediately
                    // and the inflater will take care of adding it.
                    AutoPtr<ILayoutInflater> layoutInflater;
                    f->GetLayoutInflater(savedFragmentState, (ILayoutInflater**)&layoutInflater);
                    AutoPtr<IView> createView;
                    f->PerformCreateView(layoutInflater, NULL, savedFragmentState, (IView**)&createView);
                    f->SetView(createView);
                    AutoPtr<IView> view;
                    f->GetView((IView**)&view);
                    if (view != NULL) {
                        view->SetSaveFromParentEnabled(FALSE);
                        Boolean hidden;
                        f->IsHidden(&hidden);
                        if (hidden) view->SetVisibility(IView::GONE);
                        f->OnViewCreated(view, savedFragmentState);
                    }
                }
            }
            case IFragment::CREATED:
                if (newState > IFragment::CREATED) {
                    if (DEBUG) Logger::V(TAG, "moveto ACTIVITY_CREATED: %s", TO_CSTR(f));
                    if (!fromLayout) {
                        AutoPtr<IViewGroup> container = NULL;
                        Int32 containerId;
                        f->GetContainerId(&containerId);
                        if (containerId != 0) {
                            AutoPtr<IView> findView;
                            mContainer->FindViewById(containerId, (IView**)&findView);
                            container = IViewGroup::Probe(findView);
                            Boolean restored;
                            f->GetRestored(&restored);
                            if (container == NULL && !restored) {
                                AutoPtr<IResources> res;
                                f->GetResources((IResources**)&res);
                                String resName;
                                res->GetResourceName(containerId, &resName);
                                Logger::E(TAG, "No view found for id 0x%08x (%s) for fragment %s",
                                    containerId, resName.string(), TO_CSTR(f));
                                return E_ILLEGAL_ARGUMENT_EXCEPTION;
                            }
                        }
                        f->SetContainer(container);
                        AutoPtr<ILayoutInflater> lInflater;
                        f->GetLayoutInflater(savedFragmentState, (ILayoutInflater**)&lInflater);
                        AutoPtr<IView> cView;
                        f->PerformCreateView(lInflater, container, savedFragmentState, (IView**)&cView);
                        f->SetView(cView);
                        AutoPtr<IView> view;
                        f->GetView((IView**)&view);
                        if (view != NULL) {
                            view->SetSaveFromParentEnabled(FALSE);
                            if (container != NULL) {
                                AutoPtr<IAnimator> anim;
                                LoadAnimator(f, transit, TRUE, transitionStyle, (IAnimator**)&anim);
                                if (anim != NULL) {
                                    anim->SetTarget(view);
                                    anim->Start();
                                }
                                container->AddView(view);
                            }
                            Boolean hidden;
                            f->IsHidden(&hidden);
                            if (hidden) view->SetVisibility(IView::GONE);
                            f->OnViewCreated(view, savedFragmentState);
                        }
                    }

                    f->PerformActivityCreated(savedFragmentState);
                    AutoPtr<IView> view;
                    f->GetView((IView**)&view);
                    if (view != NULL) {
                        f->RestoreViewState(savedFragmentState);
                    }
                    f->SetSavedFragmentState(NULL);
                }
            case IFragment::ACTIVITY_CREATED:
            case IFragment::STOPPED:
                if (newState > IFragment::STOPPED) {
                    if (DEBUG) Logger::V(TAG, "moveto STARTED: %s", TO_CSTR(f));
                    f->PerformStart();
                }
            case IFragment::STARTED:
                if (newState > IFragment::STARTED) {
                    if (DEBUG) Logger::V(TAG, "moveto RESUMED: %s", TO_CSTR(f));
                    f->SetResumed(TRUE);
                    f->PerformResume();
                    // Get rid of this in case we saved it and never needed it.
                    f->SetSavedFragmentState(NULL);
                    f->SetSavedViewState(NULL);
                }
        }
    } else if (state > newState) {
        switch (state) {
            case IFragment::RESUMED:
                if (newState < IFragment::RESUMED) {
                    if (DEBUG) Logger::V(TAG, "movefrom RESUMED: %s", TO_CSTR(f));
                    f->PerformPause();
                    f->SetResumed(FALSE);
                }
            case IFragment::STARTED:
                if (newState < IFragment::STARTED) {
                    if (DEBUG) Logger::V(TAG, "movefrom STARTED: %s", TO_CSTR(f));
                    f->PerformStop();
                }
            case IFragment::STOPPED:
            case IFragment::ACTIVITY_CREATED:
                if (newState < IFragment::ACTIVITY_CREATED) {
                    if (DEBUG) Logger::V(TAG, "movefrom ACTIVITY_CREATED: %s", TO_CSTR(f));
                    AutoPtr<IView> view;
                    f->GetView((IView**)&view);
                    if (view != NULL) {
                        // Need to save the current view state if not
                        // done already.
                        Boolean finishing;
                        mActivity->IsFinishing(&finishing);
                        AutoPtr<ISparseArray> savedViewState;
                        f->GetSavedViewState((ISparseArray**)&savedViewState);
                        if (!finishing && savedViewState == NULL) {
                            SaveFragmentViewState(f);
                        }
                    }
                    f->PerformDestroyView();
                    AutoPtr<IViewGroup> container;
                    f->GetContainer((IViewGroup**)&container);
                    if (view != NULL && container != NULL) {
                        AutoPtr<IAnimator> anim;
                        if (mCurState > IFragment::INITIALIZING && !mDestroyed) {
                            LoadAnimator(f, transit, FALSE, transitionStyle, (IAnimator**)&anim);
                        }
                        if (anim != NULL) {
                            AutoPtr<IView> view;
                            f->GetView((IView**)&view);
                            AutoPtr<IFragment> fragment = f;
                            container->StartViewTransition(view);
                            f->SetAnimatingAway(anim);
                            f->SetStateAfterAnimating(newState);
                            AutoPtr<AnimatorListenerAdapter> l = new AnimatorListener(container, view, fragment, this);
                            anim->AddListener(l);
                            anim->SetTarget(view);
                            anim->Start();
                        }
                        IViewManager::Probe(container)->RemoveView(view);
                    }
                    f->SetContainer(NULL);
                    f->SetView(NULL);
                }
            case IFragment::CREATED:
                if (newState < IFragment::CREATED) {
                    if (mDestroyed) {
                        if (animatingAway != NULL) {
                            // The fragment's containing activity is
                            // being destroyed, but this fragment is
                            // currently animating away.  Stop the
                            // animation right now -- it is not needed,
                            // and we can't wait any more on destroying
                            // the fragment.
                            AutoPtr<IAnimator> anim = animatingAway;
                            f->SetAnimatingAway(NULL);
                            anim->Cancel();
                        }
                    }
                    if (animatingAway != NULL) {
                        // We are waiting for the fragment's view to finish
                        // animating away.  Just make a note of the state
                        // the fragment now should move to once the animation
                        // is done.
                        f->SetStateAfterAnimating(newState);
                        newState = IFragment::CREATED;
                    } else {
                        if (DEBUG) Logger::V(TAG, "movefrom CREATED: %s", TO_CSTR(f));
                        Boolean retaining;
                        f->GetRetaining(&retaining);
                        if (!retaining) {
                            f->PerformDestroy();
                        }

                        f->SetCalled(FALSE);
                        f->OnDetach();
                        Boolean called;
                        f->GetCalled(&called);
                        if (!called) {
//                             throw new SuperNotCalledException("Fragment " + f
//                                     + " did not call through to super.onDetach()");
                            return E_SUPER_NOT_CALLED_EXCEPTION;
                        }
                        if (!keepActive) {
                            if (!retaining) {
                                MakeInactive(f);
                            } else {
                                f->SetActivity(NULL);
                                f->SetParentFragment(NULL);
                                f->SetFragmentManager(NULL);
                                Fragment* fragment = (Fragment*)f;
                                fragment->mChildFragmentManager = NULL;
                            }
                        }
                    }
               }
        }
    }

    f->SetState(newState);
    return NOERROR;
}

ECode FragmentManagerImpl::MoveToState(
    /* [in] */ IFragment* f)
{
    return MoveToState(f, mCurState, 0, 0, FALSE);
}

ECode FragmentManagerImpl::MoveToState(
    /* [in] */ Int32 newState,
    /* [in] */ Boolean always)
{
    return MoveToState(newState, 0, 0, always);
}

ECode FragmentManagerImpl::MoveToState(
    /* [in] */ Int32 newState,
    /* [in] */ Int32 transit,
    /* [in] */ Int32 transitStyle,
    /* [in] */ Boolean always)
{
    if (mActivity == NULL && newState != IFragment::INITIALIZING) {
//         throw new IllegalStateException("No activity");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (!always && mCurState == newState) {
        return NOERROR;
    }

    mCurState = newState;
    if (mActive != NULL) {
        Boolean loadersRunning = FALSE;
        Int32 N;
        mActive->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mActive->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            if (f != NULL) {
                MoveToState(f, newState, transit, transitStyle, FALSE);
                AutoPtr<ILoaderManagerImpl> loaderManager;
                f->GetLoaderManagerValue((ILoaderManagerImpl**)&loaderManager);
                if (loaderManager != NULL) {
                    Boolean hasLoaders;
                    loaderManager->HasRunningLoaders(&hasLoaders);
                    loadersRunning |= hasLoaders;
                }
            }
        }

        if (!loadersRunning) {
            StartPendingDeferredFragments();
        }

        if (mNeedMenuInvalidate && mActivity != NULL && mCurState == IFragment::RESUMED) {
            mActivity->InvalidateOptionsMenu();
            mNeedMenuInvalidate = FALSE;
        }
    }
    return NOERROR;
}

ECode FragmentManagerImpl::StartPendingDeferredFragments()
{
    if (mActive == NULL) return NOERROR;

    Int32 N;
    mActive->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mActive->Get(i, (IInterface**)&obj);
        IFragment* f = IFragment::Probe(obj);
        if (f != NULL) {
            PerformPendingDeferredStart(f);
        }
    }
    return NOERROR;
}

ECode FragmentManagerImpl::MakeActive(
    /* [in] */ IFragment* f)
{
    Int32 index;
    f->GetIndex(&index);
    if (index >= 0) {
        return NOERROR;
    }

    Int32 N;
    if (mAvailIndices == NULL || (mAvailIndices->GetSize(&N), N <= 0)) {
        if (mActive == NULL) {
            CArrayList::New((IArrayList**)&mActive);
        }
        Int32 N;
        mActive->GetSize(&N);
        f->SetIndex(N, mParent);
        mActive->Add(f);
    }
    else {
        AutoPtr<IInterface> idxObj;
        mAvailIndices->Remove(N - 1, (IInterface**)&idxObj);
        Int32 idx;
        IInteger32::Probe(idxObj)->GetValue(&idx);
        f->SetIndex(idx, mParent);
        f->GetIndex(&index);
        mActive->Set(index, f);
    }
    if (DEBUG) Logger::V(TAG, "Allocated fragment index %s", TO_CSTR(f));
    return NOERROR;
}

ECode FragmentManagerImpl::MakeInactive(
    /* [in] */ IFragment* f)
{
    Int32 index;
    f->GetIndex(&index);
    if (index < 0) {
        return NOERROR;
    }

    if (DEBUG) Logger::V(TAG, "Freeing fragment index %s", TO_CSTR(f));
    mActive->Set(index, NULL);
    if (mAvailIndices == NULL) {
        CArrayList::New((IArrayList**)&mAvailIndices);
    }
    mAvailIndices->Add(CoreUtils::Convert(index));
    String who;
    f->GetWho(&who);
    TO_ACTIVITY(mActivity)->InvalidateFragment(who);
    f->InitState();
    return NOERROR;
}

ECode FragmentManagerImpl::AddFragment(
    /* [in] */ IFragment* fragment,
    /* [in] */ Boolean MoveToStateNow)
{
    if (mAdded == NULL) {
        CArrayList::New((IArrayList**)&mAdded);
    }
    if (DEBUG) Logger::V(TAG, "add: %s", TO_CSTR(fragment));
    MakeActive(fragment);
    Boolean detached;
    fragment->IsDetached(&detached);
    if (!detached) {
        Boolean contains = FALSE;
        if (mAdded->Contains(fragment, &contains), contains) {
            Logger::E(TAG, "Fragment already added: %s", TO_CSTR(fragment));
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mAdded->Add(fragment);
        fragment->SetAdded(TRUE);
        fragment->SetRemoving(FALSE);
        Boolean hasMenu, menuVisible;
        if ((fragment->GetHasMenu(&hasMenu), hasMenu) &&
            (fragment->GetMenuVisible(&menuVisible), menuVisible)) {
            mNeedMenuInvalidate = TRUE;
        }
        if (MoveToStateNow) {
            MoveToState(fragment);
        }
    }
    return NOERROR;
}

ECode FragmentManagerImpl::RemoveFragment(
    /* [in] */ IFragment* fragment,
    /* [in] */ Int32 transition,
    /* [in] */ Int32 transitionStyle)
{

    if (DEBUG) {
        Int32 backStackNesting;
        fragment->GetBackStackNesting(&backStackNesting);
        Logger::V(TAG, "remove: %s nesting=%d", TO_CSTR(fragment), backStackNesting);
    }
    Boolean inbs, detached;
    fragment->IsInBackStack(&inbs);
    Boolean inactive = !inbs;
    if ((fragment->IsDetached(&detached), !detached) || inactive) {
        if (FALSE) {
            // Would be nice to catch a bad remove here, but we need
            // time to test this to make sure we aren't crashes cases
            // where it is not a problem.
            Boolean contains = FALSE;
            if (mAdded->Contains(fragment, &contains), !contains) {
                Logger::E(TAG, "Fragment not added: %s", TO_CSTR(fragment));
                return E_ILLEGAL_STATE_EXCEPTION;
            }
        }
        if (mAdded != NULL) {
            mAdded->Remove(fragment);
        }
        Boolean hasMenu, menuVisible;
        if ((fragment->GetHasMenu(&hasMenu), hasMenu) &&
            (fragment->GetMenuVisible(&menuVisible), menuVisible)) {
            mNeedMenuInvalidate = TRUE;
        }
        fragment->SetAdded(FALSE);
        fragment->SetRemoving(TRUE);
        MoveToState(fragment, inactive ? IFragment::INITIALIZING : IFragment::CREATED,
                transition, transitionStyle, FALSE);
    }
    return NOERROR;
}

ECode FragmentManagerImpl::HideFragment(
    /* [in] */ IFragment* fragment,
    /* [in] */ Int32 transition,
    /* [in] */ Int32 transitionStyle)
{
    if (DEBUG) Logger::V(TAG, "hide: %s", TO_CSTR(fragment));
    Boolean hidden;
    fragment->IsHidden(&hidden);
    if (!hidden) {
        fragment->SetHidden(TRUE);
        AutoPtr<IView> view;
        fragment->GetView((IView**)&view);
        if (view != NULL) {
            AutoPtr<IAnimator> anim;
            LoadAnimator(fragment, transition, FALSE, transitionStyle, (IAnimator**)&anim);
            if (anim != NULL) {
                anim->SetTarget(view);
                // Delay the actual hide operation until the animation finishes, otherwise
                // the fragment will just immediately disappear
                AutoPtr<IFragment> finalFragment = fragment;
                AutoPtr<AnimatorListenerAdapter> l = new AnimatorListenerEx(finalFragment);
                anim->AddListener(l);
                anim->Start();
            } else {
                view->SetVisibility(IView::GONE);
            }
        }
        Boolean added, hasMenu, menuVisible;
        fragment->GetAdded(&added);
        fragment->GetHasMenu(&hasMenu);
        fragment->GetMenuVisible(&menuVisible);
        if (added && hasMenu && menuVisible) {
            mNeedMenuInvalidate = TRUE;
        }
        fragment->OnHiddenChanged(TRUE);
    }
    return NOERROR;
}

ECode FragmentManagerImpl::ShowFragment(
    /* [in] */ IFragment* fragment,
    /* [in] */ Int32 transition,
    /* [in] */ Int32 transitionStyle)
{
    if (DEBUG) Logger::V(TAG, "show: %s", TO_CSTR(fragment));
    Boolean hidden;
    fragment->IsHidden(&hidden);
    if (hidden) {
        fragment->SetHidden(FALSE);
        AutoPtr<IView> view;
        fragment->GetView((IView**)&view);
        if (view != NULL) {
            AutoPtr<IAnimator> anim;
            LoadAnimator(fragment, transition, TRUE, transitionStyle, (IAnimator**)&anim);
            if (anim != NULL) {
                anim->SetTarget(view);
                anim->Start();
            }
            view->SetVisibility(IView::VISIBLE);
        }
        Boolean added, hasMenu, menuVisible;
        fragment->GetAdded(&added);
        fragment->GetHasMenu(&hasMenu);
        fragment->GetMenuVisible(&menuVisible);
        if (added && hasMenu && menuVisible) {
            mNeedMenuInvalidate = TRUE;
        }
        fragment->OnHiddenChanged(FALSE);
    }
    return NOERROR;
}

ECode FragmentManagerImpl::DetachFragment(
    /* [in] */ IFragment* fragment,
    /* [in] */ Int32 transition,
    /* [in] */ Int32 transitionStyle)
{
    if (DEBUG) Logger::V(TAG, "detach: %s", TO_CSTR(fragment));
    Boolean detached;
    fragment->IsDetached(&detached);
    if (!detached) {
        fragment->SetDetached(TRUE);
        Boolean added;
        fragment->GetAdded(&added);
        if (added) {
            // We are not already in back stack, so need to remove the fragment.
            if (mAdded != NULL) {
                if (DEBUG) Logger::V(TAG, "remove from detach: %s", TO_CSTR(fragment));
                mAdded->Remove(fragment);
            }
            Boolean hasMenu, menuVisible;
            if ((fragment->GetHasMenu(&hasMenu), hasMenu) &&
                (fragment->GetMenuVisible(&menuVisible), menuVisible)) {
                mNeedMenuInvalidate = TRUE;
            }
            fragment->SetAdded(FALSE);
            MoveToState(fragment, IFragment::CREATED, transition, transitionStyle, FALSE);
        }
    }
    return NOERROR;
}

ECode FragmentManagerImpl::AttachFragment(
    /* [in] */ IFragment* fragment,
    /* [in] */ Int32 transition,
    /* [in] */ Int32 transitionStyle)
{
    if (DEBUG) Logger::V(TAG, "attach: %s", TO_CSTR(fragment));
    Boolean detached;
    fragment->IsDetached(&detached);
    if (detached) {
        fragment->SetDetached(FALSE);
        Boolean added;
        fragment->GetAdded(&added);
        if (!added) {
            if (mAdded == NULL) {
                CArrayList::New((IArrayList**)&mAdded);
            }
            Boolean contains = FALSE;
            if (mAdded->Contains(fragment, &contains), contains) {
                Logger::E(TAG, "Fragment already added: %s", TO_CSTR(fragment));
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            if (DEBUG) Logger::V(TAG, "add from attach: %s", TO_CSTR(fragment));
            mAdded->Add(fragment);
            fragment->SetAdded(TRUE);
            Boolean hasMenu, menuVisible;
            if ((fragment->GetHasMenu(&hasMenu), hasMenu) &&
                (fragment->GetMenuVisible(&menuVisible), menuVisible)) {
                mNeedMenuInvalidate = TRUE;
            }
            MoveToState(fragment, mCurState, transition, transitionStyle, FALSE);
        }
    }
    return NOERROR;
}

ECode FragmentManagerImpl::FindFragmentById(
    /* [in] */ Int32 id,
    /* [out] */ IFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);

    if (mAdded != NULL) {
        // First look through added fragments.
        Int32 N;
        mAdded->GetSize(&N);
        for (Int32 i = N - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mAdded->Get(i, (IInterface**)&obj);
            Logger::I(TAG, " >> mAdded %d: %s", i, TO_CSTR(obj));
            IFragment* f = IFragment::Probe(obj);
            Int32 fId;
            if (f != NULL && (f->GetFragmentId(&fId), fId == id)) {
                *fragment = f;
                REFCOUNT_ADD(*fragment);
                return NOERROR;
            }
        }
    }
    if (mActive != NULL) {
        // Now for any known fragment.
        Int32 N;
        mActive->GetSize(&N);
        for (Int32 i = N - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mActive->Get(i, (IInterface**)&obj);
            Logger::I(TAG, " >> mActive %d: %s", i, TO_CSTR(obj));
            IFragment* f = IFragment::Probe(obj);
            Int32 fId;
            if (f != NULL && (f->GetFragmentId(&fId), fId == id)) {
                *fragment = f;
                REFCOUNT_ADD(*fragment);
                return NOERROR;
            }
        }
    }
    *fragment = NULL;
    Logger::I(TAG, " >> FindFragmentById: %08x, null", id);
    return NOERROR;
}

ECode FragmentManagerImpl::FindFragmentByTag(
    /* [in] */ const String& tag,
    /* [out] */ IFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    if (mAdded != NULL && !tag.IsNull()) {
        // First look through added fragments.
        Int32 N;
        mAdded->GetSize(&N);
        for (Int32 i = N - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mAdded->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            String ftag;
            if (f != NULL && (f->GetTag(&ftag), tag.Equals(ftag))) {
                *fragment = f;
                REFCOUNT_ADD(*fragment);
                return NOERROR;
            }
        }
    }
    if (mActive != NULL && !tag.IsNull()) {
        // Now for any known fragment.
        Int32 N;
        mActive->GetSize(&N);
        for (Int32 i = N - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mActive->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            String ftag;
            if (f != NULL && (f->GetTag(&ftag), tag.Equals(ftag))) {
                *fragment = f;
                REFCOUNT_ADD(*fragment);
                return NOERROR;
            }
        }
    }
    *fragment = NULL;
    return NOERROR;
}

ECode FragmentManagerImpl::FindFragmentByWho(
    /* [in] */ const String& who,
    /* [out] */ IFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    if (mActive != NULL && !who.IsNull()) {
        Int32 N;
        mActive->GetSize(&N);
        for (Int32 i = N - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mActive->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            AutoPtr<IFragment> fwho;
            if (f != NULL &&
                    (f->FindFragmentByWho(who, (IFragment**)&fwho), fwho != NULL)) {
                *fragment = fwho;
                REFCOUNT_ADD(*fragment);
                return NOERROR;
            }
        }
    }
    *fragment = NULL;
    return NOERROR;
}

ECode FragmentManagerImpl::CheckStateLoss()
{
    if (mStateSaved) {
        Logger::V(TAG, "Can not perform this action after onSaveInstanceState");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (!mNoTransactionsBecause.IsNull()) {
        Logger::V(TAG, "Can not perform this action inside of %s", mNoTransactionsBecause.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode FragmentManagerImpl::EnqueueAction(
    /* [in] */ IRunnable* action,
    /* [in] */ Boolean allowStateLoss)
{
    if (!allowStateLoss) {
        FAIL_RETURN(CheckStateLoss());
    }
    {
        AutoLock lock(this);
        if (mDestroyed || mActivity == NULL) {
            Logger::E(TAG, "Activity has been destroyed");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        if (mPendingActions == NULL) {
            CArrayList::New((IArrayList**)&mPendingActions);
        }
        mPendingActions->Add(action);
        Int32 N;
        mPendingActions->GetSize(&N);
        if (N == 1) {
            Activity* activity = TO_ACTIVITY(mActivity);
            activity->mHandler->RemoveCallbacks((IRunnable*)mExecCommit);
            Boolean result;
            activity->mHandler->Post((IRunnable*)mExecCommit, &result);
        }
    }
    return NOERROR;
}

ECode FragmentManagerImpl::AllocBackStackIndex(
    /* [in] */ IBackStackRecord* bse,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    AutoLock lock(this);

    Int32 N;
    if (mAvailBackStackIndices == NULL || (mAvailBackStackIndices->GetSize(&N), N <= 0)) {
        if (mBackStackIndices == NULL) {
            CArrayList::New((IArrayList**)&mBackStackIndices);
        }
        Int32 size;
        mBackStackIndices->GetSize(&size);
        if (DEBUG) Logger::V(TAG, "Setting back stack index %d to %s", size, TO_CSTR(bse));
        mBackStackIndices->Add(bse);
        *index = size;
        return NOERROR;
    }
    else {
        AutoPtr<IInterface> obj;
        mAvailBackStackIndices->Remove(N - 1, (IInterface**)&obj);
        Int32 idx;
        IInteger32::Probe(obj)->GetValue(&idx);
        if (DEBUG) Logger::V(TAG, "Adding back stack index %d with %s", idx, TO_CSTR(bse));
        mBackStackIndices->Set(idx, bse);
        *index = idx;
        return NOERROR;
    }
}

ECode FragmentManagerImpl::SetBackStackIndex(
    /* [in] */ Int32 index,
    /* [in] */ IBackStackRecord* bse)
{
    AutoLock lock(this);

    if (mBackStackIndices == NULL) {
        CArrayList::New((IArrayList**)&mBackStackIndices);
    }
    Int32 N;
    mBackStackIndices->GetSize(&N);
    if (index < N) {
        if (DEBUG) Logger::V(TAG, "Setting back stack index %d to %s", index, TO_CSTR(bse));
        mBackStackIndices->Set(index, bse);
    }
    else {
        while (N < index) {
            mBackStackIndices->Add(NULL);
            if (mAvailBackStackIndices == NULL) {
                CArrayList::New((IArrayList**)&mAvailBackStackIndices);
            }
            if (DEBUG) Logger::V(TAG, "Adding available back stack index %d", N);
            mAvailBackStackIndices->Add(CoreUtils::Convert(N));
            N++;
        }
        if (DEBUG) Logger::V(TAG, "Adding back stack index %d with %s", index, TO_CSTR(bse));
        mBackStackIndices->Add(bse);
    }
    return NOERROR;
}

ECode FragmentManagerImpl::FreeBackStackIndex(
    /* [in] */ Int32 index)
{
    if (DEBUG) Logger::V(TAG, "Freeing back stack index %d", index);

    AutoLock look(this);

    mBackStackIndices->Set(index, NULL);
    if (mAvailBackStackIndices == NULL) {
        CArrayList::New((IArrayList**)&mAvailBackStackIndices);
    }
    mAvailBackStackIndices->Add(CoreUtils::Convert(index));
    return NOERROR;
}

ECode FragmentManagerImpl::ExecPendingActions(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mExecutingActions) {
//         throw new IllegalStateException("Recursive entry to executePendingTransactions");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Activity* activity = TO_ACTIVITY(mActivity);
    AutoPtr<ILooper> myLooper = Looper::GetMyLooper();
    AutoPtr<ILooper> aLooper;
    activity->mHandler->GetLooper((ILooper**)&aLooper);
    if (myLooper.Get() != aLooper.Get()) {
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Must be called from main thread of process");
    }

    Boolean didSomething = FALSE;

    while (TRUE) {
        Int32 numActions;

        {
            AutoLock lock(this);
            if (mPendingActions == NULL || (mPendingActions->GetSize(&numActions), numActions == 0)) {
                break;
            }

            if (mTmpActions == NULL || mTmpActions->GetLength() < numActions) {
                mTmpActions = ArrayOf<IRunnable*>::Alloc(numActions);
            }
            AutoPtr< ArrayOf<IInterface*> > actionArray;
            mPendingActions->ToArray((ArrayOf<IInterface*>**)&actionArray);
            for (Int32 i = 0; i < numActions; i++) {
                mTmpActions->Set(i, IRunnable::Probe((*actionArray)[i]));
            }
            mPendingActions->Clear();
            activity->mHandler->RemoveCallbacks(mExecCommit);
        }

        mExecutingActions = TRUE;
        for (Int32 i = 0; i < numActions; i++) {
            (*mTmpActions)[i]->Run();
            mTmpActions->Set(i, NULL);
        }
        mExecutingActions = FALSE;
        didSomething = TRUE;
    }

    if (mHavePendingDeferredStart) {
        Boolean loadersRunning = FALSE;
        Int32 N;
        mActive->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mActive->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            AutoPtr<ILoaderManagerImpl> loaderManager;
            if (f != NULL &&
                (f->GetLoaderManagerValue((ILoaderManagerImpl**)&loaderManager), loaderManager != NULL)) {
                Boolean hasRLoaders;
                loaderManager->HasRunningLoaders(&hasRLoaders);
                loadersRunning |= hasRLoaders;
            }
        }
        if (!loadersRunning) {
            mHavePendingDeferredStart = FALSE;
            StartPendingDeferredFragments();
        }
    }
    *result = didSomething;
    return NOERROR;
}

ECode FragmentManagerImpl::ReportBackStackChanged()
{
    if (mBackStackChangeListeners != NULL) {
        Int32 N;
        mBackStackChangeListeners->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mBackStackChangeListeners->Get(i, (IInterface**)&obj);
            IFragmentManagerOnBackStackChangedListener::Probe(obj)->OnBackStackChanged();
        }
    }
    return NOERROR;
}

ECode FragmentManagerImpl::AddBackStackState(
    /* [in] */ IBackStackRecord* state)
{
    if (mBackStack == NULL) {
        CArrayList::New((IArrayList**)&mBackStack);
    }
    mBackStack->Add(state);
    ReportBackStackChanged();
    return NOERROR;
}

ECode FragmentManagerImpl::PopBackStackState(
    /* [in] */ IHandler* handler,
    /* [in] */ const String& name,
    /* [in] */ Int32 id,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mBackStack == NULL) return NOERROR;

    if (name.IsNull() && id < 0 && (flags & POP_BACK_STACK_INCLUSIVE) == 0) {
        Int32 N;
        mBackStack->GetSize(&N);
        Int32 last = N - 1;
        if (last < 0) return NOERROR;
        AutoPtr<IInterface> obj;
        mBackStack->Remove(last, (IInterface**)&obj);
        IBackStackRecord* record = IBackStackRecord::Probe(obj);
        AutoPtr<ISparseArray> firstOutFragments, lastInFragments;
        CSparseArray::New((ISparseArray**)&firstOutFragments);
        CSparseArray::New((ISparseArray**)&lastInFragments);
        BackStackRecord* bss = (BackStackRecord*)record;
        bss->CalculateBackFragments(firstOutFragments, lastInFragments);
        AutoPtr<IBackStackRecordTransitionState> tmp;
        bss->PopFromBackStack(TRUE, NULL, firstOutFragments, lastInFragments, (IBackStackRecordTransitionState**)&tmp);
        ReportBackStackChanged();
    }
    else {
        Int32 N;
        Int32 index = -1;
        if (!name.IsNull() || id >= 0) {
            // If a name or ID is specified, look for that place in
            // the stack.
            mBackStack->GetSize(&N);
            index = N - 1;
            while (index >= 0) {
                AutoPtr<IInterface> obj;
                mBackStack->Get(index, (IInterface**)&obj);
                IBackStackRecord* bss = IBackStackRecord::Probe(obj);
                String bssName;
                if (!name.IsNull() && (bss->GetName(&bssName), name.Equals(bssName))) {
                    break;
                }
                Int32 bssIndex;
                if (id >= 0 && (bss->GetId(&bssIndex), id == bssIndex)) {
                    break;
                }
                index--;
            }
            if (index < 0) {
                *result = FALSE;
                return NOERROR;
            }
            if ((flags & POP_BACK_STACK_INCLUSIVE) != 0) {
                index--;
                // Consume all following entries that match.
                while (index >= 0) {
                    AutoPtr<IInterface> obj;
                    mBackStack->Get(index, (IInterface**)&obj);
                    IBackStackRecord* bss = IBackStackRecord::Probe(obj);
                    String bssName;
                    Int32 bssIndex;
                    if ((!name.IsNull() && (bss->GetName(&bssName), name.Equals(bssName)))
                            || (id >= 0 && (bss->GetId(&bssIndex), id == bssIndex))) {
                        index--;
                        continue;
                    }
                    break;
                }
            }
        }
        mBackStack->GetSize(&N);
        if (index == N - 1) {
            *result = FALSE;
            return NOERROR;
        }
        AutoPtr<IArrayList> states;
        CArrayList::New((IArrayList**)&states);
        for (Int32 i = N - 1; i > index; i--) {
            AutoPtr<IInterface> obj;
            mBackStack->Remove(i, (IInterface**)&obj);
            states->Add(obj);
        }
        Int32 LAST;
        states->GetSize(&LAST);
        LAST = LAST - 1;
        AutoPtr<ISparseArray> firstOutFragments, lastInFragments;
        CSparseArray::New((ISparseArray**)&firstOutFragments);
        CSparseArray::New((ISparseArray**)&lastInFragments);
        for (Int32 i = 0; i <= LAST; i++) {
            AutoPtr<IInterface> obj;
            states->Get(i, (IInterface**)&obj);
            IBackStackRecord::Probe(obj)->CalculateBackFragments(firstOutFragments, lastInFragments);
        }
        AutoPtr<IBackStackRecordTransitionState> state, tmp;
        for (Int32 i = 0; i <= LAST; i++) {
            AutoPtr<IInterface> obj;
            states->Get(i, (IInterface**)&obj);
            IBackStackRecord* bsr = IBackStackRecord::Probe(obj);
            if (DEBUG) Logger::V(TAG, "Popping back stack state: %s", TO_CSTR(bsr));
            tmp = state;
            state = NULL;
            bsr->PopFromBackStack(i == LAST, tmp, firstOutFragments, lastInFragments,
                (IBackStackRecordTransitionState**)&state);
        }
        ReportBackStackChanged();
    }
    *result = TRUE;
    return NOERROR;
}

ECode FragmentManagerImpl::RetainNonConfig(
    /* [out] */ IArrayList** retains)
{
    VALIDATE_NOT_NULL(retains);
    *retains = NULL;

    AutoPtr<IArrayList> fragments;
    if (mActive != NULL) {
        Int32 N;
        mActive->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mActive->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            Boolean retainInstance;
            if (f != NULL && (f->GetRetainInstance(&retainInstance), retainInstance)) {
                if (fragments == NULL) {
                    CArrayList::New((IArrayList**)&fragments);
                }
                fragments->Add(f);
                f->SetRetaining(TRUE);
                AutoPtr<IFragment> target;
                f->GetTarget((IFragment**)&target);
                if (target != NULL) {
                    Int32 index;
                    target->GetIndex(&index);
                    f->SetTargetIndex(index);
                }
                else f->SetTargetIndex(-1);
                if (DEBUG) Logger::V(TAG, "retainNonConfig: keeping retained %s", TO_CSTR(f));
            }
        }
    }
    *retains = fragments;
    REFCOUNT_ADD(*retains);
    return NOERROR;
}

ECode FragmentManagerImpl::SaveFragmentViewState(
    /* [in] */ IFragment* f)
{
    AutoPtr<IView> view;
    f->GetView((IView**)&view);
    if (view == NULL) {
        return NOERROR;
    }
    if (mStateArray == NULL) {
        CSparseArray::New((ISparseArray**)&mStateArray);
    }
    else {
        mStateArray = NULL;
    }

    view->SaveHierarchyState(mStateArray);
    Int32 size;
    mStateArray->GetSize(&size);
    if (size > 0) {
        f->SetSavedViewState(mStateArray);
        mStateArray = NULL;
    }
    return NOERROR;
}

ECode FragmentManagerImpl::SaveFragmentBasicState(
    /* [in] */ IFragment* f,
    /* [out] */ IBundle** rst)
{
    VALIDATE_NOT_NULL(rst);
    AutoPtr<IBundle> result = NULL;

    if (mStateBundle == NULL) {
        CBundle::New((IBundle**)&mStateBundle);
    }
    f->PerformSaveInstanceState(mStateBundle);
    Boolean isEmpty;
    mStateBundle->IsEmpty(&isEmpty);
    if (!isEmpty) {
        result = mStateBundle;
        mStateBundle = NULL;
    }

    AutoPtr<IView> view;
    f->GetView((IView**)&view);
    if (view != NULL) {
        SaveFragmentViewState(f);
    }

    AutoPtr<ISparseArray> savedViewState;
    f->GetSavedViewState((ISparseArray**)&savedViewState);
    if (savedViewState != NULL) {
        if (result == NULL) {
            CBundle::New((IBundle**)&result);
        }
        result->PutSparseParcelableArray(
            IFragmentManagerImpl::VIEW_STATE_TAG, savedViewState);
    }
    Boolean userVisibleHint;
    f->GetUserVisibleHint(&userVisibleHint);
    if (!userVisibleHint) {
        if (result == NULL) {
            CBundle::New((IBundle**)&result);
        }
        // Only add this if it's not the default value
        result->PutBoolean(IFragmentManagerImpl::USER_VISIBLE_HINT_TAG, userVisibleHint);
    }

    *rst = result;
    REFCOUNT_ADD(*rst);
    return NOERROR;
}

ECode FragmentManagerImpl::SaveAllState(
    /* [out] */ IParcelable** state)
{
    VALIDATE_NOT_NULL(state);
    *state = NULL;

    // Make sure all pending operations have now been executed to get
    // our state update-to-date.
    Boolean executed;
    ExecPendingActions(&executed);

    mStateSaved = TRUE;

    Int32 N;
    if (mActive == NULL || (mActive->GetSize(&N), N <= 0)) {
        *state = NULL;
        return NOERROR;
    }

    // First collect all active fragments.
    AutoPtr<ArrayOf<IFragmentState*> > active = ArrayOf<IFragmentState*>::Alloc(N);
    Boolean haveFragments = FALSE;
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mActive->Get(i, (IInterface**)&obj);
        IFragment* f = IFragment::Probe(obj);
        if (f != NULL) {
            Int32 index;
            f->GetIndex(&index);
            if (index < 0) {
                Logger::V(TAG, "Failure saving state: active %p has cleared index: %d", f, index);
                return E_ILLEGAL_STATE_EXCEPTION;
            }

            haveFragments = TRUE;

            AutoPtr<CFragmentState> fs;
            CFragmentState::NewByFriend(f, (CFragmentState**)&fs);
            active->Set(i, (IFragmentState*)fs.Get());

            Int32 fstate;
            f->GetState(&fstate);
            if (fstate > IFragment::INITIALIZING && fs->mSavedFragmentState == NULL) {
                SaveFragmentBasicState(f, (IBundle**)&fs->mSavedFragmentState);

                AutoPtr<IFragment> t;
                f->GetTarget((IFragment**)&t);
                if (t != NULL) {
                    Int32 tIndex = 0;
                    t->GetIndex(&tIndex);
                    if (tIndex < 0) {
                        Logger::V(TAG, "Failure saving state: %s has target not in fragment manager: %s",
                            TO_CSTR(f), TO_CSTR(t));
                        return E_ILLEGAL_STATE_EXCEPTION;
                    }
                    if (fs->mSavedFragmentState == NULL) {
                        CBundle::New((IBundle**)&fs->mSavedFragmentState);
                    }

                    PutFragment(fs->mSavedFragmentState,
                            IFragmentManagerImpl::TARGET_STATE_TAG, t);

                    Int32 code = 0;
                    f->GetTargetRequestCode(&code);
                    if (code != 0) {
                        fs->mSavedFragmentState->PutInt32(
                            IFragmentManagerImpl::TARGET_REQUEST_CODE_STATE_TAG, code);
                    }
                }
            }
            else {
                fs->mSavedFragmentState = NULL;
                f->GetSavedFragmentState((IBundle**)&fs->mSavedFragmentState);
            }

            if (DEBUG) Logger::V(TAG, "Saved state of %s: in %s",
                TO_CSTR(f), TO_CSTR(fs->mSavedFragmentState));
        }
    }

    if (!haveFragments) {
        if (DEBUG) Logger::V(TAG, "saveAllState: no fragments!");
        *state = NULL;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > added;
    AutoPtr<ArrayOf<IBackStackState*> > backStack;

    // Build list of currently added fragments.
    if (mAdded != NULL) {
        mAdded->GetSize(&N);
        if (N > 0) {
            added = ArrayOf<Int32>::Alloc(N);
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                mAdded->Get(i, (IInterface**)&obj);
                IFragment* f = IFragment::Probe(obj);
                Int32 index;
                f->GetIndex(&index);
                (*added)[i] = index;
                if ((*added)[i] < 0) {
                    Logger::V(TAG, "Failure saving state: active %s has cleared index: %d",
                            TO_CSTR(f), index);
                    return E_ILLEGAL_STATE_EXCEPTION;
                }
                if (DEBUG) Logger::V(TAG, "saveAllState: adding fragment #%d: %s", i, TO_CSTR(f));
            }
        }
    }

    // Now save back stack.
    if (mBackStack != NULL) {
        mBackStack->GetSize(&N);
        if (N > 0) {
            backStack = ArrayOf<IBackStackState*>::Alloc(N);
            for (Int32 i = 0; i < N; i++) {
                AutoPtr<IInterface> obj;
                mBackStack->Get(i, (IInterface**)&obj);
                AutoPtr<IBackStackState> value;
                CBackStackState::New(this, IBackStackRecord::Probe(obj), (IBackStackState**)&value);
                backStack->Set(i, value);
                if (DEBUG) Logger::V(TAG, "saveAllState: adding back stack #%d: %s", i, TO_CSTR(obj));
            }
        }
    }

    AutoPtr<CFragmentManagerState> fms;
    CFragmentManagerState::NewByFriend((CFragmentManagerState**)&fms);
    fms->mActive = active;
    fms->mAdded = added;
    fms->mBackStack = backStack;
    *state = IParcelable::Probe(fms);
    REFCOUNT_ADD(*state);
    return NOERROR;
}

ECode FragmentManagerImpl::RestoreAllState(
    /* [in] */ IParcelable* state,
    /* [in] */ IArrayList* nonConfig)
{
    // If there is no saved state at all, then there can not be
    // any nonConfig fragments either, so that is that.
    if (state == NULL) return NOERROR;
    CFragmentManagerState* fms = (CFragmentManagerState*)state;
    if (fms->mActive == NULL) return NOERROR;

    // First re-attach any non-config instances we are retaining back
    // to their saved state, so we don't try to instantiate them again.
    if (nonConfig != NULL) {
        Int32 size;
        nonConfig->GetSize(&size);
        IFragment* f;
        IContext* ctx = IContext::Probe(mActivity);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> data;
            nonConfig->Get(i, (IInterface**)&data);
            f = IFragment::Probe(data);
            assert(f != NULL);
            if (DEBUG) Logger::V(TAG, "restoreAllState: re-attaching retained %s", TO_CSTR(f));
            Int32 index = 0;
            f->GetIndex(&index);
            CFragmentState* fs = (CFragmentState*)(*fms->mActive)[index];
            fs->mInstance = f;
            f->SetSavedViewState(NULL);
            f->SetBackStackNesting(0);
            f->SetInLayout(FALSE);
            f->SetAdded(FALSE);
            f->SetTarget(NULL);
            if (fs->mSavedFragmentState != NULL) {
                AutoPtr<IClassLoader> classLoader;
                ctx->GetClassLoader((IClassLoader**)&classLoader);
                fs->mSavedFragmentState->SetClassLoader(classLoader);

                AutoPtr<ISparseArray> map;
                fs->mSavedFragmentState->GetSparseParcelableArray(
                    IFragmentManagerImpl::VIEW_STATE_TAG, (ISparseArray**)&map);
                f->SetSavedViewState(map);
                f->SetSavedFragmentState(fs->mSavedFragmentState);
            }
        }
    }

    // Build the full list of active fragments, instantiating them from
    // their saved state.
    mActive = NULL;
    CArrayList::New(fms->mActive->GetLength(), (IArrayList**)&mActive);
    if (mAvailIndices != NULL) {
        mAvailIndices->Clear();
    }
    for (Int32 i = 0; i < fms->mActive->GetLength(); i++) {
        CFragmentState* fs = (CFragmentState*)(*fms->mActive)[i];
        if (fs != NULL) {
            AutoPtr<IFragment> f;
            fs->Instantiate(mActivity, mParent, (IFragment**)&f);
            if (DEBUG) {
                Logger::V(TAG, "restoreAllState: active #%d: %s", i , TO_CSTR(f));
            }
            mActive->Add(f);
            // Now that the fragment is instantiated (or came from being
            // retained above), clear mInstance in case we end up re-restoring
            // from this FragmentState again.
            fs->mInstance = NULL;
        }
        else {
            mActive->Add(NULL);
            if (mAvailIndices == NULL) {
                CArrayList::New((IArrayList**)&mAvailIndices);
            }
            if (DEBUG) Logger::V(TAG, "restoreAllState: avail #%d", i);
            mAvailIndices->Add(CoreUtils::Convert(i));
        }
    }

    // Update the target of all retained fragments.
    if (nonConfig != NULL) {
        Int32 size;
        nonConfig->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> data;
            nonConfig->Get(i, (IInterface**)&data);
            IFragment* f = IFragment::Probe(data);
            Int32 targetIndex = 0;
            f->GetTargetIndex(&targetIndex);
            if (targetIndex >= 0) {
                Int32 N;
                mActive->GetSize(&N);
                if (targetIndex < N) {
                    AutoPtr<IInterface> obj;
                    mActive->Get(targetIndex, (IInterface**)&obj);
                    f->SetTarget(IFragment::Probe(obj));
                }
                else {
                    Logger::W(TAG, "Re-attaching retained fragment %s target no longer exists: %d",
                        TO_CSTR(f), targetIndex);
                    f->SetTarget(NULL);
                }
            }
        }
    }

    // Build the list of currently added fragments.
    if (fms->mAdded != NULL) {
        mAdded = NULL;
        CArrayList::New(fms->mAdded->GetLength(), (IArrayList**)&mAdded);
        for (Int32 i = 0; i < fms->mAdded->GetLength(); i++) {
            AutoPtr<IInterface> obj;
            mActive->Get((*fms->mAdded)[i], (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            if (f == NULL) {
                Logger::W(TAG, "No instantiated fragment for index #%d", (*fms->mAdded)[i]);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            f->SetAdded(TRUE);
            if (DEBUG) Logger::V(TAG, "restoreAllState: added #%d: %s", i, TO_CSTR(f));
            Boolean contains;
            if (mAdded->Contains(f, &contains), contains) {
                Logger::W(TAG, "Already added!");
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            mAdded->Add(f);
        }
    }
    else {
        mAdded = NULL;
    }

    // Build the back stack.
    if (fms->mBackStack != NULL) {
        mBackStack = NULL;
        CArrayList::New(fms->mBackStack->GetLength(), (IArrayList**)&mBackStack);
        for (Int32 i = 0; i < fms->mBackStack->GetLength(); i++) {
            AutoPtr<IBackStackRecord> bse;
            (*fms->mBackStack)[i]->Instantiate(this, (IBackStackRecord**)&bse);
            if (DEBUG) {
                Logger::V(TAG, "restoreAllState: back stack #%d (index %d): %s",
                    i, ((BackStackRecord*)bse.Get())->mIndex, TO_CSTR(bse));
                // TODO
                // LogWriter logw = new LogWriter(Log.VERBOSE, TAG);
                // PrintWriter pw = new FastPrintWriter(logw, false, 1024);
                // bse.dump("  ", pw, false);
                // pw.flush();
            }
            mBackStack->Add(bse);
            if (((BackStackRecord*)bse.Get())->mIndex >= 0) {
                SetBackStackIndex(((BackStackRecord*)bse.Get())->mIndex, bse);
            }
        }
    }
    else {
        mBackStack = NULL;
    }

    return NOERROR;
}

ECode FragmentManagerImpl::AttachActivity(
    /* [in] */ IActivity* activity,
    /* [in] */ IFragmentContainer* container,
    /* [in] */ IFragment* parent)
{
    if (mActivity != NULL) {
        Logger::E(TAG, "IllegalStateException: Already attached");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mActivity = activity;
    mContainer = container;
    mParent = parent;
    return NOERROR;
}

ECode FragmentManagerImpl::NoteStateNotSaved()
{
    mStateSaved = FALSE;
    return NOERROR;
}

ECode FragmentManagerImpl::DispatchCreate()
{
    mStateSaved = FALSE;
    return MoveToState(IFragment::CREATED, FALSE);
}

ECode FragmentManagerImpl::DispatchActivityCreated()
{
    mStateSaved = FALSE;
    return MoveToState(IFragment::ACTIVITY_CREATED, FALSE);
}

ECode FragmentManagerImpl::DispatchStart()
{
    mStateSaved = FALSE;
    return MoveToState(IFragment::STARTED, FALSE);
}

ECode FragmentManagerImpl::DispatchResume()
{
    mStateSaved = FALSE;
    return MoveToState(IFragment::RESUMED, FALSE);
}

ECode FragmentManagerImpl::DispatchPause()
{
    return MoveToState(IFragment::STARTED, FALSE);
}

ECode FragmentManagerImpl::DispatchStop()
{
    return MoveToState(IFragment::STOPPED, FALSE);
}

ECode FragmentManagerImpl::DispatchDestroyView()
{
    return MoveToState(IFragment::CREATED, FALSE);
}

ECode FragmentManagerImpl::DispatchDestroy()
{
    mDestroyed = TRUE;
    Boolean executed;
    ExecPendingActions(&executed);
    MoveToState(IFragment::INITIALIZING, FALSE);
    mActivity = NULL;
    mContainer = NULL;
    mParent = NULL;
    return NOERROR;
}

ECode FragmentManagerImpl::DispatchConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    if (mAdded != NULL) {
        Int32 N;
        mAdded->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mAdded->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            if (f != NULL) {
                f->PerformConfigurationChanged(newConfig);
            }
        }
    }
    return NOERROR;
}

ECode FragmentManagerImpl::DispatchLowMemory()
{
    if (mAdded != NULL) {
        Int32 N;
        mAdded->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mAdded->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            if (f != NULL) {
                f->PerformLowMemory();
            }
        }
    }
    return NOERROR;
}

ECode FragmentManagerImpl::DispatchTrimMemory(
    /* [in] */ Int32 level)
{
    if (mAdded != NULL) {
        Int32 N;
        mAdded->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mAdded->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            if (f != NULL) {
                f->PerformTrimMemory(level);
            }
        }
    }
    return NOERROR;
}

ECode FragmentManagerImpl::DispatchCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst);
    Boolean show = FALSE;
    AutoPtr<IArrayList> newMenus;
    if (mAdded != NULL) {
        Int32 N;
        mAdded->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mAdded->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            if (f != NULL) {
                Boolean created;
                f->PerformCreateOptionsMenu(menu, inflater, &created);
                if (created) {
                    show = TRUE;
                    if (newMenus == NULL) {
                        CArrayList::New((IArrayList**)&newMenus);
                    }
                    newMenus->Add(f);
                }
            }
        }
    }

    if (mCreatedMenus != NULL) {
        Int32 N;
        mCreatedMenus->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mCreatedMenus->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            Boolean contains;
            if (newMenus == NULL || (newMenus->Contains(f, &contains), !contains)) {
                f->OnDestroyOptionsMenu();
            }
        }
    }

    mCreatedMenus = newMenus;

    *rst = show;
    return NOERROR;
}

ECode FragmentManagerImpl::DispatchPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst);
    Boolean show = FALSE;
    if (mAdded != NULL) {
        Int32 N;
        mAdded->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mAdded->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            if (f != NULL) {
                Boolean prepared;
                f->PerformPrepareOptionsMenu(menu, &prepared);
                if (prepared) {
                    show = TRUE;
                }
            }
        }
    }
    *rst = show;
    return NOERROR;
}

ECode FragmentManagerImpl::DispatchOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst);
    if (mAdded != NULL) {
        Int32 N;
        mAdded->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mAdded->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            if (f != NULL) {
                Boolean selected;
                f->PerformOptionsItemSelected(item, &selected);
                if (selected) {
                    *rst = TRUE;
                    return NOERROR;
                }
            }
        }
    }
    *rst = FALSE;
    return NOERROR;
}

ECode FragmentManagerImpl::DispatchContextItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst);
    if (mAdded != NULL) {
        Int32 N;
        mAdded->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mAdded->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            if (f != NULL) {
                Boolean selected;
                f->PerformContextItemSelected(item, &selected);
                if (selected) {
                    *rst = TRUE;
                    return NOERROR;
                }
            }
        }
    }
    *rst = FALSE;
    return NOERROR;
}

ECode FragmentManagerImpl::DispatchOptionsMenuClosed(
    /* [in] */ IMenu* menu)
{
    if (mAdded != NULL) {
        Int32 N;
        mAdded->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mAdded->Get(i, (IInterface**)&obj);
            IFragment* f = IFragment::Probe(obj);
            if (f != NULL) {
                f->PerformOptionsMenuClosed(menu);
            }
        }
    }
    return NOERROR;
}

ECode FragmentManagerImpl::InvalidateOptionsMenu()
{
    if (mActivity != NULL && mCurState == IFragment::RESUMED) {
        mActivity->InvalidateOptionsMenu();
    } else {
        mNeedMenuInvalidate = TRUE;
    }
    return NOERROR;
}

ECode FragmentManagerImpl::ReverseTransit(
    /* [in] */ Int32 transit,
    /* [out] */ Int32* reverse)
{
    VALIDATE_NOT_NULL(reverse);
    Int32 rev = 0;
    switch (transit) {
        case IFragmentTransaction::TRANSIT_FRAGMENT_OPEN:
            rev = IFragmentTransaction::TRANSIT_FRAGMENT_CLOSE;
            break;
        case IFragmentTransaction::TRANSIT_FRAGMENT_CLOSE:
            rev = IFragmentTransaction::TRANSIT_FRAGMENT_OPEN;
            break;
        case IFragmentTransaction::TRANSIT_FRAGMENT_FADE:
            rev = IFragmentTransaction::TRANSIT_FRAGMENT_FADE;
            break;
    }
    *reverse = rev;
    return NOERROR;

}

ECode FragmentManagerImpl::TransitToStyleIndex(
    /* [in] */ Int32 transit,
    /* [in] */ Boolean enter,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    Int32 animAttr = -1;
    switch (transit) {
        case IFragmentTransaction::TRANSIT_FRAGMENT_OPEN:
            animAttr = enter
                ? R::styleable::FragmentAnimation_fragmentOpenEnterAnimation
                : R::styleable::FragmentAnimation_fragmentOpenExitAnimation;
            break;
        case IFragmentTransaction::TRANSIT_FRAGMENT_CLOSE:
            animAttr = enter
                ? R::styleable::FragmentAnimation_fragmentCloseEnterAnimation
                : R::styleable::FragmentAnimation_fragmentCloseExitAnimation;
            break;
        case IFragmentTransaction::TRANSIT_FRAGMENT_FADE:
            animAttr = enter
                ? R::styleable::FragmentAnimation_fragmentFadeEnterAnimation
                : R::styleable::FragmentAnimation_fragmentFadeExitAnimation;
            break;
    }
    *index = animAttr;
    return NOERROR;
}

//@Override
ECode FragmentManagerImpl::OnCreateView(
    /* [in] */ IView* parent,
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (!name.Equals("fragment")) {
        return NOERROR;
    }

    String fname;
    attrs->GetAttributeValue(String(NULL), String("class"), &fname);
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::Fragment);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    if (fname.IsNull()) {
        a->GetString(R::styleable::Fragment_name, &fname);
    }
    Int32 id;
    a->GetResourceId(R::styleable::Fragment_id, IView::NO_ID, &id);
    String tag;
    a->GetString(R::styleable::Fragment_tag, &tag);
    a->Recycle();

    Int32 containerId = 0;
    if (parent != NULL)
        parent->GetId(&containerId);
    if (containerId == IView::NO_ID && id == IView::NO_ID && tag == NULL) {
        String str;
        attrs->GetPositionDescription(&str);
        Logger::E(TAG,
            "IllegalArgumentException: %s : Must specify unique android:id, "
            "android:tag, or have a parent with an id for %s",
            str.string(), fname.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // If we restored from a previous state, we may already have
    // instantiated this fragment from the state and should use
    // that instance instead of making a new one.
    AutoPtr<IFragment> fragment;
    if (id != IView::NO_ID) {
        FindFragmentById(id, (IFragment**)&fragment);
    }

    if (fragment == NULL && tag != NULL) {
        FindFragmentByTag(tag, (IFragment**)&fragment);
    }
    if (fragment == NULL && containerId != IView::NO_ID) {
        FindFragmentById(containerId, (IFragment**)&fragment);
    }

    Fragment* f = (Fragment*)fragment.Get();
    if (FragmentManagerImpl::DEBUG)
        Logger::V(TAG, "onCreateView: id=0x%08x fname=%s existing=%s",
            id, fname.string(), TO_CSTR(fragment));
    if (fragment == NULL) {
        Fragment::Instantiate(context, fname, (IFragment**)&fragment);
        f = (Fragment*)fragment.Get();
        f->mFromLayout = true;
        f->mFragmentId = id != 0 ? id : containerId;
        f->mContainerId = containerId;
        f->mTag = tag;
        f->mInLayout = true;
        f->mFragmentManager = this;
        f->OnInflate(mActivity, attrs, f->mSavedFragmentState);
        AddFragment(fragment, true);
    }
    else if (f->mInLayout) {
        // A fragment already exists and it is not one we restored from
        // previous state.
        String str;
        attrs->GetPositionDescription(&str);
        Logger::E(TAG, "IllegalArgumentException %s : Duplicate id 0x%08x"
            " with another fragment for ",
            str.string(), id, tag.string(), containerId, fname.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else {
        // This fragment was retained from a previous instance; get it
        // going now.
        f->mInLayout = TRUE;
        // If this fragment is newly instantiated (either right now, or
        // from last saved state), then give it the attributes to
        // initialize itself.
        if (!f->mRetaining) {
            f->OnInflate(mActivity, attrs, f->mSavedFragmentState);
        }
    }

    // If we haven't finished entering the CREATED state ourselves yet,
    // push the inflated child fragment along.
    if (mCurState < IFragment::CREATED && f->mFromLayout) {
        MoveToState(fragment, IFragment::CREATED, 0, 0, false);
    }
    else {
        MoveToState(fragment);
    }

    if (f->mView == NULL) {
        Logger::E(TAG, "IllegalStateException: Fragment %s did not create a view.",
            fname.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (id != 0) {
        f->mView->SetId(id);
    }

    AutoPtr<IInterface> tagObj;
    f->mView->GetTag((IInterface**)&tagObj);
    if (tagObj == NULL) {
        tagObj = CoreUtils::Convert(tag).Get();
        f->mView->SetTag(tagObj);
    }
    *result = f->mView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//@Override
ECode FragmentManagerImpl::OnCreateView(
    /* [in] */ const String& name,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode FragmentManagerImpl::GetLayoutInflaterFactory(
    /* [out] */ ILayoutInflaterFactory2** fact)
{
    VALIDATE_NOT_NULL(fact)
    *fact = this;
    REFCOUNT_ADD(*fact)
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
