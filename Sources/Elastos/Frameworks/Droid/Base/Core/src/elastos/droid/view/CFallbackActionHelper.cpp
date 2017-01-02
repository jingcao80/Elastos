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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CFallbackActionHelper.h"
#include "elastos/droid/view/KeyCharacterMap.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CFallbackActionHelper, Singleton, IFallbackActionHelper);

CAR_SINGLETON_IMPL(CFallbackActionHelper);

ECode CFallbackActionHelper::Obtain(
    /* [out] */ IFallbackAction** action)
{
    VALIDATE_NOT_NULL(action);

    AutoPtr<KeyCharacterMap::FallbackAction> target =
            KeyCharacterMap::FallbackAction::Obtain();
    *action = (IFallbackAction*)target.Get();
    REFCOUNT_ADD(*action);

    return NOERROR;
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
