#ifndef __ELASTOS_DROID_UTILITY_CXMLPULLATTRIBUTES_H__
#define __ELASTOS_DROID_UTILITY_CXMLPULLATTRIBUTES_H__

#include <Elastos.CoreLibrary.External.h>
#include "_Elastos_Droid_Utility_CXmlPullAttributes.h"
#include <elastos/core/Object.h>

using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CXmlPullAttributes)
    , public Object
    , public IXmlPullAttributes
    , public IAttributeSet
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CXmlPullAttributes();

    CARAPI constructor(
        /* [in] */ IXmlPullParser* parser);

    CARAPI GetAttributeCount(
        /* [out] */ Int32 * count);

    CARAPI GetAttributeName(
        /* [in] */ Int32 index,
        /* [out] */ String * name);

    CARAPI GetAttributeValue(
        /* [in] */ Int32 index,
        /* [out] */ String * value);

    CARAPI GetAttributeValue(
        /* [in] */ const String& namespace1,
        /* [in] */ const String& name,
        /* [out] */ String * value);

    CARAPI GetPositionDescription(
        /* [out] */ String * des);

    CARAPI GetAttributeNameResource(
        /* [in] */ Int32 index,
        /* [out] */ Int32 * resId);

    CARAPI GetAttributeListValue(
        /* [in] */ const String& namespace1,
        /* [in] */ const String& attribute,
        /* [in] */ ArrayOf<String>* options,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32 * index);

    CARAPI GetAttributeBooleanValue(
        /* [in] */ const String& namespace1,
        /* [in] */ const String& attribute,
        /* [in] */ Boolean defaultValue,
        /* [out] */ Boolean * value);

    CARAPI GetAttributeResourceValue(
        /* [in] */ const String& namespace1,
        /* [in] */ const String& attribute,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32 * value);

    CARAPI GetAttributeIntValue(
        /* [in] */ const String& namespace1,
        /* [in] */ const String& attribute,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32 * value);

    CARAPI GetAttributeUnsignedIntValue(
        /* [in] */ const String& namespace1,
        /* [in] */ const String& attribute,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32 * value);

    CARAPI GetAttributeFloatValue(
        /* [in] */ const String& namespace1,
        /* [in] */ const String& attribute,
        /* [in] */ Float defaultValue,
        /* [out] */ Float * value);

    CARAPI GetAttributeListValue(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<String>* options,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32 * value);

    CARAPI GetAttributeBooleanValue(
        /* [in] */ Int32 index,
        /* [in] */ Boolean defaultValue,
        /* [out] */ Boolean * value);

    CARAPI GetAttributeResourceValue(
        /* [in] */ Int32 index,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32 * value);

    CARAPI GetAttributeIntValue(
        /* [in] */ Int32 index,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32 * value);

    CARAPI GetAttributeUnsignedIntValue(
        /* [in] */ Int32 index,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32 * value);

    CARAPI GetAttributeFloatValue(
        /* [in] */ Int32 index,
        /* [in] */ Float defaultValue,
        /* [out] */ Float * value);

    CARAPI GetIdAttribute(
        /* [out] */ String * attr);

    CARAPI GetClassAttribute(
        /* [out] */ String * attr);

    CARAPI GetIdAttributeResourceValue(
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32 * value);

    CARAPI GetStyleAttribute(
        /* [out] */ Int32 * value);

public:
    AutoPtr<IXmlPullParser> mParser;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif
