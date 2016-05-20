
#ifndef __ELASTOS_SQL_CSQLPERMISSION_H__
#define __ELASTOS_SQL_CSQLPERMISSION_H__

#include "_Elastos_Sql_CSQLPermission.h"
#include "Object.h"
//#include "BasicPermission.h"

//TODO: temp.
//using Elastos::Security::BasicPermission;
using Elastos::Security::IPermission;

namespace Elastos {
namespace Sql {
//TODO need the class BasicPermission.
CarClass(CSQLPermission)
    // , public BasicPermission
    , public Object
    , public ISQLPermission
{
public:
    //TODO: to delete
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);

    CARAPI GetActions(
        /* [out] */ String * str);

    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean * value);
};

} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_CSQLPERMISSION_H__
