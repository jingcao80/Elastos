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

#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Database.h"
#include "elastos/droid/widget/CursorFilter.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::EIID_ICursor;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(CursorFilter, Filter, ICursorFilter);
ECode CursorFilter::constructor(
    /* [in] */ ICursorFilterClient* client)
{
    mClient = client;
    return NOERROR;
}

ECode CursorFilter::ConvertResultToString(
    /* [in] */ IInterface* resultValue,
    /* [out] */ ICharSequence** cs)
{
    VALIDATE_NOT_NULL(cs);
    return mClient->ConvertToString(ICursor::Probe(resultValue), cs);
}

ECode CursorFilter::PerformFiltering(
    /* [in] */ ICharSequence* constraint,
    /* [out] */ IFilterResults** filterResults)
{
    VALIDATE_NOT_NULL(filterResults);
    AutoPtr<ICursor> cursor;
    mClient->RunQueryOnBackgroundThread(constraint, (ICursor**)&cursor);

    AutoPtr<Filter::FilterResults> results = new Filter::FilterResults();
    if (cursor != NULL) {
        cursor->GetCount(&results->mCount);
        results->mValues = cursor;
    } else {
        results->mCount = 0;
        results->mValues = NULL;
    }

    *filterResults = results;
    REFCOUNT_ADD(*filterResults);

    return NOERROR;
}

ECode CursorFilter::PublishResults(
    /* [in] */ ICharSequence* constraint,
    /* [in] */ IFilterResults* res)
{
    AutoPtr<ICursor> oldCursor;
    mClient->GetCursor((ICursor**)&oldCursor);

    Filter::FilterResults* results = (Filter::FilterResults*)res;
    if (results->mValues != NULL && results->mValues != oldCursor) {
        mClient->ChangeCursor((ICursor*) results->mValues.Get());
    }

    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
