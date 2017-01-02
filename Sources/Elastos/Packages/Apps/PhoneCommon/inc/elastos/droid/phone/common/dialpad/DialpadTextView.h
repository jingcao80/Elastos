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

#ifndef __ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIALPADTEXTVIEW_H__
#define __ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIALPADTEXTVIEW_H__

#include "Elastos.Droid.Graphics.h"
#include <elastos/droid/widget/TextView.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Dialpad {

/**
 * EditText which suppresses IME show up.
 */
class DialpadTextView
    : public TextView
{
public:
    DialpadTextView();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Draw the text to fit within the height/width which have been specified during measurement.
     */
    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

protected:
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    AutoPtr<IRect> mTextBounds;
    String mTextStr;
};

} // namespace Dialpad
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIALPADTEXTVIEW_H__
