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

#ifndef __ELASTOS_DROID_SERVICE_MEDIA_CMEDIABROWSERSERVICEBROWSERROOT_H__
#define __ELASTOS_DROID_SERVICE_MEDIA_CMEDIABROWSERSERVICEBROWSERROOT_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Service_Media_CMediaBrowserServiceBrowserRoot.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Media {

/**
 * Contains information that the browser service needs to send to the client
 * when first connected.
 */
CarClass(CMediaBrowserServiceBrowserRoot)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CMediaBrowserServiceBrowserRoot();

    /**
     * Constructs a browser root.
     * @param rootId The root id for browsing.
     * @param extras Any extras about the browser service.
     */
    CARAPI constructor(
        /* [in] */ const String& rootId,
        /* [in] */ IBundle * extras);

    /**
     * Gets the root id for browsing.
     */
    CARAPI GetRootId(
        /* [out] */ String * result);

    /**
     * Gets any extras about the brwoser service.
     */
    CARAPI GetExtras(
        /* [out] */ IBundle ** result);

private:
    String mRootId;
    AutoPtr<IBundle> mExtras;
};

} // namespace Media
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_MEDIA_CMEDIABROWSERSERVICEBROWSERROOT_H__
