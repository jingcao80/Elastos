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

#ifndef __ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIGITSEDITTEXT_H__
#define __ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIGITSEDITTEXT_H__

#include "elastos/droid/phone/common/widget/ResizingTextEditText.h"

using Elastos::Droid::Phone::Common::Widget::ResizingTextEditText;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Dialpad {

/**
 * EditText which suppresses IME show up.
 */
class DigitsEditText
    : public ResizingTextEditText
{
public:
    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

protected:
    // @Override
    CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean focused,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);
};

} // namespace Dialpad
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIGITSEDITTEXT_H__
