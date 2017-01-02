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

#include "elastos/droid/widget/CToastHelper.h"
#include "elastos/droid/widget/Toast.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(CToastHelper, Singleton, IToastHelper);

CAR_SINGLETON_IMPL(CToastHelper);

ECode CToastHelper::MakeText(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 duration,
    /* [out] */ IToast** toast)
{
    VALIDATE_NOT_NULL(toast);
    return Toast::MakeText(context, text, duration, toast);
}

ECode CToastHelper::MakeText(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId,
    /* [in] */ Int32 duration,
    /* [out] */ IToast** toast)
{
    VALIDATE_NOT_NULL(toast);
    return Toast::MakeText(context, resId, duration, toast);
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
