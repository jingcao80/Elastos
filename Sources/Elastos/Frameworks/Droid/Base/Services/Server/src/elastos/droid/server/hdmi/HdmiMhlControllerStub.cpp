
#include "elastos/droid/server/hdmi/HdmiMhlControllerStub.h"
#include <Elastos.Droid.Utility.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiPortInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const AutoPtr<ISparseArray> HdmiMhlControllerStub::mLocalDevices = InitArray();
const AutoPtr<ArrayOf<IHdmiPortInfo*> > HdmiMhlControllerStub::EMPTY_PORT_INFO = ArrayOf<IHdmiPortInfo*>::Alloc(0);
const Int32 HdmiMhlControllerStub::INVALID_MHL_VERSION = 0;
const Int32 HdmiMhlControllerStub::NO_SUPPORTED_FEATURES = 0;
const Int32 HdmiMhlControllerStub::INVALID_DEVICE_ROLES = 0;

HdmiMhlControllerStub::HdmiMhlControllerStub(
    /* [in] */ HdmiControlService* service)
{}

ECode HdmiMhlControllerStub::IsReady(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return FALSE;
#endif
}

ECode HdmiMhlControllerStub::Create(
    /* [in] */ HdmiControlService* service,
    /* [out] */ HdmiMhlControllerStub** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return new HdmiMhlControllerStub(service);
#endif
}

ECode HdmiMhlControllerStub::GetPortInfos(
    /* [out, callee] */ ArrayOf<IHdmiPortInfo*>* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return EMPTY_PORT_INFO;
#endif
}

ECode HdmiMhlControllerStub::GetLocalDevice(
    /* [in] */ Int32 portId,
    /* [out] */ HdmiMhlLocalDeviceStub** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return NULL;
#endif
}

ECode HdmiMhlControllerStub::GetLocalDeviceById(
    /* [in] */ Int32 deviceId,
    /* [out] */ HdmiMhlLocalDeviceStub** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return NULL;
#endif
}

ECode HdmiMhlControllerStub::GetAllLocalDevices(
    /* [out] */ ISparseArray** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mLocalDevices;
#endif
}

ECode HdmiMhlControllerStub::RemoveLocalDevice(
    /* [in] */ Int32 portId,
    /* [out] */ HdmiMhlLocalDeviceStub** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return NULL;
#endif
}

ECode HdmiMhlControllerStub::AddLocalDevice(
    /* [in] */ HdmiMhlLocalDeviceStub* device,
    /* [out] */ HdmiMhlLocalDeviceStub** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return NULL;
#endif
}

ECode HdmiMhlControllerStub::ClearAllLocalDevices()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode HdmiMhlControllerStub::SendVendorCommand(
    /* [in] */ Int32 portId,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Byte>* data)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode HdmiMhlControllerStub::SetOption(
    /* [in] */ Int32 flag,
    /* [in] */ Int32 value)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode HdmiMhlControllerStub::GetMhlVersion(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return INVALID_MHL_VERSION;
#endif
}

ECode HdmiMhlControllerStub::GetPeerMhlVersion(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return INVALID_MHL_VERSION;
#endif
}

ECode HdmiMhlControllerStub::GetSupportedFeatures(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return NO_SUPPORTED_FEATURES;

#endif
}

ECode HdmiMhlControllerStub::GetEcbusDeviceRoles(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return INVALID_DEVICE_ROLES;

#endif
}

AutoPtr<ISparseArray> HdmiMhlControllerStub::InitArray()
{
    AutoPtr<ISparseArray> rev;
#if 0 // TODO: Translate codes below
    = new SparseArray<>()
#endif
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
