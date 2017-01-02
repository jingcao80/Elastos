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

#ifndef __ELASTOS_DROID_TEXT_CBIDIFORMATTER_HELPER_H__
#define __ELASTOS_DROID_TEXT_CBIDIFORMATTER_HELPER_H__

#include "_Elastos_Droid_Text_CBidiFormatterHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CBidiFormatterHelper)
    , public Singleton
    , public IBidiFormatterHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Factory for creating an instance of BidiFormatter for the default locale directionality.
     *
     */
    CARAPI GetInstance(
        /* [out] */ IBidiFormatter** instance);

    /**
     * Factory for creating an instance of BidiFormatter given the context directionality.
     *
     * @param rtlContext Whether the context directionality is RTL.
     */
    CARAPI GetInstance(
        /* [in] */ Boolean rtlContext,
        /* [out] */ IBidiFormatter** instance);

    /**
     * Factory for creating an instance of BidiFormatter given the context locale.
     *
     * @param locale The context locale.
     */
    CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ IBidiFormatter** instance);
};

}//namespace Text
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CBIDIFORMATTER_HELPER_H__
