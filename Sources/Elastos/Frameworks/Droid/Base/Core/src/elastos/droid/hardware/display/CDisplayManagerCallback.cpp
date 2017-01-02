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

#include "elastos/droid/hardware/display/CDisplayManagerCallback.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CAR_INTERFACE_IMPL(CDisplayManagerCallback, Object, IIDisplayManagerCallback)

CAR_OBJECT_IMPL(CDisplayManagerCallback)

ECode CDisplayManagerCallback::constructor(
    /* [in] */ Handle32 owner)
{
    mOwner = (DisplayManagerGlobal*)owner;
    return NOERROR;
}

//@Override
ECode CDisplayManagerCallback::OnDisplayEvent(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 event)
{
    mOwner->HandleDisplayEvent(displayId, event);

    return NOERROR;
}

ECode CDisplayManagerCallback::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos