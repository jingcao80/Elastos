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

#include "CASN1IntegerHelper.h"
#include "CASN1Integer.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_SINGLETON_IMPL(CASN1IntegerHelper)

CAR_INTERFACE_IMPL(CASN1IntegerHelper, Singleton, IASN1IntegerHelper)

ECode CASN1IntegerHelper::GetInstance(
    /* [out] */ IASN1Integer** instance)
{
    return CASN1Integer::GetInstance(instance);
}

ECode CASN1IntegerHelper::ToIntValue(
    /* [in] */ IInterface* decoded,
    /* [out] */ Int32* intValue)
{
    return CASN1Integer::ToIntValue(decoded, intValue);
}

ECode CASN1IntegerHelper::ToBigIntegerValue(
    /* [in] */ IInterface* decoded,
    /* [out] */ IBigInteger** bigInteger)
{
    return CASN1Integer::ToBigIntegerValue(decoded, bigInteger);
}

ECode CASN1IntegerHelper::FromIntValue(
    /* [in] */ Int32 value,
    /* [out] */ IInterface** fromIntValue)
{
    return CASN1Integer::FromIntValue(value, fromIntValue);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

