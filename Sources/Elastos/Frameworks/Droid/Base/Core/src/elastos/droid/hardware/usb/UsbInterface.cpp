
#include "elastos/droid/hardware/usb/UsbInterface.h"
#include <elastos/core/StringBuffer.h>

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CAR_INTERFACE_IMPL_2(UsbInterface, Object, IUsbInterface, IParcelable);

ECode UsbInterface::constructor()
{
    return NOERROR;
}

ECode UsbInterface::constructor(
    /* [in] */ Int32 id,
    /* [in] */ Int32 alternateSetting,
    /* [in] */ const String& name,
    /* [in] */ Int32 Class,
    /* [in] */ Int32 subClass,
    /* [in] */ Int32 protocol)
{
    mId = id;
    mAlternateSetting = alternateSetting;
    mName = name;
    mClass = Class;
    mSubclass = subClass;
    mProtocol = protocol;

    return NOERROR;
}

ECode UsbInterface::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mId;
    return NOERROR;
}

ECode UsbInterface::GetAlternateSetting(
    /* [out] */ Int32* setting)
{
    VALIDATE_NOT_NULL(setting);

    *setting = mAlternateSetting;
    return NOERROR;
}

ECode UsbInterface::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode UsbInterface::GetInterfaceClass(
    /* [out] */ Int32* cls)
{
    VALIDATE_NOT_NULL(cls);

    *cls = mClass;
    return NOERROR;
}

ECode UsbInterface::GetInterfaceSubclass(
    /* [out] */ Int32* cls)
{
    VALIDATE_NOT_NULL(cls);

    *cls = mSubclass;
    return NOERROR;
}

ECode UsbInterface::GetInterfaceProtocol(
    /* [out] */ Int32* protocol)
{
    VALIDATE_NOT_NULL(protocol);

    *protocol = mProtocol;
    return NOERROR;
}

ECode UsbInterface::GetEndpointCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    *count = mEndpoints->GetLength();
    return NOERROR;
}

ECode UsbInterface::GetEndpoint(
    /* [in] */ Int32 index,
    /* [out] */ IUsbEndpoint** endpoint)
{
    VALIDATE_NOT_NULL(endpoint);

    if (index < 0 || index >= mEndpoints->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    AutoPtr<IUsbEndpoint> tmpEndpoint = IUsbEndpoint::Probe((*mEndpoints)[index]);
    *endpoint = tmpEndpoint;
    REFCOUNT_ADD(*endpoint);

    return NOERROR;
}

ECode UsbInterface::SetEndpoints(
    /* [in] */ ArrayOf<IParcelable*>* endpoints)
{
    mEndpoints = endpoints;
    return NOERROR;
}

ECode UsbInterface::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuffer buf;
    buf += "UsbInterface[mId=";
    buf += mId;

    buf += ",mAlternateSetting=";
    buf += mAlternateSetting;

    buf += ",mName=";
    buf += mName;

    buf += ",mClass=";
    buf += mClass;

    buf += ",mSubclass=";
    buf += mSubclass;

    buf += ",mProtocol=";
    buf += mProtocol;

    buf += ",mEndpoints=[";

    for (Int32 i = 0; i < mEndpoints->GetLength(); i++) {
        buf += "\n";
        String tmp;
        IObject::Probe((*mEndpoints)[i])->ToString(&tmp);
        buf += tmp;
    }

    buf += "]";

    buf.ToString(str);
    return NOERROR;
}

ECode UsbInterface::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadInt32(&mId))
    FAIL_RETURN(source->ReadInt32(&mClass))
    FAIL_RETURN(source->ReadInt32(&mSubclass))
    FAIL_RETURN(source->ReadInt32(&mProtocol))
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&mEndpoints))
    return NOERROR;
}

ECode UsbInterface::WriteToParcel(
    /* [out] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteInt32(mId))
    FAIL_RETURN(dest->WriteInt32(mAlternateSetting))
    FAIL_RETURN(dest->WriteString(mName))
    FAIL_RETURN(dest->WriteInt32(mClass))
    FAIL_RETURN(dest->WriteInt32(mSubclass))
    FAIL_RETURN(dest->WriteInt32(mProtocol))
    FAIL_RETURN(dest->WriteArrayOf((Handle32)mEndpoints.Get()))
    return NOERROR;
}

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
