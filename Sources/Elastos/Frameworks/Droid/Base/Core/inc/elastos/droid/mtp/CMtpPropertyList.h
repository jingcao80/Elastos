
#ifndef __ELASTOS_DROID_MTP_CMTPPROPERTYLIST_H__
#define __ELASTOS_DROID_MTP_CMTPPROPERTYLIST_H__

#include "_Elastos_Droid_Mtp_CMtpPropertyList.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Mtp {

CarClass(CMtpPropertyList)
    , public Object
    , public IMtpPropertyList
{
public:
    CMtpPropertyList();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 maxCount,
        /* [in] */ Int32 result);

    CARAPI Append(
        /* [in] */ Int32 handle,
        /* [in] */ Int32 property,
        /* [in] */ Int32 type,
        /* [in] */ Int64 value);

    CARAPI Append(
        /* [in] */ Int32 handle,
        /* [in] */ Int32 property,
        /* [in] */ const String& value);

    CARAPI SetResult(
        /* [in] */ Int32 result);

public:
    Int32 mResult;
    // list of object handles (first field in quadruplet)
    AutoPtr<ArrayOf<Int32> > mObjectHandles;
    // list of object propery codes (second field in quadruplet)
    AutoPtr<ArrayOf<Int32> > mPropertyCodes;
    // list of data type codes (third field in quadruplet)
    AutoPtr<ArrayOf<Int32> > mDataTypes;
    // list of long int property values (fourth field in quadruplet, when value is integer type)
    AutoPtr<ArrayOf<Int64> > mLongValues;
    // list of long int property values (fourth field in quadruplet, when value is string type)
    AutoPtr<ArrayOf<String> > mStringValues;

public:
    // number of results returned
    Int32 mCount;
    // maximum number of results
    Int32 mMaxCount;
};

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MTP_CMTPPROPERTYLIST_H__
