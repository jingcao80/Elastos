#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTGROUPSOURCEREQ_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTGROUPSOURCEREQ_H__

#include "Elastos.CoreLibrary.Net.h"
#include "core/Object.h"
#include "_Elastos_Droid_System_CStructGroupSourceReq.h"

using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Droid {
namespace System {

CarClass(CStructGroupSourceReq)
    , public Object
    , public IStructGroupSourceReq
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 itf,
        /* [in] */ IInetAddress* group,
        /* [in] */ IInetAddress* source);

    CARAPI GetGsrInterface(
        /* [out] */ Int32* itf);

    CARAPI GetGsrGroup(
        /* [out] */ IInetAddress** group);

    CARAPI GetGsrSource(
        /* [out] */ IInetAddress** source);

private:
    Int32 gsr_interface;
    AutoPtr<IInetAddress> gsr_group;
    AutoPtr<IInetAddress> gsr_source;
};

}// namespace System
}// namespace Droid
}// namespace Elastos


#endif
