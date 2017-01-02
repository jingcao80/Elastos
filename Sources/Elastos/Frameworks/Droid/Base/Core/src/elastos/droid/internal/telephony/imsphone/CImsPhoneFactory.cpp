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

#include "elastos/droid/internal/telephony/imsphone/CImsPhoneFactory.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneFactory.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

CAR_INTERFACE_IMPL(CImsPhoneFactory, Singleton, IImsPhoneFactory)
CAR_SINGLETON_IMPL(CImsPhoneFactory)

ECode CImsPhoneFactory::MakePhone(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* phoneNotifier,
    /* [in] */ IPhone* defaultPhone,
    /* [out] */ IImsPhone** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IImsPhone> imsPhone = ImsPhoneFactory::MakePhone(context, phoneNotifier, defaultPhone);
    *result = imsPhone;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace ImsPhone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
