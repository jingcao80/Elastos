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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CDhcpStateMachineHelper.h"
#include "elastos/droid/net/DhcpStateMachine.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CDhcpStateMachineHelper, Singleton, IDhcpStateMachineHelper)

CAR_SINGLETON_IMPL(CDhcpStateMachineHelper)

ECode CDhcpStateMachineHelper::MakeDhcpStateMachine(
    /* [in] */ IContext* context,
    /* [in] */ IStateMachine* controller,
    /* [in] */ const String& intf,
    /* [out] */ IDhcpStateMachine** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IDhcpStateMachine> temp = DhcpStateMachine::MakeDhcpStateMachine(context, controller, intf);
    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
