
#include "CTimestamp.h"
#include "StringBuilder.h"
#include "CDate.h"

using Elastos::Core::StringBuilder;
using Elastos::IO::EIID_ISerializable;
using Elastos::Utility::CDate;
using Elastos::Utility::IList;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CTimestamp)
CAR_INTERFACE_IMPL_2(CTimestamp, Object, ITimestamp, ISerializable)
ECode CTimestamp::constructor(
    /* [in] */ IDate* timestamp,
    /* [in] */ ICertPath* signerCertPath)
{
    if (timestamp == NULL) {
        // throw new NullPointerException("timestamp == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (signerCertPath == NULL) {
        // throw new NullPointerException("signerCertPath == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    // Clone timestamp to prevent modifications
    Int64 t = 0;
    timestamp->GetTime(&t);
    CDate::New(t, (IDate**)&mTimestamp);
    mSignerCertPath = signerCertPath;
    return NOERROR;
}

ECode CTimestamp::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (obj == this->Probe(EIID_IInterface)) {
        *result = TRUE;
        return NOERROR;
    }
    if (ITimestamp::Probe(obj)) {
        CTimestamp* that = (CTimestamp*)ITimestamp::Probe(obj);
        Boolean e1 = FALSE, e2 = FALSE;
        IObject::Probe(mTimestamp)->Equals(that->mTimestamp, &e1);
        IObject::Probe(mSignerCertPath)->Equals(that->mSignerCertPath, &e2);
        *result = e1 && e2;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CTimestamp::GetSignerCertPath(
    /* [out] */ ICertPath** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSignerCertPath;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTimestamp::GetTimestamp(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    ICloneable::Probe(mTimestamp)->Clone((IInterface**)&obj);
    *result = IDate::Probe(obj);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTimestamp::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mHash == 0) {
        Int32 h1 = 0, h2 = 0;
        IObject::Probe(mTimestamp)->GetHashCode(&h1);
        IObject::Probe(mSignerCertPath)->GetHashCode(&h2);
        mHash = h1 ^ h2;
    }
    *result = mHash;
    return NOERROR;
}

ECode CTimestamp::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder buf;
    // Dump only the first certificate
    buf.Append("Timestamp [");
    buf.Append(TO_CSTR(mTimestamp));
    buf.Append(" certPath=");
    AutoPtr<IList> list;
    mSignerCertPath->GetCertificates((IList**)&list);
    AutoPtr<IInterface> obj;
    list->Get(0, (IInterface**)&obj);
    buf.Append(TO_CSTR(obj));
    buf.Append("]");
    return buf.ToString(result);
}

}
}
