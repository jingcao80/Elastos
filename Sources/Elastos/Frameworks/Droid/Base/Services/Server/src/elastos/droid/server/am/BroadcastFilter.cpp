
#include "elastos/droid/server/am/BroadcastFilter.h"
// #include "elastos/droid/server/am/ReceiverList.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL(BroadcastFilter, IntentFilter, IBroadcastFilter)

BroadcastFilter::BroadcastFilter(
    /* [in] */ IIntentFilter* intentFilter,
    /* [in] */ ReceiverList* receiverList,
    /* [in] */ const String& packageName,
    /* [in] */ const String& requiredPermission,
    /* [in] */ Int32 owningUid,
    /* [in] */ Int32 userId)
    : IntentFilter(intentFilter)
    , mReceiverList(receiverList)
    , mPackageName(packageName)
    , mRequiredPermission(requiredPermission)
    , mOwningUid(owningUid)
    , mOwningUserId(userId)
{
    IntentFilter::constructor(intentFilter);
}

BroadcastFilter::~BroadcastFilter()
{
}

void BroadcastFilter::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    assert(0);
    // DumpInReceiverList(pw, new PrintWriterPrinter(pw), prefix);
    // mReceiverList->DumpLocal(pw, prefix);
}

void BroadcastFilter::DumpBrief(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    DumpBroadcastFilterState(pw, prefix);
}

void BroadcastFilter::DumpInReceiverList(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IPrinter* pr,
    /* [in] */ const String& prefix)
{
    IntentFilter::Dump(pr, prefix);
    DumpBroadcastFilterState(pw, prefix);
}

void BroadcastFilter::DumpBroadcastFilterState(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    if (mRequiredPermission != NULL) {
        pw->Print(prefix);
        pw->Print(String("mRequiredPermission="));
        pw->Println(mRequiredPermission);
    }
}

String BroadcastFilter::ToString()
{
    StringBuilder sb(128);
    sb += ("BroadcastFilter{");
    sb += (StringUtils::ToString(Int32(this), 16));
    sb += ("userId=");
    sb += (mOwningUserId);
    sb += ' ';
    assert(0);
    // sb += mReceiverList->ToString();
    sb += "}";
    return sb.ToString();
}

ECode BroadcastFilter::ToString(
        /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ToString();
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
