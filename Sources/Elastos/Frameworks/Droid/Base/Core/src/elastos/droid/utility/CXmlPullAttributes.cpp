
#include <Elastos.CoreLibrary.External.h>
#include "elastos/droid/utility/CXmlPullAttributes.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Droid::Internal::Utility::XmlUtils;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL_2(CXmlPullAttributes, Object, IXmlPullAttributes, IAttributeSet)

CAR_OBJECT_IMPL(CXmlPullAttributes)

CXmlPullAttributes::CXmlPullAttributes()
{}

ECode CXmlPullAttributes:: constructor(
    /* [in] */ IXmlPullParser* parser)
{
    VALIDATE_NOT_NULL(parser)
    mParser = parser;
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeCount(
    /* [out] */ Int32 * count)
{
    return mParser->GetAttributeCount(count);
}

ECode CXmlPullAttributes:: GetAttributeName(
    /* [in] */ Int32 index,
    /* [out] */ String * name)
{
    return mParser->GetAttributeName(index, name);
}

ECode CXmlPullAttributes:: GetAttributeValue(
    /* [in] */ Int32 index,
    /* [out] */ String * value)
{
    return mParser->GetAttributeValue(index, value);
}

ECode CXmlPullAttributes:: GetAttributeValue(
    /* [in] */ const String& namespace1,
    /* [in] */ const String& name,
    /* [out] */ String * value)
{
    return mParser->GetAttributeValue(namespace1, name, value);
}

ECode CXmlPullAttributes:: GetPositionDescription(
    /* [out] */ String * des)
{
    return mParser->GetPositionDescription(des);
}

ECode CXmlPullAttributes:: GetAttributeNameResource(
    /* [in] */ Int32 index,
    /* [out] */ Int32 * resId)
{
    VALIDATE_NOT_NULL(resId)
    *resId = 0;
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeListValue(
    /* [in] */ const String& namespace1,
    /* [in] */ const String& attribute,
    /* [in] */ ArrayOf<String>*  options,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32 * index)
{
    VALIDATE_NOT_NULL(index)
    String value;
    GetAttributeValue(namespace1, attribute, &value);
    AutoPtr<ICharSequence> cs;
    CString::New(value, (ICharSequence**)&cs);
    *index = XmlUtils::ConvertValueToList(cs, *options, defaultValue);
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeBooleanValue(
    /* [in] */ const String& namespace1,
    /* [in] */ const String& attribute,
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)
    String v;
    GetAttributeValue(namespace1, attribute, &v);
    AutoPtr<ICharSequence> cs;
    CString::New(v, (ICharSequence**)&cs);
    *value = XmlUtils::ConvertValueToBoolean(cs, defaultValue);;
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeResourceValue(
    /* [in] */ const String& namespace1,
    /* [in] */ const String& attribute,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    String v;
    GetAttributeValue(namespace1, attribute, &v);
    AutoPtr<ICharSequence> cs;
    CString::New(v, (ICharSequence**)&cs);
    *value = XmlUtils::ConvertValueToInt32(cs, defaultValue);;
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeIntValue(
    /* [in] */ const String& namespace1,
    /* [in] */ const String& attribute,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    String v;
    GetAttributeValue(namespace1, attribute, &v);
    AutoPtr<ICharSequence> cs;
    CString::New(v, (ICharSequence**)&cs);
    *value = XmlUtils::ConvertValueToInt32(cs, defaultValue);;
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeUnsignedIntValue(
    /* [in] */ const String& namespace1,
    /* [in] */ const String& attribute,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    String v;
    GetAttributeValue(namespace1, attribute, &v);
    AutoPtr<ICharSequence> cs;
    CString::New(v, (ICharSequence**)&cs);
    *value = XmlUtils::ConvertValueToInt32(cs, defaultValue);
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeFloatValue(
    /* [in] */ const String& namespace1,
    /* [in] */ const String& attribute,
    /* [in] */ Float defaultValue,
    /* [out] */ Float * value)
{
    VALIDATE_NOT_NULL(value)
    String v;
    GetAttributeValue(namespace1, attribute, &v);
    if (v != NULL) {
        *value = StringUtils::ParseFloat(v);
    }
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeListValue(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<String>* options,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    String v;
    GetAttributeValue(index, &v);
    AutoPtr<ICharSequence> cs;
    CString::New(v, (ICharSequence**)&cs);
    *value = XmlUtils::ConvertValueToList(cs, *options, defaultValue);
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeBooleanValue(
    /* [in] */ Int32 index,
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)
    String v;
    GetAttributeValue(index, &v);
    AutoPtr<ICharSequence> cs;
    CString::New(v, (ICharSequence**)&cs);
    *value = XmlUtils::ConvertValueToBoolean(cs, defaultValue);
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeResourceValue(
    /* [in] */ Int32 index,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    String v;
    GetAttributeValue(index, &v);
    AutoPtr<ICharSequence> cs;
    CString::New(v, (ICharSequence**)&cs);
    *value = XmlUtils::ConvertValueToInt32(cs, defaultValue);
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeIntValue(
    /* [in] */ Int32 index,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    String v;
    GetAttributeValue(index, &v);
    AutoPtr<ICharSequence> cs;
    CString::New(v, (ICharSequence**)&cs);
    *value = XmlUtils::ConvertValueToInt32(cs, defaultValue);
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeUnsignedIntValue(
    /* [in] */ Int32 index,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    String v;
    GetAttributeValue(index, &v);
    AutoPtr<ICharSequence> cs;
    CString::New(v, (ICharSequence**)&cs);
    *value = XmlUtils::ConvertValueToInt32(cs, defaultValue);
    return NOERROR;
}

ECode CXmlPullAttributes:: GetAttributeFloatValue(
    /* [in] */ Int32 index,
    /* [in] */ Float defaultValue,
    /* [out] */ Float * value)
{
    VALIDATE_NOT_NULL(value)
    *value = defaultValue;
    String v;
    GetAttributeValue(index, &v);
    if (v != NULL) {
        return StringUtils::Parse(v, value);
    }
    return NOERROR;
}

ECode CXmlPullAttributes:: GetIdAttribute(
    /* [out] */ String * attr)
{
    return GetAttributeValue(String(NULL), String("id"), attr);
}

ECode CXmlPullAttributes:: GetClassAttribute(
    /* [out] */ String * attr)
{
    return GetAttributeValue(String(NULL), String("class"), attr);
}

ECode CXmlPullAttributes:: GetIdAttributeResourceValue(
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32 * value)
{
    return GetAttributeResourceValue(String(NULL), String("id"), defaultValue, value);
}

ECode CXmlPullAttributes:: GetStyleAttribute(
    /* [out] */ Int32 * value)
{
    return GetAttributeResourceValue(String(NULL), String("style"), 0, value);
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
