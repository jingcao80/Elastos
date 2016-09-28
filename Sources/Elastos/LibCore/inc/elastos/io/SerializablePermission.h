
#ifndef __ELASTOS_IO_SERIALIZABLEPERMISSION_H__
#define __ELASTOS_IO_SERIALIZABLEPERMISSION_H__

#include "Elastos.CoreLibrary.IO.h"
#include "BasicPermission.h"

using Elastos::Security::BasicPermission;

namespace Elastos {
namespace IO {

/**
 * Legacy security code; do not use.
 */
class SerializablePermission
    : public BasicPermission
{
public:
    CARAPI constructor(
        /* [in] */ const String& permissionName);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);

    // @Override
    CARAPI GetActions(
        /* [out] */ String* result);

    // @Override
    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result);
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_SERIALIZABLEPERMISSION_H__
