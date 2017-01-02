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

#ifndef __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKENHELPER_H__
#define __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKENHELPER_H__

#include "_Elastos_Droid_Text_Utility_CRfc822TokenHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

CarClass(CRfc822TokenHelper)
    , public Singleton
    , public IRfc822TokenHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI QuoteNameIfNecessary(
        /* [in] */ const String& name,
        /* [out] */ String* ret);

    CARAPI QuoteName(
        /* [in] */ const String& name,
        /* [out] */ String* ret);

    CARAPI QuoteComment(
        /* [in] */ const String& comment,
        /* [out] */ String* ret);
};

} //namespace Utility
} //namespace Text
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_TEXT_UTILITY_CRFC822TOKENHELPER_H__
