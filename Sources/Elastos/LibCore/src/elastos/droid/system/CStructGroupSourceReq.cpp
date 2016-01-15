
#include "Elastos.CoreLibrary.Net.h"
#include "CStructGroupSourceReq.h"

namespace Elastos {
namespace Droid {
namespace System {

CAR_OBJECT_IMPL(CStructGroupSourceReq)

CAR_INTERFACE_IMPL(CStructGroupSourceReq, Object, IStructGroupSourceReq)

ECode CStructGroupSourceReq::constructor(
    /* [in] */ Int32 itf,
    /* [in] */ IInetAddress* group,
    /* [in] */ IInetAddress* source)
{
    gsr_interface = itf;
    gsr_group = group;
    gsr_source = source;
    return NOERROR;
}

ECode CStructGroupSourceReq::GetGsrInterface(
    /* [out] */ Int32* itf)
{
    VALIDATE_NOT_NULL(itf)
    *itf = gsr_interface;
    return NOERROR;
}

ECode CStructGroupSourceReq::GetGsrGroup(
    /* [out] */ IInetAddress** group)
{
    VALIDATE_NOT_NULL(group)
    *group = gsr_group;
    REFCOUNT_ADD(*group)
    return NOERROR;
}

ECode CStructGroupSourceReq::GetGsrSource(
    /* [out] */ IInetAddress** source)
{
    VALIDATE_NOT_NULL(source)
    *source = gsr_source;
    REFCOUNT_ADD(*source)
    return NOERROR;
}

}// namespace System
}// namespace Droid
}// namespace Elastos
