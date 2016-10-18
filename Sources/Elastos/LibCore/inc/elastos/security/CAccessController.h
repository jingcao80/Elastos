
#ifndef __ELASTOS_SECURITY_CACCESSCONTROLLER_H__
#define __ELASTOS_SECURITY_CACCESSCONTROLLER_H__

#include "_Elastos_Security_CAccessController.h"
#include "Singleton.h"

namespace Elastos {
namespace Security {

CarClass(CAccessController)
    , public Singleton
    , public IAccessController
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI DoPrivileged(
        /* [in] */ IPrivilegedAction *action,
        /* [out] */ IInterface **result);

    CARAPI DoPrivileged(
        /* [in] */ IPrivilegedAction *action,
        /* [in] */ IAccessControlContext *context,
        /* [out] */ IInterface **result);

    CARAPI DoPrivileged(
        /* [in] */ IPrivilegedExceptionAction *action,
        /* [out] */ IInterface **result);

    CARAPI DoPrivileged(
        /* [in] */ IPrivilegedExceptionAction *action,
        /* [in] */ IAccessControlContext *context,
        /* [out] */ IInterface **result);

    CARAPI DoPrivilegedWithCombiner(
        /* [in] */ IPrivilegedAction *action,
        /* [out] */ IInterface **result);

    CARAPI DoPrivilegedWithCombiner(
        /* [in] */ IPrivilegedExceptionAction *action,
        /* [out] */ IInterface **result);

    CARAPI CheckPermission(
        /* [in] */ IPermission *permission);

    CARAPI GetContext(
        /* [out] */ IAccessControlContext **context);
};

}
}

#endif // __ELASTOS_SECURITY_CACCESSCONTROLLER_H__
