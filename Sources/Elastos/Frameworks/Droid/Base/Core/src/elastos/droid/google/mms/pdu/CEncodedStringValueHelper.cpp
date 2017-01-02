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

#include "elastos/droid/google/mms/pdu/CEncodedStringValueHelper.h"
#include "elastos/droid/google/mms/pdu/EncodedStringValue.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CAR_INTERFACE_IMPL(CEncodedStringValueHelper, Singleton, IEncodedStringValueHelper)
CAR_SINGLETON_IMPL(CEncodedStringValueHelper)


ECode CEncodedStringValueHelper::Extract(
    /* [in] */ const String& src,
    /* [out, callee] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<IEncodedStringValue*> > array = EncodedStringValue::Extract(src);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CEncodedStringValueHelper::Concat(
    /* [in] */ ArrayOf<IEncodedStringValue*>* addr,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = EncodedStringValue::Concat(addr);
    return NOERROR;
}

ECode CEncodedStringValueHelper::Copy(
    /* [in] */ IEncodedStringValue* value,
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IEncodedStringValue> esv = EncodedStringValue::Copy(value);
    *result = esv;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CEncodedStringValueHelper::EncodeStrings(
    /* [in] */ ArrayOf<String>* array,
    /* [out, callee] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<IEncodedStringValue*> > res = EncodedStringValue::EncodeStrings(array);
    *result = res;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
