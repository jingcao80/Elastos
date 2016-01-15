
#ifndef __ELASTOS_DROID_DATABASE_CDATASETOBSERVABLE_H__
#define __ELASTOS_DROID_DATABASE_CDATASETOBSERVABLE_H__

#include "_Elastos_Droid_Database_CDataSetObservable.h"
#include "elastos/droid/database/DataSetObservable.h"

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CDataSetObservable), public DataSetObservable
{
public:
    CAR_OBJECT_DECL()
};

} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_CDATASETOBSERVABLE_H__
