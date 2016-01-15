
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/hardware/CConsumerIrManager.h"
#include "elastos/droid/hardware/CCarrierFrequencyRange.h"
#include "elastos/droid/os/ServiceManager.h"

using Elastos::Droid::Os::ServiceManager;

namespace Elastos {
namespace Droid {
namespace Hardware {

const String CConsumerIrManager::TAG = String("ConsumerIr");

CAR_INTERFACE_IMPL(CConsumerIrManager, Object, IConsumerIrManager)

CAR_OBJECT_IMPL(CConsumerIrManager)

ECode CConsumerIrManager::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(context->GetPackageName(&mPackageName))
    mService = (IIConsumerIrService*)ServiceManager::GetService(IContext::CONSUMER_IR_SERVICE).Get();
    return NOERROR;
}

CARAPI CConsumerIrManager::HasIrEmitter(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    if (mService == NULL) {
        //Log.w(TAG, "no consumer ir service.");
        *value = FALSE;
        return NOERROR;
    }

    // try {
        FAIL_RETURN(mService->HasIrEmitter(value))
    // } catch (RemoteException e) {
    // }
    *value = FALSE;
    return NOERROR;
}

CARAPI CConsumerIrManager::Transmit(
    /* [in] */ Int32 carrierFrequency,
    /* [in] */ ArrayOf<Int32>* pattern)
{
    if (mService == NULL) {
        //Log.w(TAG, "failed to transmit; no consumer ir service.");
        return NOERROR;
    }

    // try {
        FAIL_RETURN(mService->Transmit(mPackageName, carrierFrequency, pattern))
    // } catch (RemoteException e) {
    //     Log.w(TAG, "failed to transmit.", e);
    // }
    return NOERROR;
}

CARAPI CConsumerIrManager::GetCarrierFrequencies(
    /* [out, callee] */ ArrayOf<ICarrierFrequencyRange*>** outarr)
{
    VALIDATE_NOT_NULL(outarr);

    if (mService == NULL) {
        //Log.w(TAG, "no consumer ir service.");
        *outarr = NULL;
        return NOERROR;
    }

    //try {
        AutoPtr<ArrayOf<Int32> > freqs;
        FAIL_RETURN(mService->GetCarrierFrequencies((ArrayOf<Int32>**)&freqs))
        if (freqs->GetLength() % 2 != 0) {
            //Log.w(TAG, "consumer ir service returned an uneven number of frequencies.");
            *outarr = NULL;
            return NOERROR;
        }
        AutoPtr<ArrayOf<ICarrierFrequencyRange*> > range = ArrayOf<ICarrierFrequencyRange*>::Alloc(freqs->GetLength() / 2);

        for (Int32 i = 0; i < freqs->GetLength(); i += 2) {
            CCarrierFrequencyRange::New((*freqs)[i], (*freqs)[i+1],  (ICarrierFrequencyRange**)&(*range)[i / 2]);
        }
        *outarr = range;
        REFCOUNT_ADD(*outarr);
        return NOERROR;
    // } catch (RemoteException e) {
    // }
    *outarr = NULL;
    return NOERROR;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos
