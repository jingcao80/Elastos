#include "elastos/droid/ext/frameworkdef.h"
#include "CActivityOne.h"
#include "CTestClass.h"
#include <stdio.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuffer.h>
#include "R.h"

using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::StringBuffer;
using Elastos::Core::CStringWrapper;
using Elastos::Droid::Utility::IPropertyHelper;
using Elastos::Droid::Utility::CPropertyHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace PropertyDemo {

#define GetTargetValue1(T1, T2, T3, getValueFunc, nameStr)                                   \
T1 getValue, realValue;                                                                      \
StringBuffer sb("反射结果：");                                                                 \
AutoPtr<T2> ptr;                                                                             \
AutoPtr<IProperty> property = CreateProperty(CarDataType_##T1, nameStr);                     \
property->Get(mClsTarget, (IInterface**)&ptr);                                               \
if(!ptr)                                                                                     \
return E_UNAVAILABLE_EXCEPTION;                                                              \
ptr->getValueFunc(&getValue);                                                                \
mClsTarget->Get##T1##Value(&realValue);                                                      \
mRstStr = GetResultStr_Get((T3)getValue, (T3)realValue);                                     \
mHost->mTextView->SetText(mRstStr);                                                          \
return NOERROR;

#define GetTargetValue2(T1, T2, getValueFunc, nameStr)                                       \
        GetTargetValue1(T1, T2, T1, getValueFunc, nameStr)

#define SetTargetValue1(T1, T2, T3, T4, defaultValue, nameStr)                               \
T1 setValue = defaultValue, realValue;                                                       \
AutoPtr<T2> ptr;                                                                             \
AutoPtr<IProperty> property = CreateProperty(CarDataType_##T1, nameStr);                     \
T3::New(setValue, (T2**)&ptr);                                                               \
property->Set(mClsTarget, ptr);                                                              \
mClsTarget->Get##T1##Value(&realValue);                                                      \
mRstStr = GetResultStr_Set((T4)setValue, (T4)realValue);                                     \
mHost->mTextView->SetText(mRstStr);                                                          \
return NOERROR;

#define SetTargetValue2(T1, T2, T3, defaultValue, nameStr)                                   \
        SetTargetValue1(T1, T2, T3, T1, defaultValue, nameStr)

CAR_INTERFACE_IMPL(CActivityOne::MyListener, IViewOnClickListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{
    CTestClass::New((ITestClass**)&mClsTarget);
}

AutoPtr<IProperty> CActivityOne::MyListener::CreateProperty(
    /* [in] */ CarDataType carDateType,
    /* [in] */ const String& name)
{
    AutoPtr<IProperty> property;
    AutoPtr<IObject> obj = IObject::Probe(mClsTarget);
    assert(obj != NULL);
    ClassID clsId;
    obj->GetClassID(&clsId);
    AutoPtr<IPropertyHelper> helper;
    CPropertyHelper::AcquireSingleton((IPropertyHelper**)&helper);
    ASSERT_SUCCEEDED(helper->Of(clsId, carDateType, name, (IProperty**)&property));
    return property;
}

template<typename T>
AutoPtr<ICharSequence> CActivityOne::MyListener::GetResultStr_Get(
        /* [in] */ T getValue,
        /* [in] */ T realValue)
{
    StringBuffer sb("反射结果：");
    if(getValue == realValue)
    {
        sb += "   获取正确，获取到的值为： ";
        sb += getValue;
    } else {
        sb += "   获取错误，获取到的值为： ";
        sb += getValue;
        sb += "  真实值为 ： ";
        sb += realValue;
    }
    AutoPtr<ICharSequence> cs = sb.ToCharSequence();
    return cs;
}

template<typename T>
AutoPtr<ICharSequence> CActivityOne::MyListener::GetResultStr_Set(
        /* [in] */ T setValue,
        /* [in] */ T realValue)
{
    StringBuffer sb("反射结果：");
    if(setValue == realValue)
    {
        sb += "   设置正确，设置后的值为： ";
        sb += realValue;
    } else {
        sb += "   设置错误，想要设置的值为： ";
        sb += setValue;
        sb += "  真实值为 ： ";
        sb += realValue;
    }
    AutoPtr<ICharSequence> cs = sb.ToCharSequence();
    return cs;
}



ECode CActivityOne::MyListener::GetInt32()
{
    GetTargetValue2(Int32, IInteger32, GetValue, String("int32Value"))
}

ECode CActivityOne::MyListener::SetInt32()
{
    static Byte i = 1;
    SetTargetValue2(Int32, IInteger32, CInteger32, 32 * (i < 100 ? i++ : i = 0), String("int32Value"))
}

ECode CActivityOne::MyListener::GetByte()
{
    GetTargetValue1(Byte, IByte, Int32, GetValue, String("byteValue"))
}

ECode CActivityOne::MyListener::SetByte()
{
    static Byte i = 'A';
    SetTargetValue1(Byte, IByte, CByte, Int32, (i < 'z' ? i++ : i = 'A'), String("byteValue"))
}

ECode CActivityOne::MyListener::GetChar8()
{
    GetTargetValue2(Char8, IChar8, GetValue, String("char8Value"))
}

ECode CActivityOne::MyListener::SetChar8()
{
    static Byte i = 1;
    SetTargetValue2(Char8, IChar8, CChar8, (i < 127 ? i++ : i = 0), String("char8Value"))
}

ECode CActivityOne::MyListener::GetChar16()
{
    GetTargetValue2(Char16, IChar16, GetValue, String("char16Value"))
}

ECode CActivityOne::MyListener::SetChar16()
{
    static Byte i = 1;
    SetTargetValue2(Char16, IChar16, CChar16, 1616 + (i < 127 ? i++ : i = 0), String("char16Value"))
}

ECode CActivityOne::MyListener::GetBoolean()
{
    GetTargetValue2(Boolean, IBoolean, GetValue, String("booleanValue"))
}

ECode CActivityOne::MyListener::SetBoolean()
{
    static Boolean bValue = FALSE;
    bValue = !bValue;
    SetTargetValue2(Boolean, IBoolean, CBoolean, bValue, String("booleanValue"))
}

ECode CActivityOne::MyListener::GetInt16()
{
    GetTargetValue2(Int16, IInteger16, GetValue, String("int16Value"))
}

ECode CActivityOne::MyListener::SetInt16()
{
    static Byte i = 1;
    SetTargetValue2(Int16, IInteger16, CInteger16, 16 * (i < 127 ? i++ : i = 0), String("int16Value"))
}

ECode CActivityOne::MyListener::GetInt64()
{
    GetTargetValue2(Int64, IInteger64, GetValue, String("int64Value"))
}

ECode CActivityOne::MyListener::SetInt64()
{
    static Byte i = 1;
    SetTargetValue2(Int64, IInteger64, CInteger64, 64 * (i < 127 ? i++ : i = 0), String("int64Value"))
}

ECode CActivityOne::MyListener::GetString()
{
    GetTargetValue2(String, ICharSequence, ToString, String("stringValue"))
}

ECode CActivityOne::MyListener::SetString()
{
    String setValue("Zhang.Jingcheng Test Reflection");
    // SetTargetValue2(String, ICharSequence, CStringWrapper, pm, String("stringValue"))
    String realValue;
    AutoPtr<ICharSequence> ptr;
    AutoPtr<IProperty> property = CreateProperty(CarDataType_String, String("stringValue"));
    CStringWrapper::New(setValue, (ICharSequence**)&ptr);
    property->Set(mClsTarget, ptr);
    mClsTarget->GetStringValue(&realValue);
    mRstStr = GetResultStr_Set(setValue, realValue);
    mHost->mTextView->SetText(mRstStr);
    return NOERROR;
}


ECode CActivityOne::MyListener::GetFloat()
{
    GetTargetValue2(Float, IFloat, GetValue, String("floatValue"))
}

ECode CActivityOne::MyListener::SetFloat()
{
    static Byte i = 1;
    SetTargetValue2(Float, IFloat, CFloat, 10.1402f * (i < 15 ? i++ : i = 0), String("floatValue"))
}


ECode CActivityOne::MyListener::GetDouble()
{
    GetTargetValue2(Double, IDouble, GetValue, String("doubleValue"))
}

ECode CActivityOne::MyListener::SetDouble()
{
    static Byte i = 1;
    SetTargetValue2(Double, IDouble, CDouble, 10926.145455544502f * (i < 50 ? i++ : i = 0), String("doubleValue"))
}

ECode CActivityOne::MyListener::GetInterface()
{
    AutoPtr<ICharSequence> ptr;
    AutoPtr<IProperty> property = CreateProperty(CarDataType_Interface, String("charSequenceValue"));
    property->Get(mClsTarget, (IInterface**)&ptr);
    mHost->mTextView->SetText(ptr);
    return NOERROR;
}

ECode CActivityOne::MyListener::SetInterface()
{
    AutoPtr<ICharSequence> ptr;
    CStringWrapper::New(String("设置Interface的值成功"), (ICharSequence**)&ptr);
    AutoPtr<IProperty> property = CreateProperty(CarDataType_Interface, String("charSequenceValue"));
    property->Set(mClsTarget, ptr);
    AutoPtr<ICharSequence> realValue;
    mClsTarget->GetCharSequenceValue((ICharSequence**)&realValue);
    mHost->mTextView->SetText(realValue);
    return NOERROR;
}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId;
    v->GetId(&viewId);
    switch(viewId)
    {
    case R::id::bt_getByte :
        return GetByte();
    case R::id::bt_setByte :
        return SetByte();
    case R::id::bt_getChar8 :
        return GetChar8();
    case R::id::bt_setChar8 :
        return SetChar8();
    case R::id::bt_getChar16 :
        return GetChar16();
    case R::id::bt_setChar16 :
        return SetChar16();
    case R::id::bt_getBoolean :
        return GetBoolean();
    case R::id::bt_setBoolean :
        return SetBoolean();
    case R::id::bt_getInt16 :
        return GetInt16();
    case R::id::bt_setInt16 :
        return SetInt16();
    case R::id::bt_getInt32 :
        return GetInt32();
    case R::id::bt_setInt32 :
        return SetInt32();
    case R::id::bt_getInt64 :
        return GetInt64();
    case R::id::bt_setInt64 :
        return SetInt64();
    case R::id::bt_getFloat :
        return GetFloat();
    case R::id::bt_setFloat :
        return SetFloat();
    case R::id::bt_getDouble :
        return GetDouble();
    case R::id::bt_setDouble :
        return SetDouble();
    case R::id::bt_getString :
        return GetString();
    case R::id::bt_setString :
        return SetString();
    case R::id::bt_getInterface :
        return GetInterface();
    case R::id::bt_setInterface :
        return SetInterface();

    default:
        return NOERROR;
    }
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);
    AutoPtr<IViewOnClickListener> l = new MyListener(this);
    mGetByteButton = IButton::Probe(FindViewById(R::id::bt_getByte));
    mSetByteButton = IButton::Probe(FindViewById(R::id::bt_setByte));
    mGetChar8Button = IButton::Probe(FindViewById(R::id::bt_getChar8));
    mSetChar8Button = IButton::Probe(FindViewById(R::id::bt_setChar8));
    mGetChar16Button = IButton::Probe(FindViewById(R::id::bt_getChar16));
    mSetChar16Button = IButton::Probe(FindViewById(R::id::bt_setChar16));
    mGetBooleanButton = IButton::Probe(FindViewById(R::id::bt_getBoolean));
    mSetBooleanButton = IButton::Probe(FindViewById(R::id::bt_setBoolean));
    mGetInt16Button = IButton::Probe(FindViewById(R::id::bt_getInt16));
    mSetInt16Button = IButton::Probe(FindViewById(R::id::bt_setInt16));
    mGetInt32Button = IButton::Probe(FindViewById(R::id::bt_getInt32));
    mSetInt32Button = IButton::Probe(FindViewById(R::id::bt_setInt32));
    mGetInt64Button = IButton::Probe(FindViewById(R::id::bt_getInt64));
    mSetInt64Button = IButton::Probe(FindViewById(R::id::bt_setInt64));
    mGetStringButton = IButton::Probe(FindViewById(R::id::bt_getString));
    mSetStringButton = IButton::Probe(FindViewById(R::id::bt_setString));
    mGetFloatButton = IButton::Probe(FindViewById(R::id::bt_getFloat));
    mSetFloatButton = IButton::Probe(FindViewById(R::id::bt_setFloat));
    mGetDoubleButton = IButton::Probe(FindViewById(R::id::bt_getDouble));
    mSetDoubleButton = IButton::Probe(FindViewById(R::id::bt_setDouble));
    mGetInterfaceButton = IButton::Probe(FindViewById(R::id::bt_getInterface));
    mSetInterfaceButton = IButton::Probe(FindViewById(R::id::bt_setInterface));

    mTextView = ITextView::Probe(FindViewById(R::id::tx_result));
    mGetByteButton->SetOnClickListener(l);
    mSetByteButton->SetOnClickListener(l);
    mGetChar8Button->SetOnClickListener(l);
    mSetChar8Button->SetOnClickListener(l);
    mGetChar16Button->SetOnClickListener(l);
    mSetChar16Button->SetOnClickListener(l);
    mGetBooleanButton->SetOnClickListener(l);
    mSetBooleanButton->SetOnClickListener(l);
    mGetInt16Button->SetOnClickListener(l);
    mSetInt16Button->SetOnClickListener(l);
    mGetInt32Button->SetOnClickListener(l);
    mSetInt32Button->SetOnClickListener(l);
    mGetInt64Button->SetOnClickListener(l);
    mSetInt64Button->SetOnClickListener(l);
    mGetStringButton->SetOnClickListener(l);
    mSetStringButton->SetOnClickListener(l);
    mGetFloatButton->SetOnClickListener(l);
    mSetFloatButton->SetOnClickListener(l);
    mGetDoubleButton->SetOnClickListener(l);
    mSetDoubleButton->SetOnClickListener(l);
    mGetInterfaceButton->SetOnClickListener(l);
    mSetInterfaceButton->SetOnClickListener(l);

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnStart");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnResume");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    PRINT_ENTER_LEAVE("CActivityOne::OnPause");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    PRINT_ENTER_LEAVE("OnStop");
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    PRINT_ENTER_LEAVE("OnDestroy");
    // TODO: Add your code here
    return NOERROR;
}

} // namespace PropertyDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
