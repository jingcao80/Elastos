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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_REFERENCECOUNTEDTRIGGER_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_REFERENCECOUNTEDTRIGGER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/core/Runnable.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Content::IContext;
using Elastos::Core::IRunnable;
using Elastos::Core::Runnable;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

/**
 * Used to generate successive incremented names.
 */
class ReferenceCountedTrigger
    : public Object
{
private:
    class IncrementRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("ReferenceCountedTrigger::IncrementRunnable")

        IncrementRunnable(
            /* [in] */ ReferenceCountedTrigger* host);

        // @Override
        CARAPI Run();

    private:
        ReferenceCountedTrigger* mHost;
    };

    class DecrementRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("ReferenceCountedTrigger::DecrementRunnable")

        DecrementRunnable(
            /* [in] */ ReferenceCountedTrigger* host);

        // @Override
        CARAPI Run();

    private:
        ReferenceCountedTrigger* mHost;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("ReferenceCountedTrigger::MyAnimatorListenerAdapter")

        MyAnimatorListenerAdapter(
            /* [in] */ IWeakReference* host);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    ReferenceCountedTrigger();

    ~ReferenceCountedTrigger();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IRunnable* firstIncRunnable,
        /* [in] */ IRunnable* lastDecRunnable,
        /* [in] */ IRunnable* errorRunanable);

    /** Increments the ref count */
    CARAPI_(void) Increment();

    /** Convenience method to increment this trigger as a runnable */
    CARAPI_(AutoPtr<IRunnable>) IncrementAsRunnable();

    /** Adds a runnable to the last-decrement runnables list. */
    CARAPI_(void) AddLastDecrementRunnable(
        /* [in] */ IRunnable* r);

    /** Decrements the ref count */
    CARAPI_(void) Decrement();

    /** Convenience method to decrement this trigger as a runnable. */
    CARAPI_(AutoPtr<IRunnable>) DecrementAsRunnable();

    /** Convenience method to decrement this trigger as a animator listener. */
    CARAPI_(AutoPtr<IAnimatorListener>) DecrementOnAnimationEnd();

    /** Returns the current ref count */
    CARAPI_(Int32) GetCount();

public:
    // Convenience runnables
    AutoPtr<IncrementRunnable> mIncrementRunnable;
    AutoPtr<DecrementRunnable> mDecrementRunnable;

    AutoPtr<IContext> mContext;
    Int32 mCount;
    AutoPtr<IArrayList> mFirstIncRunnables;
    AutoPtr<IArrayList> mLastDecRunnables;
    AutoPtr<IRunnable> mErrorRunnable;
};

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_REFERENCECOUNTEDTRIGGER_H__
