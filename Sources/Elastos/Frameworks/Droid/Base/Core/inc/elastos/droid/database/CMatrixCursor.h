#ifndef __ELASTOS_DROID_DATABASE_CMATRIXCURSOR_H__
#define __ELASTOS_DROID_DATABASE_CMATRIXCURSOR_H__

#include "_Elastos_Droid_Database_CMatrixCursor.h"
#include "elastos/droid/database/MatrixCursor.h"

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CMatrixCursor), public MatrixCursor
{
public:
    CAR_OBJECT_DECL()
};

} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_CMATRIXCURSOR_H__
