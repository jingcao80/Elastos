
#include "org/apache/http/impl/cookie/NetscapeDraftHeaderParser.h"
#include "org/apache/http/message/CBasicHeaderElement.h"
#include "elastos/core/CArrayOf.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::INameValuePair;
using Org::Apache::Http::Message::CBasicHeaderElement;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

static AutoPtr<NetscapeDraftHeaderParser> InitDefault()
{
    AutoPtr<NetscapeDraftHeaderParser> parser = new NetscapeDraftHeaderParser();
    return parser;
}
const AutoPtr<NetscapeDraftHeaderParser> NetscapeDraftHeaderParser::DEFAULT = InitDefault();

static AutoPtr< ArrayOf<Char32> > InitDelimiters()
{
    AutoPtr< ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(1);
    (*chars)[0] = ';';
    return chars;
}
const AutoPtr< ArrayOf<Char32> > NetscapeDraftHeaderParser::DELIMITERS = InitDelimiters();

NetscapeDraftHeaderParser::NetscapeDraftHeaderParser()
{
    mNvpParser = BasicHeaderValueParser::DEFAULT;
}

ECode NetscapeDraftHeaderParser::ParseHeader(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IParserCursor* cursor,
    /* [out] */ IHeaderElement** element)
{
    VALIDATE_NOT_NULL(element)
    *element = NULL;

    if (buffer == NULL) {
        Logger::E("NetscapeDraftHeaderParser", "Char array buffer may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (cursor == NULL) {
        Logger::E("NetscapeDraftHeaderParser", "Parser cursor may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<INameValuePair> nvp;
    mNvpParser->ParseNameValuePair(buffer, cursor, DELIMITERS, (INameValuePair**)&nvp);
    AutoPtr<IArrayList> array;
    CArrayList::New((IArrayList**)&array);
    AutoPtr<IList> params = IList::Probe(array);
    Boolean atEnd;
    while (cursor->AtEnd(&atEnd), !atEnd) {
        AutoPtr<INameValuePair> param;
        mNvpParser->ParseNameValuePair(buffer, cursor, DELIMITERS, (INameValuePair**)&param);
        params->Add(param);
    }
    String name, value;
    nvp->GetName(&name);
    nvp->GetValue(&value);
    Int32 size;
    params->GetSize(&size);
    AutoPtr<IArrayOf> pairs;
    CArrayOf::New(EIID_INameValuePair, size, (IArrayOf**)&pairs);
    AutoPtr<IIterator> it;
    params->GetIterator((IIterator**)&it);
    Int32 i = 0;
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<INameValuePair> pair = INameValuePair::Probe(value);
        pairs->Set(i, pair);
        i++;
    }

    return CBasicHeaderElement::New(name, value, pairs, element);
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org