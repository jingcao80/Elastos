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

#ifndef __LIBCORE_IO_SOCKETTAGGER_H__
#define __LIBCORE_IO_SOCKETTAGGER_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "elastos/core/Object.h"

using Libcore::IO::ISocketTagger;
using Libcore::IO::EIID_ISocketTagger;
using Elastos::IO::IFileDescriptor;
using Elastos::Core::Object;
using Elastos::Net::ISocket;

namespace Libcore {
namespace IO {

EXTERN const _ELASTOS ClassID ECLSID_SocketTagger;

class ECO_PUBLIC SocketTagger
    : public Object
    , public ISocketTagger
{
public:
    CAR_OBJECT_DECL()
    CAR_INTERFACE_DECL()

    CARAPI Tag(
        /* [in] */ ISocket* socket);

    CARAPI Untag(
        /* [in] */ ISocket* socket);

    using ISocketTagger::Tag;

    using ISocketTagger::Untag;

    /**
     * Sets this process' socket tagger to {@code tagger}.
     */
    static CARAPI Set(
        /* [in] */ ISocketTagger* tagger);

    /**
     * Returns this process socket tagger.
     */
    static CARAPI Get(
        /* [out] */ ISocketTagger** tagger);

private:
    ECO_LOCAL static AutoPtr<ISocketTagger> sTagger;

    ECO_LOCAL static Object sLock;
};

class SocketTaggerInner
    : public SocketTagger
{
public:
    CARAPI Tag(
        /* [in] */ IFileDescriptor* socketDescriptor);

    CARAPI Untag(
        /* [in] */ IFileDescriptor* socketDescriptor);
};

} // namespace IO
} // namespace Libcore

#endif // __LIBCORE_IO_SOCKETTAGGER_H__
