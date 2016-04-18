
#include "elastos/droid/content/pm/ServiceInfo.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(ServiceInfo, ComponentInfo, IServiceInfo, IParcelable)

ServiceInfo::ServiceInfo()
    : mFlags(0)
{}

ServiceInfo::~ServiceInfo()
{}

ECode ServiceInfo::constructor()
{
    return ComponentInfo::constructor();
}

ECode ServiceInfo::constructor(
    /* [in] */ IServiceInfo* orig)
{
    VALIDATE_NOT_NULL(orig)

    FAIL_RETURN(ComponentInfo::constructor(IComponentInfo::Probe(orig)))
    orig->GetPermission(&mPermission);
    orig->GetFlags(&mFlags);
    return NOERROR;
}

ECode ServiceInfo::Dump(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    // super.dumpFront(pw, prefix);
    // pw.println(prefix + "permission=" + permission);
    // pw.println(prefix + "flags=0x" + Integer.toHexString(flags));
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ServiceInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("ServiceInfo{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += " ";
    sb += mName;
    sb += "";
    sb += " exported=";
    sb += mExported;
    *str = sb.ToString();
    return NOERROR;
}

ECode ServiceInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    ComponentInfo::ReadFromParcel(source);
    source->ReadString(&mPermission);
    source->ReadInt32(&mFlags);
    return NOERROR;
}

ECode ServiceInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    ComponentInfo::WriteToParcel(dest);
    dest->WriteString(mPermission);
    dest->WriteInt32(mFlags);
    return NOERROR;
}

ECode ServiceInfo::GetPermission(
    /* [out] */ String* permission)
{
    VALIDATE_NOT_NULL(permission);
    *permission = mPermission;
    return NOERROR;
}

ECode ServiceInfo::SetPermission(
    /* [in] */ const String& permission)
{
    mPermission = permission;
    return NOERROR;
}

ECode ServiceInfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode ServiceInfo::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
