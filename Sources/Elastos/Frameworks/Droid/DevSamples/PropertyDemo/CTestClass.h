#ifndef __CTESTCLASS_H__
#define __CTESTCLASS_H__

#include "_CTestClass.h"

using Elastos::Core::IByte;
using Elastos::Core::IChar8;
using Elastos::Core::IChar16;
using Elastos::Core::IBoolean;
using Elastos::Core::IFloat;
using Elastos::Core::IDouble;
using Elastos::Core::IInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::CByte;
using Elastos::Core::CChar8;
using Elastos::Core::CChar16;
using Elastos::Core::CBoolean;
using Elastos::Core::CFloat;
using Elastos::Core::CDouble;
using Elastos::Core::CInteger16;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace PropertyDemo {

CarClass(CTestClass)
{
public:
    CTestClass();

    CARAPI constructor();

    CARAPI GetByteValue(
        /* [out] */ Byte* value);

    CARAPI SetByteValue(
        /* [in] */ Byte value);

    CARAPI GetChar8Value(
        /* [out] */ Char8* value);

    CARAPI SetChar8Value(
        /* [in] */ Char8 value);

    CARAPI GetChar16Value(
        /* [out] */ Char16* value);

    CARAPI SetChar16Value(
        /* [in] */ Char16 value);

    CARAPI GetBooleanValue(
        /* [out] */ Boolean* value);

    CARAPI SetBooleanValue(
        /* [in] */ Boolean value);

    CARAPI GetStringValue(
        /* [out] */ String* value);

    CARAPI SetStringValue(
        /* [in] */ const String& value);

    CARAPI GetInt16Value(
        /* [out] */ Int16* value);

    CARAPI SetInt16Value(
        /* [in] */ Int16 value);

    CARAPI GetInt32Value(
        /* [out] */ Int32* value);

    CARAPI SetInt32Value(
        /* [in] */ Int32 value);

    CARAPI GetInt64Value(
        /* [out] */ Int64* value);

    CARAPI SetInt64Value(
        /* [in] */ Int64 value);

    CARAPI GetFloatValue(
        /* [out] */ Float* value);

    CARAPI SetFloatValue(
        /* [in] */ Float value);

    CARAPI GetDoubleValue(
        /* [out] */ Double* value);

    CARAPI SetDoubleValue(
        /* [in] */ Double value);

    CARAPI GetCharSequenceValue(
        /* [out] */ ICharSequence** value);

    CARAPI SetCharSequenceValue(
        /* [in] */ ICharSequence* value);

private:
    Byte mByteValue;
    Char8 mChar8Value;
    Char16 mChar16Value;
    Boolean mBooleanValue;
    Int16 mInt16Value;
    Int32 mInt32Value;
    Int64 mInt64Value;
    Float mFloatValue;
    Double mDoubleValue;
    String mStringValue;
    AutoPtr<ICharSequence> mInterfaceValue;
};

} // namespace PropertyDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
#endif
