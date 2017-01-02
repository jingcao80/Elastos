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

#ifndef __ELASTOS_TEXT_CNUMBERFORMATHELPER_H__
#define __ELASTOS_TEXT_CNUMBERFORMATHELPER_H__

#include "_Elastos_Text_CNumberFormatHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Text {

CarClass(CNumberFormatHelper)
    , public Singleton
    , public INumberFormatHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetCurrencyInstance(
        /* [out] */ INumberFormat** instance);

    CARAPI GetCurrencyInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    CARAPI GetIntegerInstance(
        /* [out] */ INumberFormat** instance);

    CARAPI GetIntegerInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    CARAPI GetInstance(
        /* [out] */ INumberFormat** instance);

    CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    CARAPI GetNumberInstance(
        /* [out] */ INumberFormat** instance);

    CARAPI GetNumberInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    CARAPI GetPercentInstance(
        /* [out] */ INumberFormat** instance);

    CARAPI GetPercentInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CNUMBERFORMATHELPER_H__
