
#include "NetworkManagementSocketTagger.h"
#include <elastos/utility/logging/Logger.h>
#include <cutils/qtaguid.h>
#include <errno.h>

using Libcore::IO::EIID_ISocketTagger;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {

const String NetworkManagementSocketTagger::TAG("NetworkManagementSocketTagger");
const Boolean NetworkManagementSocketTagger::LOGD = FALSE;

const String NetworkManagementSocketTagger::PROP_QTAGUID_ENABLED("net.qtaguid_enabled");

CAR_INTERFACE_IMPL(NetworkManagementSocketTagger::SocketTaggerInner, ISocketTagger);

ECode NetworkManagementSocketTagger::SocketTaggerInner::Tag(
    /* [in] */ IFileDescriptor* socketDescriptor)
{
    return NOERROR;
}

ECode NetworkManagementSocketTagger::SocketTaggerInner::Untag(
    /* [in] */ IFileDescriptor* socketDescriptor)
{
    return NOERROR;
}

ECode NetworkManagementSocketTagger::SocketTaggerInner::Tag(
    /* [in] */ ISocket* socket)
{
    Boolean ret;
    FAIL_RETURN(socket->IsClosed(&ret))
    if (!ret) {
        AutoPtr<IFileDescriptor> descriptor;
        socket->GetFileDescriptor((IFileDescriptor**)&descriptor);
        return Tag(descriptor);
    }
    return NOERROR;
}

ECode NetworkManagementSocketTagger::SocketTaggerInner::Untag(
    /* [in] */ ISocket* socket)
{
    Boolean ret;
    FAIL_RETURN(socket->IsClosed(&ret))
    if (!ret) {
        AutoPtr<IFileDescriptor> descriptor;
        FAIL_RETURN(socket->GetFileDescriptor((IFileDescriptor**)&descriptor))
        return Untag(descriptor);
    }
    return NOERROR;
}

AutoPtr<ISocketTagger> NetworkManagementSocketTagger::sTagger = new SocketTaggerInner();

CAR_INTERFACE_IMPL(NetworkManagementSocketTagger, ISocketTagger);

ECode NetworkManagementSocketTagger::Install()
{
    AutoPtr<ISocketTagger> tagger = new NetworkManagementSocketTagger();
    if (tagger == NULL) {
        // throw new NullPointerException("tagger == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    NetworkManagementSocketTagger::sTagger = tagger;
    return NOERROR;
}

void NetworkManagementSocketTagger::SetThreadSocketStatsTag(
    /* [in] */ Int32 tag)
{
    // TODO:
    // threadSocketTags.get().statsTag = tag;
}

Int32 NetworkManagementSocketTagger::GetThreadSocketStatsTag()
{
    // TODO:
    // return threadSocketTags.get().statsTag;
}

void NetworkManagementSocketTagger::SetThreadSocketStatsUid(
    /* [in] */ Int32 uid)
{
    // TODO:
    // threadSocketTags.get().statsUid = uid;
}

// @Override
ECode NetworkManagementSocketTagger::Tag(
    /* [in] */ IFileDescriptor* socketDescriptor)
{
    // TODO:
    // final SocketTags options = threadSocketTags.get();
    // if (LOGD) {
    //     Log.d(TAG, "tagSocket(" + fd.getInt$() + ") with statsTag=0x"
    //             + Integer.toHexString(options.statsTag) + ", statsUid=" + options.statsUid);
    // }
    // // TODO: skip tagging when options would be no-op
    // tagSocketFd(fd, options.statsTag, options.statsUid);
}

void NetworkManagementSocketTagger::TagSocketFd(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 uid)
{
    if (tag == -1 && uid == -1) return;

    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean result;
    if (sp->GetBoolean(PROP_QTAGUID_ENABLED, FALSE, &result), result) {
        const Int32 result = NativeTagSocketFd(fd, tag, uid);
        if (result < 0) {
            // Logger::I(TAG, "tagSocketFd(" + fd.getInt$() + ", "
            //       + tag + ", " +
            //       + uid + ") failed with errno" + result);
        }
    }
}

// @Override
ECode NetworkManagementSocketTagger::Untag(
    /* [in] */ IFileDescriptor* fd)
{
    if (LOGD) {
        // Logger::I(TAG, "untagSocket(" + fd.getInt$() + ")");
    }
    UnTagSocketFd(fd);
}

ECode NetworkManagementSocketTagger::Tag(
    /* [in] */ ISocket* socket)
{
    Boolean ret;
    FAIL_RETURN(socket->IsClosed(&ret))
    if (!ret) {
        AutoPtr<IFileDescriptor> descriptor;
        FAIL_RETURN(socket->GetFileDescriptor((IFileDescriptor**)&descriptor))
        return Tag(descriptor);
    }
    return NOERROR;
}

ECode NetworkManagementSocketTagger::Untag(
    /* [in] */ ISocket* socket)
{
    Boolean ret;
    FAIL_RETURN(socket->IsClosed(&ret))
    if (!ret) {
        AutoPtr<IFileDescriptor> descriptor;
        FAIL_RETURN(socket->GetFileDescriptor((IFileDescriptor**)&descriptor))
        return Untag(descriptor);
    }
    return NOERROR;
}

void NetworkManagementSocketTagger::UnTagSocketFd(
    /* [in] */ IFileDescriptor* fd)
{
    // TODO:
    // final SocketTags options = threadSocketTags.get();
    // if (options.statsTag == -1 && options.statsUid == -1) return;

    // if (SystemProperties.getBoolean(PROP_QTAGUID_ENABLED, false)) {
    //     final int errno = native_untagSocketFd(fd);
    //     if (errno < 0) {
    //         Log.w(TAG, "untagSocket(" + fd.getInt$() + ") failed with errno " + errno);
    //     }
    // }
}

void NetworkManagementSocketTagger::SetKernelCounterSet(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 counterSet)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean result;
    if (sp->GetBoolean(PROP_QTAGUID_ENABLED, FALSE, &result), result) {
        const Int32 result = NativeSetCounterSet(counterSet, uid);
        if (result < 0) {
            Logger::W(TAG, "setKernelCountSet(%d, %d) failed with errno %d", uid, counterSet, result);
        }
    }
}

void NetworkManagementSocketTagger::ResetKernelUidStats(
    /* [in] */ Int32 uid)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean result;
    if (sp->GetBoolean(PROP_QTAGUID_ENABLED, FALSE, &result), result) {
        Int32 result = NativeDeleteTagData(0, uid);
        if (result < 0) {
            Logger::W(TAG, "problem clearing counters for uid %d : errno %d", uid, result);
        }
    }
}

