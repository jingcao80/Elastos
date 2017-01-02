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

#include "elastos/droid/app/CNotificationBuilderHelper.h"
#include "elastos/droid/app/CNotificationBuilder.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_SINGLETON_IMPL(CNotificationBuilderHelper);
CAR_INTERFACE_IMPL(CNotificationBuilderHelper, Object, INotificationBuilderHelper);
ECode CNotificationBuilderHelper::CalculateTopPadding(
    /* [in] */ IContext* ctx,
    /* [in] */ Boolean hasThreeLines,
    /* [in] */ Float fontScale,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CNotificationBuilder::CalculateTopPadding(ctx, hasThreeLines, fontScale);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
