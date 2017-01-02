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

#ifndef __ELASTOS_UTILITY_ABSTRACTSEQUENTIALLIST_H__
#define __ELASTOS_UTILITY_ABSTRACTSEQUENTIALLIST_H__

#include "AbstractList.h"

namespace Elastos {
namespace Utility {

class AbstractSequentialList
    : public AbstractList
{
public:
    CARAPI Add(
        /* [in] */ Int32 location,
        /* [in] */ IInterface* object);

    CARAPI AddAll(
        /* [in] */ Int32 location,
        /* [in] */ ICollection* collection,
        /* [out] */ Boolean* result);

    CARAPI Get(
        /* [in] */ Int32 location,
        /* [out] */ IInterface** object);

    CARAPI GetIterator(
        /* [out] */ IIterator** result);

    virtual CARAPI GetListIterator(
        /* [in] */ Int32 location,
        /* [out] */ IListIterator** it) = 0;

    CARAPI Remove(
        /* [in] */ Int32 location,
        /* [out] */ IInterface** object);

    CARAPI Set(
        /* [in] */ Int32 location,
        /* [in] */ IInterface* object,
        /* [out] */ IInterface** prevObject);

    using AbstractList::Add;
    using AbstractList::AddAll;
    using AbstractList::Remove;
    using AbstractList::Set;
    using AbstractList::GetListIterator;

protected:
    /**
      * Constructs a new instance of this AbstractSequentialList.
    */
    AbstractSequentialList() {}
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__ABSTRACTSEQUENTIALLIST_H__
