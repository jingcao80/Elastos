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

#ifndef __ELASTOS_DROID_WEBKIT_CPLUGINDATA_H__
#define __ELASTOS_DROID_WEBKIT_CPLUGINDATA_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Webkit_CPluginData.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::IO::IInputStream;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CPluginData)
    , public Object
    , public IPluginData
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IInputStream* stream,
        /* [in] */ Int64 length,
        /* [in] */ IMap* headers,
        /* [in] */ Int32 code);

    CARAPI GetInputStream(
        /* [out] */ IInputStream** stream);

    CARAPI GetContentLength(
        /* [out] */ Int64* length);

    CARAPI GetHeaders(
        /* [out] */ IMap** headers);

    CARAPI GetStatusCode(
        /* [out] */ Int32* code);

private:
    /**
     * The content stream.
     */
    AutoPtr<IInputStream> mStream;

    /**
     * The content length.
     */
    Int64 mContentLength;

    /**
     * The associated HTTP response headers stored as a map of
     * lowercase header name to [ unmodified header name, header value].
     * TODO: This design was always a hack. Remove (involves updating
     * the Gears C++ side).
     */
    AutoPtr<IMap> mHeaders;

    /**
     * The associated HTTP response code.
     */
    Int32 mStatusCode;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CPLUGINDATA_H__
