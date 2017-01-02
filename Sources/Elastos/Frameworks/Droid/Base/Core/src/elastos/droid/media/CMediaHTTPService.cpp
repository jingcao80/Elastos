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

#include "elastos/droid/media/CMediaHTTPService.h"
#include "elastos/droid/media/CMediaHTTPConnection.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaHTTPService::TAG("MediaHTTPService");

CAR_INTERFACE_IMPL_2(CMediaHTTPService, Object, IIMediaHTTPService, IBinder)

CAR_OBJECT_IMPL(CMediaHTTPService)

CMediaHTTPService::CMediaHTTPService()
{
}

CMediaHTTPService::~CMediaHTTPService()
{
}

ECode CMediaHTTPService::constructor()
{
    return NOERROR;
}

ECode CMediaHTTPService::MakeHTTPConnection(
    /* [out] */ IIMediaHTTPConnection** result)
{
    AutoPtr<IIMediaHTTPConnection> conn;
    CMediaHTTPConnection::New((IIMediaHTTPConnection**)&conn);
    *result = IIMediaHTTPConnection::Probe(conn);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaHTTPService::CreateHttpServiceBinderIfNecessary(
    /* [in] */ const String& path,
    /* [out] */ IBinder** result)
{
    if (path.StartWith("http://")
            || path.StartWith("https://")
            || path.StartWith("widevine://")) {
        AutoPtr<IIMediaHTTPService> service;
        CMediaHTTPService::New((IIMediaHTTPService**)&service);
        *result = IBinder::Probe(service);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    *result = NULL;
    return NOERROR;
}

ECode CMediaHTTPService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CMediaHTTPService";
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
