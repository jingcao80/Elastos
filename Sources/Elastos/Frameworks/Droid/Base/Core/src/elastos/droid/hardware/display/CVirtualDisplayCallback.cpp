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

#include "elastos/droid/hardware/display/CVirtualDisplayCallback.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CAR_INTERFACE_IMPL(CVirtualDisplayCallback, Object, IVirtualDisplayCallback)

CAR_OBJECT_IMPL(CVirtualDisplayCallback)

ECode CVirtualDisplayCallback::OnPaused()
{
    return NOERROR;
}

ECode CVirtualDisplayCallback::OnResumed()
{
    return NOERROR;
}

ECode CVirtualDisplayCallback::OnStopped()
{
    return NOERROR;
}

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos