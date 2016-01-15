#include "elastos/droid/media/tv/CTvContentRating.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

String CTvContentRating::DELIMITER("/");

CAR_INTERFACE_IMPL(CTvContentRating, Object, ITvContentRating)

CAR_OBJECT_IMPL(CTvContentRating)

CTvContentRating::CTvContentRating()
    : mHashCode(0)
{
}

CTvContentRating::~CTvContentRating()
{
}

ECode CTvContentRating::constructor(
    /* [in] */ const String& domain,
    /* [in] */ const String& ratingSystem,
    /* [in] */ const String& rating,
    /* [in] */ ArrayOf<String>* subRatings)
{
    mDomain = domain;
    mRatingSystem = ratingSystem;
    mRating = rating;
    if (subRatings == NULL || subRatings->GetLength() == 0) {
        mSubRatings = NULL;
    } else {
//TODO: Need Arrays::Sort
        // Arrays::Sort(subRatings);
        mSubRatings = subRatings;
    }
    mHashCode = 31 * (31 + 31 * mDomain.GetHashCode()
        + mRating.GetHashCode());
//TODO: Need Arrays::GetHashCode
        // + Arrays::GetHashCode(mSubRatings);
    return NOERROR;
}

ECode CTvContentRating::GetDomain(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDomain;
    return NOERROR;
}

ECode CTvContentRating::GetRatingSystem(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRatingSystem;
    return NOERROR;
}

ECode CTvContentRating::GetMainRating(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRating;
    return NOERROR;
}

ECode CTvContentRating::GetSubRatings(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    if (mSubRatings == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IList> list;
    Arrays::AsList(mSubRatings, (IList**)&list);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    return collections->UnmodifiableList(list, result);
}

ECode CTvContentRating::FlattenToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder builder;
    builder.Append(mDomain);
    builder.Append(DELIMITER);
    builder.Append(mRatingSystem);
    builder.Append(DELIMITER);
    builder.Append(mRating);
    if (mSubRatings != NULL) {
        for(Int32 i = 0; i < mSubRatings->GetLength(); i++) {
            builder.Append(DELIMITER);
            builder.Append((*mSubRatings)[i]);
        }
    }
    return builder.ToString(result);
}

ECode CTvContentRating::Contains(
    /* [in] */ ITvContentRating* rating,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (rating == NULL) {
        // throw new IllegalArgumentException("rating cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String mainRating;
    rating->GetMainRating(&mainRating);
    if (!mainRating.Equals(mRating)) {
        *result = FALSE;
        return NOERROR;
    }
    String domain;
    rating->GetDomain(&domain);
    String ratingSystem;
    rating->GetRatingSystem(&ratingSystem);
    if (!domain.Equals(mDomain) ||
            !ratingSystem.Equals(mRatingSystem) ||
            !mainRating.Equals(mRating)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IList> subRatings;
    GetSubRatings((IList**)&subRatings);
    AutoPtr<IList> subRatingsOther;
    rating->GetSubRatings((IList**)&subRatingsOther);
    if (subRatings == NULL && subRatingsOther == NULL) {
        *result = TRUE;
        return NOERROR;
    } else if (subRatings == NULL && subRatingsOther != NULL) {
        *result = FALSE;
        return NOERROR;
    } else if (subRatings != NULL && subRatingsOther == NULL) {
        *result = TRUE;
        return NOERROR;
    } else {
        subRatings->ContainsAll(ICollection::Probe(subRatingsOther), result);
    }
    return NOERROR;
}

ECode CTvContentRating::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHashCode;
    return NOERROR;
}

ECode CTvContentRating::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if(ITvContentRating::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<CTvContentRating> other = (CTvContentRating*)ITvContentRating::Probe(obj);
    if (mHashCode != other->mHashCode) {
        *result = FALSE;
        return NOERROR;
    }
    if (!mDomain.Equals(other->mDomain)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!mRatingSystem.Equals(other->mRatingSystem)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!mRating.Equals(other->mRating)) {
        *result = FALSE;
        return NOERROR;
    }
    *result = Arrays::Equals(mSubRatings, other->mSubRatings);
    return NOERROR;
}

ECode CTvContentRating::CreateRating(
    /* [in] */ const String& domain,
    /* [in] */ const String& ratingSystem,
    /* [in] */ const String& rating,
    /* [in] */ ArrayOf<String>* subRatings,
    /* [out] */ ITvContentRating** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (domain.IsEmpty()) {
        // throw new IllegalArgumentException("domain cannot be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (ratingSystem.IsEmpty()) {
        // throw new IllegalArgumentException("ratingSystem cannot be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (rating.IsEmpty()) {
        // throw new IllegalArgumentException("rating cannot be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return CTvContentRating::New(domain, ratingSystem, rating, subRatings, result);
}

ECode CTvContentRating::UnflattenFromString(
    /* [in] */ const String& ratingString,
    /* [out] */ ITvContentRating** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (ratingString.IsEmpty()) {
        // throw new IllegalArgumentException("ratingString cannot be empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ArrayOf<String> > strs;
    StringUtils::Split(ratingString, DELIMITER, (ArrayOf<String>**)&strs);
    if (strs->GetLength() < 3) {
        // throw new IllegalArgumentException("Invalid rating string: " + ratingString);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (strs->GetLength() > 3) {
        AutoPtr<ArrayOf<String> > subRatings = ArrayOf<String>::Alloc(strs->GetLength() - 3);
        for(Int32 i = 0; i < subRatings->GetLength(); i++) {
            subRatings->Set(i, (*strs)[i+3]);
        }
        return CTvContentRating::New((*strs)[0], (*strs)[1], (*strs)[2], subRatings, result);
    }
    return CTvContentRating::New((*strs)[0], (*strs)[1], (*strs)[2], NULL, result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
