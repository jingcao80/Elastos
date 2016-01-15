
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

