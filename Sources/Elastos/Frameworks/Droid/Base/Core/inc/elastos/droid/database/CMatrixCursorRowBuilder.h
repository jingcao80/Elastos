#ifndef __ELASTOS_DROID_DATABASE_CMATRIXCURSORROWBUILDER_H__
#define __ELASTOS_DROID_DATABASE_CMATRIXCURSORROWBUILDER_H__

#include "_Elastos_Droid_Database_CMatrixCursorRowBuilder.h"
#include "elastos/droid/database/MatrixCursor.h"

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CMatrixCursorRowBuilder)
    , public MatrixCursor::RowBuilder
{
public:
    CAR_OBJECT_DECL()
};

} //Database
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_DATABASE_CMATRIXCURSORROWBUILDER_H__
