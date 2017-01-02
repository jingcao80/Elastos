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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_DOWNLOADDRMHELPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_DOWNLOADDRMHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

class DownloadDrmHelper
{
public:
    /**
      * Checks if the Media Type is a DRM Media Type
      *
      * @param drmManagerClient A DrmManagerClient
      * @param mimetype Media Type to check
      * @return True if the Media Type is DRM else false
      */
    static CARAPI_(Boolean) IsDrmMimeType(
        /* [in] */ IContext* context,
        /* [in] */ const String& mimetype);

    /**
      * Checks if the Media Type needs to be DRM converted
      *
      * @param mimetype Media type of the content
      * @return True if convert is needed else false
      */
    static CARAPI_(Boolean) IsDrmConvertNeeded(
        /* [in] */ const String& mimetype);

    /**
      * Modifies the file extension for a DRM Forward Lock file NOTE: This
      * function shouldn't be called if the file shouldn't be DRM converted
      */
    static CARAPI_(String) ModifyDrmFwLockFileExtension(
        /* [in] */ const String& filename);

    /**
      * Gets the original mime type of DRM protected content.
      *
      * @param context The context
      * @param path Path to the file
      * @param containingMime The current mime type of of the file i.e. the
      *            containing mime type
      * @return The original mime type of the file if DRM protected else the
      *         currentMime
      */
    static CARAPI_(String) GetOriginalMimeType(
        /* [in] */ IContext* context,
        /* [in] */ const String& path,
        /* [in] */ const String& containingMime);

private:
    static const String TAG;
};

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_UTILITY_DOWNLOADDRMHELPER_H__

