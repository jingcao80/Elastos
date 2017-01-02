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

#ifndef __ELASTOS_DROID_OS_CCANCELLATIONSIGNALHELPER_H__
#define __ELASTOS_DROID_OS_CCANCELLATIONSIGNALHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Os_CCancellationSignalHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CCancellationSignalHelper)
    , public Singleton
    , public ICancellationSignalHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CreateTransport(
        /* [out] */ IICancellationSignal** cancellationSignal);

    CARAPI FromTransport(
        /* [in] */ IICancellationSignal* transport,
        /* [out] */ ICancellationSignal** cancellationSignal);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CCANCELLATIONSIGNALHELPER_H__
