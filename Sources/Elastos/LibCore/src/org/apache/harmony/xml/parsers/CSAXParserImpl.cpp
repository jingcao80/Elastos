#include "org/apache/harmony/xml/parsers/CSAXParserImpl.h"
#include "org/xml/sax/helpers/CXMLReaderAdapter.h"
#include "org/apache/harmony/xml/CExpatReader.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/Collections.h"

using Org::Xml::Sax::Helpers::CXMLReaderAdapter;
using Elastos::Core::IBoolean;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Collections;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

ECode CSAXParserImpl::constructor(
    /* [in] */ IMap* features)
{
    Boolean isEmpty;
    features->IsEmpty(&isEmpty);
    //this.initialFeatures = initialFeatures.isEmpty()
    //    ? Collections.<String, Boolean>emptyMap()
    //    : new HashMap<String, Boolean>(initialFeatures);
    if (isEmpty) {
        initialFeatures = new Collections::EmptyMap();
    }
    else {
        CHashMap::New(features, (IMap**)&initialFeatures);
    }
    ResetInternal();
    return NOERROR;
}

ECode CSAXParserImpl::Reset()
{
    /*
     * The exceptions are impossible. If any features are unrecognized or
     * unsupported, construction of this instance would have failed.
     */
    //try {
    return ResetInternal();
    //} catch (SAXNotRecognizedException e) {
    //    throw new AssertionError();
    //} catch (SAXNotSupportedException e) {
    //    throw new AssertionError();
    //}
}

ECode CSAXParserImpl::ResetInternal()
{
    CExpatReader::New((IXMLReader**)&reader);
    AutoPtr<ISet> entrySet;
    initialFeatures->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(p);
        AutoPtr<IInterface> v;
        entry->GetValue((IInterface**)&v);
        AutoPtr<IInterface> k;
        entry->GetKey((IInterface**)&k);

        AutoPtr<ICharSequence> ics = ICharSequence::Probe(k);
        String keyStr;
        ics->ToString(&keyStr);

        IBoolean* ib = IBoolean::Probe(v);
        Boolean value;
        ib->GetValue(&value);

        reader->SetFeature(keyStr, value);
    }
    return NOERROR;
}

ECode CSAXParserImpl::GetParser(
    /* [out] */ Org::Xml::Sax::IParser ** ppParser)
{
    VALIDATE_NOT_NULL(ppParser);
    *ppParser = NULL;
    if (parser == NULL) {
        //parser = new XMLReaderAdapter(reader);
        CXMLReaderAdapter::New(reader, (IParser**)&parser);
    }

    *ppParser = parser;
    REFCOUNT_ADD(*ppParser);
    return NOERROR;
}

ECode CSAXParserImpl::GetXMLReader(
    /* [out] */ Org::Xml::Sax::IXMLReader ** ppReader)
{
    VALIDATE_NOT_NULL(ppReader);
    *ppReader = reader;
    REFCOUNT_ADD(*ppReader);
    return NOERROR;
}

ECode CSAXParserImpl::IsNamespaceAware(
    /* [out] */ Boolean * pIsAware)
{
    VALIDATE_NOT_NULL(pIsAware);
    *pIsAware = FALSE;
    //try {
    return reader->GetFeature(String("http://xml.org/sax/features/namespaces"), pIsAware);
    //} catch (SAXException ex) {
    //    return false;
    //}
}

ECode CSAXParserImpl::IsValidating(
    /* [out] */ Boolean * pIsValidating)
{
    VALIDATE_NOT_NULL(pIsValidating);
    *pIsValidating = FALSE;
    return NOERROR;
}

ECode CSAXParserImpl::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ IInterface * pValue)
{
    return reader->SetProperty(name, pValue);
}

ECode CSAXParserImpl::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ IInterface ** ppProperty)
{
    return reader->GetProperty(name, ppProperty);
}

}
}
}
}
}

