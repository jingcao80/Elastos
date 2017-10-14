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

#include "elastos/droid/internal/utility/CUserIcons.h"
#include "elastos/droid/internal/utility/UserIcons.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CUserIcons, Singleton, IUserIcons)

CAR_SINGLETON_IMPL(CUserIcons)

ECode CUserIcons::ConvertToBitmap(
    /* [in] */ IDrawable* icon,
    /* [out] */ IBitmap** bitmap)
{
    return UserIcons::ConvertToBitmap(icon, bitmap);
}

ECode CUserIcons::GetDefaultUserIcon(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean light,
    /* [out] */ IDrawable** drawable)
{
    return UserIcons::GetDefaultUserIcon(userId, light, drawable);
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
