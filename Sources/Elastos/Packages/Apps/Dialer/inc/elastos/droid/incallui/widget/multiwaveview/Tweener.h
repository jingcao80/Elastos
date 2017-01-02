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

#ifndef __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_TWEENER_H__
#define __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_TWEENER_H__

#include "Elastos.Droid.Animation.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimator;

using Elastos::Core::Object;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

class Tweener
    : public Object
{
private:
    class TweenerAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);
    };

public:
    Tweener(
        /* [in] */ IObjectAnimator* anim);

    static CARAPI To(
        /* [in] */ IInterface* object,
        /* [in] */ Int64 duration,
        /* [in] */ ArrayOf<IInterface*>* vars,
        /* [out] */ Tweener** result);

    CARAPI From(
        /* [in] */ IInterface* object,
        /* [in] */ Int64 duration,
        /* [in] */ ArrayOf<IInterface*>* vars,
        /* [out] */ Tweener** result);

    static CARAPI_(void) Reset();

    static CARAPI_(void) Replace(
        /* [in] */ IArrayList* props,
        /* [in] */ ArrayOf<IInterface*>* args);

private:
    static CARAPI_(void) Remove(
        /* [in] */ IAnimator* animator);

    static CARAPI_(AutoPtr<IHashMap>) Init_sTweens();

public:
    AutoPtr<IObjectAnimator> mAnimator;

private:
    static const String TAG;
    static const Boolean DEBUG;

    static AutoPtr<IHashMap> sTweens;

    // Listener to watch for completed animations and remove them.
    static AutoPtr<IAnimatorListener> sCleanupListener;
};

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_TWEENER_H__
