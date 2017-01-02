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

#ifndef __ELASTOS_DROID_TEXT_AndroidBidi_H__
#define __ELASTOS_DROID_TEXT_AndroidBidi_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * Access the ICU bidi implementation.
 * @hide
 */
/* package */
class AndroidBidi
{
public:
    static CARAPI_(Int32) Bidi(
        /* [in] */ Int32 dir,
        /* [in] */ ArrayOf<Char32>* chs,
        /* [in] */ ArrayOf<Byte>* chInfo,
        /* [in] */ Int32 n,
        /* [in] */ Boolean haveInfo);

    /**
     * Returns run direction information for a line within a paragraph.
     *
     * @param dir base line direction, either Layout.DIR_LEFT_TO_RIGHT or
     *     Layout.DIR_RIGHT_TO_LEFT
     * @param levels levels as returned from {@link #bidi}
     * @param lstart start of the line in the levels array
     * @param chars the character array (used to determine whitespace)
     * @param cstart the start of the line in the chars array
     * @param len the length of the line
     * @return the directions
     */
    static CARAPI_(AutoPtr<ILayoutDirections>) Directions(
        /* [in] */ Int32 dir,
        /* [in] */ ArrayOf<Byte>* levels,
        /* [in] */ Int32 lstart,
        /* [in] */ ArrayOf<Char32>* chars,
        /* [in] */ Int32 cstart,
        /* [in] */ Int32 len);
private:
    //native
    static CARAPI_(Int32) RunBidi(
        /* [in] */ Int32 dir,
        /* [in] */ ArrayOf<Char32>* chs,
        /* [in] */ ArrayOf<Byte>* chInfo,
        /* [in] */ Int32 n,
        /* [in] */ Boolean haveInfo);

private:
    AndroidBidi();
    AndroidBidi(const AndroidBidi&);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_AndroidBidi_H__
