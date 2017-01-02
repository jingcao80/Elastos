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
#include "elastos/droid/internal/telephony/dataconnection/CDctController.h"
#include "elastos/droid/internal/telephony/dataconnection/CDctControllerHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(CDctControllerHelper, Singleton, IDctControllerHelper)

CAR_SINGLETON_IMPL(CDctControllerHelper)

ECode CDctControllerHelper::GetInstance(
        /* [out] */ IDctController** result)
{
    return CDctController::GetInstance(result);
}

ECode CDctControllerHelper::MakeDctController(
        /* [in] */ ArrayOf<IPhoneProxy*>* phones,
        /* [in] */ ILooper* looper,
        /* [out] */ IDctController** result)
{
    return CDctController::MakeDctController(phones, looper, result);
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
