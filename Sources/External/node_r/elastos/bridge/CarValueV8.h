
#ifndef __CARVALUEV8_H__
#define __CARVALUEV8_H__

#include "eldef.h"

namespace Elastos {
namespace Node {

struct CarValue
{
    CarValue();

    CarValue(const CarValue& value);

    ~CarValue();

    ParamIOAttribute mIOAttribute;
    CarDataType mType;
    Byte mByteValue;
    Boolean mBooleanValue;
    Int16 mInt16Value;
    Int32 mInt32Value;
    Int64 mInt64Value;
    Char32 mCharValue;
    Float mFloatValue;
    Double mDoubleValue;
    String mStringValue;
    ECode mECodeValue;
    Int32 mEnumValue;
    CarQuintet* mCarQuintet;
    CarDataType mElementType;
    AutoPtr<IInterface> mObjectValue;
    EMuid mIID;
    EGuid mCLSID;
    char mUUnm[256];
};

} // namespace Node
} // namespace Elastos

#endif //__CARVALUEV8_H__
