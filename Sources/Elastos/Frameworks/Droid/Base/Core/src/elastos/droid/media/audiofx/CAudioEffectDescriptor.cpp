
#include "elastos/droid/media/audiofx/CAudioEffectDescriptor.h"

using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAudioEffectDescriptor::CAudioEffectDescriptor()
    : mConnectMode(String(NULL))
    , mName(String(NULL))
    , mImplementor(String(NULL))
{}

CAR_INTERFACE_IMPL(CAudioEffectDescriptor, Object, IAudioEffectDescriptor)

CAR_OBJECT_IMPL(CAudioEffectDescriptor)

ECode CAudioEffectDescriptor::constructor(
    /* [in] */ const String& type,
    /* [in] */ const String& uuid,
    /* [in] */ const String& connectMode,
    /* [in] */ const String& name,
    /* [in] */ const String& implementor)
{
    mConnectMode = connectMode;
    mName = name;
    mImplementor = implementor;

    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    helper->FromString(type,(IUUID**)&mType);
    helper->FromString(uuid,(IUUID**)&mUuid);
    return NOERROR;
}

ECode CAudioEffectDescriptor::GetType(
    /* [out] */ IUUID** type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    REFCOUNT_ADD(*type);
    return NOERROR;
}

ECode CAudioEffectDescriptor::SetType(
    /* [in] */ IUUID* type)
{
    VALIDATE_NOT_NULL(type);
    mType = type;
    return NOERROR;
}

ECode CAudioEffectDescriptor::GetUuid(
    /* [out] */ IUUID** uuid)
{
    VALIDATE_NOT_NULL(uuid);
    *uuid = mUuid;
    REFCOUNT_ADD(*uuid);
    return NOERROR;
}

ECode CAudioEffectDescriptor::SetUuid(
    /* [in] */ IUUID* uuid)
{
    VALIDATE_NOT_NULL(uuid);
    mUuid = uuid;
    return NOERROR;
}

ECode CAudioEffectDescriptor::GetConnectMode(
    /* [out] */ String* connectMode)
{
    VALIDATE_NOT_NULL(connectMode);
    *connectMode = mConnectMode;
    return NOERROR;
}

ECode CAudioEffectDescriptor::SetConnectMode(
    /* [in] */ const String& connectMode)
{
    mConnectMode = connectMode;
    return NOERROR;
}

ECode CAudioEffectDescriptor::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode CAudioEffectDescriptor::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode CAudioEffectDescriptor::GetImplementor(
    /* [out] */ String* implementor)
{
    VALIDATE_NOT_NULL(implementor);
    *implementor = mImplementor;
    return NOERROR;
}

ECode CAudioEffectDescriptor::SetImplementor(
    /* [in] */ const String& implementor)
{
    mImplementor = implementor;
    return NOERROR;
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
