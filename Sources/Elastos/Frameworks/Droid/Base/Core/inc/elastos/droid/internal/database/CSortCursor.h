
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CSORTCURSOR_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CSORTCURSOR_H__

#include "_Elastos_Droid_Internal_Database_CSortCursor.h"
#include "elastos/droid/internal/database/SortCursor.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Database {

CarClass(CSortCursor)
    , public SortCursor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Database
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CSORTCURSOR_H__
