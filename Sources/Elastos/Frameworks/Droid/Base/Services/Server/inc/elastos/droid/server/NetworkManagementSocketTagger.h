
#ifndef __ELASTOS_DROID_SERVER_NETWORKMANAGEMENTSOCKETTAGGER_H__
#define __ELASTOS_DROID_SERVER_NETWORKMANAGEMENTSOCKETTAGGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"

using Elastos::IO::IFileDescriptor;
using Elastos::Net::ISocket;
using Libcore::IO::ISocketTagger;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Assigns tags to sockets for traffic stats.
 */
class NetworkManagementSocketTagger
    : public ElRefBase
    , public ISocketTagger
{
public:
    class SocketTags
    {
    public:
        SocketTags()
            : mStatsTag(-1)
            , mStatsUid(-1)
        {}

        Int32 mStatsTag;
        Int32 mStatsUid;
    };

private:
    class SocketTaggerInner
        : public ElRefBase
        , public ISocketTagger
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI Tag(
            /* [in] */ IFileDescriptor* socketDescriptor);

        CARAPI Untag(
            /* [in] */ IFileDescriptor* socketDescriptor);

        CARAPI Tag(
            /* [in] */ ISocket* socket);

        CARAPI Untag(
            /* [in] */ ISocket* socket);
    };

public:
    CAR_INTERFACE_DECL();

    static CARAPI Install();

    static CARAPI_(void) SetThreadSocketStatsTag(
        /* [in] */ Int32 tag);

    static CARAPI_(Int32) GetThreadSocketStatsTag();

    static CARAPI_(void) SetThreadSocketStatsUid(
        /* [in] */ Int32 uid);

    CARAPI Tag(
        /* [in] */ IFileDescriptor* socketDescriptor);

    CARAPI Untag(
        /* [in] */ IFileDescriptor* fd);

    CARAPI Tag(
        /* [in] */ ISocket* socket);

    CARAPI Untag(
        /* [in] */ ISocket* socket);

    static void SetKernelCounterSet(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 counterSet);

    static void ResetKernelUidStats(
        /* [in] */ Int32 uid);

    static Int32 KernelToTag(
        /* [in] */ const String& string);

private:
    void TagSocketFd(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 tag,
        /* [in] */ Int32 uid);

    void UnTagSocketFd(
        /* [in] */ IFileDescriptor* fd);

    static Int32 NativeTagSocketFd(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 tag,
        /* [in] */ Int32 uid);

    static Int32 NativeUntagSocketFd(
        /* [in] */ IFileDescriptor* fd);

    static Int32 NativeSetCounterSet(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 counterSetNum);

    static Int32 NativeDeleteTagData(
        /* [in] */ Int32 tag,
        /* [in] */ Int32 uid);

    // private static ThreadLocal<SocketTags> threadSocketTags = new ThreadLocal<SocketTags>() {
    //     @Override
    //     protected SocketTags initialValue() {
    //         return new SocketTags();
    //     }
    // };

public:
    /**
     * {@link SystemProperties} key that indicates if {@code qtaguid} bandwidth
     * controls have been enabled.
     */
    // TODO: remove when always enabled, or once socket tagging silently fails.
    static const String PROP_QTAGUID_ENABLED;

private:
    static const String TAG;
    static const Boolean LOGD;

    static AutoPtr<ISocketTagger> sTagger;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NETWORKMANAGEMENTSOCKETTAGGER_H__
