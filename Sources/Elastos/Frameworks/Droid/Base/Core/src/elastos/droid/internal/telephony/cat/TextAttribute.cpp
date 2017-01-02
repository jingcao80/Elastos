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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/TextAttribute.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                            TextAttribute
//=====================================================================
CAR_INTERFACE_IMPL(TextAttribute, Object, ITextAttribute);

TextAttribute::TextAttribute()
{
}

ECode TextAttribute::constructor(
    /* [in] */ Int32 start,
    /* [in] */ Int32 length,
    /* [in] */ TextAlignment align,
    /* [in] */ FontSize size,
    /* [in] */ Boolean bold,
    /* [in] */ Boolean italic,
    /* [in] */ Boolean underlined,
    /* [in] */ Boolean strikeThrough,
    /* [in] */ TextColor color)
{
    mStart = start;
    mLength = length;
    mAlign = align;
    mSize = size;
    mBold = bold;
    mItalic = italic;
    mUnderlined = underlined;
    mStrikeThrough = strikeThrough;
    mColor = color;
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
