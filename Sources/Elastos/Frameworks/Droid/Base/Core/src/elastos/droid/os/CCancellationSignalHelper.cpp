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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CCancellationSignalHelper.h"
#include "elastos/droid/os/CCancellationSignal.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CCancellationSignalHelper, Singleton, ICancellationSignalHelper)

CAR_SINGLETON_IMPL(CCancellationSignalHelper)

ECode CCancellationSignalHelper::CreateTransport(
    /* [out] */ IICancellationSignal** cancellationSignal)
{
    VALIDATE_NOT_NULL(cancellationSignal);
    AutoPtr<IICancellationSignal> transport = CCancellationSignal::CreateTransport();
    *cancellationSignal = transport.Get();
    REFCOUNT_ADD(*cancellationSignal);
    return NOERROR;
}

ECode CCancellationSignalHelper::FromTransport(
    /* [in] */ IICancellationSignal* transport,
    /* [out] */ ICancellationSignal** cancellationSignal)
{
    VALIDATE_NOT_NULL(cancellationSignal)
    AutoPtr<ICancellationSignal> value = CCancellationSignal::FromTransport(transport);
    *cancellationSignal = value;
    REFCOUNT_ADD(*cancellationSignal)
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
