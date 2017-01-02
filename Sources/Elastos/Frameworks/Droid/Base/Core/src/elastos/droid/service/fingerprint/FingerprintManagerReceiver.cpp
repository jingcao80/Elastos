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