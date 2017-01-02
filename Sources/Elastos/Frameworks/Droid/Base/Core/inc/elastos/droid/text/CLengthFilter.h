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

#ifndef __ELASTOS_DROID_TEXT_CLENGTHFILTER_H__
#define __ELASTOS_DROID_TEXT_CLENGTHFILTER_H__

#include "_Elastos_Droid_Text_CLengthFilter.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CLengthFilter)
    , public Object
    , public ILengthFilter
    , public IInputFilter
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CLengthFilter();

    CARAPI Filter(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ISpanned* dest,
        /* [in] */ Int32 dstart,
        /* [in] */ Int32 dend,
        /* [out] */ ICharSequence** cs);

    CARAPI constructor(
        /* [in] */ Int32 max);

    /**
     * @return the maximum length enforced by this input filter
     */
    CARAPI GetMax(
        /* [out] */ Int32* max);
private:
    Int32 mMax;
};

}//namespace Text
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CLENGTHFILTER_H__
