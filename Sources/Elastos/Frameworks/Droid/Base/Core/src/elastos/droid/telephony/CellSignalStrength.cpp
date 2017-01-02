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

#include "elastos/droid/telephony/CellSignalStrength.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL(CellSignalStrength, Object, ICellSignalStrength)

static AutoPtr< ArrayOf<String> > InitSIGNAL_STRENGTH_NAMES() {
    AutoPtr< ArrayOf<String> > ssn = ArrayOf<String>::Alloc(5);
    (*ssn)[0] = String("none");
    (*ssn)[0] = String("poor");
    (*ssn)[0] = String("moderate");
    (*ssn)[0] = String("good");
    (*ssn)[0] = String("great");
    return ssn;
}

AutoPtr< ArrayOf<String> > CellSignalStrength::SIGNAL_STRENGTH_NAMES = InitSIGNAL_STRENGTH_NAMES();

ECode CellSignalStrength::SetDefaultValues()
{
    return NOERROR;
}

ECode CellSignalStrength::GetLevel(
    /* [out] */ Int32* level)
{
    return NOERROR;
}

ECode CellSignalStrength::GetAsuLevel(
    /* [out] */ Int32* asulevel)
{
    return NOERROR;
}

ECode CellSignalStrength::GetDbm(
    /* [out] */ Int32* dbm)
{
    return NOERROR;
}

ECode CellSignalStrength::Copy(
    /* [out] */ ICellSignalStrength** css)
{
    return NOERROR;
}

ECode CellSignalStrength::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return NOERROR;
}

ECode CellSignalStrength::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* res)
{
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

