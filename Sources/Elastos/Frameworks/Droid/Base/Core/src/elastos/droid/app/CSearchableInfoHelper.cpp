#include "elastos/droid/app/CSearchableInfoHelper.h"
#include "elastos/droid/app/CSearchableInfo.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CSearchableInfoHelper, Singleton, ISearchableInfoHelper)

CAR_SINGLETON_IMPL(CSearchableInfoHelper)

ECode CSearchableInfoHelper::GetActivityMetaData(
    /* [in] */ IContext* ctx,
    /* [in] */ IActivityInfo* activityInfo,
    /* [in] */ Int32 userId,
    /* [out] */ ISearchableInfo** info)
{
    return CSearchableInfo::GetActivityMetaData(ctx, activityInfo, userId, info);
}

}// namespace App
}// namespace Droid
}// namespace Elastos
