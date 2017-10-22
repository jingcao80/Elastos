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

#include "elastos/droid/telephony/CCellIdentityWcdma.h"
#include "elastos/droid/telephony/CCellInfoWcdma.h"
#include "elastos/droid/telephony/CCellSignalStrengthWcdma.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CCellInfoWcdma::TAG("CCellInfoWcdma");
const Boolean CCellInfoWcdma::DBG = FALSE;

CAR_INTERFACE_IMPL(CCellInfoWcdma, CellInfo, ICellInfoWcdma)

CAR_OBJECT_IMPL(CCellInfoWcdma)

CCellInfoWcdma::CCellInfoWcdma()
{
}

CCellInfoWcdma::~CCellInfoWcdma()
{
}

ECode CCellInfoWcdma::constructor()
{
    CellInfo::constructor();
    CCellIdentityWcdma::New((ICellIdentityWcdma**)&mCellIdentityWcdma);
    CCellSignalStrengthWcdma::New((ICellSignalStrengthWcdma**)&mCellSignalStrengthWcdma);
    return NOERROR;
}

ECode CCellInfoWcdma::constructor(
    /* [in] */ ICellInfoWcdma* ci)
{
    CellInfo::constructor(ICellInfo::Probe(ci));
    AutoPtr<ICellIdentityWcdma> cWcdma;
    AutoPtr<ICellSignalStrengthWcdma> sWcdma;
    ci->GetCellIdentity((ICellIdentityWcdma**)&cWcdma);
    ci->GetCellSignalStrength((ICellSignalStrengthWcdma**)&sWcdma);
    cWcdma->Copy((ICellIdentityWcdma**)&mCellIdentityWcdma);
    AutoPtr<ICellSignalStrength> strength;
    ICellSignalStrength::Probe(sWcdma)->Copy((ICellSignalStrength**)&strength);
    mCellSignalStrengthWcdma = ICellSignalStrengthWcdma::Probe(strength);
    return NOERROR;
}

ECode CCellInfoWcdma::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 super, cWcdma, cssWcdma;
    CellInfo::GetHashCode(&super);
    IObject::Probe(mCellIdentityWcdma)->GetHashCode(&cWcdma);
    IObject::Probe(mCellSignalStrengthWcdma)->GetHashCode(&cssWcdma);
    *hashCode = super + cWcdma + cssWcdma;
    return NOERROR;
}

ECode CCellInfoWcdma::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tempRes, tempRes2;
    CellInfo::Equals(other, &tempRes);
    if (!tempRes) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ICellInfoWcdma> o = ICellInfoWcdma::Probe(other);
    if (o != NULL) {
        AutoPtr<ICellIdentityWcdma> cWcdma;
        o->GetCellIdentity((ICellIdentityWcdma**)&cWcdma);
        IObject::Probe(mCellIdentityWcdma)->Equals(cWcdma, &tempRes);
        AutoPtr<ICellSignalStrengthWcdma> sWcdma;
        o->GetCellSignalStrength((ICellSignalStrengthWcdma**)&sWcdma);
        IObject::Probe(mCellSignalStrengthWcdma)->Equals(sWcdma, &tempRes2);
        *result = tempRes && tempRes2;
    }
    else *result = FALSE;
    return NOERROR;
}

ECode CCellInfoWcdma::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("CellInfoWcdma:{");
    String strSuper;
    CellInfo::ToString(&strSuper);
    sb.Append(strSuper);
    sb.Append(" ");
    String strcWcdma;
    IObject::Probe(mCellIdentityWcdma)->ToString(&strcWcdma);
    sb.Append(strcWcdma);
    String strcssWcdma;
    IObject::Probe(mCellSignalStrengthWcdma)->ToString(&strcssWcdma);
    sb.Append(" ");
    sb.Append(strcssWcdma);
    sb.Append("}");

    *str = sb.ToString();
    return NOERROR;
}

ECode CCellInfoWcdma::GetCellIdentity(
    /* [out] */ ICellIdentityWcdma** res)
{
    VALIDATE_NOT_NULL(res);
    *res = mCellIdentityWcdma;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CCellInfoWcdma::SetCellIdentity(
    /* [in] */ ICellIdentityWcdma* cid)
{
    mCellIdentityWcdma = cid;
    return NOERROR;
}

ECode CCellInfoWcdma::GetCellSignalStrength(
    /* [out] */ ICellSignalStrengthWcdma** res)
{
    VALIDATE_NOT_NULL(res);
    *res = mCellSignalStrengthWcdma;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CCellInfoWcdma::SetCellSignalStrength(
    /* [in] */ ICellSignalStrengthWcdma* css)
{
    mCellSignalStrengthWcdma = css;
    return NOERROR;
}

ECode CCellInfoWcdma::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    CellInfo::ReadFromParcel(source);
    source->ReadInterfacePtr((Handle32*)&mCellIdentityWcdma);
    source->ReadInterfacePtr((Handle32*)&mCellSignalStrengthWcdma);
    return NOERROR;
}

ECode CCellInfoWcdma::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    CellInfo::WriteToParcel(dest);
    dest->WriteInterfacePtr(mCellIdentityWcdma);
    dest->WriteInterfacePtr(mCellSignalStrengthWcdma);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

