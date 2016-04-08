#ifndef __PATTTERN_TEST_H
#define __PATTTERN_TEST_H

#include <elastos.h>
#include "Org.Xml.Sax.h"

using namespace Elastos;
using namespace Org::Xml::Sax;
using namespace Org::Xml::Sax::Helpers;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest(){};
    ~CTest(){};

public:
    int testNoPrefixesNoNamespaces(int argc, char* argv[]);
    int testNoPrefixesYesNamespaces(int argc, char* argv[]);
    int testYesPrefixesYesNamespaces(int argc, char* argv[]);
    int testYesPrefixesNoNamespaces(int argc, char* argv[]);
    int testDisableExternalGeneralEntities(int argc, char* argv[]);
    int testDisableExternalParameterEntities(int argc, char* argv[]);

private:
    void testDisableExternalEntities(String feature, String xml);
    void parse(Boolean prefixes, Boolean namespaces, String xml, AutoPtr<IContentHandler> handler);
    void assertOneOf(String expected, String sentinel, String actual);
};


#endif //__PATTTERN_TEST_H
