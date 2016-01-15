#include "elastos/droid/media/tv/CTvContentRatingHelper.h"
#include "elastos/droid/media/tv/CTvContentRating.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL(CTvContentRatingHelper, Singleton, ITvContentRatingHelper)

CAR_SINGLETON_IMPL(CTvContentRatingHelper)

ECode CTvContentRatingHelper::CreateRating(
    /* [in] */ const String& domainStr,
    /* [in] */ const String& ratingSystem,
    /* [in] */ const String& rating,
    /* [in] */ ArrayOf<String> * subRatings,
    /* [out] */ ITvContentRating ** result)
{
    return CTvContentRating::CreateRating(domainStr, ratingSystem
        , rating, subRatings, result);
}

ECode CTvContentRatingHelper::UnflattenFromString(
    /* [in] */ const String& ratingString,
    /* [out] */ ITvContentRating ** result)
{
    return CTvContentRating::UnflattenFromString(ratingString, result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
