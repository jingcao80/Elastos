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

#include "XmlPullParserFactory.h"
#include "CXmlPullParserFactory.h"
#include "CPathClassLoader.h"
#include "CArrayList.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::IClassLoader;
using Elastos::Core::CPathClassLoader;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Xmlpull {
namespace V1 {

const String XmlPullParserFactory::PROPERTY_NAME("org.xmlpull.v1.XmlPullParserFactory");
const String XmlPullParserFactory::RESOURCE_NAME("/META-INF/services/org.xmlpull.v1.XmlPullParserFactory");

CAR_INTERFACE_IMPL(XmlPullParserFactory, Object, IXmlPullParserFactory)


ECode XmlPullParserFactory::constructor()
{
    CArrayList::New((IArrayList**)&mParserClasses);
    CArrayList::New((IArrayList**)&mSerializerClasses);

    AutoPtr<IClassLoader> cl;
    CPathClassLoader::New(String("/system/lib/Elastos.CoreLibrary.eco"), NULL, (IClassLoader**)&cl);
    AutoPtr<IClassInfo> ci;
    cl->LoadClass(String("Org.Kxml2.IO.CKXmlParser"), (IClassInfo**)&ci);
    mParserClasses->Add(ci);
    ci = NULL;
    cl->LoadClass(String("Org.Kxml2.IO.CKXmlSerializer"), (IClassInfo**)&ci);
    mSerializerClasses->Add(ci);
    return NOERROR;
}

ECode XmlPullParserFactory::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean state)
{
    mFeatures[name] = state;
    return NOERROR;
}

ECode XmlPullParserFactory::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean* isGot)
{
    VALIDATE_NOT_NULL(isGot)

    HashMap<String, Boolean>::Iterator iter = mFeatures.Find(name);
    *isGot = iter != mFeatures.End() ? iter->mSecond : FALSE;
    return NOERROR;
}

ECode XmlPullParserFactory::SetNamespaceAware(
    /* [in] */ Boolean awareness)
{
    mFeatures[IXmlPullParser::FEATURE_PROCESS_NAMESPACES] = awareness;
    return NOERROR;
}

ECode XmlPullParserFactory::IsNamespaceAware(
    /* [out] */ Boolean* isNspAware)
{
    return GetFeature(IXmlPullParser::FEATURE_PROCESS_NAMESPACES, isNspAware);
}

ECode XmlPullParserFactory::SetValidating(
    /* [in] */ Boolean validating)
{
    mFeatures[IXmlPullParser::FEATURE_VALIDATION] = validating;
    return NOERROR;
}

ECode XmlPullParserFactory::IsValidating(
    /* [out] */ Boolean* isValidating)
{
    VALIDATE_NOT_NULL(isValidating)

    return GetFeature(IXmlPullParser::FEATURE_VALIDATION, isValidating);
}

ECode XmlPullParserFactory::NewPullParser(
    /* [out] */ IXmlPullParser** pullParser)
{
    AutoPtr<IXmlPullParser> pp;
    FAIL_RETURN(GetParserInstance((IXmlPullParser**)&pp))
    if (pp) {
        HashMap<String, Boolean>::Iterator it;
        for (it = mFeatures.Begin(); it != mFeatures.End(); ++it) {
            // NOTE: This test is needed for compatibility reasons. We guarantee
            // that we only set a feature on a parser if its value is true.
            if (it->mSecond) {
                pp->SetFeature(it->mFirst, it->mSecond);
            }
        }
    }

    *pullParser = pp;
    REFCOUNT_ADD(*pullParser)
    return NOERROR;
}

ECode XmlPullParserFactory::NewSerializer(
    /* [out] */ IXmlSerializer** serial)
{
    return GetSerializerInstance(serial);
}

ECode XmlPullParserFactory::GetParserInstance(
    /* [out] */ IXmlPullParser** parser)
{
    VALIDATE_NOT_NULL(parser)
    *parser = NULL;

    if (mParserClasses) {
        AutoPtr<IIterator> it;
        mParserClasses->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            if (obj != NULL) {
                AutoPtr<IClassInfo> ci = IClassInfo::Probe(obj);
                AutoPtr<IInterface> object;
                ECode ec = ci->CreateObject((IInterface**)&object);
                if (FAILED(ec)) continue;
                *parser = IXmlPullParser::Probe(object);
                REFCOUNT_ADD(*parser)
                return NOERROR;
            }
        }
    }

    Logger::E("XmlPullParserFactory", "Invalid parser class list.");
    return E_FAIL;
}

ECode XmlPullParserFactory::GetSerializerInstance(
    /* [out] */ IXmlSerializer** serializer)
{
    VALIDATE_NOT_NULL(serializer)
    *serializer = NULL;

    assert(0 && "TODO");
    // ArrayList<Exception> exceptions = null;

    // if (serializerClasses != null && !serializerClasses.isEmpty()) {
    //     exceptions = new ArrayList<Exception>();
    //     for (Object o : serializerClasses) {
    //         try {
    //             if (o != null) {
    //                 Class<?> serializerClass = (Class<?>) o;
    //                 return (XmlSerializer) serializerClass.newInstance();
    //             }
    //         } catch (InstantiationException e) {
    //             exceptions.add(e);
    //         } catch (IllegalAccessException e) {
    //             exceptions.add(e);
    //         } catch (ClassCastException e) {
    //             exceptions.add(e);
    //         }
    //     }
    // }

    // throw newInstantiationException("Invalid serializer class list", exceptions);
    return NOERROR;
}

// private static XmlPullParserException newInstantiationException(String message,
//         ArrayList<Exception> exceptions) {
//     if (exceptions == null || exceptions.isEmpty()) {
//         return new XmlPullParserException(message);
//     } else {
//         XmlPullParserException exception = new XmlPullParserException(message);
//         for (Exception ex : exceptions) {
//             exception.addSuppressed(ex);
//         }

//         return exception;
//     }
// }

ECode XmlPullParserFactory::NewInstance(
    /* [out] */ IXmlPullParserFactory** xmlPullParserFac)
{
    VALIDATE_NOT_NULL(xmlPullParserFac)

    AutoPtr<IXmlPullParserFactory> factory;
    CXmlPullParserFactory::New((IXmlPullParserFactory**)&factory);
    *xmlPullParserFac = factory;
    REFCOUNT_ADD(*xmlPullParserFac);
    return NOERROR;
}

ECode XmlPullParserFactory::NewInstance(
    /* [in] */ const String& classNames,
 // /* [in] */  Class context,
    /* [out] */ IXmlPullParserFactory** xmlPullParserFac)
{
    return NewInstance(xmlPullParserFac);
}

} // namespace V1
} // namespace Xmlpull
} // namespace Org
