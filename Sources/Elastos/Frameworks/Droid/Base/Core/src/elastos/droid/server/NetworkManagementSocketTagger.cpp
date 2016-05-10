#include "elastos/droid/server/NetworkManagementSocketTagger.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/os/SystemProperties.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::SystemProperties;
using Elastos::Core::CInteger64;
using Elastos::Core::EIID_IThreadLocal;
using Elastos::Core::INumber;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::CSocketTaggerHelper;
using Libcore::IO::ISocketTagger;
using Libcore::IO::ISocketTaggerHelper;

namespace Elastos {
namespace Droid {
namespace Server {

//=======================================
// NetworkManagementSocketTagger::SocketTags
//=======================================

CAR_INTERFACE_IMPL(NetworkManagementSocketTagger::SocketTags, Object, ISocketTags)

NetworkManagementSocketTagger::SocketTags::SocketTags()
    : mStatsTag(-1)
    , mStatsUid(-1)
{
}

ECode NetworkManagementSocketTagger::SocketTags::GetStatsTag(
    /* [out] */ Int32* statsTag)
{
    VALIDATE_NOT_NULL(statsTag)
    *statsTag = mStatsTag;
    return NOERROR;
}

ECode NetworkManagementSocketTagger::SocketTags::SetStatsTag(
    /* [in] */ Int32 statsTag)
{
    mStatsTag = statsTag;
    return NOERROR;
}

ECode NetworkManagementSocketTagger::SocketTags::GetStatsUid(
    /* [out] */ Int32* statsUid)
{
    VALIDATE_NOT_NULL(statsUid)
    *statsUid = mStatsUid;
    return NOERROR;
}

ECode NetworkManagementSocketTagger::SocketTags::SetStatsUid(
    /* [in] */ Int32 statsUid)
{
    mStatsUid = statsUid;
    return NOERROR;
}

//=======================================
// NetworkManagementSocketTagger::ThreadSocketTags
//=======================================

CAR_INTERFACE_IMPL(NetworkManagementSocketTagger::ThreadSocketTags, Object, IThreadLocal)

NetworkManagementSocketTagger::ThreadSocketTags::ThreadSocketTags()
{
}

ECode NetworkManagementSocketTagger::ThreadSocketTags::InitialValue(
    /* [out] */ IInterface** st)
{
    VALIDATE_NOT_NULL(st)
    AutoPtr<SocketTags> _st = new SocketTags();
    *st = IInterface::Probe((ISocketTags*)_st.Get());
    REFCOUNT_ADD(*st)
    return NOERROR;
}

//=======================================
// NetworkManagementSocketTagger
//=======================================

const String NetworkManagementSocketTagger::TAG("NetworkManagementSocketTagger");
const Boolean NetworkManagementSocketTagger::LOGD = FALSE;

AutoPtr<IThreadLocal> NetworkManagementSocketTagger::sThreadSocketTags;

NetworkManagementSocketTagger::NetworkManagementSocketTagger()
{
    AutoPtr<ThreadSocketTags> tst = new ThreadSocketTags();
    sThreadSocketTags = (IThreadLocal*)tst.Get();
}

ECode NetworkManagementSocketTagger::constructor()
{
    return NOERROR;
}

ECode NetworkManagementSocketTagger::Install()
{
    AutoPtr<ISocketTaggerHelper> sth;
    CSocketTaggerHelper::AcquireSingleton((ISocketTaggerHelper**)&sth);
    AutoPtr<NetworkManagementSocketTagger> nmst = new NetworkManagementSocketTagger();
    sth->Set((ISocketTagger*)(nmst.Get()));
    return NOERROR;
}

ECode NetworkManagementSocketTagger::SetThreadSocketStatsTag(
    /* [in] */ Int32 tag)
{
    AutoPtr<IInterface> obj;
    sThreadSocketTags->Get((IInterface**)&obj);
    AutoPtr<ISocketTags> st = ISocketTags::Probe(obj);
    if (st != NULL)
        st->SetStatsTag(tag);
    return NOERROR;
}

ECode NetworkManagementSocketTagger::GetThreadSocketStatsTag(
    /* [out] */ Int32* tag)
{
    VALIDATE_NOT_NULL(tag)
    AutoPtr<IInterface> obj;
    sThreadSocketTags->Get((IInterface**)&obj);
    AutoPtr<ISocketTags> st = ISocketTags::Probe(obj);
    if (st != NULL)
        st->GetStatsTag(tag);
    return NOERROR;
}

ECode NetworkManagementSocketTagger::SetThreadSocketStatsUid(
    /* [in] */ Int32 uid)
{
    AutoPtr<IInterface> obj;
    sThreadSocketTags->Get((IInterface**)&obj);
    AutoPtr<ISocketTags> st = ISocketTags::Probe(obj);
    if (st != NULL)
        st->SetStatsUid(uid);
    return NOERROR;
}

ECode NetworkManagementSocketTagger::Tag(
    /* [in] */ IFileDescriptor* fd)
{
    AutoPtr<IInterface> obj;
    sThreadSocketTags->Get((IInterface**)&obj);
    const AutoPtr<ISocketTags> options = ISocketTags::Probe(obj);
    if (options == NULL) {
        assert(0&& "should not be null");
        return NOERROR;
    }
    Int32 statsTag, statsUid;
    options->GetStatsTag(&statsTag);
    options->GetStatsUid(&statsUid);
    if (LOGD) {
        Int32 descriptor;
        fd->GetInt(&descriptor);
        Logger::D(TAG, "tagSocket(%d) with statsTag=0x%s, statsUid=%d",
            descriptor, (StringUtils::ToHexString(statsTag)).string(), statsUid);
    }
    // TODO: skip tagging when options would be no-op
    return TagSocketFd(fd, statsTag, statsUid);
}

ECode NetworkManagementSocketTagger::TagSocketFd(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 uid)
{
    if (tag == -1 && uid == -1) return E_NULL_POINTER_EXCEPTION;

    Boolean result;
    SystemProperties::GetBoolean(
        INetworkManagementSocketTagger::PROP_QTAGUID_ENABLED, FALSE, &result);
    if (result) {
        const Int32 errorno = native_tagSocketFd(fd, tag, uid);
        if (errorno < 0) {
            Int32 descriptor;
            fd->GetInt(&descriptor);
            Logger::I(TAG, "tagSocketFd(%d, %d, %d) failed with errorno%d",
                descriptor, tag, uid, errorno);
        }
    }
    return NOERROR;
}

ECode NetworkManagementSocketTagger::Untag(
    /* [in] */ IFileDescriptor* fd)
{
    if (LOGD) {
        Int32 descriptor;
        fd->GetInt(&descriptor);
        Logger::I(TAG, "untagSocket(%d)", descriptor);
    }
    return UnTagSocketFd(fd);
}

ECode NetworkManagementSocketTagger::UnTagSocketFd(
    /* [in] */ IFileDescriptor* fd)
{
    AutoPtr<IInterface> obj;
    sThreadSocketTags->Get((IInterface**)&obj);
    const AutoPtr<ISocketTags> options = ISocketTags::Probe(obj);
    if (options == NULL) {
        assert(0&& "should not be null");
        return NOERROR;
    }
    Int32 statsTag, statsUid;
    options->GetStatsTag(&statsTag);
    options->GetStatsUid(&statsUid);
    if (statsTag == -1 && statsUid == -1) return E_NULL_POINTER_EXCEPTION;

    Boolean result;
    SystemProperties::GetBoolean(
        INetworkManagementSocketTagger::PROP_QTAGUID_ENABLED, FALSE, &result);
    if (result) {
        const Int32 errorno = native_untagSocketFd(fd);
        if (errorno < 0) {
            Int32 descriptor;
            fd->GetInt(&descriptor);
            Logger::W(TAG, "untagSocket(%d) failed with errorno %d",descriptor, errorno);
        }
    }
    return NOERROR;
}

ECode NetworkManagementSocketTagger::SetKernelCounterSet(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 counterSet)
{
    Boolean result;
    SystemProperties::GetBoolean(
        INetworkManagementSocketTagger::PROP_QTAGUID_ENABLED, FALSE, &result);
    if (result) {
        const Int32 errorno = native_setCounterSet(counterSet, uid);
        if (errorno < 0) {
            Logger::W(TAG, "setKernelCountSet(%d, %d) failed with errorno %d", uid, counterSet, errorno);
        }
    }
    return NOERROR;
}

ECode NetworkManagementSocketTagger::ResetKernelUidStats(
    /* [in] */ Int32 uid)
{
    Boolean result;
    SystemProperties::GetBoolean(
        INetworkManagementSocketTagger::PROP_QTAGUID_ENABLED, FALSE, &result);
    if (result) {
        Int32 errorno = native_deleteTagData(0, uid);
        if (errorno < 0) {
            Slogger::W(TAG, "problem clearing counters for uid %d : errorno %d", uid, errorno);
        }
    }
    return NOERROR;
}

ECode NetworkManagementSocketTagger::KernelToTag(
    /* [in] */ const String& string,
    /* [out] */ Int32* tag)
{
    VALIDATE_NOT_NULL(tag)
    Int32 length = string.GetLength();
    if (length > 10) {
        Int64 value = StringUtils::ParseInt64(string.Substring(0, length - 8));
        AutoPtr<INumber> number;
        CInteger64::New(value, (INumber**)&number);
        number->Int32Value(tag);
    } else {
        *tag = 0;
    }
    return NOERROR;
}

Int32 NetworkManagementSocketTagger::native_tagSocketFd(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 uid)
{
    assert(0);
    // int userFd = jniGetFDFromFileDescriptor(env, fileDescriptor);

    // if (env->ExceptionOccurred() != NULL) {
    //     Logger::E(TAG, "Can't get FileDescriptor num");
    //     return (jint)-1;
    // }

    // int res = qtaguid_tagSocket(userFd, tagNum, uid);
    // if (res < 0) {
    //     return (jint)-errno;
    // }
    // return (jint)res;
    //TODO
    return 0;
}

Int32 NetworkManagementSocketTagger::native_untagSocketFd(
    /* [in] */ IFileDescriptor* fd)
{
    assert(0);
    // int userFd = jniGetFDFromFileDescriptor(env, fileDescriptor);

    // if (env->ExceptionOccurred() != NULL) {
    //     ALOGE("Can't get FileDescriptor num");
    //     return (jint)-1;
    // }

    // int res = qtaguid_untagSocket(userFd);
    // if (res < 0) {
    //     return (jint)-errno;
    // }
    // return (jint)res;
    //TODO
    return 0;
}

Int32 NetworkManagementSocketTagger::native_setCounterSet(
    /* [in] */ Int32 tag,
    /* [in] */ Int32 uid)
{
    assert(0);
    // int res = qtaguid_setCounterSet(setNum, uid);
    // if (res < 0) {
    //     return (jint)-errno;
    // }
    // return (jint)res;
    //TODO
    return 0;
}

Int32 NetworkManagementSocketTagger::native_deleteTagData(
    /* [in] */ Int32 tag,
    /* [in] */ Int32 uid)
{
    assert(0);
    // int res = qtaguid_deleteTagData(tagNum, uid);
    // if (res < 0) {
    //     return (jint)-errno;
    // }
    // return (jint)res;
    //TODO
    return 0;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
