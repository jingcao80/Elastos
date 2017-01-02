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

#include "CXMLReaderFactory.h"
#include "NewInstanceHelper.h"
#include "CParserFactory.h"
#include "CParserAdapter.h"
#include "CSystem.h"
#include "CBufferedReader.h"
#include "CInputStreamReader.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::IInputStream;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IReader;
using Elastos::IO::Charset::ICharset;
//using Elastos::IO::Charset::IStandardCharsets;
//using Elastos::IO::Charset::CStandardCharsets;

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

const String XMLReaderFactory::property("org.xml.sax.driver");

CAR_INTERFACE_IMPL(CXMLReaderFactory, Singleton, IXMLReaderFactory)

CAR_SINGLETON_IMPL(CXMLReaderFactory)

ECode CXMLReaderFactory::CreateXMLReader(
    /* [out] */ IXMLReader** reader)
{
    return XMLReaderFactory::CreateXMLReader(reader);
}

ECode CXMLReaderFactory::CreateXMLReader(
    /* [in] */ const String& className,
    /* [out] */ IXMLReader** reader)
{
    return XMLReaderFactory::CreateXMLReader(className,reader);
}

ECode XMLReaderFactory::CreateXMLReader(
    /* [out] */ IXMLReader** xmlreader)
{
    VALIDATE_NOT_NULL(xmlreader)

    AutoPtr<IClassLoader> loader = NewInstanceHelper::GetClassLoader();

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    // 1. try the JVM-instance-wide system property
    // try {
    String className;
    system->GetProperty(property, &className);
    // }
    // catch (RuntimeException e) { /* normally fails for applets */ }

    // 2. if that fails, try META-INF/services/
    if (className.IsNull()) {
        // try {
        String service = String("META-INF/services/") + property;
        AutoPtr<IInputStream> in;
        AutoPtr<IBufferedReader> reader;

        if (loader == NULL)
            assert(0 && "TODO");
            // in = ClassLoader.getSystemResourceAsStream (service);
        else
            assert(0 && "TODO");
            // in = loader.getResourceAsStream (service);

        if (in != NULL) {
            // AutoPtr<IStandardCharsets> csets;
            // CStandardCharsets::AcquireSingleton((IStandardCharsets**)&csets);
            AutoPtr<ICharset> charset;
            // csets->GetUTF_8((ICharset**)&charset);
            String utf8str;
            charset->GetName(&utf8str);
            AutoPtr<IReader> isr;
            FAIL_RETURN(CInputStreamReader::New(in, utf8str, (IReader**)&isr));
            FAIL_RETURN(CBufferedReader::New(isr, (IBufferedReader**)&reader));
            reader->ReadLine(&className);
            in->Close ();
        }
        // } catch (Exception e) {
        // }
    }

    // 3. Distro-specific fallback
    if (className.IsNull()) {
    // BEGIN DISTRIBUTION-SPECIFIC

            // EXAMPLE:
            // className = "com.example.sax.XmlReader";
            // or a $JAVA_HOME/jre/lib/*properties setting...

    // END DISTRIBUTION-SPECIFIC
    }

    // do we know the XMLReader implementation class yet?
    if (!className.IsNull())
        return LoadClass(loader, className, xmlreader);

    // 4. panic -- adapt any SAX1 parser
    // try {
    AutoPtr<IXMLReader> outreader;
    AutoPtr<IParser> outparser;
    ParserFactory::MakeParser((IParser**)&outparser);

    CParserAdapter::New(outparser, (IXMLReader**)&outreader);
    *xmlreader = outreader;
    REFCOUNT_ADD(*xmlreader)
    // } catch (Exception e) {
    //     throw new SAXException ("Can't create default XMLReader; "
    //         + "is system property org.xml.sax.driver set?");
    // }
    return NOERROR;
}

ECode XMLReaderFactory::CreateXMLReader(
    /* [in] */ const String& className,
    /* [out] */ IXMLReader** reader)
{
    AutoPtr<IClassLoader> loader = NewInstanceHelper::GetClassLoader();
    return LoadClass(loader, className, reader);
}

ECode XMLReaderFactory::LoadClass(
    /* [in] */ IClassLoader* loader,
    /* [in] */ const String& className,
    /* [out] */ IXMLReader** xmlReader)
{
    VALIDATE_NOT_NULL(xmlReader)
    // try {
    AutoPtr<IXMLReader> outreader = (IXMLReader*) NewInstanceHelper::NewInstance(loader, className)->Probe(EIID_IXMLReader);
    if (outreader == NULL)
    {
        return E_SAX_EXCEPTION;
    }
    // } catch (ClassNotFoundException e1) {
    //     throw new SAXException("SAX2 driver class " + className +
    //                " not found", e1);
    // } catch (IllegalAccessException e2) {
    //     throw new SAXException("SAX2 driver class " + className +
    //                " found but cannot be loaded", e2);
    // } catch (InstantiationException e3) {
    //     throw new SAXException("SAX2 driver class " + className +
    //    " loaded but cannot be instantiated (no empty public constructor?)",
    //                e3);
    // } catch (ClassCastException e4) {
    //     throw new SAXException("SAX2 driver class " + className +
    //                " does not implement XMLReader", e4);
    // }
    *xmlReader = outreader;
    REFCOUNT_ADD(*xmlReader)
    return NOERROR;
}

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org
