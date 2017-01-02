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

#include "elastos/droid/internal/telephony/cdma/CCdmaInboundSmsHandlerHelper.h"
#include "elastos/droid/internal/telephony/cdma/CdmaInboundSmsHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL(CCdmaInboundSmsHandlerHelper, Singleton, ICdmaInboundSmsHandlerHelper)
CAR_SINGLETON_IMPL(CCdmaInboundSmsHandlerHelper)


ECode CCdmaInboundSmsHandlerHelper::MakeInboundSmsHandler(
    /* [in] */ IContext* context,
    /* [in] */ ISmsStorageMonitor* storageMonitor,
    /* [in] */ IPhoneBase* phone,
    /* [in] */ ICdmaSMSDispatcher* smsDispatcher,
    /* [out] */ ICdmaInboundSmsHandler** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICdmaInboundSmsHandler> cish = CdmaInboundSmsHandler::MakeInboundSmsHandler(context, storageMonitor, phone, smsDispatcher);
    *result = cish;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
