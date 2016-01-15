
#ifndef __ELASTOS_DROID_SERVER_PM_DUMPSTATE_H__
#define __ELASTOS_DROID_SERVER_PM_DUMPSTATE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/pm/SharedUserSetting.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class DumpState
{
public:
    DumpState();

    CARAPI_(Boolean) IsDumping(
        /* [in] */ Int32 type);

    CARAPI_(void) SetDump(
        /* [in] */ Int32 type);

    CARAPI_(Boolean) IsOptionEnabled(
        /* [in] */ Int32 option);

    CARAPI_(void) SetOptionEnabled(
        /* [in] */ Int32 option);

    CARAPI_(Boolean) OnTitlePrinted();

    CARAPI_(Boolean) GetTitlePrinted();

    CARAPI_(void) SetTitlePrinted(
        /* [in] */ Boolean enabled);

    CARAPI_(AutoPtr<SharedUserSetting>) GetSharedUser();

    CARAPI_(void) SetSharedUser(
        /* [in] */ SharedUserSetting* user);

public:
    static const Int32 DUMP_LIBS = 1 << 0;

    static const Int32 DUMP_FEATURES = 1 << 1;

    static const Int32 DUMP_RESOLVERS = 1 << 2;

    static const Int32 DUMP_PERMISSIONS = 1 << 3;

    static const Int32 DUMP_PACKAGES = 1 << 4;

    static const Int32 DUMP_SHARED_USERS = 1 << 5;

    static const Int32 DUMP_MESSAGES = 1 << 6;

    static const Int32 DUMP_PROVIDERS = 1 << 7;

    static const Int32 DUMP_VERIFIERS = 1 << 8;

    static const Int32 DUMP_PREFERRED = 1 << 9;

    static const Int32 DUMP_PREFERRED_XML = 1 << 10;

    static const Int32 OPTION_SHOW_FILTERS = 1 << 0;

private:
    Int32 mTypes;

    Int32 mOptions;

    Boolean mTitlePrinted;

    AutoPtr<SharedUserSetting> mSharedUser;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_DUMPSTATE_H__
