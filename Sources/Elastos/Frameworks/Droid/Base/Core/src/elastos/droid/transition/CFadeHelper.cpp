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

#include "elastos/droid/transition/CFadeHelper.h"
#include "elastos/droid/transition/CFade.h"

namespace Elastos {
namespace Droid {
namespace Transition {

CAR_SINGLETON_IMPL(CFadeHelper)

CAR_INTERFACE_IMPL(CFadeHelper, Singleton, IFadeHelper)

ECode CFadeHelper::GetIN(
    /* [out] */ Int32* _in)
{
    VALIDATE_NOT_NULL(_in)

    *_in = CFade::IN;
    return NOERROR;
}

ECode CFadeHelper::GetOUT(
    /* [out] */ Int32* _out)
{
    VALIDATE_NOT_NULL(_out)

    *_out = CFade::OUT;
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
