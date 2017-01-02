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

#ifndef __ELASTOS_DROID_VIEW_CABSSAVEDSTATEHELPER_H__
#define __ELASTOS_DROID_VIEW_CABSSAVEDSTATEHELPER_H__

#include "_Elastos_Droid_View_CAbsSavedStateHelper.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::View::IAbsSavedState;
using Elastos::Droid::View::IAbsSavedStateHelper;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CAbsSavedStateHelper)
    , public Singleton
    , public IAbsSavedStateHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEMPTY_STATE(
        /* [out] */ IAbsSavedState** emptyState);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CABSSAVEDSTATEHELPER_H__
