
#ifndef __ELASTOS_IO_CFILEPERMISSION_H__
#define __ELASTOS_IO_CFILEPERMISSION_H__

#include "Object.h"
#include "_Elastos_IO_CFilePermission.h"
// #include "Permission.h"

using Elastos::Security::IGuard;
// using Elastos::Security::Permission;

namespace Elastos {
namespace IO {

CarClass(CFilePermission)
    , public Object
    // , public Permission
    , public IPermission //TODO
    , public IGuard
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& path,
        /* [in] */ const String& actions);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI NewPermissionCollection(
        /* [out] */ IPermissionCollection** object);

    CARAPI GetActions(
        /* [out] */ String* actions);

    CARAPI Implies(
        /* [in] */ IPermission* p,
        /* [out] */ Boolean* result);

    CARAPI CheckGuard(
        /* [in] */ IInterface* object);
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CFILEPERMISSION_H__
