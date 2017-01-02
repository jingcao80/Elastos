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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_PASSWORDENTRYKEYBOARDVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_PASSWORDENTRYKEYBOARDVIEW_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/inputmethodservice/KeyboardView.h"

using Elastos::Droid::InputMethodService::KeyboardView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class PasswordEntryKeyboardView
    : public KeyboardView
    , public IPasswordEntryKeyboardView
{
public:
    CAR_INTERFACE_DECL()

    PasswordEntryKeyboardView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI SetShifted(
        /* [in] */ Boolean shifted,
        /* [out] */ Boolean* res);

protected:
    static const Int32 KEYCODE_OPTIONS;
    static const Int32 KEYCODE_SHIFT_LONGPRESS;
    static const Int32 KEYCODE_VOICE;
    static const Int32 KEYCODE_F1;
    static const Int32 KEYCODE_NEXT_LANGUAGE;
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_PASSWORDENTRYKEYBOARDVIEW_H__
