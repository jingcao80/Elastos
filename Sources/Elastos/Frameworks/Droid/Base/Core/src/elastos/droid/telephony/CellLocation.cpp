
#include "CellLocation.h"
#include "CTelephonyManager.h"
#include "CTelephonyManagerHelper.h"
#include "CCdmaCellLocation.h"
#include "CGsmCellLocation.h"
#include "elastos/droid/os/ServiceManager.h"

using Elastos::Droid::Internal::Telephony::IITelephony;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Os::ServiceManager;

namespace Elastos {
namespace Droid {
namespace Telephony {

ECode CellLocation::FillInNotifierBundle(
    /* [in] */ IBundle* bundle)
{
    return NOERROR;
}

ECode CellLocation::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return NOERROR;
}

ECode CellLocation::RequestLocationUpdate()
{
    //try {
    AutoPtr<IITelephony> phone = IITelephony::Probe(ServiceManager::GetService(String("phone")));
    if (phone != NULL) {
        Boolean res;
        phone->UpdateServiceLocation(&res);
    }
    /*} catch (RemoteException ex) {
        // ignore it
    }*/

    return NOERROR;
}

ECode CellLocation::NewFromBundle(
    /* [in] */ IBundle* bundle,
    /* [out] */ ICellLocation** cl)
{
    VALIDATE_NOT_NULL(cl);
    // TelephonyManager.getDefault().getCurrentPhoneType() handles the case when
    // ITelephony interface is not up yet.
    AutoPtr<ITelephonyManagerHelper> helper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
    AutoPtr<ITelephonyManager> tm;
    helper->GetDefault((ITelephonyManager**)&tm);
    Int32 currentPhoneType;
    tm->GetCurrentPhoneType(&currentPhoneType);
    switch(currentPhoneType) {
    case IPhoneConstants::PHONE_TYPE_CDMA:
        {
            AutoPtr<ICdmaCellLocation> cc;
            CCdmaCellLocation::New(bundle, (ICdmaCellLocation**)&cc);
            *cl = ICellLocation::Probe(cc);
            REFCOUNT_ADD(*cl);
            return NOERROR;
        }
    case IPhoneConstants::PHONE_TYPE_GSM:
        {
            AutoPtr<IGsmCellLocation> gc;
            CGsmCellLocation::New(bundle, (IGsmCellLocation**)&gc);
            *cl = ICellLocation::Probe(gc);
            REFCOUNT_ADD(*cl);
            return NOERROR;
        }
    default:
        *cl = NULL;
        return NOERROR;
    }
}

ECode CellLocation::GetEmpty(
    /* [out] */ ICellLocation** cl)
{
    VALIDATE_NOT_NULL(cl);
    // TelephonyManager.getDefault().getCurrentPhoneType() handles the case when
    // ITelephony interface is not up yet.
    AutoPtr<ITelephonyManagerHelper> helper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&helper);
    AutoPtr<ITelephonyManager> tm;
    helper->GetDefault((ITelephonyManager**)&tm);
    Int32 currentPhoneType;
    tm->GetCurrentPhoneType(&currentPhoneType);
    switch(currentPhoneType) {
    case IPhoneConstants::PHONE_TYPE_CDMA:
        {
            AutoPtr<ICdmaCellLocation> cc;
            CCdmaCellLocation::New((ICdmaCellLocation**)&cc);
            *cl = ICellLocation::Probe(cc);
            REFCOUNT_ADD(*cl);
            return NOERROR;
        }
    case IPhoneConstants::PHONE_TYPE_GSM:
        {
            AutoPtr<IGsmCellLocation> gc;
            CGsmCellLocation::New((IGsmCellLocation**)&gc);
            *cl = ICellLocation::Probe(gc);
            REFCOUNT_ADD(*cl);
            return NOERROR;
        }
    default:
        *cl = NULL;
        return NOERROR;
    }

}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

