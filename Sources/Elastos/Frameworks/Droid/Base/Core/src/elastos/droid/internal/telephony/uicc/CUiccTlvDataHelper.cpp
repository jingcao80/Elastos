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

#include "elastos/droid/internal/telephony/uicc/CUiccTlvDataHelper.h"
#include "elastos/droid/internal/telephony/uicc/UiccTlvData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CAR_INTERFACE_IMPL(CUiccTlvDataHelper, Singleton, IUiccTlvDataHelper)

CAR_SINGLETON_IMPL(CUiccTlvDataHelper)

ECode CUiccTlvDataHelper::IsUiccTlvData(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = UiccTlvData::IsUiccTlvData(data);
    return NOERROR;
}

ECode CUiccTlvDataHelper::Parse(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IUiccTlvData** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUiccTlvData> utd = UiccTlvData::Parse(data);
    *result = utd;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
