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

#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/providers/downloads/CDownloadDrmHelper.h"
#include <drm/DrmManagerClient.h>

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

const String CDownloadDrmHelper::MIMETYPE_DRM_MESSAGE("application/vnd.oma.drm.message");

const String CDownloadDrmHelper::EXTENSION_DRM_MESSAGE(".dm");

const String CDownloadDrmHelper::EXTENSION_INTERNAL_FWDL(".fl");

CAR_SINGLETON_IMPL(CDownloadDrmHelper)

CAR_INTERFACE_IMPL(CDownloadDrmHelper, Singleton, IDownloadDrmHelper)

ECode CDownloadDrmHelper::IsDrmConvertNeeded(
    /* [in] */ const String& mimetype,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = MIMETYPE_DRM_MESSAGE.Equals(mimetype);
    return NOERROR;
}

ECode CDownloadDrmHelper::ModifyDrmFwLockFileExtension(
    /* [in] */ const String& filename,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String _filename = filename;
    if (_filename != NULL) {
        Int32 extensionIndex = _filename.LastIndexOf(".");
        if (extensionIndex != -1) {
            _filename = _filename.Substring(0, extensionIndex);
        }
        _filename = _filename + EXTENSION_INTERNAL_FWDL;
    }
    *result = _filename;
    return NOERROR;
}

ECode CDownloadDrmHelper::GetOriginalMimeType(
    /* [in] */ IContext* context,
    /* [in] */ IFile* file,
    /* [in] */ const String& currentMime,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    android::DrmManagerClient* client = new android::DrmManagerClient(); // android::DrmManagerClient(context);

    String rawFile;
    file->ToString(&rawFile);
    android::String8 _rawFile = (android::String8)rawFile;
    android::String8 strNull; // = NULL
    if (client->canHandle(_rawFile, strNull)) {
        int id = 0;
        android::String8 res = client->getOriginalMimeType(_rawFile, id);
        // client->release();
        *result = (String)res;
        return NOERROR;
    }
    else {
        // client->release();
        *result = currentMime;
        return NOERROR;
    }
}

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
