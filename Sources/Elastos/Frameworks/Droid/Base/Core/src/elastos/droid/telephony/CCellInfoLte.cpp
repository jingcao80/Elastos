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

#include "elastos/droid/telephony/CCellIdentityLte.h"
#include "elastos/droid/telephony/CCellInfoLte.h"
#include "elastos/droid/telephony/CCellSignalStrengthLte.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CCellInfoLte::TAG("CCellInfoLte");
const Boolean CCellInfoLte::DBG = FALSE;

CAR_INTERFACE_IMPL(CCellInfoLte, CellInfo, ICellInfoLte)

CAR_OBJECT_IMPL(CCellInfoLte)

CCellInfoLte::CCellInfoLte()
{
}

CCellInfoLte::~CCellInfoLte()
{
}

ECode CCellInfoLte::constructor()
{
    CellInfo::constructor();
    CCellIdentityLte::New((ICellIdentityLte**)&mCellIdentityLte);
    CCellSignalStrengthLte::New((ICellSignalStrengthLte**)&mCellSignalStrengthLte);
    return NOERROR;
}

ECode CCellInfoLte::constructor(
    /* [in] */ ICellInfoLte* ci)
{
    CellInfo::constructor(ICellInfo::Probe(ci));
    AutoPtr<ICellIdentityLte> cLte;
    AutoPtr<ICellSignalStrengthLte> sLte;
    ci->GetCellIdentity((ICellIdentityLte**)&cLte);
    ci->GetCellSignalStrength((ICellSignalStrengthLte**)&sLte);
    cLte->Copy((ICellIdentityLte**)&mCellIdentityLte);
    sLte->Copy((ICellSignalStrengthLte**)&mCellSignalStrengthLte);
    return NOERROR;
}

ECode CCellInfoLte::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 super, clte, csslte;
    CellInfo::GetHashCode(&super);
    IObject::Probe(mCellIdentityLte)->GetHashCode(&clte);
    IObject::Probe(mCellSignalStrengthLte)->GetHashCode(&csslte);
    *hashCode = super + clte + csslte;
    return NOERROR;
}

ECode CCellInfoLte::Equals(
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
    AutoPtr<ICellInfoLte> o = ICellInfoLte::Probe(other);
    if (o != NULL) {
        AutoPtr<ICellIdentityLte> clte;
        o->GetCellIdentity((ICellIdentityLte**)&clte);
        IObject::Probe(mCellIdentityLte)->Equals(clte, &tempRes);
        AutoPtr<ICellSignalStrengthLte> slte;
        o->GetCellSignalStrength((ICellSignalStrengthLte**)&slte);
        IObject::Probe(mCellSignalStrengthLte)->Equals(slte, &tempRes2);
        *result = tempRes && tempRes2;
    }
    else *result = FALSE;
    return NOERROR;
}

ECode CCellInfoLte::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("CellInfoLte:{");
    String strSuper;
    CellInfo::ToString(&strSuper);
    sb.Append(strSuper);
    sb.Append(" ");
    String strclte;
    IObject::Probe(mCellIdentityLte)->ToString(&strclte);
    sb.Append(strclte);
    String strcsslte;
    IObject::Probe(mCellSignalStrengthLte)->ToString(&strcsslte);
    sb.Append(" ");
    sb.Append(strcsslte);
    sb.Append("}");

    *str = sb.ToString();
    return NOERROR;
}

ECode CCellInfoLte::GetCellIdentity(
    /* [out] */ ICellIdentityLte** res)
{
    VALIDATE_NOT_NULL(res);
    *res = mCellIdentityLte;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CCellInfoLte::SetCellIdentity(
    /* [in] */ ICellIdentityLte* cid)
{
    mCellIdentityLte = cid;
    return NOERROR;
}

ECode CCellInfoLte::GetCellSignalStrength(
    /* [out] */ ICellSignalStrengthLte** res)
{
    VALIDATE_NOT_NULL(res);
    *res = mCellSignalStrengthLte;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CCellInfoLte::SetCellSignalStrength(
    /* [in] */ ICellSignalStrengthLte* css)
{
    mCellSignalStrengthLte = css;
    return NOERROR;
}

ECode CCellInfoLte::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    CellInfo::ReadFromParcel(source);
    source->ReadInterfacePtr((Handle32*)&mCellIdentityLte);
    source->ReadInterfacePtr((Handle32*)&mCellSignalStrengthLte);
    return NOERROR;
}

ECode CCellInfoLte::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    CellInfo::WriteToParcel(dest);
    dest->WriteInterfacePtr(mCellIdentityLte);
    dest->WriteInterfacePtr(mCellSignalStrengthLte);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

