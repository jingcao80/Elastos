
#include "CCellInfoGsm.h"
#include "CCellIdentityGsm.h"
#include "CCellSignalStrengthGsm.h"
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CCellInfoGsm::LOG_TAG("CCellInfoGsm");
const Boolean CCellInfoGsm::DBG = FALSE;

ECode CCellInfoGsm::constructor()
{
    CCellIdentityGsm::New((ICellIdentityGsm**)&mCellIdentityGsm);
    CCellSignalStrengthGsm::New((ICellSignalStrengthGsm**)&mCellSignalStrengthGsm);
    return NOERROR;
}

ECode CCellInfoGsm::constructor(
    /* [in] */ ICellInfoGsm* ci)
{
    AutoPtr<ICellIdentityGsm> cgsm;
    AutoPtr<ICellSignalStrengthGsm> sgsm;
    ci->GetCellIdentity((ICellIdentityGsm**)&cgsm);
    ci->GetCellSignalStrength((ICellSignalStrengthGsm**)&sgsm);
    cgsm->Copy((ICellIdentityGsm**)&mCellIdentityGsm);
    sgsm->Copy((ICellSignalStrengthGsm**)&mCellSignalStrengthGsm);
    return NOERROR;
}

PInterface CCellInfoGsm::Probe(
    /* [in]  */ REIID riid)
{
    return _CCellInfoGsm::Probe(riid);
}

ECode CCellInfoGsm::IsRegistered(
    /* [out] */ Boolean* registered)
{
    VALIDATE_NOT_NULL(registered);
    return CellInfo::IsRegistered(registered);
}

ECode CCellInfoGsm::SetRegisterd(
    /* [in] */ Boolean registered)
{
    return CellInfo::SetRegisterd(registered);
}

ECode CCellInfoGsm::GetTimeStamp(
    /* [out] */ Int64* timeStamp)
{
    VALIDATE_NOT_NULL(timeStamp);
    return CellInfo::GetTimeStamp(timeStamp);
}

ECode CCellInfoGsm::SetTimeStamp(
    /* [in] */ Int64 timeStamp)
{
    return CellInfo::SetTimeStamp(timeStamp);
}

ECode CCellInfoGsm::GetTimeStampType(
    /* [out] */ Int32* timeStampType)
{
    VALIDATE_NOT_NULL(timeStampType);
    return CellInfo::GetTimeStampType(timeStampType);
}

ECode CCellInfoGsm::SetTimeStampType(
    /* [in] */ Int32 timeStampType)
{
    return CellInfo::SetTimeStampType(timeStampType);
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
    mCellIdentityGsm->GetHashCode(&cgsm);
    mCellSignalStrengthGsm->GetHashCode(&cssgsm);
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
        mCellIdentityGsm->Equals(cgsm, &tempRes);
        AutoPtr<ICellSignalStrengthGsm> sgsm;
        o->GetCellSignalStrength((ICellSignalStrengthGsm**)&sgsm);
        mCellSignalStrengthGsm->Equals(sgsm, &tempRes2);
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
    sb.Append("CellInfoGsm:");
    String strSuper;
    CellInfo::ToString(&strSuper);
    sb.Append(strSuper);
    sb.Append(", ");
    String strcGsm;
    mCellIdentityGsm->ToString(&strcGsm);
    sb.Append(strcGsm);
    String strcssGsm;
    mCellSignalStrengthGsm->ToString(&strcssGsm);
    sb.Append(", ");
    sb.Append(strcssGsm);
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
    CellInfo::WriteToParcel(dest, TYPE_GSM);
    dest->WriteInterfacePtr(mCellIdentityGsm);
    dest->WriteInterfacePtr(mCellSignalStrengthGsm);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

