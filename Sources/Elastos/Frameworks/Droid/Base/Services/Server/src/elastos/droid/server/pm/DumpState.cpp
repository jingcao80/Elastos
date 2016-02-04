
#include "elastos/droid/server/pm/DumpState.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

const Int32 DumpState::DUMP_LIBS;
const Int32 DumpState::DUMP_FEATURES;
const Int32 DumpState::DUMP_RESOLVERS;
const Int32 DumpState::DUMP_PERMISSIONS;
const Int32 DumpState::DUMP_PACKAGES;
const Int32 DumpState::DUMP_SHARED_USERS;
const Int32 DumpState::DUMP_MESSAGES;
const Int32 DumpState::DUMP_PROVIDERS;
const Int32 DumpState::DUMP_VERIFIERS;
const Int32 DumpState::DUMP_PREFERRED;
const Int32 DumpState::DUMP_PREFERRED_XML;
const Int32 DumpState::OPTION_SHOW_FILTERS;

DumpState::DumpState()
    : mTypes(0)
    , mOptions(0)
    , mTitlePrinted(FALSE)
    , mSharedUser(0)
{}

Boolean DumpState::IsDumping(
    /* [in] */ Int32 type)
{
    if (mTypes == 0 && type != DUMP_PREFERRED_XML) {
        return TRUE;
    }

    return (mTypes & type) != 0;
}

void DumpState::SetDump(
    /* [in] */ Int32 type)
{
    mTypes |= type;
}

Boolean DumpState::IsOptionEnabled(
    /* [in] */ Int32 option)
{
    return (mOptions & option) != 0;
}

void DumpState::SetOptionEnabled(
    /* [in] */ Int32 option)
{
    mOptions |= option;
}

Boolean DumpState::OnTitlePrinted()
{
    Boolean printed = mTitlePrinted;
    mTitlePrinted = TRUE;
    return printed;
}

Boolean DumpState::GetTitlePrinted()
{
    return mTitlePrinted;
}

void DumpState::SetTitlePrinted(
    /* [in] */ Boolean enabled)
{
    mTitlePrinted = enabled;
}

AutoPtr<SharedUserSetting> DumpState::GetSharedUser()
{
    return mSharedUser;
}

void DumpState::SetSharedUser(
    /* [in] */ SharedUserSetting* user)
{
    mSharedUser = user;
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
