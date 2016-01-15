
#ifndef __ELASTOS_DROID_INTERNAL_APP_CPROCESSDATACOLLECTION_H__
#define __ELASTOS_DROID_INTERNAL_APP_CPROCESSDATACOLLECTION_H__

#include "_Elastos_Droid_Internal_App_CProcessDataCollection.h"
#include "elastos/droid/internal/app/ProcessStats.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CarClass(CProcessDataCollection)
    , public ProcessStats::ProcessDataCollection
{
public:
    CAR_OBJECT_DECL()
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CPROCESSDATACOLLECTION_H__
