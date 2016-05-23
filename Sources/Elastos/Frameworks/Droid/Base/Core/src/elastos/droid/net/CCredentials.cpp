
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CCredentials.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_OBJECT_IMPL(CCredentials)

CAR_INTERFACE_IMPL(CCredentials, Object, ICredentials)

ECode CCredentials::constructor(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    mPid = pid;
    mUid = uid;
    mGid = gid;
    return NOERROR;
}

ECode CCredentials::GetPid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);

    *pid = mPid;
    return NOERROR;
}

ECode CCredentials::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);

    *uid = mUid;
    return NOERROR;
}

ECode CCredentials::GetGid(
    /* [out] */ Int32* gid)
{
    VALIDATE_NOT_NULL(gid);

    *gid = mGid;
    return NOERROR;
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
