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

#include "elastos/droid/content/CContentProviderOperationHelper.h"
#include "elastos/droid/content/CContentProviderOperation.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CContentProviderOperationHelper, Singleton, IContentProviderOperationHelper)

CAR_SINGLETON_IMPL(CContentProviderOperationHelper)

ECode CContentProviderOperationHelper::NewInsert(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderOperationBuilder** operationBuilder)
{
    VALIDATE_NOT_NULL(operationBuilder)
    AutoPtr<IContentProviderOperationBuilder> tmp = CContentProviderOperation::NewInsert(uri);
    *operationBuilder = tmp;
    REFCOUNT_ADD(*operationBuilder);
    return NOERROR;
}

ECode CContentProviderOperationHelper::NewUpdate(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderOperationBuilder** operationBuilder)
{
    VALIDATE_NOT_NULL(operationBuilder)
    AutoPtr<IContentProviderOperationBuilder> tmp = CContentProviderOperation::NewUpdate(uri);
    *operationBuilder = tmp;
    REFCOUNT_ADD(*operationBuilder);
    return NOERROR;
}

ECode CContentProviderOperationHelper::NewDelete(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderOperationBuilder** operationBuilder)
{
    VALIDATE_NOT_NULL(operationBuilder)
    AutoPtr<IContentProviderOperationBuilder> tmp = CContentProviderOperation::NewDelete(uri);
    *operationBuilder = tmp;
    REFCOUNT_ADD(*operationBuilder);
    return NOERROR;
}

ECode CContentProviderOperationHelper::NewAssertQuery(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderOperationBuilder** operationBuilder)
{
    VALIDATE_NOT_NULL(operationBuilder)
    AutoPtr<IContentProviderOperationBuilder> tmp = CContentProviderOperation::NewAssertQuery(uri);
    *operationBuilder = tmp;
    REFCOUNT_ADD(*operationBuilder);
    return NOERROR;
}

}
}
}

