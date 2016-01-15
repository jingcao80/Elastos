
#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTPASSWD_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTPASSWD_H__

#include "core/Object.h"
#include "_Elastos_Droid_System_CStructPasswd.h"


namespace Elastos {
namespace Droid {
namespace System {

/**
 * Information returned by getpwnam(3) and getpwuid(3). Corresponds to C's
 * {@code struct passwd} from
 * <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pwd.h.html">&lt;pwd.h&gt;</a>
 */
CarClass(CStructPasswd)
    , public Object
    , public IStructPasswd
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetUid(
        /* [out] */ Int32* uid);

    CARAPI GetGid(
        /* [out] */ Int32* gid);

    CARAPI GetDir(
        /* [out] */ String* dir);

    CARAPI GetShell(
        /* [out] */ String* schell);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid,
        /* [in] */ const String& dir,
        /* [in] */ const String& shell);

private:
    String mPw_name;
    Int32 mPw_uid; /* uid_t */
    Int32 mPw_gid; /* gid_t */
    String mPw_dir;
    String mPw_shell;
};

} // namespace System
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEM_CSTRUCTPASSWD_H__
