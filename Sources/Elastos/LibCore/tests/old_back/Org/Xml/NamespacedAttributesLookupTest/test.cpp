#include "test.h"

const String CTest::SAX_PROPERTY_NS = String("http://xml.org/sax/features/namespaces");
const String CTest::SAX_PROPERTY_NS_PREFIXES = String("http://xml.org/sax/features/namespace-prefixes");

String CTest::xml = String("<?xml version='1.0' encoding='UTF-8'?>") +
                    String("<test xmlns='http://foo' xmlns:bar='http://bar' xmlns:baz='http://baz' baz:c='a'>") +
                    String("<b c='w' bar:c='x'/>") +
                    String("<bar:e baz:c='y' bar:c='z'/>") +
                    String("</test>");

int CTest::testNamespace(int argc, char* argv[])
{
    List<String> expected;
    expected.PushBack(
            String("http://foo,test\n") +
            String("  http://baz,c\n") +
            String("  http://bar+c=null,\n") +
            String("  bar:c=null\n"));
    expected.PushBack(
            String("http://foo,b\n") +
            String("  ,c\n") +
            String("  http://bar,c\n") +
            String("  http://bar+c=x,\n") +
            String("  bar:c=x\n"));
    expected.PushBack(
            String("http://bar,e\n") +
            String("  http://baz,c\n") +
            String("  http://bar,c\n") +
            String("  http://bar+c=z,\n") +
            String("  bar:c=z\n"));

    Boolean names = true;
    Boolean namespacePrefixes = false;
    AutoPtr<List<String> > outlist = getStartElements(xml, names, namespacePrefixes);
    // assertEquals(expected == outlist);
    return 0;
}

int CTest::testNamespacePrefixes(int argc, char* argv[])
{
    List<String> expected;
    expected.PushBack(
            String("test\n") +
            String("  xmlns\n") +
            String("  xmlns:bar\n") +
            String("  xmlns:baz\n") +
            String("  baz:c\n") +
            String("  http://bar+c=null,\n") +
            String("  bar:c=null\n"));
    expected.PushBack(
            String("b\n") +
            String("  c\n") +
            String("  bar:c\n") +
            String("  http://bar+c=null,\n") +
            String("  bar:c=x\n"));
    expected.PushBack(
            String("bar:e\n") +
            String("  baz:c\n") +
            String("  bar:c\n") +
            String("  http://bar+c=null,\n") +
            String("  bar:c=z\n"));

    Boolean names = false;
    Boolean namespacePrefixes = true;
    AutoPtr<List<String> > outlist = getStartElements(xml, names, namespacePrefixes);
    // assertEquals(expected == outlist);
    return 0;
}

AutoPtr<List<String> > CTest::getStartElements(String xml, const Boolean names, Boolean namespacePrefixes)
{
    // final List<String> result = new ArrayList<String>();
    // XMLReader reader = SAXParserFactory.newInstance().newSAXParser().getXMLReader();
    // reader.setFeature(SAX_PROPERTY_NS, namespace);
    // reader.setFeature(SAX_PROPERTY_NS_PREFIXES, namespacePrefixes);
    // reader.setContentHandler(new DefaultHandler() {
    //     @Override public final void startElement(
    //             String uri, String localName, String qName, Attributes attributes) {
    //         StringBuilder serialized = new StringBuilder();
    //         /*
    //          * Only supply the uri+localName or qname depending on whether namespaces are
    //          * enabled. It's an optional parameter and the RI only supplies one or the other.
    //          */
    //         if (namespace) {
    //             serialized.append(uri).append(",");
    //             serialized.append(localName);
    //         } else {
    //             serialized.append(qName);
    //         }
    //         for (int i = 0; i < attributes.getLength(); i++) {
    //             serialized.append("\n  ");
    //             if (namespace) {
    //                 serialized.append(attributes.getURI(i)).append(",");
    //                 serialized.append(attributes.getLocalName(i));
    //             } else {
    //                 serialized.append(attributes.getQName(i));
    //             }
    //         }
    //         serialized.append("\n  http://bar+c=")
    //                 .append(attributes.getValue("http://bar", "c")).append(",")
    //                 .append("\n  bar:c=")
    //                 .append(attributes.getValue("bar:c"))
    //                 .append("\n");
    //         result.add(serialized.toString());
    //     }
    // });
    // reader.parse(new InputSource(new StringReader(xml)));
    // return result;
    return NULL;
}
