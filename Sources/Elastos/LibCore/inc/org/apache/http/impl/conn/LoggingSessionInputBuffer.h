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

#ifndef __ORG_APACHE_HTTP_IMPL_CONN_LOGGINGSESSIONINPUTBUFFER_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_LOGGINGSESSIONINPUTBUFFER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/impl/conn/Wire.h"

using Org::Apache::Http::IO::ISessionInputBuffer;
using Org::Apache::Http::IO::IHttpTransportMetrics;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

/**
 * Logs all data read to the wire LOG.
 *
 * @author Ortwin Glueck
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class LoggingSessionInputBuffer
    : public Object
    , public ISessionInputBuffer
{
public:
    LoggingSessionInputBuffer(
        /* [in] */ ISessionInputBuffer* in,
        /* [in] */ Wire* wire);

    CAR_INTERFACE_DECL()

    CARAPI IsDataAvailable(
        /* [in] */ Int32 timeout,
        /* [out] */ Boolean* isDataAvailable);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [out] */ Int32* result);

    CARAPI Read(
        /* [out] */ Int32* result);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* b,
        /* [out] */ Int32* result);

    CARAPI ReadLine(
        /* [out] */ String* result);

    CARAPI ReadLine(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [out] */ Int32* result);

    CARAPI GetMetrics(
        /* [out] */ IHttpTransportMetrics** metrics);

private:
    /** Original session input buffer. */
    AutoPtr<ISessionInputBuffer> mIn;

    /** The wire log to use for writing. */
    AutoPtr<Wire> mWire;
};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_LOGGINGSESSIONINPUTBUFFER_H__
