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

#include "org/apache/http/message/BasicListHeaderIterator.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::EIID_IIterator;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

BasicListHeaderIterator::BasicListHeaderIterator()
    : mCurrentIndex(-1)
    , mLastIndex(-1)
{}

CAR_INTERFACE_IMPL_3(BasicListHeaderIterator, Object, IBasicListHeaderIterator, IHeaderIterator, IIterator)

ECode BasicListHeaderIterator::Init(
    /* [in] */ IList* headers,
    /* [in] */ const String& name)
{
    if (headers == NULL) {
        Logger::E("BasicListHeaderIterator", "Header list must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mAllHeaders = headers;
    mHeaderName = name;
    FindNext(-1, &mCurrentIndex);
    mLastIndex = -1;
    return NOERROR;
}

ECode BasicListHeaderIterator::FindNext(
    /* [in] */ Int32 from,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    if (from < -1) {
        *index = -1;
        return NOERROR;
    }

    Int32 size;
    mAllHeaders->GetSize(&size);
    Int32 to = size - 1;
    Boolean found = FALSE;
    while (!found && (from < to)) {
        from++;
        FilterHeader(from, &found);
    }
    *index = found ? from : -1;
    return NOERROR;
}

ECode BasicListHeaderIterator::FilterHeader(
    /* [in] */ Int32 index,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mHeaderName.IsNull()) {
        *result = TRUE;
        return NOERROR;
    }

    // non-header elements, including null, will trigger exceptions
    AutoPtr<IInterface> value;
    mAllHeaders->Get(index, (IInterface**)&value);
    AutoPtr<IHeader> h = IHeader::Probe(value);
    String name;
    h->GetName(&name);

    *result = mHeaderName.EqualsIgnoreCase(name);
    return NOERROR;
}

ECode BasicListHeaderIterator::HasNext(
    /* [out] */ Boolean* hasNext)
{
    VALIDATE_NOT_NULL(hasNext)
    *hasNext = (mCurrentIndex >= 0);
    return NOERROR;
}

ECode BasicListHeaderIterator::NextHeader(
    /* [out] */ IHeader** nextHeader)
{
    VALIDATE_NOT_NULL(nextHeader)
    *nextHeader = NULL;

    Int32 current = mCurrentIndex;
    if (current < 0) {
        Logger::E("BasicListHeaderIterator", "Iteration already finished.");
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }

    mLastIndex    = current;
    FindNext(current, &mCurrentIndex);
    AutoPtr<IInterface> value;
    mAllHeaders->Get(current, (IInterface**)&value);
    *nextHeader = IHeader::Probe(value);
    REFCOUNT_ADD(*nextHeader)
    return NOERROR;
}

ECode BasicListHeaderIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IHeader> h;
    NextHeader((IHeader**)&h);
    *object = h->Probe(EIID_IInterface);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode BasicListHeaderIterator::Remove()
{
    if (mLastIndex < 0) {
        Logger::E("BasicListHeaderIterator", "No header to remove.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mAllHeaders->Remove(mLastIndex);
    mLastIndex = -1;
    mCurrentIndex--; // adjust for the removed element
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
