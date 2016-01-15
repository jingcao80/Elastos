
#include "CCellInfoLte.h"
#include "CCellIdentityLte.h"
#include "CCellSignalStrengthLte.h"
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CCellInfoLte::LOG_TAG("CCellInfoLte");
const Boolean CCellInfoLte::DBG = FALSE;

ECode CCellInfoLte::constructor()
{
    CCellIdentityLte::New((ICellIdentityLte**)&mCellIdentityLte);
    CCellSignalStrengthLte::New((ICellSignalStrengthLte**)&mCellSignalStrengthLte);
    return NOERROR;
}

ECode CCellInfoLte::constructor(
    /* [in] */ ICellInfoLte* ci)
{
    AutoPtr<ICellIdentityLte> cLte;
    AutoPtr<ICellSignalStrengthLte> sLte;
    ci->GetCellIdentity((ICellIdentityLte**)&cLte);
    ci->GetCellSignalStrength((ICellSignalStrengthLte**)&sLte);
    cLte->Copy((ICellIdentityLte**)&mCellIdentityLte);
    sLte->Copy((ICellSignalStrengthLte**)&mCellSignalStrengthLte);
    return NOERROR;
}

PInterface CCellInfoLte::Probe(
    /* [in]  */ REIID riid)
{
    return _CCellInfoLte::Probe(riid);
}

ECode CCellInfoLte::IsRegistered(
    /* [out] */ Boolean* registered)
{
    VALIDATE_NOT_NULL(registered);
    return CellInfo::IsRegistered(registered);
}

ECode CCellInfoLte::SetRegisterd(
    /* [in] */ Boolean registered)
{
    return CellInfo::SetRegisterd(registered);
}

ECode CCellInfoLte::GetTimeStamp(
    /* [out] */ Int64* timeStamp)
{
    VALIDATE_NOT_NULL(timeStamp);
    return CellInfo::GetTimeStamp(timeStamp);
}

ECode CCellInfoLte::SetTimeStamp(
    /* [in] */ Int64 timeStamp)
{
    return CellInfo::SetTimeStamp(timeStamp);
}

ECode CCellInfoLte::GetTimeStampType(
    /* [out] */ Int32* timeStampType)
{
    VALIDATE_NOT_NULL(timeStampType);
    return CellInfo::GetTimeStampType(timeStampType);
}

ECode CCellInfoLte::SetTimeStampType(
    /* [in] */ Int32 timeStampType)
{
    return CellInfo::SetTimeStampType(timeStampType);
}

ECode CCellInfoLte::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 super, clte, csslte;
    CellInfo::GetHashCode(&super);
    mCellIdentityLte->GetHashCode(&clte);
    mCellSignalStrengthLte->GetHashCode(&csslte);
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
        mCellIdentityLte->Equals(clte, &tempRes);
        AutoPtr<ICellSignalStrengthLte> slte;
        o->GetCellSignalStrength((ICellSignalStrengthLte**)&slte);
        mCellSignalStrengthLte->Equals(slte, &tempRes2);
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
    sb.Append("CellInfoLte:");
    String strSuper;
    CellInfo::ToString(&strSuper);
    sb.Append(strSuper);
    sb.Append(", ");
    String strclte;
    mCellIdentityLte->ToString(&strclte);
    sb.Append(strclte);
    String strcsslte;
    mCellSignalStrengthLte->ToString(&strcsslte);
    sb.Append(", ");
    sb.Append(strcsslte);
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
    CellInfo::WriteToParcel(dest, TYPE_LTE);
    dest->WriteInterfacePtr(mCellIdentityLte);
    dest->WriteInterfacePtr(mCellSignalStrengthLte);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

