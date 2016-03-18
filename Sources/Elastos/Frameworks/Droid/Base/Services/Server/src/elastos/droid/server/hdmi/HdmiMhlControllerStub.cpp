
#include "elastos/droid/server/hdmi/HdmiMhlControllerStub.h"
#include <Elastos.Droid.Utility.h>

using Elastos::Droid::Utility::CSparseArray;

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
    /* [in] */ IHdmiControlService* service)
{}

ECode HdmiMhlControllerStub::IsReady(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

        *result = FALSE;
        return NOERROR;
}

ECode HdmiMhlControllerStub::Create(
    /* [in] */ IHdmiControlService* service,
    /* [out] */ HdmiMhlControllerStub** result)
{
    VALIDATE_NOT_NULL(result)

    *result = new HdmiMhlControllerStub(service);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiMhlControllerStub::GetPortInfos(
    /* [out, callee] */ ArrayOf<IHdmiPortInfo*>** result)
{
    *result = EMPTY_PORT_INFO;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiMhlControllerStub::GetLocalDevice(
    /* [in] */ Int32 portId,
    /* [out] */ HdmiMhlLocalDeviceStub** result)
{
    VALIDATE_NOT_NULL(result)

    *result = NULL;
    return NOERROR;
}

ECode HdmiMhlControllerStub::GetLocalDeviceById(
    /* [in] */ Int32 deviceId,
    /* [out] */ HdmiMhlLocalDeviceStub** result)
{
    VALIDATE_NOT_NULL(result)

    *result = NULL;
    return NOERROR;
}

ECode HdmiMhlControllerStub::GetAllLocalDevices(
    /* [out] */ ISparseArray** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mLocalDevices;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiMhlControllerStub::RemoveLocalDevice(
    /* [in] */ Int32 portId,
    /* [out] */ HdmiMhlLocalDeviceStub** result)
{
    VALIDATE_NOT_NULL(result)

    *result = NULL;
    return NOERROR;
}

ECode HdmiMhlControllerStub::AddLocalDevice(
    /* [in] */ HdmiMhlLocalDeviceStub* device,
    /* [out] */ HdmiMhlLocalDeviceStub** result)
{
    VALIDATE_NOT_NULL(result)

    *result = NULL;
    return NOERROR;
}

ECode HdmiMhlControllerStub::ClearAllLocalDevices()
{
    return NOERROR;
}

ECode HdmiMhlControllerStub::SendVendorCommand(
    /* [in] */ Int32 portId,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Byte>* data)
{
    return NOERROR;
}

ECode HdmiMhlControllerStub::SetOption(
    /* [in] */ Int32 flag,
    /* [in] */ Int32 value)
{
    return NOERROR;
}

ECode HdmiMhlControllerStub::GetMhlVersion(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = INVALID_MHL_VERSION;
    return NOERROR;
}

ECode HdmiMhlControllerStub::GetPeerMhlVersion(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = INVALID_MHL_VERSION;
    return NOERROR;
}

ECode HdmiMhlControllerStub::GetSupportedFeatures(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = NO_SUPPORTED_FEATURES;
    return NOERROR;
}

ECode HdmiMhlControllerStub::GetEcbusDeviceRoles(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = INVALID_DEVICE_ROLES;
    return NOERROR;
}

AutoPtr<ISparseArray> HdmiMhlControllerStub::InitArray()
{
    AutoPtr<ISparseArray> rev;
    CSparseArray::New((ISparseArray**)&rev);
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
