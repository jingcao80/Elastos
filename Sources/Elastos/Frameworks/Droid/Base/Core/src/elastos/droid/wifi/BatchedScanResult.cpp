
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
        AutoPtr<IScanResult> s;
        iter->GetNext((IInterface**)&s);
        AutoPtr<IScanResult> sr;
        CScanResult::New(s, (IScanResult**)&sr);
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
        AutoPtr<IScanResult> s;
        iter->GetNext((IInterface**)&s);
        String _str;
        assert(0);
        // TODO
        // s->ToString(&_str);
        sb.Append(" <");
        sb.Append(_str);
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
        AutoPtr<IScanResult> s;
        iter->GetNext((IInterface**)&s);
        assert(0);
        // TODO
        // s->WriteToParcel(dest, flags);
    }
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
