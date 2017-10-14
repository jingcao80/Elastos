//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_NETWORKMANAGEMENTSOCKETTAGGER_H__
#define __ELASTOS_DROID_SERVER_NETWORKMANAGEMENTSOCKETTAGGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Core.Server.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.IO.h"
#include <libcore/io/SocketTagger.h>

using Elastos::Droid::Server::INetworkManagementSocketTagger;
using Elastos::Droid::Server::ISocketTags;
using Elastos::Core::IThreadLocal;
using Elastos::IO::IFileDescriptor;
using Libcore::IO::SocketTagger;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Assigns tags to sockets for traffic stats.
 */
class NetworkManagementSocketTagger
    : public SocketTagger
{
public:
    class SocketTags
        : public Object
        , public ISocketTags
    {
    public:
        CAR_INTERFACE_DECL()

        SocketTags();

        CARAPI GetStatsTag(
            /* [out] */ Int32* statsTag);

        CARAPI SetStatsTag(
            /* [in] */ Int32 statsTag);

        CARAPI GetStatsUid(
            /* [out] */ Int32* statsUid);

        CARAPI SetStatsUid(
            /* [in] */ Int32 statsUid);

    public:
        Int32 mStatsTag;
        Int32 mStatsUid;
    };

private:
    class ThreadSocketTags
        : public Object
        , public IThreadLocal
    {
    public:
        CAR_INTERFACE_DECL()

        ThreadSocketTags();

        //Override
        CARAPI InitialValue(
            /* [out] */ IInterface** st);

        //TODO delete
        /////////////////////remove these after finish ThreadLocal/////////////
        CARAPI Get(
            /* [out] */ IInterface** obj){return NOERROR;}
        CARAPI Set(
            /* [in] */ IInterface* obj){return NOERROR;}
        CARAPI Remove(){return NOERROR;}
        ///////////////////remove these after finish ThreadLocal/////////////
    };

public:
    NetworkManagementSocketTagger();

    CARAPI constructor();

    static CARAPI Install();

    static CARAPI SetThreadSocketStatsTag(
        /* [in] */ Int32 tag);

    static CARAPI GetThreadSocketStatsTag(
        /* [out] */ Int32* tag);

    static CARAPI SetThreadSocketStatsUid(
        /* [in] */ Int32 uid);

    using SocketTagger::Tag;

    // @Override
    CARAPI Tag(
        /* [in] */ IFileDescriptor* fd);

    using SocketTagger::Untag;

    // @Override
    CARAPI Untag(
        /* [in] */ IFileDescriptor* fd);

    static CARAPI SetKernelCounterSet(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 counterSet);

    static CARAPI ResetKernelUidStats(
        /* [in] */ Int32 uid);

    /**
     * Convert {@code /proc/} tag format to {@link Integer}. Assumes incoming
     * format like {@code 0x7fffffff00000000}.
     */
    static CARAPI KernelToTag(
        /* [in] */ const String& string,
        /* [out] */ Int32* tag);

private:
    CARAPI TagSocketFd(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 tag,
        /* [in] */ Int32 uid);

    CARAPI UnTagSocketFd(
        /* [in] */ IFileDescriptor* fd);

    static CARAPI_(Int32) native_tagSocketFd(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 tag,
        /* [in] */ Int32 uid);

    static CARAPI_(Int32) native_untagSocketFd(
        /* [in] */ IFileDescriptor* fd);

    static CARAPI_(Int32) native_setCounterSet(
        /* [in] */ Int32 tag,
        /* [in] */ Int32 uid);

    static CARAPI_(Int32) native_deleteTagData(
        /* [in] */ Int32 tag,
        /* [in] */ Int32 uid);

private:
    const static String TAG;
    const static Boolean LOGD;
    static AutoPtr<IThreadLocal> sThreadSocketTags;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NETWORKMANAGEMENTSOCKETTAGGER_H__