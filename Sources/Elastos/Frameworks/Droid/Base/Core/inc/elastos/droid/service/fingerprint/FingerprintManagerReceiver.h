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

#ifndef __ELASTOS_DROID_SERVICE_FINGERPRINT_FINGERPRINTMANAGERRECEIVER_H__
#define __ELASTOS_DROID_SERVICE_FINGERPRINT_FINGERPRINTMANAGERRECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Fingerprint {

class ECO_PUBLIC FingerprintManagerReceiver
    : public Object
    , public IFingerprintManagerReceiver
{
public:
    CAR_INTERFACE_DECL()

    FingerprintManagerReceiver();

    CARAPI constructor();

    /**
     * Fingerprint enrollment progress update. Enrollment is considered complete if
     * remaining hits 0 without {@link #onError(int)} being called.
     *
     * @param fingerprintId the fingerprint we're currently enrolling
     * @param remaining the number of samples required to complete enrollment. It's up to
     * the hardware to define what each step in enrollment means. Some hardware
     * requires multiple samples of the same part of the finger.  Others require sampling of
     * different parts of the finger.  The enrollment flow can use remaining to
     * mean "step x" of the process or "just need another sample."
     */
    CARAPI OnEnrollResult(
        /* [in] */ Int32 fingerprintId,
        /* [in] */ Int32 remaining);

    /**
     * Fingerprint touch detected, but not processed yet. Clients will use this message to
     * determine a good or bad scan before the fingerprint is processed.  This is meant for the
     * client to provide feedback about the scan or alert the user that recognition is to follow.
     *
     * @param acquiredInfo one of:
     * {@link FingerprintManager#FINGERPRINT_ACQUIRED_GOOD},
     * {@link FingerprintManager#FINGERPRINT_ACQUIRED_PARTIAL},
     * {@link FingerprintManager#FINGERPRINT_ACQUIRED_INSUFFICIENT},
     * {@link FingerprintManager#FINGERPRINT_ACQUIRED_IMAGER_DIRTY},
     * {@link FingerprintManager#FINGERPRINT_ACQUIRED_TOO_SLOW},
     * {@link FingerprintManager#FINGERPRINT_ACQUIRED_TOO_FAST}
     */
    CARAPI OnAcquired(
        /* [in] */ Int32 acquiredInfo);

    /**
     * Fingerprint has been detected and processed.  A non-zero return indicates a valid
     * fingerprint was detected.
     *
     * @param fingerprintId the finger id, or 0 if not recognized.
     */
    CARAPI OnProcessed(
        /* [in] */ Int32 fingerprintId);

    /**
     * An error was detected during scan or enrollment.  One of
     * {@link FingerprintManager#FINGERPRINT_ERROR_HW_UNAVAILABLE},
     * {@link FingerprintManager#FINGERPRINT_ERROR_UNABLE_TO_PROCESS} or
     * {@link FingerprintManager#FINGERPRINT_ERROR_TIMEOUT}
     * {@link FingerprintManager#FINGERPRINT_ERROR_NO_SPACE}
     *
     * @param error one of the above error codes
     */
    CARAPI OnError(
        /* [in] */ Int32 error);

    /**
     * The given fingerprint template was successfully removed by the driver.
     * See {@link FingerprintManager#remove(int)}
     *
     * @param fingerprintId id of template to remove.
     */
    CARAPI OnRemoved(
        /* [in] */ Int32 fingerprintId);
};

} // namespace Fingerprint
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_FINGERPRINT_FINGERPRINTMANAGERRECEIVER_H__
