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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/animation/Animator.h"
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL_2(Animator, Object, IAnimator, ICloneable)

Animator::Animator()
    : mPaused(FALSE)
    , mParent(NULL)
    , mParentRefCount(0)
{
}

Animator::~Animator()
{
    if (mParentRefCount > 0) {
        Logger::E("Animator", " parent:%s not released. mParentRefCount:%d.",
            TO_CSTR(mParent), mParentRefCount);
        assert(mParentRefCount == 0);
    }
}

ECode Animator::Start()
{
    return NOERROR;
}

ECode Animator::Cancel()
{
    return NOERROR;
}

ECode Animator::End()
{
    return NOERROR;
}

ECode Animator::Pause()
{
    Boolean started = FALSE;
    IsStarted(&started);
    if (started && !mPaused) {
        mPaused = TRUE;
        if (mPauseListeners != NULL) {
            AutoPtr<IInterface> obj;
            ICloneable::Probe(mPauseListeners)->Clone((IInterface**)&obj);
            IArrayList* tmpListeners = IArrayList::Probe(obj);
            Int32 numListeners = 0;
            tmpListeners->GetSize(&numListeners);
            for (Int32 i = 0; i < numListeners; ++i) {
                AutoPtr<IInterface> listener;
                tmpListeners->Get(i, (IInterface**)&listener);
                IAnimatorPauseListener::Probe(listener)->OnAnimationPause(this);
            }
        }
    }
    return NOERROR;
}

ECode Animator::Resume()
{
    if (mPaused) {
        mPaused = FALSE;
        if (mPauseListeners != NULL) {
            AutoPtr<IInterface> obj;
            ICloneable::Probe(mPauseListeners)->Clone((IInterface**)&obj);
            IArrayList* tmpListeners = IArrayList::Probe(obj);
            Int32 numListeners = 0;
            tmpListeners->GetSize(&numListeners);
            for (Int32 i = 0; i < numListeners; ++i) {
                AutoPtr<IInterface> listener;
                tmpListeners->Get(i, (IInterface**)&listener);
                IAnimatorPauseListener::Probe(listener)->OnAnimationResume(this);
            }
        }
    }
    return NOERROR;
}

ECode Animator::IsPaused(
    /* [out] */ Boolean* paused)
{
    VALIDATE_NOT_NULL(paused);
    *paused = mPaused;
    return NOERROR;
}

ECode Animator::GetInterpolator(
    /* [out] */ ITimeInterpolator** interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = NULL;
    return NOERROR;
}

ECode Animator::IsStarted(
    /* [out] */ Boolean* started)
{
    VALIDATE_NOT_NULL(started);
    return IsRunning(started);
}

ECode Animator::AddListener(
    /* [in] */ IAnimatorListener* listener)
{
    if (mListeners == NULL) {
        CArrayList::New((IArrayList**)&mListeners);
    }
    mListeners->Add(listener);
    return NOERROR;
}

ECode Animator::RemoveListener(
    /* [in] */ IAnimatorListener* listener)
{
    if (mListeners == NULL) {
        return NOERROR;
    }
    mListeners->Remove(listener);
    Int32 size;
    mListeners->GetSize(&size);
    if (size == 0) {
        mListeners = NULL;
    }
    return NOERROR;
}

ECode Animator::GetListeners(
    /* [out] */ IArrayList** listeners)
{
    VALIDATE_NOT_NULL(listeners);
    *listeners = mListeners;
    REFCOUNT_ADD(*listeners);
    return NOERROR;
}

ECode Animator::AddPauseListener(
    /* [in] */ IAnimatorPauseListener* listener)
{
    if (mPauseListeners == NULL) {
        CArrayList::New((IArrayList**)&mPauseListeners);
    }
    return mPauseListeners->Add(listener);
}

ECode Animator::RemovePauseListener(
    /* [in] */ IAnimatorPauseListener* listener)
{
    if (mPauseListeners == NULL) {
        return NOERROR;
    }
    mPauseListeners->Remove(listener);
    Int32 size = 0;
    mPauseListeners->GetSize(&size);
    if (size == 0) {
        mPauseListeners = NULL;
    }
    return NOERROR;
}

ECode Animator::RemoveAllListeners()
{
    if (mListeners != NULL) {
        mListeners->Clear();
        mListeners = NULL;
    }
    if (mPauseListeners != NULL) {
        mPauseListeners->Clear();
        mPauseListeners = NULL;
    }
    return NOERROR;
}

ECode Animator::CloneImpl(
    /* [in] */ IAnimator* _anim)
{
    VALIDATE_NOT_NULL(_anim)

    Animator* anim = (Animator*)_anim;
    if (mListeners != NULL) {
        AutoPtr<IArrayList> oldListeners = mListeners;
        CArrayList::New((IArrayList**)&anim->mListeners);
        Int32 numListeners;
        oldListeners->GetSize(&numListeners);
        for (Int32 i = 0; i < numListeners; ++i) {
            AutoPtr<IInterface> listener;
            oldListeners->Get(i, (IInterface**)&listener);
            anim->mListeners->Add(listener);
        }
    }
    if (mPauseListeners != NULL) {
        AutoPtr<IArrayList> oldListeners = mPauseListeners;
        anim->mPauseListeners = NULL;
        CArrayList::New((IArrayList**)&anim->mPauseListeners);
        Int32 numListeners = 0;
        oldListeners->GetSize(&numListeners);
        for (Int32 i = 0; i < numListeners; ++i) {
            AutoPtr<IInterface> listener;
            oldListeners->Get(i, (IInterface**)&listener);
            anim->mPauseListeners->Add(listener);
        }
    }
    return NOERROR;
}

ECode Animator::SetupStartValues()
{
    return NOERROR;
}

ECode Animator::SetupEndValues()
{
    return NOERROR;
}

ECode Animator::SetTarget(
    /* [in] */ IInterface* target)
{
    assert(0 && "error: subclass must implements this method.");
    return NOERROR;
}

ECode Animator::CanReverse(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = FALSE;
    return NOERROR;
}

ECode Animator::Reverse()
{
    // throw new IllegalStateException("Reverse is not supported");
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode Animator::SetAllowRunningAsynchronously(
    /* [in] */ Boolean mayRunAsync)
{
    // It is up to subclasses to support this, if they can.
    return NOERROR;
}

ECode Animator::SetParent(
    /* [in] */ IAnimatorSet* parent)
{
    if (mParent == NULL) {
        mParent = parent;
    }
    else {
        if (mParent != parent) {
            Logger::E("Animator", " parent:%p already exists.", mParent);
            assert(0);
        }
    }

    mParent->AddRef();
    ++mParentRefCount;
    //Logger::I("Animator", " %p SetParent %p mParentRefCount: %d", this, mParent, mParentRefCount);
    return NOERROR;
}

ECode Animator::ReleaseParent()
{
    if (mParent != NULL) {
        --mParentRefCount;
        //Logger::I("Animator", " %p ReleaseParent %p mParentRefCount: %d", this, mParent, mParentRefCount);

        mParent->Release();
        if (mParentRefCount <= 0) {
            mParent = NULL;
        }
    }
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
