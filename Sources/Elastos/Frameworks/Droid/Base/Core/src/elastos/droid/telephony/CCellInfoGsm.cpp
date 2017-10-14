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

#include "elastos/droid/telephony/CCellIdentityGsm.h"
#include "elastos/droid/telephony/CCellInfoGsm.h"
#include "elastos/droid/telephony/CCellSignalStrengthGsm.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CCellInfoGsm::TAG("CCellInfoGsm");
const Boolean CCellInfoGsm::DBG = FALSE;

CAR_INTERFACE_IMPL(CCellInfoGsm, CellInfo, ICellInfoGsm)

CAR_OBJECT_IMPL(CCellInfoGsm)

CCellInfoGsm::CCellInfoGsm()
{
}

CCellInfoGsm::~CCellInfoGsm()
{
}

ECode CCellInfoGsm::constructor()
{
    CellInfo::constructor();
    CCellIdentityGsm::New((ICellIdentityGsm**)&mCellIdentityGsm);
    CCellSignalStrengthGsm::New((ICellSignalStrengthGsm**)&mCellSignalStrengthGsm);
    return NOERROR;
}

ECode CCellInfoGsm::constructor(
    /* [in] */ ICellInfoGsm* ci)
{
    CellInfo::constructor(ICellInfo::Probe(ci));
    AutoPtr<ICellIdentityGsm> cgsm;
    AutoPtr<ICellSignalStrengthGsm> sgsm;
    ci->GetCellIdentity((ICellIdentityGsm**)&cgsm);
    ci->GetCellSignalStrength((ICellSignalStrengthGsm**)&sgsm);
    cgsm->Copy((ICellIdentityGsm**)&mCellIdentityGsm);
    sgsm->Copy((ICellSignalStrengthGsm**)&mCellSignalStrengthGsm);
    return NOERROR;
}

ECode CCellInfoGsm::GetCellIdentity(
    /* [out] */ ICellIdentityGsm** cid)
{
    VALIDATE_NOT_NULL(cid);
    *cid = mCellIdentityGsm;
    REFCOUNT_ADD(*cid);
    return NOERROR;
}

ECode CCellInfoGsm::SetCellIdentity(
    /* [in] */ ICellIdentityGsm* cid)
{
    mCellIdentityGsm = cid;
    return NOERROR;
}

ECode CCellInfoGsm::GetCellSignalStrength(
    /* [out] */ ICellSignalStrengthGsm** css)
{
    VALIDATE_NOT_NULL(css);
    *css = mCellSignalStrengthGsm;
    REFCOUNT_ADD(*css);
    return NOERROR;
}

ECode CCellInfoGsm::SetCellSignalStrength(
    /* [in] */ ICellSignalStrengthGsm* css)
{
    mCellSignalStrengthGsm = css;
    return NOERROR;
}

ECode CCellInfoGsm::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 super, cgsm, cssgsm;
    CellInfo::GetHashCode(&super);
    IObject::Probe(mCellIdentityGsm)->GetHashCode(&cgsm);
    IObject::Probe(mCellSignalStrengthGsm)->GetHashCode(&cssgsm);
    *hashCode = super + cgsm + cssgsm;
    return NOERROR;
}

ECode CCellInfoGsm::Equals(
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
    AutoPtr<ICellInfoGsm> o = ICellInfoGsm::Probe(other);
    if (o != NULL) {
        AutoPtr<ICellIdentityGsm> cgsm;
        o->GetCellIdentity((ICellIdentityGsm**)&cgsm);
        IObject::Probe(mCellIdentityGsm)->Equals(cgsm, &tempRes);
        AutoPtr<ICellSignalStrengthGsm> sgsm;
        o->GetCellSignalStrength((ICellSignalStrengthGsm**)&sgsm);
        IObject::Probe(mCellSignalStrengthGsm)->Equals(sgsm, &tempRes2);
        *result = tempRes && tempRes2;
    }
    else *result = FALSE;
    return NOERROR;
}

ECode CCellInfoGsm::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("CellInfoGsm:{");
    String strSuper;
    CellInfo::ToString(&strSuper);
    sb.Append(strSuper);
    sb.Append(" ");
    String strcGsm;
    IObject::Probe(mCellIdentityGsm)->ToString(&strcGsm);
    sb.Append(strcGsm);
    String strcssGsm;
    IObject::Probe(mCellSignalStrengthGsm)->ToString(&strcssGsm);
    sb.Append(" ");
    sb.Append(strcssGsm);
    sb.Append("}");

    *str = sb.ToString();
    return NOERROR;
}

ECode CCellInfoGsm::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    CellInfo::ReadFromParcel(source);
    source->ReadInterfacePtr((Handle32*)&mCellIdentityGsm);
    source->ReadInterfacePtr((Handle32*)&mCellSignalStrengthGsm);
    return NOERROR;
}

ECode CCellInfoGsm::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    CellInfo::WriteToParcel(dest);
    dest->WriteInterfacePtr(mCellIdentityGsm);
    dest->WriteInterfacePtr(mCellSignalStrengthGsm);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

