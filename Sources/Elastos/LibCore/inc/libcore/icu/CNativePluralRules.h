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

#ifndef __LIBCORE_ICU_CNATIVEPLURALRULES_H__
#define __LIBCORE_ICU_CNATIVEPLURALRULES_H__

#include <Object.h>
#include "_Libcore_ICU_CNativePluralRules.h"

using Elastos::Core::Object;
using Elastos::Utility::ILocale;

namespace Libcore {
namespace ICU {

CarClass(CNativePluralRules)
    , public Object
    , public INativePluralRules
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI QuantityForInt(
        /* [in] */ Int32 value,
        /* [out] */ Int32 * result);

    static CARAPI_(AutoPtr<INativePluralRules>) ForLocale(
        /* [in] */ ILocale* locale);

    CARAPI constructor(
        /* [in] */ Int64 address);

    ~CNativePluralRules();

private:
    static CARAPI_(void) FinalizeImpl(
        /* [in] */ Int64 address);

    static CARAPI_(Int64) ForLocaleImpl(
        /* [in] */ String& localeName);

    static CARAPI_(Int32) QuantityForIntImpl(
        /* [in] */ Int64 address,
        /* [in] */ Int32 value);

private:
    Int64 mAddress;
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_CNATIVEPLURALRULES_H__
