
#ifndef __ELASTOS_DROID_TELEPHONY_CCDMASMSCBPROGRAMRESULTS_H__
#define __ELASTOS_DROID_TELEPHONY_CCDMASMSCBPROGRAMRESULTS_H__

#include "_Elastos_Droid_Telephony_CCdmaSmsCbProgramResults.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CCdmaSmsCbProgramResults)
{
public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 category,
        /* [in] */ Int32 language,
        /* [in] */ Int32 categoryResult);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetCategory(
        /* [out] */ Int32* res);

    CARAPI GetLanguage(
        /* [out] */ Int32* res);

    CARAPI GetCategoryResult(
        /* [out] */ Int32* res);

    CARAPI SetCategory(
        /* [in] */ Int32 category);

    CARAPI SetLanguage(
        /* [in] */ Int32 language);

    CARAPI SetCategoryResult(
        /* [in] */ Int32 categoryresult);

    CARAPI ToString(
        /* [out] */ String* str);

private:

    /** Service category to modify. */
    Int32 mCategory;

    /** Language used for service category name (defined in BearerData.LANGUAGE_*). */
    Int32 mLanguage;

    /** Result of service category programming for this category. */
    Int32 mCategoryResult;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CCDMASMSCBPROGRAMRESULTS_H__
