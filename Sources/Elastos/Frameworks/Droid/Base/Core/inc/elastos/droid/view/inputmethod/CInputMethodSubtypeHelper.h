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

#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODSUBTYPEHELPER_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODSUBTYPEHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_View_InputMethod_CInputMethodSubtypeHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CInputMethodSubtypeHelper)
    , public Singleton
    , public IInputMethodSubtypeHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Sort(
        /* [in] */ IContext* context,
        /* [in] */ Int32 flags,
        /* [in] */ IInputMethodInfo* imi,
        /* [in] */ IList* subtypeList,
        /* [out, callee] */ IList** sortedSubtypeList);
};


} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_INPUTMETHOD_CINPUTMETHODSUBTYPEHELPER_H__
