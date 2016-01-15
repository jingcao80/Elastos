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
    int testNamespace(int argc, char* argv[]);
    int testNamespacePrefixes(int argc, char* argv[]);

public:
    AutoPtr<List<String> > getStartElements(String xml, const Boolean names, Boolean namespacePrefixes);

private:
    static const String SAX_PROPERTY_NS; // = "http://xml.org/sax/features/namespaces";
    static const String SAX_PROPERTY_NS_PREFIXES; // = "http://xml.org/sax/features/namespace-prefixes";

    static String xml; // = "<?xml version='1.0' encoding='UTF-8'?>" +
    //         "<test xmlns='http://foo' xmlns:bar='http://bar' xmlns:baz='http://baz' baz:c='a'>" +
    //         "<b c='w' bar:c='x'/>" +
    //         "<bar:e baz:c='y' bar:c='z'/>" +
    //         "</test>";
};

#endif //__EXPATSAXPARSER_TEST_H
