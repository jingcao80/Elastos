
#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTGROUPREQ_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTGROUPREQ_H__

#include "Elastos.CoreLibrary.Net.h"
#include "core/Object.h"
#include "_Elastos_Droid_System_CStructGroupReq.h"

using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Droid {
namespace System {

/**
 * Corresponds to C's {@code struct group_req}.
 */
CarClass(CStructGroupReq)
    , public Object
    , public IStructGroupReq
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 gr_interface,
        /* [in] */ IInetAddress* gr_group);

    CARAPI GetGrInterface(
        /* [out] */ Int32* grInterface);

    CARAPI GetGrGroup(
        /* [out] */ IInetAddress** grGroup);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int32 mGr_interface;
    AutoPtr<IInetAddress> mGr_group;
};

} // namespace System
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEM_CSTRUCTGROUPREQ_H__
