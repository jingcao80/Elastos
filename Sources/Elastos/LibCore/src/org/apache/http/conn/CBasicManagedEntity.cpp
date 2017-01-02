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

#include "org/apache/http/conn/CBasicManagedEntity.h"
#include "org/apache/http/conn/CEofSensorInputStream.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {

CAR_INTERFACE_IMPL_2(CBasicManagedEntity, HttpEntityWrapper, IConnectionReleaseTrigger, IEofSensorWatcher)

CAR_OBJECT_IMPL(CBasicManagedEntity)

ECode CBasicManagedEntity::IsRepeatable(
    /* [out] */ Boolean* isRepeatable)
{
    VALIDATE_NOT_NULL(isRepeatable)
    *isRepeatable = FALSE;
    return NOERROR;
}

ECode CBasicManagedEntity::GetContent(
    /* [out] */ IInputStream** inputStream)
{
    VALIDATE_NOT_NULL(inputStream)
    AutoPtr<IInputStream> ins;
    mWrappedEntity->GetContent((IInputStream**)&ins);
    AutoPtr<IEofSensorInputStream> eofInputStream;
    CEofSensorInputStream::New(ins, this, (IEofSensorInputStream**)&eofInputStream);
    *inputStream = IInputStream::Probe(eofInputStream);
    REFCOUNT_ADD(*inputStream)
    return NOERROR;
}

ECode CBasicManagedEntity::ConsumeContent()
{
    if (mManagedConn == NULL) return NOERROR;

    // try {
    if (mAttemptReuse) {
        // this will not trigger a callback from EofSensorInputStream
        mWrappedEntity->ConsumeContent();
        mManagedConn->MarkReusable();
    }
    // } finally {
    //     releaseManagedConnection();
    // }
    ReleaseConnection();
    return NOERROR;
}

ECode CBasicManagedEntity::WriteTo(
    /* [in] */ IOutputStream* outstream)
{
    HttpEntityWrapper::WriteTo(outstream);
    return ConsumeContent();
}

ECode CBasicManagedEntity::ReleaseConnection()
{
    return ConsumeContent();
}

ECode CBasicManagedEntity::AbortConnection()
{
    if (mManagedConn != NULL) {
        // try {
        IConnectionReleaseTrigger::Probe(mManagedConn)->AbortConnection();
        // } finally {
        //     managedConn = null;
        // }
        mManagedConn = NULL;
    }
    return NOERROR;
}

ECode CBasicManagedEntity::EofDetected(
    /* [in] */ IInputStream* wrapped,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    if (mAttemptReuse && (mManagedConn != NULL)) {
        // there may be some cleanup required, such as
        // reading trailers after the response body:
        ICloseable::Probe(wrapped)->Close();
        mManagedConn->MarkReusable();
    }
    // } finally {
    //     releaseManagedConnection();
    // }
    ReleaseManagedConnection();
    *result = FALSE;
    return NOERROR;
}

ECode CBasicManagedEntity::StreamClosed(
    /* [in] */ IInputStream* wrapped,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    if (mAttemptReuse && (mManagedConn != NULL)) {
        // this assumes that closing the stream will
        // consume the remainder of the response body:
        ICloseable::Probe(wrapped)->Close();
        mManagedConn->MarkReusable();
    }
    // } finally {
    //     releaseManagedConnection();
    // }
    ReleaseManagedConnection();
    *result = FALSE;
    return NOERROR;
}

ECode CBasicManagedEntity::StreamAbort(
    /* [in] */ IInputStream* wrapped,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mManagedConn != NULL) {
        IConnectionReleaseTrigger::Probe(mManagedConn)->AbortConnection();
    }
    *result = FALSE;
    return NOERROR;
}

ECode CBasicManagedEntity::ReleaseManagedConnection()
{
    if (mManagedConn != NULL) {
        // try {
        IConnectionReleaseTrigger::Probe(mManagedConn)->AbortConnection();
        // } finally {
        //     managedConn = null;
        // }
        mManagedConn = NULL;
    }
    return NOERROR;
}

ECode CBasicManagedEntity::constructor(
    /* [in] */ IHttpEntity* entity,
    /* [in] */ IManagedClientConnection* conn,
    /* [in] */ Boolean reuse)
{
    Init(entity);

    if (conn == NULL) {
        Logger::E("CBasicManagedEntity", "Connection may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mManagedConn = conn;
    mAttemptReuse = reuse;
    return NOERROR;
}

} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org
