
#include "elastos/droid/server/am/ActivityResult.h"

using Elastos::Droid::App::CResultInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

ActivityResult::ActivityResult(
    /* [in] */ ActivityRecord* from,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
    : mFrom(from)
{
    CResultInfo::New(resultWho, requestCode, resultCode, data, (IResultInfo**)&mResultInfo);
}

ActivityResult::~ActivityResult()
{
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
