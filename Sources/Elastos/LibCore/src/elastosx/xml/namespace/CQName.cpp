
#include "CQName.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastosx {
namespace Xml {
namespace Namespace {

CAR_INTERFACE_IMPL(CQName, Object, IQName)

CAR_OBJECT_IMPL(CQName)

ECode CQName::GetNamespaceURI(
    /* [out] */ String * pUri)
{
    *pUri = mNamespaceURI;
    return NOERROR;
}

ECode CQName::GetLocalPart(
    /* [out] */ String * pPart)
{
    *pPart = mLocalPart;
    return NOERROR;
}

ECode CQName::GetPrefix(
    /* [out] */ String * pPrefix)
{
    *pPrefix = mPrefix;
    return NOERROR;
}

ECode CQName::Equals(
    /* [in] */ IInterface * pObjectToTest,
    /* [out] */ Boolean * pIsEqual)
{
    // Is this the same object?
    if (pObjectToTest == (IQName*)this) {
        *pIsEqual = TRUE;
        return NOERROR;
    }
    // Is this a QName?
    IQName* tmp = IQName::Probe(pObjectToTest);
    if (tmp != NULL) {
        CQName* qName = (CQName*) tmp;
        *pIsEqual = mLocalPart.Equals(qName->mLocalPart) && mNamespaceURI.Equals(qName->mNamespaceURI);
        return NOERROR;
    }
    *pIsEqual = FALSE;
    return NOERROR;
}

ECode CQName::GetHashCode(
    /* [out] */ Int32 * pHashCode)
{
    // TODO: Add your code here
    *pHashCode = mNamespaceURI.GetHashCode() ^ mLocalPart.GetHashCode();
    return NOERROR;
}

ECode CQName::ToString(
    /* [out] */ String * pStr)
{
    String _qNameAsString = mQNameAsString;
    if (_qNameAsString == NULL) {
        Int32 nsLength = mNamespaceURI.GetLength();
        if (nsLength == 0) {
            _qNameAsString = mLocalPart;
        }
        else {
            StringBuilder buffer(nsLength + mLocalPart.GetLength() + 2);
            buffer += '{';
            buffer += mNamespaceURI;
            buffer += '}';
            buffer += mLocalPart;
            _qNameAsString = buffer.ToString();
        }
        mQNameAsString = _qNameAsString;
    }
    *pStr = _qNameAsString;
    return NOERROR;
}

ECode CQName::constructor(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localPart)
{
    // TODO: Add your code here
    return constructor(namespaceURI, localPart, IXMLConstants::DEFAULT_NS_PREFIX);
}

ECode CQName::constructor(
    /* [in] */ const String& namespaceURI,
    /* [in] */ const String& localPart,
    /* [in] */ const String& prefix)
{

    // map null Namespace URI to default to preserve compatibility with QName 1.0
    if (namespaceURI == NULL) {
        mNamespaceURI = IXMLConstants::NULL_NS_URI;
    } else {
        mNamespaceURI = namespaceURI;
    }

    // local part is required.  "" is allowed to preserve compatibility with QName 1.0
    if (localPart == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("local part cannot be \"null\" when creating a QName");
    }
    mLocalPart = localPart;

    // prefix is required
    if (prefix == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("prefix cannot be \"null\" when creating a QName");
    }
    mPrefix = prefix;
    return NOERROR;
}

ECode CQName::constructor(
    /* [in] */ const String& localPart)
{
    // TODO: Add your code here
    return constructor(IXMLConstants::NULL_NS_URI, localPart, IXMLConstants::DEFAULT_NS_PREFIX);
}

ECode CQName::ValueOf(
    /* [in] */ String qNameAsString,
    /* [out] */  IQName** qName)
{
    // null is not valid
    if (qNameAsString == NULL) {
        // throw new IllegalArgumentException("cannot create QName from \"null\" or \"\" String");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // "" local part is valid to preserve compatible behavior with QName 1.0
    if (qNameAsString.GetLength() == 0) {
        return CQName::New(
            IXMLConstants::NULL_NS_URI,
            qNameAsString,
            IXMLConstants::DEFAULT_NS_PREFIX,
            qName);
    }

    // local part only?
    if (qNameAsString.GetChar(0) != '{') {
        return CQName::New(
            IXMLConstants::NULL_NS_URI,
            qNameAsString,
            IXMLConstants::DEFAULT_NS_PREFIX,
            qName);
    }

    // Namespace URI improperly specified?
    if (qNameAsString.StartWith(String("{") + IXMLConstants::NULL_NS_URI + "}")) {
        // throw new IllegalArgumentException(
        //     "Namespace URI .equals(XMLConstants.NULL_NS_URI), "
        //     + ".equals(\"" + XMLConstants.NULL_NS_URI + "\"), "
        //     + "only the local part, "
        //     + "\"" + qNameAsString.substring(2 + XMLConstants.NULL_NS_URI.length()) + "\", "
        //     + "should be provided.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Namespace URI and local part specified
    Int32 endOfNamespaceURI = qNameAsString.IndexOf('}');
    if (endOfNamespaceURI == -1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException(
        //     "cannot create QName from \""
        //         + qNameAsString
        //         + "\", missing closing \"}\"");
    }
    return CQName::New(
        qNameAsString.Substring(1, endOfNamespaceURI),
        qNameAsString.Substring(endOfNamespaceURI + 1),
        IXMLConstants::DEFAULT_NS_PREFIX,
        qName);
}

}
}
}

