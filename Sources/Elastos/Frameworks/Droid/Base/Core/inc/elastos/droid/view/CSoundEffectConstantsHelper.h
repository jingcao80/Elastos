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

#ifndef _ELASTOS_DROID_VIEW_CSOUNDEFFECTCONSTANTSHELPER_H_
#define _ELASTOS_DROID_VIEW_CSOUNDEFFECTCONSTANTSHELPER_H_

#include "_Elastos_Droid_View_CSoundEffectConstantsHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CSoundEffectConstantsHelper)
    , public Singleton
{
public:
    CAR_SINGLETON_DECL()

    CARAPI GetContantForFocusDirection(
        /* [in] */ Int32 direction,
        /* [out] */ Int32* result);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_VIEW_CSOUNDEFFECTCONSTANTSHELPER_H_

