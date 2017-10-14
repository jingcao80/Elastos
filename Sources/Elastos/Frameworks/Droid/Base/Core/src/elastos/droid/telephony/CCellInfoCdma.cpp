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

#include "elastos/droid/telephony/CCellIdentityCdma.h"
#include "elastos/droid/telephony/CCellInfoCdma.h"
#include "elastos/droid/telephony/CCellSignalStrengthCdma.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CCellInfoCdma::TAG("CellInfoCdma");
const Boolean CCellInfoCdma::DBG = FALSE;

CAR_INTERFACE_IMPL(CCellInfoCdma, CellInfo, ICellInfoCdma)

CAR_OBJECT_IMPL(CCellInfoCdma)

CCellInfoCdma::CCellInfoCdma()
{
}

CCellInfoCdma::~CCellInfoCdma()
{
}

ECode CCellInfoCdma::constructor()
{
    CellInfo::constructor();
    CCellIdentityCdma::New((ICellIdentityCdma**)&mCellIdentityCdma);
    CCellSignalStrengthCdma::New((ICellSignalStrengthCdma**)&mCellSignalStrengthCdma);
    return NOERROR;
}

ECode CCellInfoCdma::constructor(
    /* [in] */ ICellInfoCdma* ci)
{
    CellInfo::constructor(ICellInfo::Probe(ci));
    AutoPtr<ICellIdentityCdma> ccdma;
    AutoPtr<ICellSignalStrengthCdma> csscdma;
    ci->GetCellIdentity((ICellIdentityCdma**)&ccdma);
    ci->GetCellSignalStrength((ICellSignalStrengthCdma**)&csscdma);
    ccdma->Copy((ICellIdentityCdma**)&mCellIdentityCdma);
    csscdma->Copy((ICellSignalStrengthCdma**)&mCellSignalStrengthCdma);
    return NOERROR;
}

ECode CCellInfoCdma::GetCellIdentity(
    /* [out] */ ICellIdentityCdma** cid)
{
    VALIDATE_NOT_NULL(cid);
    *cid = mCellIdentityCdma;
    REFCOUNT_ADD(*cid);
    return NOERROR;
}

ECode CCellInfoCdma::SetCellIdentity(
    /* [in] */ ICellIdentityCdma* cid)
{
    mCellIdentityCdma = cid;
    return NOERROR;
}

ECode CCellInfoCdma::GetCellSignalStrength(
    /* [out] */ ICellSignalStrengthCdma** css)
{
    VALIDATE_NOT_NULL(css);
    *css = mCellSignalStrengthCdma;
    REFCOUNT_ADD(*css);
    return NOERROR;
}

ECode CCellInfoCdma::SetCellSignalStrength(
    /* [in] */ ICellSignalStrengthCdma* css)
{
    mCellSignalStrengthCdma = css;
    return NOERROR;
}

ECode CCellInfoCdma::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 super, ccdma, csscdma;
    CellInfo::GetHashCode(&super);
    IObject::Probe(mCellIdentityCdma)->GetHashCode(&ccdma);
    IObject::Probe(mCellSignalStrengthCdma)->GetHashCode(&csscdma);
    *hashCode = super + ccdma + csscdma;
    return NOERROR;
}

ECode CCellInfoCdma::Equals(
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
    AutoPtr<ICellInfoCdma> o = ICellInfoCdma::Probe(other);
    if (o != NULL) {
        AutoPtr<ICellIdentityCdma> ccdma;
        o->GetCellIdentity((ICellIdentityCdma**)&ccdma);
        IObject::Probe(mCellIdentityCdma)->Equals((IInterface*)ccdma, &tempRes);
        AutoPtr<ICellSignalStrengthCdma> csscdma;
        o->GetCellSignalStrength((ICellSignalStrengthCdma**)&csscdma);
        IObject::Probe(mCellSignalStrengthCdma)->Equals((IInterface*)csscdma, &tempRes2);
        *result = tempRes && tempRes2;
    }
    else *result = FALSE;
    return NOERROR;
}

ECode CCellInfoCdma::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("CellInfoCdma:{");
    String strSuper;
    CellInfo::ToString(&strSuper);
    sb.Append(strSuper);
    sb.Append(", ");
    String strccdma;
    IObject::Probe(mCellIdentityCdma)->ToString(&strccdma);
    sb.Append(strccdma);
    String strcsscdma;
    IObject::Probe(mCellSignalStrengthCdma)->ToString(&strcsscdma);
    sb.Append(", ");
    sb.Append(strcsscdma);
    sb.Append("}");

    *str = sb.ToString();
    return NOERROR;
}

ECode CCellInfoCdma::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    CellInfo::ReadFromParcel(source);
    source->ReadInterfacePtr((Handle32*)&mCellIdentityCdma);
    source->ReadInterfacePtr((Handle32*)&mCellSignalStrengthCdma);
    return NOERROR;
}

ECode CCellInfoCdma::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    CellInfo::WriteToParcel(dest);
    dest->WriteInterfacePtr(mCellIdentityCdma);
    dest->WriteInterfacePtr(mCellSignalStrengthCdma);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
