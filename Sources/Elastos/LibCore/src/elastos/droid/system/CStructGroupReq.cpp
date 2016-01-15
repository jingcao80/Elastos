
#include "Elastos.CoreLibrary.Net.h"
#include "CStructGroupReq.h"
#include "core/StringUtils.h"

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace System {

CAR_OBJECT_IMPL(CStructGroupReq)

CAR_INTERFACE_IMPL(CStructGroupReq, Object, IStructGroupReq)

ECode CStructGroupReq::constructor(
    /* [in] */ Int32 gr_interface,
    /* [in] */ IInetAddress* gr_group)
{
    mGr_interface = gr_interface;
    mGr_group = gr_group;
    return NOERROR;
}

ECode CStructGroupReq::GetGrInterface(
    /* [out] */ Int32* grInterface)
{
    VALIDATE_NOT_NULL(grInterface)

    *grInterface = mGr_interface;
    return NOERROR;
}

ECode CStructGroupReq::GetGrGroup(
    /* [out] */ IInetAddress** grGroup)
{
    VALIDATE_NOT_NULL(grGroup)

    *grGroup = mGr_group;
    REFCOUNT_ADD(*grGroup)
    return NOERROR;
}

ECode CStructGroupReq::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    String outstr;
    IObject* obj = IObject::Probe(mGr_group);
    obj->ToString(&outstr);
    *str = String("StructGroupReq[gr_interface=")
            + StringUtils::ToString(mGr_interface)
            + String(",gr_group=")
            + outstr
            + String("]");
    return NOERROR;
}

} // namespace System
} // namespace Droid
} // namespace Elastos
