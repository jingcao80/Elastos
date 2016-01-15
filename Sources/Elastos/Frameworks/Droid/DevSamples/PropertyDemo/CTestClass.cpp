#include "CTestClass.h"

using Elastos::Core::CStringWrapper;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace PropertyDemo {

CTestClass::CTestClass()
    : mByteValue('b')
    , mChar8Value('a')
    , mChar16Value('f')
    , mBooleanValue(FALSE)
    , mInt16Value(166)
    , mInt32Value(3232)
    , mInt64Value(6464)
    , mFloatValue(0.7f)
    , mDoubleValue(0.1234567895)
    , mStringValue("testString")
{
    CStringWrapper::New(String("测试值： Interface Test"), (ICharSequence**)&mInterfaceValue);
}

ECode CTestClass::constructor()
{
    return NOERROR;
}

ECode CTestClass::GetByteValue(
    /* [out] */ Byte* value)
{
    *value = mByteValue;
    return NOERROR;
}

ECode CTestClass::SetByteValue(
    /* [in] */ Byte value)
{
    mByteValue = value;
    return NOERROR;
}

ECode CTestClass::GetChar8Value(
    /* [out] */ Char8* value)
{
    *value = mChar8Value;
    return NOERROR;
}

ECode CTestClass::SetChar8Value(
    /* [in] */ Char8 value)
{
    mChar8Value = value;
    return NOERROR;
}

ECode CTestClass::GetChar16Value(
    /* [out] */ Char16* value)
{
    *value = mChar16Value;
    return NOERROR;
}

ECode CTestClass::SetChar16Value(
    /* [in] */ Char16 value)
{
    mChar16Value = value;
    return NOERROR;
}

ECode CTestClass::GetBooleanValue(
    /* [out] */ Boolean* value)
{
    *value = mBooleanValue;
    return NOERROR;
}

ECode CTestClass::SetBooleanValue(
    /* [in] */ Boolean value)
{
    mBooleanValue = value;
    return NOERROR;
}

ECode CTestClass::GetStringValue(
    /* [out] */ String* value)
{
    *value = mStringValue;
    return NOERROR;
}

ECode CTestClass::SetStringValue(
    /* [in] */ const String& value)
{
    mStringValue = value;
    return NOERROR;
}

ECode CTestClass::GetInt16Value(
    /* [out] */ Int16* value)
{
    *value = mInt16Value;
    return NOERROR;
}

ECode CTestClass::SetInt16Value(
    /* [in] */ Int16 value)
{
    mInt16Value = value;
    return NOERROR;
}

ECode CTestClass::GetInt32Value(
    /* [out] */ Int32* value)
{
    *value = mInt32Value;
    return NOERROR;
}

ECode CTestClass::SetInt32Value(
    /* [in] */ Int32 value)
{
    mInt32Value = value;
    return NOERROR;
}

ECode CTestClass::GetInt64Value(
    /* [out] */ Int64* value)
{
    *value = mInt64Value;
    return NOERROR;
}

ECode CTestClass::SetInt64Value(
    /* [in] */ Int64 value)
{
    mInt64Value = value;
    return NOERROR;
}

ECode CTestClass::GetFloatValue(
    /* [out] */ Float* value)
{
    *value = mFloatValue;
    return NOERROR;
}

ECode CTestClass::SetFloatValue(
    /* [in] */ Float value)
{
    mFloatValue = value;
    return NOERROR;
}

ECode CTestClass::GetDoubleValue(
    /* [out] */ Double* value)
{
    *value = mDoubleValue;
    return NOERROR;
}

ECode CTestClass::SetDoubleValue(
    /* [in] */ Double value)
{
    mDoubleValue = value;
    return NOERROR;
}

ECode CTestClass::GetCharSequenceValue(
    /* [out] */ ICharSequence** value)
{
    *value = mInterfaceValue;
    if(*value)
        (*value)->AddRef();
    return NOERROR;
}

ECode CTestClass::SetCharSequenceValue(
    /* [in] */ ICharSequence* value)
{
    mInterfaceValue = value;
    return NOERROR;
}

} // namespace PropertyDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
