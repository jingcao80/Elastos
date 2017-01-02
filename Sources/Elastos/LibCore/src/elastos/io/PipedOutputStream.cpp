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

#include "PipedOutputStream.h"
#include "AutoLock.h"
#include "PipedInputStream.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::ISynchronize;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(PipedOutputStream, OutputStream, IPipedOutputStream)

ECode PipedOutputStream::constructor()
{
    return NOERROR;
}

ECode PipedOutputStream::constructor(
    /* [in] */ IPipedInputStream* target)
{
    VALIDATE_NOT_NULL(target)

    return Connect(target);
}

ECode PipedOutputStream::Close()
{
    // Is the pipe connected?
    AutoPtr<IPipedInputStream> stream = mTarget;
    if (stream != NULL) {
        stream->Done();
        mTarget = NULL;
    }
    return NOERROR;
}

ECode PipedOutputStream::Connect(
    /* [in] */ IPipedInputStream* stream)
{
    if (NULL == stream) {
        return E_NULL_POINTER_EXCEPTION;
    }
    {    AutoLock syncLock(stream);
        if (mTarget != NULL) {
            return E_IO_EXCEPTION;
        }
        Boolean is_connected;
        stream->IsConnected(&is_connected);
        if (is_connected) {
            return E_IO_EXCEPTION;
        }
        stream->EstablishConnection();
        mTarget = stream;
    }
    return NOERROR;
}

ECode PipedOutputStream::Flush()
{
    AutoPtr<IPipedInputStream> stream = mTarget;
    if (NULL == stream) {
        return NOERROR;
    }

    {    AutoLock syncLock(stream);
        FAIL_RETURN(ISynchronize::Probe(stream)->NotifyAll());
    }
    return NOERROR;
}

ECode PipedOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    AutoPtr<IPipedInputStream> stream = mTarget;
    if (NULL == stream) {
        return E_IO_EXCEPTION;
    }
    return ((PipedInputStream*)stream.Get())->Receive(oneByte);
}

} // namespace IO
} // namespace Elastos
