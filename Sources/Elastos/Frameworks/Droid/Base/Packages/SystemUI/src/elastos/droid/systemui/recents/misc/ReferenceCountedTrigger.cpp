#include "elastos/droid/systemui/recents/misc/ReferenceCountedTrigger.h"
#include "elastos/droid/systemui/recents/misc/Console.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

//=================================
// ReferenceCountedTrigger::IncrementRunnable
//=================================

ReferenceCountedTrigger::IncrementRunnable::IncrementRunnable(
    /* [in] */ ReferenceCountedTrigger* host)
    : mHost(host)
{}

ECode ReferenceCountedTrigger::IncrementRunnable::Run()
{
    mHost->Increment();
    return NOERROR;
}

//=================================
// ReferenceCountedTrigger::DecrementRunnable
//=================================

ReferenceCountedTrigger::DecrementRunnable::DecrementRunnable(
    /* [in] */ ReferenceCountedTrigger* host)
    : mHost(host)
{}

ECode ReferenceCountedTrigger::DecrementRunnable::Run()
{
    mHost->Decrement();
    return NOERROR;
}

//=================================
// ReferenceCountedTrigger::MyAnimatorListenerAdapter
//=================================

ReferenceCountedTrigger::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ ReferenceCountedTrigger* host)
    : AnimatorListenerAdapter()
    , mHost(host)
{
}

ECode ReferenceCountedTrigger::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->Decrement();
    return NOERROR;
}


//=================================
// ReferenceCountedTrigger
//=================================

ReferenceCountedTrigger::ReferenceCountedTrigger(
    /* [in] */ IContext* context,
    /* [in] */ IRunnable* firstIncRunnable,
    /* [in] */ IRunnable* lastDecRunnable,
    /* [in] */ IRunnable* errorRunanable)
    : mContext(context)
    , mCount(0)
    , mErrorRunnable(errorRunanable)
{
    mIncrementRunnable = new IncrementRunnable(this);
    mDecrementRunnable = new DecrementRunnable(this);
    if (firstIncRunnable != NULL) mFirstIncRunnables->Add(firstIncRunnable);
    if (lastDecRunnable != NULL) mLastDecRunnables->Add(lastDecRunnable);
}

void ReferenceCountedTrigger::Increment()
{
    Boolean isEmpty;
    mFirstIncRunnables->IsEmpty(&isEmpty);
    if (mCount == 0 && !isEmpty) {
        Int32 numRunnables;
        mFirstIncRunnables->GetSize(&numRunnables);
        for (Int32 i = 0; i < numRunnables; i++) {
            AutoPtr<IInterface> obj;
            mFirstIncRunnables->Get(i, (IInterface**)&obj);
            IRunnable::Probe(obj)->Run();
        }
    }
    mCount++;
}

AutoPtr<IRunnable> ReferenceCountedTrigger::IncrementAsRunnable()
{
    return mIncrementRunnable;
}

void ReferenceCountedTrigger::AddLastDecrementRunnable(
    /* [in] */ IRunnable* r)
{
    // To ensure that the last decrement always calls, we increment and decrement after setting
    // the last decrement runnable
    Boolean ensureLastDecrement = (mCount == 0);
    if (ensureLastDecrement) Increment();
    mLastDecRunnables->Add(r);
    if (ensureLastDecrement) Decrement();
}

void ReferenceCountedTrigger::Decrement()
{
    mCount--;
    Boolean isEmpty;
    mLastDecRunnables->IsEmpty(&isEmpty);
    if (mCount == 0 && !isEmpty) {
        Int32 numRunnables;
        mLastDecRunnables->GetSize(&numRunnables);
        for (Int32 i = 0; i < numRunnables; i++) {
            AutoPtr<IInterface> obj;
            mLastDecRunnables->Get(i, (IInterface**)&obj);
            IRunnable::Probe(obj)->Run();
        }
    }
    else if (mCount < 0) {
        if (mErrorRunnable != NULL) {
            mErrorRunnable->Run();
        }
        else {
            //TODO
            // new Throwable("Invalid ref count").printStackTrace();
            Console::LogError(mContext, String("Invalid ref count"));
        }
    }
}

AutoPtr<IRunnable> ReferenceCountedTrigger::DecrementAsRunnable()
{
    return mDecrementRunnable;
}

AutoPtr<IAnimatorListener> ReferenceCountedTrigger::DecrementOnAnimationEnd()
{
    return (IAnimatorListener*)(new MyAnimatorListenerAdapter(this));
}

Int32 ReferenceCountedTrigger::GetCount()
{
    return mCount;
}

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos