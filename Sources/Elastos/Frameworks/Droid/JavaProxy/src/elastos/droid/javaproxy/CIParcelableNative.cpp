
#include "elastos/droid/javaproxy/CIParcelableNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIParcelableNative::TAG("CIParcelableNative");

ECode CIParcelableNative::constructor()
{
    return NOERROR;
}

ECode CIParcelableNative::constructor(
    /* [in] */ const String& pkgPath,
    /* [in] */ ArrayOf<Byte>* obj)
{
    mPkgPath = pkgPath;
    mObject = obj;
    return NOERROR;
}

String CIParcelableNative::GetPackagePath()
{
    return mPkgPath;
}

ECode CIParcelableNative::GetObject(
    /* [out, callee] */ ArrayOf<Byte>** obj)
{
    assert(obj != NULL);
    *obj = mObject;
    if (*obj) (*obj)->AddRef();
    return NOERROR;
}

ECode CIParcelableNative::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mPkgPath);
    source->ReadArrayOf((Handle32*)&mObject);
    return NOERROR;
}

ECode CIParcelableNative::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mPkgPath);
    dest->WriteArrayOf((Handle32)mObject.Get());
    return NOERROR;
}

}
}
}

