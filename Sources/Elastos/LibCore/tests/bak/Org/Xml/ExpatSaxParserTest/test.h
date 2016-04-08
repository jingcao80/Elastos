#ifndef __EXPATSAXPARSER_TEST_H
#define __EXPATSAXPARSER_TEST_H

#include <elastos.h>
#include "Org.Xml.Sax.h"
#include "Elastos.CoreLibrary.h"
#include "elastos/StringBuilder.h"
#include "elastos/StringBuffer.h"
#include "elastos/StringUtils.h"
#include "elastos/HashMap.h"
#include "elastos/List.h"

using namespace Elastos;
using namespace Org::Xml::Sax;
using namespace Org::Xml::Sax::Ext;
using namespace Org::Xml::Sax::Helpers;
using namespace Elastos::Core;
using namespace Elastos::IO;

#define TEST(a, x) a.test_##x

class CTest {
public:
    class TestHandler : public CDefaultHandler
    {
    public:
        TestHandler(){};
        ~TestHandler(){};

        String startElementName;
        String endElementName;
        StringBuilder text;

        // @Override
        void StartElement(String uri, String localName, String qName, IAttributes* attributes) {

            assert(startElementName.IsNull());
            startElementName = localName;

            // Validate attributes.
            Int32 length = 0;
            attributes->GetLength(&length);
            assert(1 == length);
            String outstr;
            attributes->GetURI(0, &outstr);
            assert("" == outstr);
            attributes->GetLocalName(0, &outstr);
            assert("dad" == outstr);
            attributes->GetValue(0, &outstr);
            assert("bob" == outstr);
            attributes->GetIndex(String(""), String("dad"), &length);
            assert(0 == length);
            attributes->GetValueEx(String(""), String("dad"), &outstr);
            assert("bob" == outstr);
        }

        // @Override
        void EndElement(String uri, String localName, String qName) {
            assert(endElementName.IsNull());
            endElementName = localName;
        }

        // @Override
        void Characters(ArrayOf<Char32>* ch, int start, int length) {
            text.AppendCharsEx(*ch, start, length);
        }
    };

    CTest(){};
    ~CTest(){};

public:
    int testGlobalReferenceTableOverflow(int argc, char* argv[]);
    int testExceptions(int argc, char* argv[]);
    int testSax(int argc, char* argv[]);
    int testNamespaces(int argc, char* argv[]);
    int testDtdDoctype(int argc, char* argv[]);
    int testDtdUnparsedEntity_system(int argc, char* argv[]);
    int testDtdUnparsedEntity_public(int argc, char* argv[]);
    int testDtdNotation_system(int argc, char* argv[]);
    int testDtdNotation_public(int argc, char* argv[]);
    int testCdata(int argc, char* argv[]);
    int testProcessingInstructions(int argc, char* argv[]);
    int testExternalEntity(int argc, char* argv[]);
    int testExternalEntityDownload(int argc, char* argv[]);

private:


private:
    void validate(TestHandler* handler);
    void parse(String xml, AutoPtr<IContentHandler> contentHandler);
    void parse(AutoPtr<IReader> in, AutoPtr<IContentHandler> contentHandler);
    void parse(AutoPtr<IInputStream> in, String encoding, AutoPtr<IContentHandler> contentHandler);

private:
    static const String SNIPPET; // = "<dagny dad=\"bob\">hello</dagny>";
    static const String XML; // = "<one xmlns='ns:default' xmlns:n1='ns:1' a='b'>\n" + "  <n1:two c='d' n1:e='f' xmlns:n2='ns:2'>text</n1:two>\n" + "</one>";
};

#endif //__EXPATSAXPARSER_TEST_H
