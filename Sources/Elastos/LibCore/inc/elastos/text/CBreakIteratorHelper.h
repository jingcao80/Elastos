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

#ifndef __ELASTOS_TEXT_CBREAKITERATORHELPER_H__
#define __ELASTOS_TEXT_CBREAKITERATORHELPER_H__

#include "_Elastos_Text_CBreakIteratorHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Text {

CarClass(CBreakIteratorHelper)
    , public Singleton
    , public IBreakIteratorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetCharacterInstance(
        /* [out] */ IBreakIterator** instance);

    CARAPI GetCharacterInstance(
        /* [in] */ ILocale* where,
        /* [out] */ IBreakIterator** instance);

    CARAPI GetLineInstance(
        /* [out] */ IBreakIterator** instance);

    CARAPI GetLineInstance(
        /* [in] */ ILocale* where,
        /* [out] */ IBreakIterator** instance);

    CARAPI GetSentenceInstance(
        /* [out] */ IBreakIterator** instance);

    CARAPI GetSentenceInstance(
        /* [in] */ ILocale* where,
        /* [out] */ IBreakIterator** instance);

    CARAPI GetWordInstance(
        /* [out] */ IBreakIterator** instance);

    CARAPI GetWordInstance(
        /* [in] */ ILocale* where,
        /* [out] */ IBreakIterator** instance);
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CBREAKITERATORHELPER_H__
