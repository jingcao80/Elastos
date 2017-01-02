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

#ifndef __ELASTOS_CORE_COBJECTCONTAINER_H__
#define __ELASTOS_CORE_COBJECTCONTAINER_H__

#include "_Elastos_Core_CObjectContainer.h"
#include <elastos.h>
#include "container.h"
#include "CObjectEnumerator.h"

namespace Elastos{
namespace Core{

CarClass(CObjectContainer), public SimpleContainer
{
public:
    CAR_OBJECT_DECL()

    CObjectContainer();

    virtual ~CObjectContainer();

    CARAPI Add(
        /* [in] */ IInterface* pObject);

    CARAPI Remove(
        /* [in] */ IInterface* pObject);

    CARAPI GetObjectEnumerator(
        /* [out] */ IObjectEnumerator** ppEnumerator);

    CARAPI GetObjectCount(
        /* [out] */ Int32* pCount);

    CARAPI Contains(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* contains);

    CARAPI ContainsAll(
        /* [in] */ IObjectContainer* objectContainer,
        /* [out] */ Boolean* contains);

    CARAPI Dispose();

private:
    Int32               mCount;
    Int32               mState;
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_COBJECTCONTAINER_H__
