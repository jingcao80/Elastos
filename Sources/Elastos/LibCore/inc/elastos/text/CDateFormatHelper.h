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

#ifndef __ELASTOS_TEXT_CDATEFORMATHELPER_H__
#define __ELASTOS_TEXT_CDATEFORMATHELPER_H__

#include "_Elastos_Text_CDateFormatHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Text {

CarClass(CDateFormatHelper)
    , public Singleton
    , public IDateFormatHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*> ** locales);

    CARAPI GetDateInstance(
        /* [out] */ IDateFormat** instance);

    CARAPI GetDateInstance(
        /* [in] */ Int32 style,
        /* [out] */ IDateFormat** instance);

    CARAPI GetDateInstance(
        /* [in] */ Int32 style,
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormat** instance);

    CARAPI GetDateTimeInstance(
        /* [out] */ IDateFormat** instance);

    CARAPI GetDateTimeInstance(
        /* [in] */ Int32 dateStyle,
        /* [in] */ Int32 timeStyle,
        /* [out] */ IDateFormat** instance);

    CARAPI GetDateTimeInstance(
        /* [in] */ Int32 dateStyle,
        /* [in] */ Int32 timeStyle,
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormat** instance);

    CARAPI Set24HourTimePref(
        /* [in] */ Boolean bval);

    CARAPI GetInstance(
        /* [out] */ IDateFormat** instance);

    CARAPI GetTimeInstance(
        /* [out] */ IDateFormat** instance);

    CARAPI GetTimeInstance(
        /* [in] */ Int32 style,
        /* [out] */ IDateFormat** instance);

    CARAPI GetTimeInstance(
        /* [in] */ Int32 style,
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormat** instance);
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CDATEFORMATHELPER_H__
