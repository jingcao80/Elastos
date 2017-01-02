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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CDUMPUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CDUMPUTILS_H__

#include "_Elastos_Droid_Internal_Utility_CDumpUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Core::Singleton;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CDumpUtils)
    , public Singleton
    , public IDumpUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI DumpAsync(
        /* [in] */ IHandler* handler,
        /* [in] */ IDump* dump,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int64 timeout);
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CDUMPUTILS_H__
