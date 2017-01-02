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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIAHTTPSERVICE_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAHTTPSERVICE_H__

#include "_Elastos_Droid_Media_CMediaHTTPService.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * @hide
 */
CarClass(CMediaHTTPService)
    , public Object
    , public IIMediaHTTPService
    , public IBinder
{
public:
    CMediaHTTPService();

    virtual ~CMediaHTTPService();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI MakeHTTPConnection(
        /* [out] */ IIMediaHTTPConnection** result);

    static CARAPI CreateHttpServiceBinderIfNecessary(
        /* [in] */ const String& path,
        /* [out] */ IBinder** result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG; // = "MediaHTTPService";
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIAHTTPSERVICE_H__
