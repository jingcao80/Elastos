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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/UsimServiceTable.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                           UsimServiceTable
//=====================================================================
CAR_INTERFACE_IMPL(UsimServiceTable, IccServiceTable, IUsimServiceTable);

UsimServiceTable::UsimServiceTable()
{
}

ECode UsimServiceTable::constructor(
    /* [in] */ ArrayOf<Byte>* table)
{
    IccServiceTable::constructor(table);
    return NOERROR;
}

ECode UsimServiceTable::IsAvailable(
    /* [in] */ UsimService service,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    IccServiceTable::IsAvailable(service);
    return NOERROR;
}

String UsimServiceTable::GetTag()
{
    return String("UsimServiceTable");
}

AutoPtr<ArrayOf<IInterface*> > UsimServiceTable::GetValues()
{
    assert(0 && "TODO");
    // return UsimService::Values();
    return NULL;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
