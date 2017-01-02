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

#ifndef __ELASTOSX_SECURITY_AUTH_CSUBJECTHELPER_H__
#define __ELASTOSX_SECURITY_AUTH_CSUBJECTHELPER_H__

#include "_Elastosx_Security_Auth_CSubjectHelper.h"
#include "Singleton.h"

using Elastos::Security::IPrivilegedAction;
using Elastos::Security::IAccessControlContext;
using Elastos::Security::IPrivilegedExceptionAction;
using Elastos::Security::IAccessControlContext;
using Elastos::Core::Singleton;

namespace Elastosx {
namespace Security {
namespace Auth {

CarClass(CSubjectHelper)
    , public Singleton
    , public ISubjectHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Runs the code defined by {@code action} using the permissions granted to
     * the {@code Subject} itself and to the code as well.
     *
     * @param subject
     *            the distinguished {@code Subject}.
     * @param action
     *            the code to be run.
     * @return the {@code Object} returned when running the {@code action}.
     */
    // @SuppressWarnings("unchecked")
    CARAPI DoAs(
        /* [in] */ ISubject *subject,
        /* [in] */ IPrivilegedAction *action,
        /* [out] */ IInterface **obj);

    /**
     * Run the code defined by {@code action} using the permissions granted to
     * the {@code Subject} and to the code itself, additionally providing a more
     * specific context.
     *
     * @param subject
     *            the distinguished {@code Subject}.
     * @param action
     *            the code to be run.
     * @param context
     *            the specific context in which the {@code action} is invoked.
     *            if {@code null} a new {@link AccessControlContext} is
     *            instantiated.
     * @return the {@code Object} returned when running the {@code action}.
     */
    // @SuppressWarnings("unchecked")
    CARAPI DoAsPrivileged(
        /* [in] */ ISubject *subject,
        /* [in] */ IPrivilegedAction *action,
        /* [in] */ IAccessControlContext *context,
        /* [out] */ IInterface **obj);

    /**
     * Runs the code defined by {@code action} using the permissions granted to
     * the subject and to the code itself.
     *
     * @param subject
     *            the distinguished {@code Subject}.
     * @param action
     *            the code to be run.
     * @return the {@code Object} returned when running the {@code action}.
     * @throws PrivilegedActionException
     *             if running the {@code action} throws an exception.
     */
    // @SuppressWarnings("unchecked")
    CARAPI DoAs(
        /* [in] */ ISubject *subject,
        /* [in] */ IPrivilegedExceptionAction *action,
        /* [out] */ IInterface **obj);

    /**
     * Runs the code defined by {@code action} using the permissions granted to
     * the subject and to the code itself, additionally providing a more
     * specific context.
     *
     * @param subject
     *            the distinguished {@code Subject}.
     * @param action
     *            the code to be run.
     * @param context
     *            the specific context in which the {@code action} is invoked.
     *            if {@code null} a new {@link AccessControlContext} is
     *            instantiated.
     * @return the {@code Object} returned when running the {@code action}.
     * @throws PrivilegedActionException
     *             if running the {@code action} throws an exception.
     */
    // @SuppressWarnings("unchecked")
    CARAPI DoAsPrivileged(
        /* [in] */ ISubject *subject,
        /* [in] */ IPrivilegedExceptionAction *action,
        /* [in] */ IAccessControlContext *context,
        /* [out] */ IInterface **obj);

    /**
     * Returns the {@code Subject} that was last associated with the {@code
     * context} provided as argument.
     *
     * @param context
     *            the {@code context} that was associated with the
     *            {@code Subject}.
     * @return the {@code Subject} that was last associated with the {@code
     *         context} provided as argument.
     */
    CARAPI GetSubject(
        /* [in] */ IAccessControlContext *context,
        /* [out] */ ISubject **subject);
};

} // namespace Auth
} // namespace Security
} // namespace Elastosx

#endif // __ELASTOSX_SECURITY_AUTH_CSUBJECTHELPER_H__
