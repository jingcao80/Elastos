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

#ifndef __ELASTOS_CORE_CCLOSEGUARDHELPER_H__
#define __ELASTOS_CORE_CCLOSEGUARDHELPER_H__

#include "_Elastos_Core_CCloseGuardHelper.h"
#include "Singleton.h"

namespace Elastos {
namespace Core {

CarClass(CCloseGuardHelper)
    , public Singleton
    , public ICloseGuardHelper
{
public:

    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns a CloseGuard instance. If CloseGuard is enabled, {@code
     * #open(String)} can be used to set up the instance to warn on
     * failure to close. If CloseGuard is disabled, a non-null no-op
     * instance is returned.
     */
    CARAPI Get(
        /* [out] */ ICloseGuard** guard);

    /**
     * Used to enable or disable CloseGuard. Note that CloseGuard only
     * warns if it is enabled for both allocation and finalization.
     */
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Used to replace default Reporter used to warn of CloseGuard
     * violations. Must be non-null.
     */
    CARAPI SetReporter(
        /* [in] */ ICloseGuardReporter* reporter);

    /**
     * Returns non-null CloseGuard.Reporter.
     */
    CARAPI GetReporter(
        /* [out] */ ICloseGuardReporter** report);
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_CCLOSEGUARDHELPER_H__
