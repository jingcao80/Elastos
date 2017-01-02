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

#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/telephony/CellLocation.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include "elastos/droid/telephony/cdma/CCdmaCellLocation.h"
#include "elastos/droid/telephony/gsm/CGsmCellLocation.h"

using Elastos::Droid::Internal::Telephony::IITelephony;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Telephony::Cdma::CCdmaCellLocation;
using Elastos::Droid::Telephony::Gsm::CGsmCellLocation;
using Elastos::Droid::Telephony::Cdma::ICdmaCellLocation;
using Elastos::Droid::Telephony::Gsm::IGsmCellLocation;
using Elastos::Droid::Os::ServiceManager;

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL(CellLocation, Object, ICellLocation)

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
    AutoPtr<IITelephony> phone = IITelephony::Probe(ServiceManager::GetService(IContext::TELEPHONY_SERVICE));
    if (phone != NULL) {
        phone->UpdateServiceLocation();
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
    }
    return NOERROR;
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
    }
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

