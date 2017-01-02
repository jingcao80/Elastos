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

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Net.h"
#include "SocketTagger.h"
#include "AutoLock.h"

using Elastos::IO::IFileDescriptor;

namespace Libcore {
namespace IO {

// {1d7f90b3-c0ca-45d9-af25-63fac0171aa5}
extern const _ELASTOS ClassID ECLSID_SocketTagger = {
    { 0x1d7f90b3, 0xc0ca, 0x45d9, { 0xaf, 0x25, 0x63, 0xfa, 0xc0, 0x17, 0x1a, 0xa5}},
    (char *)c_pElastos_CoreLibraryUunm,
    0x2d2b67d7 };

Object SocketTagger::sLock;

ECode SocketTaggerInner::Tag(
    /* [in] */ IFileDescriptor* socketDescriptor)
{
    return NOERROR;
}

ECode SocketTaggerInner::Untag(
    /* [in] */ IFileDescriptor* socketDescriptor)
{
    return NOERROR;
}

AutoPtr<ISocketTagger> SocketTagger::sTagger = new SocketTaggerInner();

CAR_OBJECT_IMPL(SocketTagger)
CAR_INTERFACE_IMPL(SocketTagger, Object, ISocketTagger)

ECode SocketTagger::Tag(
    /* [in] */ ISocket* socket)
{
    Boolean ret;
    FAIL_RETURN(socket->IsClosed(&ret))
    if (!ret) {
        AutoPtr<IFileDescriptor> descriptor;
        FAIL_RETURN(socket->GetFileDescriptor((IFileDescriptor**)&descriptor))
        return Tag(descriptor.Get());
    }
    return NOERROR;
}

ECode SocketTagger::Untag(
    /* [in] */ ISocket* socket)
{
    Boolean ret;
    FAIL_RETURN(socket->IsClosed(&ret))
    if (!ret) {
        AutoPtr<IFileDescriptor> descriptor;
        FAIL_RETURN(socket->GetFileDescriptor((IFileDescriptor**)&descriptor))
        return Untag(descriptor.Get());
    }
    return NOERROR;
}

ECode SocketTagger::Set(
    /* [in] */ ISocketTagger* tagger)
{
    AutoLock lock(sLock);

    if (tagger == NULL) {
        //throw new NullPointerException("tagger == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    sTagger = tagger;
    return NOERROR;
}

/**
 * Returns this process socket tagger.
 */
ECode SocketTagger::Get(
    /* [out] */ ISocketTagger** tagger)
{
    VALIDATE_NOT_NULL(tagger);

    AutoLock lock(sLock);
    *tagger = sTagger;
    REFCOUNT_ADD(*tagger)
    return NOERROR;
}

} // namespace IO
} // namespace Libcore