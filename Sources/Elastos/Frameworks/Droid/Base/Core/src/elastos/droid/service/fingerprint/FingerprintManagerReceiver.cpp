
#include "elastos/droid/service/fingerprint/FingerprintManagerReceiver.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Fingerprint {

CAR_INTERFACE_IMPL(FingerprintManagerReceiver, Object, IFingerprintManagerReceiver)

FingerprintManagerReceiver::FingerprintManagerReceiver()
{
}

ECode FingerprintManagerReceiver::constructor()
{
    return NOERROR;
}

ECode FingerprintManagerReceiver::OnEnrollResult(
    /* [in] */ Int32 fingerprintId,
    /* [in] */ Int32 remaining)
{
    return NOERROR;
}

ECode FingerprintManagerReceiver::OnAcquired(
    /* [in] */ Int32 acquiredInfo)
{
    return NOERROR;
}

ECode FingerprintManagerReceiver::OnProcessed(
    /* [in] */ Int32 fingerprintId)
{
    return NOERROR;
}

ECode FingerprintManagerReceiver::OnError(
    /* [in] */ Int32 error)
{
    return NOERROR;
}

ECode FingerprintManagerReceiver::OnRemoved(
    /* [in] */ Int32 fingerprintId)
{
    return NOERROR;
}

} // namespace Fingerprint
} // namespace Service
} // namepsace Droid
} // namespace Elastos