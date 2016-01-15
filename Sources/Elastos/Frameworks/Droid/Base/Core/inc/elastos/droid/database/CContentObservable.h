#ifndef __ELASTOS_DROID_DATABASE_CCONTENTOBSERVABLE_H__
#define __ELASTOS_DROID_DATABASE_CCONTENTOBSERVABLE_H__

#include "_Elastos_Droid_Database_CContentObservable.h"
#include "elastos/droid/database/ContentObservable.h"

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CContentObservable) , public ContentObservable
{
public:
    CAR_OBJECT_DECL()
};

} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_CCONTENTOBSERVABLE_H__