/**
 * Convert {@code /proc/} tag format to {@link Integer}. Assumes incoming
 * format like {@code 0x7fffffff00000000}.
 */
Int32 NetworkManagementSocketTagger::KernelToTag(
    /* [in] */ const String& string)
{
    Int32 length = string.GetLength();
    if (length > 10) {
        // TODO:
        // return Long.decode(string.substring(0, length - 8)).intValue();
    } else {
        return 0;
    }
}

Int32 NetworkManagementSocketTagger::NativeTagSocketFd(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 uid)
{
    Int32 userFd;

    if (FAILED(fd->GetDescriptor(&userFd))) {
      Logger::E(TAG, "Can't get FileDescriptor num");
      return -1;
    }

    Int32 res = qtaguid_tagSocket(userFd, tag, uid);
    if (res < 0) {
      return (Int32)-errno;
    }
    return res;
}

Int32 NetworkManagementSocketTagger::NativeUntagSocketFd(
    /* [in] */ IFileDescriptor* fd)
{
    Int32 userFd;

    if (FAILED(fd->GetDescriptor(&userFd))) {
      Logger::E(TAG, "Can't get FileDescriptor num");
      return -1;
    }

    Int32 res = qtaguid_untagSocket(userFd);
    if (res < 0) {
      return (Int32)-errno;
    }
    return res;
}

Int32 NetworkManagementSocketTagger::NativeSetCounterSet(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 counterSetNum)
{
    Int32 res = qtaguid_setCounterSet(counterSetNum, uid);
    if (res < 0) {
        return (Int32)-errno;
    }
    return res;
}

Int32 NetworkManagementSocketTagger::NativeDeleteTagData(
    /* [in] */ Int32 tag,
    /* [in] */ Int32 uid)
{
    Int32 res = qtaguid_deleteTagData(tag, uid);
    if (res < 0) {
       return (Int32)-errno;
    }
    return res;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
