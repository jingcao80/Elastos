
#include "CISerializableNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CISerializableNative::TAG("CISerializableNative");

ECode CISerializableNative::constructor()
{
    return NOERROR;
}

ECode CISerializableNative::constructor(
    /* [in] */ const String& pkgPath,
    /* [in] */ ArrayOf<Byte>* obj)
{
    mPkgPath = pkgPath;
    mObject = obj;
    return NOERROR;
}

String CISerializableNative::GetPackagePath()
{
    return mPkgPath;
}

ECode CISerializableNative::GetObject(
    /* [out, callee] */ ArrayOf<Byte>** obj)
{
    assert(obj != NULL);
    *obj = mObject;
    if (*obj) (*obj)->AddRef();
    return NOERROR;
}

ECode CISerializableNative::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mPkgPath);
    source->ReadArrayOf((Handle32*)&mObject);
    return NOERROR;
}

ECode CISerializableNative::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mPkgPath);
    dest->WriteArrayOf((Handle32)mObject.Get());
    return NOERROR;
}

}
}
}

