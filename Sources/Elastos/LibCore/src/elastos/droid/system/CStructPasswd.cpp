
#include "CStructPasswd.h"

namespace Elastos {
namespace Droid {
namespace System {
CAR_OBJECT_IMPL(CStructPasswd)

CAR_INTERFACE_IMPL(CStructPasswd, Object, IStructPasswd)

ECode CStructPasswd::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mPw_name;
    return NOERROR;
}

ECode CStructPasswd::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);

    *uid = mPw_uid;
    return NOERROR;
}

ECode CStructPasswd::GetGid(
    /* [out] */ Int32* gid)
{
    VALIDATE_NOT_NULL(gid);

    *gid = mPw_gid;
    return NOERROR;
}

ECode CStructPasswd::GetDir(
    /* [out] */ String* dir)
{
    VALIDATE_NOT_NULL(dir);

    *dir = mPw_dir;
    return NOERROR;
}

ECode CStructPasswd::GetShell(
    /* [out] */ String* shell)
{
    VALIDATE_NOT_NULL(shell);

    *shell = mPw_shell;
    return NOERROR;
}

ECode CStructPasswd::constructor(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid,
    /* [in] */ const String& dir,
    /* [in] */ const String& shell)
{
    mPw_name = name;
    mPw_uid = uid;
    mPw_gid = gid;
    mPw_dir = dir;
    mPw_shell = shell;
    return NOERROR;
}

} // namespace System
} // namespace Droid
} // namespace Elastos
