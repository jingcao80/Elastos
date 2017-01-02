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

#include "elastos/droid/internal/telephony/gsm/CGsmCellBroadcastHandlerHelper.h"
#include "elastos/droid/internal/telephony/gsm/CGsmCellBroadcastHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CGsmCellBroadcastHandlerHelper, Singleton, IGsmCellBroadcastHandlerHelper)

CAR_SINGLETON_IMPL(CGsmCellBroadcastHandlerHelper)

ECode CGsmCellBroadcastHandlerHelper::MakeGsmCellBroadcastHandler(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneBase* phone,
    /* [out] */ IGsmCellBroadcastHandler** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IGsmCellBroadcastHandler> h;
    h = CGsmCellBroadcastHandler::MakeGsmCellBroadcastHandler(context, phone);
    *result = h;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos