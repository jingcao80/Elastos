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

#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/provider/CDownloads.h"
#include "elastos/droid/provider/CDownloadsImpl.h"

using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace Provider {

const String CDownloads::QUERY_WHERE_CLAUSE = IDownloadsImpl::COLUMN_NOTIFICATION_PACKAGE + "=? AND "
        + IDownloadsImpl::COLUMN_NOTIFICATION_CLASS + "=?";

CAR_SINGLETON_IMPL(CDownloads)

CAR_INTERFACE_IMPL(CDownloads, Object, IDownloads)

/**
 * Delete all the downloads for a package/class pair.
 */
ECode CDownloads::RemoveAllDownloadsByPackage(
    /* [in] */ IContext* context,
    /* [in] */ const String& notification_package,
    /* [in] */ const String& notification_class)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    AutoPtr<IUri> uri;
    impl->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    array->Set(0, notification_package);
    array->Set(1, notification_class);
    Int32 res;
    return resolver->Delete(uri, QUERY_WHERE_CLAUSE, array, &res);
}

} //namespace Provider
} //namespace Droid
} //namespace Elastos