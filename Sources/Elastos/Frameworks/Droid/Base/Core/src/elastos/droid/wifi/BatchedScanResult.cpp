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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/wifi/BatchedScanResult.h"
#include "elastos/droid/wifi/CScanResult.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Wifi {

const String BatchedScanResult::TAG("BatchedScanResult");

BatchedScanResult::BatchedScanResult()
    : truncated(FALSE)
{
}

BatchedScanResult::BatchedScanResult(
    /* [in] */ const BatchedScanResult& source)
{
    truncated = source.truncated;
    AutoPtr<IIterable> iterable = IIterable::Probe(source.scanResults);
    AutoPtr<IIterator> iter;
    iterable->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> s;
        iter->GetNext((IInterface**)&s);
        AutoPtr<IScanResult> sr;
        CScanResult::New(IScanResult::Probe(s), (IScanResult**)&sr);
        scanResults->Add(sr);
    }
}

ECode BatchedScanResult::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuffer sb;

    sb.Append("BatchedScanResult: ");
    sb.Append("truncated: ");
    sb.Append(StringUtils::BooleanToString(truncated));
    sb.Append("scanResults: [");
    AutoPtr<IIterable> iterable = IIterable::Probe(scanResults);
    AutoPtr<IIterator> iter;
    iterable->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> s;
        iter->GetNext((IInterface**)&s);
        sb.Append(" <");
        sb.Append(Object::ToString(s));
        sb.Append("> ");
    }
    sb.Append(" ]");

    return sb.ToString(str);
}

/** Implement the Parcelable interface {@hide} */
ECode BatchedScanResult::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode BatchedScanResult::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    dest->WriteInt32(truncated ? 1 : 0);
    Int32 size;
    scanResults->GetSize(&size);
    dest->WriteInt32(size);
    AutoPtr<IIterable> iterable = IIterable::Probe(scanResults);
    AutoPtr<IIterator> iter;
    iterable->GetIterator((IIterator**)&iter);
    Boolean bNext;
    iter->HasNext(&bNext);
    for (; bNext; iter->HasNext(&bNext)) {
        AutoPtr<IInterface> s;
        iter->GetNext((IInterface**)&s);
        IParcelable::Probe(s)->WriteToParcel(dest);
    }
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
