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

#ifndef __ELASTOS_DROID_UTILITY_FAST_IMMUTABLE_ARRAYSET_H__
#define __ELASTOS_DROID_UTILITY_FAST_IMMUTABLE_ARRAYSET_H__

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/AbstractSet.h>

using Elastos::Utility::ISet;
using Elastos::Utility::AbstractSet;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Utility {

class FastImmutableArraySet
    : public AbstractSet
    , public IFastImmutableArraySet
{
private:
    class FastIterator
        : public Object
        , public IIterator
    {
    public:
        CAR_INTERFACE_DECL()

        FastIterator(
            /* [in] */ ArrayOf<IInterface*>* contents);

        CARAPI HasNext(
            /* [out] */ Boolean* next);

        CARAPI GetNext(
            /* [out] */ IInterface** obj);

        CARAPI Remove();

    private:
        friend class FastImmutableArraySet;

        AutoPtr<ArrayOf<IInterface*> > mContents;
        Int32 mIndex;
    };

public:
    CAR_INTERFACE_DECL()

    FastImmutableArraySet();

    CARAPI constructor(
        /* [in] */ ArrayOf<IInterface*>* contents);

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    CARAPI GetSize(
        /* [out] */ Int32* size);

protected:
    CARAPI_(String) GetClassName() { return String("FastImmutableArraySet"); }

private:
    AutoPtr<ArrayOf<IInterface*> > mContents;
    AutoPtr<FastIterator> mIterator;
};

} // Utility
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_UTILITY_FAST_IMMUTABLE_ARRAYSET_H__
