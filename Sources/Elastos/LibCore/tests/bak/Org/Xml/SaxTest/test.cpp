#include "test.h"
#include <stdio.h>
#include <Elastos.CoreLibrary.h>
#include <elautoptr.h>

using namespace Elastos;
using namespace Elastos::Core;

int CTest::testNoPrefixesNoNamespaces(int argc, char* argv[])
{
    // new DefaultHandler() {
    //     @Override public void startElement(String uri, String localName,
    //             String qName, Attributes attributes) {
    //         assertEquals("", uri);
    //         assertEquals("", localName);
    //         assertEquals("foo", qName);
    //         assertEquals(1, attributes.getLength());
    //         assertEquals("", attributes.getURI(0));
    //         assertOneOf("bar", "", attributes.getLocalName(0));
    //         assertEquals("bar", attributes.getQName(0));
    //     }
    //     }
    AutoPtr<IEntityResolver> er;
    CDefaultHandler::New((IEntityResolver**)&er);
    AutoPtr<IContentHandler> ch = (IContentHandler*)er->Probe(EIID_IContentHandler);
    parse(false, false, String("<foo bar=\"baz\"/>"), ch);

    // parse(false, false, "<a:foo a:bar=\"baz\"/>", new DefaultHandler() {
    //     @Override public void startElement(String uri, String localName,
    //             String qName, Attributes attributes) {
    //         assertEquals("", uri);
    //         assertEquals("", localName);
    //         assertEquals("a:foo", qName);
    //         assertEquals(1, attributes.getLength());
    //         assertEquals("", attributes.getURI(0));
    //         assertOneOf("a:bar", "", attributes.getLocalName(0));
    //         assertEquals("a:bar", attributes.getQName(0));
    //     }
    // });
    return 0;
}

int CTest::testNoPrefixesYesNamespaces(int argc, char* argv[])
{
    // parse(false, true, "<foo bar=\"baz\"/>", new DefaultHandler() {
    //     @Override public void startElement(String uri, String localName,
    //             String qName, Attributes attributes) {
    //         assertEquals("", uri);
    //         assertEquals("foo", localName);
    //         assertEquals("foo", qName);
    //         assertEquals(1, attributes.getLength());
    //         assertEquals("", attributes.getURI(0));
    //         assertEquals("bar", attributes.getLocalName(0));
    //         assertEquals("bar", attributes.getQName(0));
    //     }
    // });

    // parse(false, true, "<a:foo a:bar=\"baz\" xmlns:a=\"http://quux\"/>", new DefaultHandler() {
    //     @Override public void startElement(String uri, String localName,
    //             String qName, Attributes attributes) {
    //         assertEquals("http://quux", uri);
    //         assertEquals("foo", localName);
    //         assertEquals("a:foo", qName);
    //         assertEquals(1, attributes.getLength());
    //         assertEquals("http://quux", attributes.getURI(0));
    //         assertEquals("bar", attributes.getLocalName(0));
    //         assertEquals("a:bar", attributes.getQName(0));
    //     }
    // });
    return 0;
}

int CTest::testYesPrefixesYesNamespaces(int argc, char* argv[])
{
    // parse(true, true, "<foo bar=\"baz\"/>", new DefaultHandler() {
    //     @Override public void startElement(String uri, String localName,
    //             String qName, Attributes attributes) {
    //         assertEquals("", uri);
    //         assertEquals("foo", localName);
    //         assertEquals("foo", qName);
    //         assertEquals(1, attributes.getLength());
    //         assertEquals("", attributes.getURI(0));
    //         assertEquals("bar", attributes.getLocalName(0));
    //         assertEquals("bar", attributes.getQName(0));
    //     }
    // });

    // parse(true, true, "<a:foo a:bar=\"baz\" xmlns:a=\"http://quux\"/>", new DefaultHandler() {
    //     @Override public void startElement(String uri, String localName,
    //             String qName, Attributes attributes) {
    //         assertEquals("http://quux", uri);
    //         assertEquals("foo", localName);
    //         assertEquals("a:foo", qName);
    //         assertEquals(2, attributes.getLength());
    //         assertEquals("http://quux", attributes.getURI(0));
    //         assertEquals("bar", attributes.getLocalName(0));
    //         assertEquals("a:bar", attributes.getQName(0));
    //         assertEquals("", attributes.getURI(1));
    //         assertEquals("", attributes.getLocalName(1));
    //         assertEquals("xmlns:a", attributes.getQName(1));
    //     }
    // });
    return 0;
}

