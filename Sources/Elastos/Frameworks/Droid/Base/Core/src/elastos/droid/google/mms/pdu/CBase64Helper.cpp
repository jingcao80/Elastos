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

#include "elastos/droid/google/mms/pdu/CBase64Helper.h"
#include "elastos/droid/google/mms/pdu/Base64.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CAR_INTERFACE_IMPL(CBase64Helper, Singleton, IBase64Helper)
CAR_SINGLETON_IMPL(CBase64Helper)

ECode CBase64Helper::DecodeBase64(
    /* [in] */ ArrayOf<Byte>* base64Data,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > array = Base64::DecodeBase64(base64Data);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBase64Helper::DiscardNonBase64(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > array = Base64::DiscardNonBase64(data);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
