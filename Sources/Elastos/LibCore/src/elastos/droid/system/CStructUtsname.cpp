
#include "CStructUtsname.h"

namespace Elastos {
namespace Droid {
namespace System {

CAR_OBJECT_IMPL(CStructUtsname)

CAR_INTERFACE_IMPL(CStructUtsname, Object, IStructUtsname)

ECode CStructUtsname::GetSysname(
    /* [out] */ String* sysname)
{
    VALIDATE_NOT_NULL(sysname);

    *sysname = mSysname;
    return NOERROR;
}

ECode CStructUtsname::GetNodename(
    /* [out] */ String* nodename)
{
    VALIDATE_NOT_NULL(nodename);

    *nodename = mNodename;
    return NOERROR;
}

ECode CStructUtsname::GetRelease(
    /* [out] */ String* release)
{
    VALIDATE_NOT_NULL(release);

    *release = mRelease;
    return NOERROR;
}

ECode CStructUtsname::GetVersion(
    /* [out] */ String* version)
{
    VALIDATE_NOT_NULL(version);

    *version = mVersion;
    return NOERROR;
}

ECode CStructUtsname::GetMachine(
    /* [out] */ String* machine)
{
    VALIDATE_NOT_NULL(machine);

    *machine = mMachine;
    return NOERROR;
}

ECode CStructUtsname::constructor(
    /* [in] */ const String& sysname,
    /* [in] */ const String& nodename,
    /* [in] */ const String& release,
    /* [in] */ const String& version,
    /* [in] */ const String& machine)
{
    mSysname = sysname;
    mNodename = nodename;
    mRelease = release;
    mVersion = version;
    mMachine = machine;
    return NOERROR;
}

} // namespace System
} // namespace Droid
} // namespace Elastos
