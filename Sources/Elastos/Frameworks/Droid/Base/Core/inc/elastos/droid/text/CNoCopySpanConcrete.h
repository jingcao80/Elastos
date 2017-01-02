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

#ifndef __ELASTOS_DROID_TEXT_CNOCOPYSPAN_CONCRETE_H__
#define __ELASTOS_DROID_TEXT_CNOCOPYSPAN_CONCRETE_H__

#include "_Elastos_Droid_Text_CNoCopySpanConcrete.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * Convenience equivalent for when you would just want a new Object() for
 * a span but want it to be no-copy.  Use this instead.
 */
CarClass(CNoCopySpanConcrete)
    , public Object
    , public INoCopySpan
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CNOCOPYSPAN_CONCRETE_H__
