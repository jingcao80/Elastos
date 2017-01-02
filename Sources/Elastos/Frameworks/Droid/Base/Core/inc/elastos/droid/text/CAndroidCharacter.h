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

#ifndef __ELASTOS_DROID_TEXT_CANDROIDCHARACTER_H__
#define __ELASTOS_DROID_TEXT_CANDROIDCHARACTER_H__

#include "_Elastos_Droid_Text_CAndroidCharacter.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CAndroidCharacter)
    , public Singleton
    , public IAndroidCharacter
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDirectionalities(
        /* [in] */ ArrayOf<Char32>* src,
        /* [in] */ ArrayOf<Byte>* dest,
        /* [in] */ Int32 count);

    CARAPI GetEastAsianWidth(
        /* [in] */ Char32 input,
        /* [out] */ Int32* result);

    CARAPI GetEastAsianWidths(
        /* [in] */ ArrayOf<Char32>* src,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Byte>* dest);

    CARAPI Mirror(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [out] */ Boolean* result);

    CARAPI GetMirror(
        /* [in] */ Char32 ch,
        /* [out] */ Char32* result);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CANDROIDCHARACTER_H__
