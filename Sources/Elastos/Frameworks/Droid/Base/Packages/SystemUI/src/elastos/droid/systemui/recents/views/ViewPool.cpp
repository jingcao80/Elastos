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

#include "elastos/droid/systemui/recents/views/ViewPool.h"

using Elastos::Utility::CLinkedList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

ViewPool::ViewPool(
    /* [in] */ IContext* context,
    /* [in] */ IViewPoolConsumer* viewCreator)
    : mContext(context)
    , mViewCreator(viewCreator)
{
    CLinkedList::New((ILinkedList**)&mPool);
}

/** Returns a view into the pool */
void ViewPool::ReturnViewToPool(
    /* [in] */ IInterface* v)
{
    mViewCreator->PrepareViewToEnterPool(v);
    mPool->Push(v);
}

/** Gets a view from the pool and prepares it */
AutoPtr<IInterface> ViewPool::PickUpViewFromPool(
    /* [in] */ IInterface* preferredData,
    /* [in] */ IInterface* prepareData)
{
    AutoPtr<IInterface> v;
    Boolean isNewView = FALSE;
    Boolean isEmpty;
    mPool->IsEmpty(&isEmpty);
    if (isEmpty) {
        mViewCreator->CreateView(mContext, (IInterface**)&v);
        isNewView = TRUE;
    }
    else {
        // Try and find a preferred view
        AutoPtr<IIterator> iter;
        mPool->GetIterator((IIterator**)&iter);
        Boolean hasNext;
        while (iter->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> vpv;
            iter->GetNext((IInterface**)&vpv);
            Boolean res;
            mViewCreator->HasPreferredData(vpv, preferredData, &res);
            if (res) {
                v = vpv;
                iter->Remove();
                break;
            }
        }
        // Otherwise, just grab the first view
        if (v == NULL) {
            mPool->Pop((IInterface**)&v);
        }
    }
    mViewCreator->PrepareViewToLeavePool(v, prepareData, isNewView);
    return v;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
