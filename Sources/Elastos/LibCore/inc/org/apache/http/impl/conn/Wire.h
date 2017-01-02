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

#ifndef __ORG_APACHE_HTTP_IMPL_CONN_WIRE_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_WIRE_H__

#include "elastos/core/Object.h"

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

/**
 * Logs data to the wire LOG.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class Wire : public Object
{
public:
    // Wire(Log log);

    CARAPI_(Boolean) Enabled();

    CARAPI Output(
        /* [in] */ IInputStream* outstream);

    CARAPI Input(
        /* [in] */ IInputStream* instream);

    CARAPI Output(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI Input(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI Output(
        /* [in] */ ArrayOf<Byte>* b);

    CARAPI Input(
        /* [in] */ ArrayOf<Byte>* b);

    CARAPI Output(
        /* [in] */ Int32 b);

    CARAPI Input(
        /* [in] */ Int32);

    CARAPI Output(
        /* [in] */ const String& s);

    CARAPI Input(
        /* [in] */ const String& s);

private:
    CARAPI_(void) DoWire(
        /* [in] */ const String& header,
        /* [in] */ IInputStream* instream);
};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_WIRE_H__
