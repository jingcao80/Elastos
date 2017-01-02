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

#include "elastos/droid/text/method/CBaseKeyListenerHelper.h"
#include "elastos/droid/text/method/BaseKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text{
namespace Method{

CAR_SINGLETON_IMPL(CBaseKeyListenerHelper)

CAR_INTERFACE_IMPL(CBaseKeyListenerHelper, Singleton, IBaseKeyListenerHelper)

CARAPI CBaseKeyListenerHelper::MakeTextContentType(
    /* [in] */ Capitalize caps,
    /* [in] */ Boolean autoText,
    /* [out] */ Int32* ret)
{
    return BaseKeyListener::MakeTextContentType(caps, autoText, ret);
}

} // namespace Elastos
} // namespace Droid
} // namespace Text
} // namespace Method