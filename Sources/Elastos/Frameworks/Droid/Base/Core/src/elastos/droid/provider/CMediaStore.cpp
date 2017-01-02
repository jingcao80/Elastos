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

#include "Elastos.Droid.Database.h"
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/provider/CMediaStore.h"
#include "elastos/droid/provider/CMediaStoreImagesMedia.h"
#include "elastos/droid/provider/CMediaStoreVideoMedia.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Media::IMiniThumbFile;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CMediaStore)

CAR_INTERFACE_IMPL(CMediaStore, Singleton, IMediaStore)

const String CMediaStore::TAG("CMediaStore");
const String CMediaStore::CONTENT_AUTHORITY_SLASH = String("content://") + IMediaStore::AUTHORITY + "/";

ECode CMediaStore::GetMediaScannerUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    return helper->Parse(CONTENT_AUTHORITY_SLASH + "none/media_scanner", uri);
}

ECode CMediaStore::GetVersion(
    /* [in] */ IContext* context,
    /* [out] */ String* version)
{
    VALIDATE_NOT_NULL(version);

    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<IUriHelper> helper;
    AutoPtr<IUri> uri;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->Parse(CONTENT_AUTHORITY_SLASH + "none/version", (IUri**)&uri);

    AutoPtr<ICursor> c;
    cr->Query(uri, NULL, String(NULL), NULL, String(NULL), (ICursor**)&c);
    if (c != NULL) {
        // try {
            Boolean b;
            if (c->MoveToFirst(&b),b) {
                c->GetString(0, version);
                return NOERROR;
            }
        // } finally {
            ICloseable::Probe(c)->Close();
        // }
    }
    return NOERROR;
}

} //namespace Provider
} //namespace Droid
} //namespace Elastos
