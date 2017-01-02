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

#include "elastos/droid/internal/telephony/CSyntheticSmsMessageHelper.h"
#include "elastos/droid/internal/telephony/SyntheticSmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CSyntheticSmsMessageHelper, Singleton, ISyntheticSmsMessageHelper)

CAR_SINGLETON_IMPL(CSyntheticSmsMessageHelper)

ECode CSyntheticSmsMessageHelper::IsSyntheticPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean res = SyntheticSmsMessage::IsSyntheticPdu(pdu);
    *result = res;
    return NOERROR;
}

ECode CSyntheticSmsMessageHelper::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ ISyntheticSmsMessage** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISyntheticSmsMessage> res = SyntheticSmsMessage::CreateFromPdu(pdu);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony