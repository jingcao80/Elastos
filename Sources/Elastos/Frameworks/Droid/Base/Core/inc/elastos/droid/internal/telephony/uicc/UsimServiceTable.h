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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_USIMSERVICETABLE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_USIMSERVICETABLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/uicc/IccServiceTable.h"

using Elastos::Droid::Internal::Telephony::Uicc::UsimService;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * Wrapper class for the USIM Service Table EF.
  * See 3GPP TS 31.102 Release 10 section 4.2.8
  */
class UsimServiceTable
    : public IccServiceTable
    , public IUsimServiceTable
{
public:
    CAR_INTERFACE_DECL();

    UsimServiceTable();

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* table);

    using IccServiceTable::IsAvailable;

    virtual CARAPI IsAvailable(
        /* [in] */ UsimService service,
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI_(String) GetTag();

    // @Override
    CARAPI_(AutoPtr<ArrayOf<IInterface*> >) GetValues();
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_USIMSERVICETABLE_H__
