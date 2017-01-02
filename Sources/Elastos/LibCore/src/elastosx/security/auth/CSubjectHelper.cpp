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

#include "CSubjectHelper.h"
#include "CSubject.h"

namespace Elastosx {
namespace Security {
namespace Auth {

CAR_SINGLETON_IMPL(CSubjectHelper)

CAR_INTERFACE_IMPL(CSubjectHelper, Singleton, ISubjectHelper)

ECode CSubjectHelper::DoAs(
    /* [in] */ ISubject *subject,
    /* [in] */ IPrivilegedAction *action,
    /* [out] */ IInterface **obj)
{
    return CSubject::DoAs(subject, action, obj);
}

ECode CSubjectHelper::DoAsPrivileged(
    /* [in] */ ISubject *subject,
    /* [in] */ IPrivilegedAction *action,
    /* [in] */ IAccessControlContext *context,
    /* [out] */ IInterface **obj)
{
    return CSubject::DoAsPrivileged(subject, action, context, obj);
}

ECode CSubjectHelper::DoAs(
    /* [in] */ ISubject *subject,
    /* [in] */ IPrivilegedExceptionAction *action,
    /* [out] */ IInterface **obj)
{
    return CSubject::DoAs(subject, action, obj);
}

ECode CSubjectHelper::DoAsPrivileged(
    /* [in] */ ISubject *subject,
    /* [in] */ IPrivilegedExceptionAction *action,
    /* [in] */ IAccessControlContext *context,
    /* [out] */ IInterface **obj)
{
    return CSubject::DoAsPrivileged(subject, action, context, obj);
}

ECode CSubjectHelper::GetSubject(
    /* [in] */ IAccessControlContext *context,
    /* [out] */ ISubject **subject)
{
    return CSubject::GetSubject(context, subject);
}

} // namespace Auth
} // namespace Security
} // namespace Elastosx

