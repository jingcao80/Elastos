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

#ifndef __ELASTOS_DROID_PHONE_COMMON_WIDGET_RESIZINGTEXTEDITTEXT_H__
#define __ELASTOS_DROID_PHONE_COMMON_WIDGET_RESIZINGTEXTEDITTEXT_H__

#include "Elastos.CoreLibrary.Core.h"
#include <elastos/droid/widget/EditText.h>

using Elastos::Droid::Widget::EditText;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Widget {

/**
 * EditText which resizes dynamically with respect to text length.
 */
class ResizingTextEditText
    : public EditText
{
public:
    ResizingTextEditText();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    // @Override
    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 lengthBefore,
        /* [in] */ Int32 lengthAfter);

    // @Override
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

private:
    Int32 mOriginalTextSize;
    Int32 mMinTextSize;
};

} // namespace Widget
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_WIDGET_RESIZINGTEXTEDITTEXT_H__
