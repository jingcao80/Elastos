//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ORG_XMLPULL_V1_XMLPULLPARSERFACTORY_H__
#define __ORG_XMLPULL_V1_XMLPULLPARSERFACTORY_H__

#include "Elastos.CoreLibrary.External.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;
using Org::Xmlpull::V1::IXmlPullParserFactory;

namespace Org {
namespace Xmlpull {
namespace V1 {

/**
 * This class is used to create implementations of XML Pull Parser defined in XMPULL V1 API.
 *
 * @see XmlPullParser
 *
 * @author <a href="http://www.extreme.indiana.edu/~aslom/">Aleksander Slominski</a>
 * @author Stefan Haustein
 */
class XmlPullParserFactory
    : public Object
    , public IXmlPullParserFactory
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean state);

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean* isGot);

    CARAPI SetNamespaceAware(
        /* [in] */ Boolean awareness);

    CARAPI IsNamespaceAware(
        /* [out] */ Boolean* isNspAware);

    CARAPI SetValidating(
        /* [in] */ Boolean validating);

    CARAPI IsValidating(
        /* [out] */ Boolean* isValidating);

    /**
     * Creates a new instance of a XML Pull Parser
     * using the currently configured factory features.
     *
     * @return A new instance of a XML Pull Parser.
     */
    CARAPI NewPullParser(
        /* [out] */ IXmlPullParser** pullParser);

    CARAPI NewSerializer(
        /* [out] */ IXmlSerializer** serial);

    static CARAPI NewInstance(
        /* [out] */ IXmlPullParserFactory** xmlPullParserFac);

    static CARAPI NewInstance(
        /* [in] */ const String& classNames,
     // /* [in] */  Class context,
        /* [out] */ IXmlPullParserFactory** xmlPullParserFac);

private:
    ECode GetParserInstance(
        /* [out] */ IXmlPullParser** parser);

    ECode GetSerializerInstance(
        /* [out] */ IXmlSerializer** serializer);

public:
    static const String PROPERTY_NAME; // = "org.xmlpull.v1.XmlPullParserFactory";

protected:
    String mClassNamesLocation;

    AutoPtr<IArrayList> mParserClasses;
    AutoPtr<IArrayList> mSerializerClasses;

    // features are kept there
    HashMap<String, Boolean> mFeatures;
private:
    // /** used as default class to server as context class in newInstance() */
    // final static Class referenceContextClass;

    // static {
    //     XmlPullParserFactory f = new XmlPullParserFactory();
    //     referenceContextClass = f.getClass();
    // }

    static const String RESOURCE_NAME; // = "/META-INF/services/" + PROPERTY_NAME;
};

} // namespace V1
} // namespace Xmlpull
} // namespace Org

#endif // __ORG_XMLPULL_V1_XMLPULLPARSERFACTORY_H__
