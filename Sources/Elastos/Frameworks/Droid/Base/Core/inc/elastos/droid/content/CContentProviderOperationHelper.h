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

#ifndef __ELASTOS_DROID_CONTENT_CCONTENTPROVIDEROPERATIONHELPER_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTPROVIDEROPERATIONHELPER_H__

#include "_Elastos_Droid_Content_CContentProviderOperationHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CContentProviderOperationHelper)
    , public Singleton
    , public IContentProviderOperationHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create a {@link Builder} suitable for building an insert {@link ContentProviderOperation}.
     * @param uri The {@link Uri} that is the target of the insert.
     * @return a {@link Builder}
     */
    CARAPI NewInsert(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderOperationBuilder** operationBuilder);

    /**
     * Create a {@link Builder} suitable for building an update {@link ContentProviderOperation}.
     * @param uri The {@link Uri} that is the target of the update.
     * @return a {@link Builder}
     */
    CARAPI NewUpdate(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderOperationBuilder** operationBuilder);

    /**
     * Create a {@link Builder} suitable for building a delete {@link ContentProviderOperation}.
     * @param uri The {@link Uri} that is the target of the delete.
     * @return a {@link Builder}
     */
    CARAPI NewDelete(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderOperationBuilder** operationBuilder);

    /**
     * Create a {@link Builder} suitable for building a
     * {@link ContentProviderOperation} to assert a set of values as provided
     * through {@link Builder#withValues(ContentValues)}.
     */
    CARAPI NewAssertQuery(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderOperationBuilder** operationBuilder);

private:
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCONTENTPROVIDEROPERATIONHELPER_H__
