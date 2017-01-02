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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_VIEWPOOL_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_VIEWPOOL_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::ILinkedList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {


/* A view pool to manage more views than we can visibly handle */
class ViewPool : public Object
{
public:
    /** Initializes the pool with a fixed predetermined pool size */
    ViewPool(
        /* [in] */ IContext* context,
        /* [in] */ IViewPoolConsumer* viewCreator);

    /** Returns a view into the pool */
    void ReturnViewToPool(
        /* [in] */ IInterface* v);

    /** Gets a view from the pool and prepares it */
    AutoPtr<IInterface> PickUpViewFromPool(
        /* [in] */ IInterface* preferredData,
        /* [in] */ IInterface* prepareData);

public:
    AutoPtr<IContext> mContext;
    AutoPtr<IViewPoolConsumer> mViewCreator;
    AutoPtr<ILinkedList> mPool;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_VIEWPOOL_H__
