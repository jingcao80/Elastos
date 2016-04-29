
#include "elastos/droid/wifi/CBatchedScanResult.h"
#include "elastos/droid/wifi/CScanResult.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CBatchedScanResult, Object, IBatchedScanResult, IParcelable)

CAR_OBJECT_IMPL(CBatchedScanResult)

const String CBatchedScanResult::TAG("BatchedScanResult");

CBatchedScanResult::CBatchedScanResult()
    : mTruncated(FALSE)
{
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    mScanResults = IList::Probe(list);
}

ECode CBatchedScanResult::constructor()
{
    return NOERROR;
}

ECode CBatchedScanResult::constructor(
    /* [in] */ IBatchedScanResult* source)
{
    source->GetTruncated(&mTruncated);

    Int32 size;
    AutoPtr<IList> list;
    source->GetScanResults((IList**)&list);
    list->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IScanResult> s;
        CScanResult::New(IScanResult::Probe(obj), (IScanResult**)&s);
        mScanResults->Add(s);
    }

    return NOERROR;
}

ECode CBatchedScanResult::GetTruncated(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTruncated;
    return NOERROR;
}

ECode CBatchedScanResult::SetTruncated(
    /* [in] */ Boolean truncated)
{
    mTruncated = truncated;
    return NOERROR;
}

/** The result of this particular scan. */
ECode CBatchedScanResult::GetScanResults(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mScanResults;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CBatchedScanResult::SetScanResults(
    /* [in] */ IList* scanResults)
{
    mScanResults = scanResults;
    return NOERROR;
}

//@Override
ECode CBatchedScanResult::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;

    sb.Append("BatchedScanResult: ");
    sb.Append("truncated: ");
    sb.Append(StringUtils::BooleanToString(mTruncated));
    sb.Append("scanResults: [");

    Int32 size;
    mScanResults->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mScanResults->Get(i, (IInterface**)&obj);
        AutoPtr<IScanResult> s = IScanResult::Probe(obj);
        sb.Append(" <");
        String tmp;
        IObject::Probe(s)->ToString(&tmp);
        sb.Append(tmp);
        sb.Append("> ");
    }
    sb.Append("]");

    return sb.ToString(str);
}

/** Implement the Parcelable interface {@hide} */
ECode CBatchedScanResult::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

ECode CBatchedScanResult::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadBoolean(&mTruncated);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mScanResults = IList::Probe(obj);
    return NOERROR;
}

ECode CBatchedScanResult::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteBoolean(mTruncated);
    dest->WriteInterfacePtr(mScanResults);
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
