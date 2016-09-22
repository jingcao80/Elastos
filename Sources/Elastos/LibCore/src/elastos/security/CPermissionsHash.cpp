
#include "CPermissionsHash.h"
#include "CHashTable.h"

using Elastos::Utility::CHashTable;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CPermissionsHash)
CPermissionsHash::CPermissionsHash()
{
    CHashTable::New((IHashTable**)&mPerms);
}

ECode CPermissionsHash::Add(
    /* [in] */ IPermission* permission)
{
    mPerms->Put(permission, permission);
    return NOERROR;
}

ECode CPermissionsHash::GetElements(
    /* [out] */ IEnumeration** permissions)
{
    VALIDATE_NOT_NULL(*permissions);
    return mPerms->GetElements(permissions);
}

ECode CPermissionsHash::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IEnumeration> elements;
    GetElements((IEnumeration**)&elements);
    Boolean has = FALSE;
    while (elements->HasMoreElements(&has), has) {
        AutoPtr<IInterface> obj;
        elements->GetNextElement((IInterface**)&obj);
        Boolean tmp = FALSE;
        IPermission::Probe(obj)->Implies(permission, &tmp);
        if (tmp) {
            *result = TRUE;
            return NOERROR;
        }

        elements = NULL;
        GetElements((IEnumeration**)&elements);
    }

    *result = FALSE;
    return NOERROR;
}

}
}
