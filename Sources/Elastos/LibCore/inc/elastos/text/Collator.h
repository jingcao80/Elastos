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

#ifndef __ELASTOS_TEXT_COLLATOR_H__
#define __ELASTOS_TEXT_COLLATOR_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.CoreLibrary.Text.h"
#include "Object.h"

using Elastos::Core::IComparator;
using Elastos::Core::ICloneable;
using Elastos::Core::Object;
using Elastos::Utility::ILocale;
using Libcore::ICU::IRuleBasedCollatorICU;

namespace Elastos {
namespace Text {

class Collator
    : public Object
    , public ICollator
    , public IComparator
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI CloneImpl(
        /* [in] */ ICollator* object);

    virtual CARAPI Clone(
        /* [out] */ IInterface** object) = 0;

    virtual CARAPI Compare(
        /* [in] */ IInterface* object1,
        /* [in] */ IInterface* object2,
        /* [out] */ Int32* result);

    virtual CARAPI Compare(
        /* [in] */ const String& string1,
        /* [in] */ const String& string2,
        /* [out] */ Int32* result) = 0;

    //@Override
    virtual CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    virtual CARAPI Equals(
        /* [in] */ const String& string1,
        /* [in] */ const String& string2,
        /* [out] */ Boolean* result);

    static CARAPI GetAvailableLocales(
       /* [out] */ ArrayOf<ILocale*>** locales);

    virtual CARAPI GetCollationKey(
        /* [in] */ const String& string,
        /* [out] */ ICollationKey** key) = 0;

    virtual CARAPI GetDecomposition(
        /* [out] */ Int32* decomposition);

    static CARAPI GetInstance(
       /* [out] */ ICollator** instance);

    static CARAPI GetInstance(
       /* [in] */ ILocale* locale,
       /* [out] */ ICollator** instance);

    virtual CARAPI GetStrength(
        /* [out] */ Int32* strength);

//    @Override
//    public abstract int hashCode();

    virtual CARAPI SetDecomposition(
        /* [in] */ Int32 value);

    virtual CARAPI SetStrength(
        /* [in] */ Int32 value);

    virtual CARAPI GetICUCollator(
        /* [out] */ IRuleBasedCollatorICU** icuCollator);

protected:
    CARAPI constructor(
        /* [in] */ IRuleBasedCollatorICU* icuColl);

    CARAPI constructor();

private:
    CARAPI DecompositionMode_Java_ICU(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* value);

    CARAPI DecompositionMode_ICU_Java(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* value);

    CARAPI Strength_Java_ICU(
        /* [in] */ Int32 value,
        /* [out] */ Int32* result);

    CARAPI Strength_ICU_Java(
        /* [in] */ Int32 value,
        /* [out] */ Int32* result);

public:
    AutoPtr<IRuleBasedCollatorICU> mICUColl;
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_COLLATOR_H__
