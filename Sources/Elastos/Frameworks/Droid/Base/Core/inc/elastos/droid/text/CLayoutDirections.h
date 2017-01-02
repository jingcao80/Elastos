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

#ifndef __ELASTOS_DROID_TEXT_CLAYOUTDIRECTIONS_H__
#define __ELASTOS_DROID_TEXT_CLAYOUTDIRECTIONS_H__

#include "_Elastos_Droid_Text_CLayoutDirections.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * Stores information about bidirectional (left-to-right or right-to-left)
 * text within the layout of a line.
 */
CarClass(CLayoutDirections)
    , public Object
    , public ILayoutDirections
{
public:
    // Directions represents directional runs within a line of text.
    // Runs are pairs of ints listed in visual order, starting from the
    // leading margin.  The first int of each pair is the offset from
    // the first character of the line to the start of the run.  The
    // second int represents both the length and level of the run.
    // The length is in the lower bits, accessed by masking with
    // DIR_LENGTH_MASK.  The level is in the higher bits, accessed
    // by shifting by DIR_LEVEL_SHIFT and masking by DIR_LEVEL_MASK.
    // To simply test for an RTL direction, test the bit using
    // DIR_RTL_FLAG, if set then the direction is rtl.

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ ArrayOf<Int32>* directions);

    CARAPI SetDirections(
            /* [in] */ ArrayOf<Int32>* directions);

    CARAPI GetDirections(
        /* [out, callee] */ ArrayOf<Int32>** directions);

private:
    AutoPtr< ArrayOf<Int32> > mDirections;
};


} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CLAYOUTDIRECTIONS_H__
