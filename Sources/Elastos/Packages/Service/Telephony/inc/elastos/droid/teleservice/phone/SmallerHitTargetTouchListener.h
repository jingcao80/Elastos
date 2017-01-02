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

#ifndef  __ELASTOS_DROID_PHONE_SMALLERHITTARGETTOUCHLISTENER_H__
#define  __ELASTOS_DROID_PHONE_SMALLERHITTARGETTOUCHLISTENER_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * OnTouchListener used to shrink the "hit target" of some onscreen buttons.
 *
 * We do this for a few specific buttons which are vulnerable to
 * "false touches" because either (1) they're near the edge of the
 * screen and might be unintentionally touched while holding the
 * device in your hand, (2) they're in the upper corners and might
 * be touched by the user's ear before the prox sensor has a chance to
 * kick in, or (3) they are close to other buttons.
 */
class SmallerHitTargetTouchListener
    : public Object
    , public IViewOnTouchListener
{
public:
    TO_STRING_IMPL("SmallerHitTargetTouchListener")

    CAR_INTERFACE_DECL()

    SmallerHitTargetTouchListener()
        : mDownEventHit(FALSE)
    {}

    /**
     * Called when a touch event is dispatched to a view. This allows listeners to
     * get a chance to respond before the target view.
     *
     * @return True if the listener has consumed the event, false otherwise.
     *         (In other words, we return true when the touch is *outside*
     *         the "smaller hit target", which will prevent the actual
     *         button from handling these events.)
     */
    //@Override
    CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

private:
    static const String TAG;

    /**
     * Edge dimensions where a touch does not register an action (in DIP).
     */
    static const Int32 HIT_TARGET_EDGE_IGNORE_DP_X;
    static const Int32 HIT_TARGET_EDGE_IGNORE_DP_Y;
    static const Int32 HIT_TARGET_MIN_SIZE_DP_X;
    static const Int32 HIT_TARGET_MIN_SIZE_DP_Y;

    // True if the most recent DOWN event was a "hit".
    Boolean mDownEventHit;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_SMALLERHITTARGETTOUCHLISTENER_H__