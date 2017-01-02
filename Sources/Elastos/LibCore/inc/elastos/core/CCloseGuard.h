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

#ifndef __ELASTOS_CORE_CCLOSEGUARD_H__
#define __ELASTOS_CORE_CCLOSEGUARD_H__

#include "_Elastos_Core_CCloseGuard.h"
#include "Object.h"

namespace Elastos {
namespace Core {

using Elastos::Core::ICloseGuardReporter;

CarClass(CCloseGuard)
    , public Object
    , public ICloseGuard
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()
    /**
     * Default Reporter which reports CloseGuard violations to the log.
     */
    class DefaultReporter
        : public Object
        , public ICloseGuardReporter
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Report (
            /* [in] */ const String& message,
            /* [in] */ IThrowable* allocationSite);
    };

    /**
     * Returns a CloseGuard instance. If CloseGuard is enabled, {@code
     * #open(String)} can be used to set up the instance to warn on
     * failure to close. If CloseGuard is disabled, a non-null no-op
     * instance is returned.
     */
    static CARAPI_(AutoPtr<ICloseGuard>) Get();

    /**
     * Used to enable or disable CloseGuard. Note that CloseGuard only
     * warns if it is enabled for both allocation and finalization.
     */
    static CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Used to replace default Reporter used to warn of CloseGuard
     * violations. Must be non-null.
     */
    static CARAPI SetReporter(
        /* [in] */ ICloseGuardReporter* reporter);

    /**
     * Returns non-null CloseGuard.Reporter.
     */
    static CARAPI_(AutoPtr<ICloseGuardReporter>) GetReporter();

    /**
     * If CloseGuard is enabled, {@code open} initializes the instance
     * with a warning that the caller should have explicitly called the
     * {@code closer} method instead of relying on finalization.
     *
     * @param closer non-null name of explicit termination method
     * @throws NullPointerException if closer is null, regardless of
     * whether or not CloseGuard is enabled
     */
    CARAPI Open(
        /* [in] */ const String& closer);

    /**
     * Marks this CloseGuard instance as closed to avoid warnings on
     * finalization.
     */
    CARAPI Close();

    /**
     * If CloseGuard is enabled, logs a warning if the caller did not
     * properly cleanup by calling an explicit close method
     * before finalization. If CloseGuard is disabled, no action is
     * performed.
     */
    CARAPI WarnIfOpen();

private:
    /**
     * Instance used when CloseGuard is disabled to avoid allocation.
     */
    static const AutoPtr<ICloseGuard> NOOP;

    /**
     * Enabled by default so we can catch issues early in VM startup.
     * Note, however, that Android disables this early in its startup,
     * but enables it with DropBoxing for system apps on debug builds.
     */
    static /*volatile*/ Boolean ENABLED;

    /**
     * Hook for customizing how CloseGuard issues are reported.
     */
    static /*volatile*/ AutoPtr<ICloseGuardReporter> REPORTER;

    AutoPtr<IThrowable> mAllocationSite;

    static const String TAG;
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_CCLOSEGUARD_H__
