
#include "CXPathConstants.h"
#include "CQName.h"
#include "coredef.h"

using Elastosx::Xml::Namespace::CQName;

namespace Elastosx {
namespace Xml {
namespace XPath {

static AutoPtr<IQName> initNUMBER()
{
    AutoPtr<CQName> cq;
    CQName::NewByFriend(String("http://www.w3.org/1999/XSL/Transform"), String("NUMBER"), (CQName**)&cq);
    return (IQName*)cq.Get();
}

static AutoPtr<IQName> initSTRING()
{
    AutoPtr<CQName> cq;
    CQName::NewByFriend(String("http://www.w3.org/1999/XSL/Transform"), String("STRING"), (CQName**)&cq);
    return (IQName*)cq.Get();
}

static AutoPtr<IQName> initBOOLEAN()
{
    AutoPtr<CQName> cq;
    CQName::NewByFriend(String("http://www.w3.org/1999/XSL/Transform"), String("BOOLEAN"), (CQName**)&cq);
    return (IQName*)cq.Get();
}

static AutoPtr<IQName> initNODESET()
{
    AutoPtr<CQName> cq;
    CQName::NewByFriend(String("http://www.w3.org/1999/XSL/Transform"), String("NODESET"), (CQName**)&cq);
    return (IQName*)cq.Get();
}

static AutoPtr<IQName> initNODE()
{
    AutoPtr<CQName> cq;
    CQName::NewByFriend(String("http://www.w3.org/1999/XSL/Transform"), String("NODE"), (CQName**)&cq);
    return (IQName*)cq.Get();
}

const AutoPtr<IQName> CXPathConstants::NUMBER = initNUMBER();
const AutoPtr<IQName> CXPathConstants::STRING = initSTRING();
const AutoPtr<IQName> CXPathConstants::BOOLEAN = initBOOLEAN();
const AutoPtr<IQName> CXPathConstants::NODESET = initNODESET();
const AutoPtr<IQName> CXPathConstants::NODE = initNODE();

CAR_INTERFACE_IMPL(CXPathConstants, Object, IXPathConstants)

CAR_OBJECT_IMPL(CXPathConstants)

ECode CXPathConstants::GetQNameNUMBER(
    /* [out] */ IQName** qName)
{
    VALIDATE_NOT_NULL(qName)
    *qName = NUMBER;
    REFCOUNT_ADD(*qName);
    return NOERROR;
}

ECode CXPathConstants::GetQNameSTRING(
    /* [out] */ IQName** qName)
{
    VALIDATE_NOT_NULL(qName)
    *qName = STRING;
    REFCOUNT_ADD(*qName);
    return NOERROR;
}

ECode CXPathConstants::GetQNameBOOLEAN(
    /* [out] */ IQName** qName)
{
    VALIDATE_NOT_NULL(qName)
    *qName = BOOLEAN;
    REFCOUNT_ADD(*qName);
    return NOERROR;
}

ECode CXPathConstants::GetQNameNODESET(
    /* [out] */ IQName** qName)
{
    VALIDATE_NOT_NULL(qName)
    *qName = NODESET;
    REFCOUNT_ADD(*qName);
    return NOERROR;
}

ECode CXPathConstants::GetQNameNODE(
    /* [out] */ IQName** qName)
{
    VALIDATE_NOT_NULL(qName)
    *qName = NODE;
    REFCOUNT_ADD(*qName);
    return NOERROR;
}

}// namespace XPath
}// namespace Xml
}// namespace Elastosx