int CTest::testYesPrefixesNoNamespaces(int argc, char* argv[])
{
    // parse(true, false, "<foo bar=\"baz\"/>", new DefaultHandler() {
    //     @Override public void startElement(String uri, String localName,
    //             String qName, Attributes attributes) {
    //         assertEquals("", uri);
    //         assertEquals("", localName);
    //         assertEquals("foo", qName);
    //         assertEquals(1, attributes.getLength());
    //         assertEquals("", attributes.getURI(0));
    //         assertOneOf("bar", "", attributes.getLocalName(0));
    //         assertEquals("bar", attributes.getQName(0));
    //     }
    // });

    // parse(true, false, "<a:foo a:bar=\"baz\"/>", new DefaultHandler() {
    //     @Override public void startElement(String uri, String localName,
    //             String qName, Attributes attributes) {
    //         assertEquals("", uri);
    //         assertEquals("", localName);
    //         assertEquals("a:foo", qName);
    //         assertEquals(1, attributes.getLength());
    //         assertEquals("", attributes.getURI(0));
    //         assertOneOf("a:bar", "", attributes.getLocalName(0));
    //         assertEquals("a:bar", attributes.getQName(0));
    //     }
    // });
    return 0;
}

int CTest::testDisableExternalGeneralEntities(int argc, char* argv[])
{
    // String xml = "<!DOCTYPE foo ["
    //         + "  <!ENTITY bar SYSTEM \"/no-such-document.xml\">"
    //         + "]>"
    //         + "<foo>&bar;</foo>";
    // testDisableExternalEntities("http://xml.org/sax/features/external-general-entities", xml);
    return 0;
}

int CTest::testDisableExternalParameterEntities(int argc, char* argv[])
{
    // String xml = "<!DOCTYPE foo ["
    //         + "  <!ENTITY % bar SYSTEM \"/no-such-document.xml\">"
    //         + "  %bar;"
    //         + "]>"
    //         + "<foo/>";
    // testDisableExternalEntities("http://xml.org/sax/features/external-parameter-entities", xml);
    return 0;
}

void CTest::testDisableExternalEntities(String feature, String xml)
{
    // SAXParser parser = SAXParserFactory.newInstance().newSAXParser();
    // XMLReader reader = parser.getXMLReader();
    // reader.setFeature(feature, false);
    // assertFalse(reader.getFeature(feature));
    // reader.setContentHandler(new ThrowingHandler() {
    //     @Override public void startElement(
    //             String uri, String localName, String qName, Attributes attributes) {
    //         assertEquals("foo", qName);
    //     }
    //     @Override public void endElement(String uri, String localName, String qName) {
    //         assertEquals("foo", qName);
    //     }
    // });
    // reader.parse(new InputSource(new StringReader(xml)));
}

void CTest::parse(Boolean prefixes, Boolean namespaces, String xml, AutoPtr<IContentHandler> handler)
{
    // SAXParser parser = SAXParserFactory.newInstance().newSAXParser();
    // XMLReader reader = parser.getXMLReader();
    // reader.setFeature("http://xml.org/sax/features/namespace-prefixes", prefixes);
    // reader.setFeature("http://xml.org/sax/features/namespaces", namespaces);
    // reader.setContentHandler(handler);
    // reader.parse(new InputSource(new StringReader(xml)));
}

void CTest::assertOneOf(String expected, String sentinel, String actual)
{
    // List<String> optionsList = Arrays.asList(sentinel, expected);
    // assertTrue("Expected one of " + optionsList + " but was " + actual,
    //         optionsList.contains(actual));
}
