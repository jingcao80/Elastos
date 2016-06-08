#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_RESULTEXCEPTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_RESULTEXCEPTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Class for errors in the Result object.
  *
  * {@hide}
  */
class ResultException
    : public Object
    //, public ICatException
{
public:
    CAR_INTERFACE_DECL();

    ResultException();

    CARAPI constructor(
        /* [in] */ ResultCode result);

    CARAPI constructor(
        /* [in] */ ResultCode result,
        /* [in] */ const String& explanation);

    CARAPI constructor(
        /* [in] */ ResultCode result,
        /* [in] */ Int32 additionalInfo);

    CARAPI constructor(
        /* [in] */ ResultCode result,
        /* [in] */ Int32 additionalInfo,
        /* [in] */ const String& explanation);

    virtual CARAPI Result(
        /* [out] */ ResultCode* result);

    virtual CARAPI HasAdditionalInfo(
        /* [out] */ Boolean* result);

    virtual CARAPI AdditionalInfo(
        /* [out] */ Int32* result);

    virtual CARAPI Explanation(
        /* [out] */ String* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    ResultCode mResult;
    Int32 mAdditionalInfo;
    String mExplanation;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_RESULTEXCEPTION_H__
