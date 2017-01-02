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

#include "elastos/droid/internal/telephony/CProxyControllerHelper.h"
#include "elastos/droid/internal/telephony/ProxyController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  CProxyControllerHelper::
//==============================================================

CAR_INTERFACE_IMPL(CProxyControllerHelper, Singleton, IProxyControllerHelper)

CAR_SINGLETON_IMPL(CProxyControllerHelper)

ECode CProxyControllerHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<IPhone*>* phoneProxy,
    /* [in] */ IUiccController* uiccController,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci,
    /* [out] */ IProxyController** result)
{
    VALIDATE_NOT_NULL(result)
    return ProxyController::GetInstance(context, phoneProxy, uiccController, ci, result);
}

ECode CProxyControllerHelper::GetInstance(
    /* [out] */ IProxyController** result)
{
    VALIDATE_NOT_NULL(result)
    return ProxyController::GetInstance(result);
}


} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
