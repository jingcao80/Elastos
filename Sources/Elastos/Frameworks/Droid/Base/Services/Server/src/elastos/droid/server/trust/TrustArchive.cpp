
#include "elastos/droid/server/trust/TrustArchive.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Content.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/net/ReturnOutValue.h>
#include <elastos/droid/os/Build.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/utility/TimeUtils.h>

using Elastos::Core::CBoolean;
using Elastos::Core::CInteger32;
using Elastos::Core::CSystem;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::IO::IWriter;
using Elastos::Utility::CArrayDeque;
using Elastos::Utility::IDeque;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Trust {

//=============================================================================
// TrustArchive::Event
//=============================================================================
TrustArchive::Event::Event(
    /* [in] */ Int32 type,
    /* [in] */ Int32 userId,
    /* [in] */ IComponentName* agent,
    /* [in] */ String message,
    /* [in] */ Int64 duration,
    /* [in] */ Boolean userInitiated,
    /* [in] */ Boolean managingTrust)
    : mType(type)
    , mUserId(userId)
    , mAgent(agent)
    , mElapsedTimestamp(SystemClock::GetElapsedRealtime())
    , mMessage(message)
    , mDuration(duration)
    , mUserInitiated(userInitiated)
    , mManagingTrust(managingTrust)
{}

//=============================================================================
// TrustArchive
//=============================================================================
const Int32 TrustArchive::TYPE_GRANT_TRUST = 0;
const Int32 TrustArchive::TYPE_REVOKE_TRUST = 1;
const Int32 TrustArchive::TYPE_TRUST_TIMEOUT = 2;
const Int32 TrustArchive::TYPE_AGENT_DIED = 3;
const Int32 TrustArchive::TYPE_AGENT_CONNECTED = 4;
const Int32 TrustArchive::TYPE_AGENT_STOPPED = 5;
const Int32 TrustArchive::TYPE_MANAGING_TRUST = 6;
const Int32 TrustArchive::HISTORY_LIMIT = 200;

TrustArchive::TrustArchive()
{
    CArrayDeque::New((IArrayDeque**)&mEvents);
}

ECode TrustArchive::LogGrantTrust(
    /* [in] */ Int32 userId,
    /* [in] */ IComponentName* agent,
    /* [in] */ String message,
    /* [in] */ Int64 duration,
    /* [in] */ Boolean userInitiated)
{
    return AddEvent(new Event(TYPE_GRANT_TRUST, userId, agent, message, duration,
            userInitiated, FALSE));
}

ECode TrustArchive::LogRevokeTrust(
    /* [in] */ Int32 userId,
    /* [in] */ IComponentName* agent)
{
    return AddEvent(new Event(TYPE_REVOKE_TRUST, userId, agent, String(NULL), 0, FALSE, FALSE));
}

ECode TrustArchive::LogTrustTimeout(
    /* [in] */ Int32 userId,
    /* [in] */ IComponentName* agent)
{
    return AddEvent(new Event(TYPE_TRUST_TIMEOUT, userId, agent, String(NULL), 0, FALSE, FALSE));
}

ECode TrustArchive::LogAgentDied(
    /* [in] */ Int32 userId,
    /* [in] */ IComponentName* agent)
{
    return AddEvent(new Event(TYPE_AGENT_DIED, userId, agent, String(NULL), 0, FALSE, FALSE));
}

ECode TrustArchive::LogAgentConnected(
    /* [in] */ Int32 userId,
    /* [in] */ IComponentName* agent)
{
    return AddEvent(new Event(TYPE_AGENT_CONNECTED, userId, agent, String(NULL), 0, FALSE, FALSE));
}

ECode TrustArchive::LogAgentStopped(
    /* [in] */ Int32 userId,
    /* [in] */ IComponentName* agent)
{
    return AddEvent(new Event(TYPE_AGENT_STOPPED, userId, agent, String(NULL), 0, FALSE, FALSE));
}

ECode TrustArchive::LogManagingTrust(
    /* [in] */ Int32 userId,
    /* [in] */ IComponentName* agent,
    /* [in] */ Boolean managing)
{
    return AddEvent(new Event(TYPE_MANAGING_TRUST, userId, agent, String(NULL), 0, FALSE, managing));
}

ECode TrustArchive::AddEvent(
    /* [in] */ Event* e)
{
    if (Ptr(mEvents)->Func(mEvents->GetSize) >= HISTORY_LIMIT) {
        AutoPtr<IInterface> obj;
        IDeque::Probe(mEvents)->RemoveFirst((IInterface**)&obj);
    }
    IDeque::Probe(mEvents)->AddLast(TO_IINTERFACE(e));
    return NOERROR;
}

ECode TrustArchive::Dump(
    /* [in] */ IPrintWriter* writer,
    /* [in] */ Int32 limit,
    /* [in] */ Int32 userId,
    /* [in] */ String linePrefix,
    /* [in] */ Boolean duplicateSimpleNames)
{
    Int32 count = 0;
    AutoPtr<IIterator> iter;
    IDeque::Probe(mEvents)->GetDescendingIterator((IIterator**)&iter);
    while (Ptr(iter)->Func(iter->HasNext) && count < limit) {
        AutoPtr<Event> ev = (Event*) IObject::Probe(Ptr(iter)->Func(iter->GetNext));
        if (userId != IUserHandle::USER_ALL && userId != ev->mUserId) {
            continue;
        }
        writer->Print(linePrefix);
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
        AutoPtr<IInteger32> i32;
        CInteger32::New(count, (IInteger32**)&i32);
        args->Set(0, i32);
        args->Set(1, StringUtils::ParseCharSequence(FormatElapsed(ev->mElapsedTimestamp)));
        args->Set(2, StringUtils::ParseCharSequence(DumpType(ev->mType)));
        writer->Printf(String("#%-2d %s %s: "), args);
        if (userId == IUserHandle::USER_ALL) {
            writer->Print(String("user="));
            writer->Print(ev->mUserId);
            writer->Print(String(", "));
        }
        writer->Print(String("agent="));
        if (duplicateSimpleNames) {
            writer->Print(Ptr(ev->mAgent)->Func(IComponentName::FlattenToShortString));
        } else {
            String s;
            GetSimpleName(ev->mAgent, &s);
            writer->Print(s);
        }
        switch (ev->mType) {
            case TYPE_GRANT_TRUST:
                {
                    String s;
                    FormatDuration(ev->mDuration, &s);
                    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
                    args->Set(0, StringUtils::ParseCharSequence(ev->mMessage));
                    args->Set(1, StringUtils::ParseCharSequence(s));
                    AutoPtr<IInteger32> i32;
                    CInteger32::New(ev->mUserInitiated ? 1 : 0, (IInteger32**)&i32);
                    args->Set(2, i32);
                    writer->Printf(String(", message=\"%s\", duration=%s, initiatedByUser=%d"), args);
                }
                break;
            case TYPE_MANAGING_TRUST:
                {
                    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                    AutoPtr<IBoolean> ib;
                    CBoolean::New(ev->mManagingTrust, (IBoolean**)&ib);
                    args->Set(0, ib);
                    writer->Printf(String(", managingTrust=%s"), args);
                }
                break;
            default:
                break;
        }
        writer->Println();
        count++;
    }
    return NOERROR;
}

ECode TrustArchive::FormatDuration(
    /* [in] */ Int64 duration,
    /* [out] */ String* result)
{
    StringBuilder sb;
    TimeUtils::FormatDuration(duration, sb);
    return sb.ToString(result);
}

String TrustArchive::FormatElapsed(
    /* [in] */ Int64 elapsed)
{
    Int64 delta = elapsed - SystemClock::GetElapsedRealtime();
    AutoPtr<ISystem> helper;
    CSystem::AcquireSingleton((ISystem**)&helper);
    Int64 wallTime = delta + Ptr(helper)->Func(ISystem::GetCurrentTimeMillis);
    return TimeUtils::LogTimeOfDay(wallTime);
}

ECode TrustArchive::GetSimpleName(
    /* [in] */ IComponentName* cn,
    /* [out] */ String* result)
{
    String name;
    cn->GetClassName(&name);
    Int32 idx = name.LastIndexOf('.');
    if (idx < name.GetLength() && idx >= 0) {
        *result = name.Substring(idx + 1);
    } else {
        *result = name;
    }
    return NOERROR;
}

String TrustArchive::DumpType(
    /* [in] */ Int32 type)
{
    switch (type) {
        case TYPE_GRANT_TRUST:
            return String("GrantTrust");
        case TYPE_REVOKE_TRUST:
            return String("RevokeTrust");
        case TYPE_TRUST_TIMEOUT:
            return String("TrustTimeout");
        case TYPE_AGENT_DIED:
            return String("AgentDied");
        case TYPE_AGENT_CONNECTED:
            return String("AgentConnected");
        case TYPE_AGENT_STOPPED:
            return String("AgentStopped");
        case TYPE_MANAGING_TRUST:
            return String("ManagingTrust");
        default:
            String rev("Unknown(");
            rev.AppendFormat("%d)", type);
            return rev;
    }
    return String(NULL);
}

} // namespace Trust
} // namespace Server
} // namespace Droid
} // namespace Elastos
