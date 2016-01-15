
#include "org/apache/http/message/BasicHeaderElementIterator.h"
#include "org/apache/http/message/BasicHeaderValueParser.h"
#include "org/apache/http/message/CParserCursor.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::EIID_IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IFormattedHeader;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_INTERFACE_IMPL_2(BasicHeaderElementIterator, Object, IHeaderElementIterator, IIterator)

ECode BasicHeaderElementIterator::Init(
    /* [in] */ IHeaderIterator* headerIterator,
    /* [in] */ IHeaderValueParser* parser)
{
    if (headerIterator == NULL) {
        Logger::E("BasicHeaderElementIterator", "Header iterator may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (parser == NULL) {
        Logger::E("BasicHeaderElementIterator", "Parser may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mHeaderIt = headerIterator;
    mParser = parser;
    return NOERROR;
}

ECode BasicHeaderElementIterator::Init(
    /* [in] */ IHeaderIterator* headerIterator)
{
    return Init(headerIterator,
            (IHeaderValueParser*)BasicHeaderValueParser::DEFAULT->Probe(EIID_IHeaderValueParser));
}

void BasicHeaderElementIterator::BufferHeaderValue()
{
    mCursor = NULL;
    mBuffer = NULL;
    Boolean hasNext;
    while (mHeaderIt->HasNext(&hasNext), hasNext) {
        AutoPtr<IHeader> h;
        mHeaderIt->NextHeader((IHeader**)&h);
        AutoPtr<IFormattedHeader> fh = IFormattedHeader::Probe(h);
        if (fh != NULL) {
            fh->GetBuffer((ICharArrayBuffer**)&mBuffer);
            Int32 len;
            mBuffer->GetLength(&len);
            CParserCursor::New(0, len, (IParserCursor**)&mCursor);
            Int32 pos;
            fh->GetValuePos(&pos);
            mCursor->UpdatePos(pos);
            break;
        }
        else {
            String value;
            h->GetValue(&value);
            if (!value.IsNull()) {
                mBuffer = NULL;
                CCharArrayBuffer::New(value.GetLength(), (ICharArrayBuffer**)&mBuffer);
                mBuffer->Append(value);
                mCursor = NULL;
                Int32 len;
                mBuffer->GetLength(&len);
                CParserCursor::New(0, len, (IParserCursor**)&mCursor);
                break;
            }
        }
    }
}

void BasicHeaderElementIterator::ParseNextElement()
{
    // loop while there are headers left to parse
    Boolean hasNext;
    while ((mHeaderIt->HasNext(&hasNext), hasNext) || mCursor != NULL) {
        Boolean atEnd;
        if (mCursor == NULL || (mCursor->AtEnd(&atEnd), atEnd)) {
            // get next header value
            BufferHeaderValue();
        }
        // Anything buffered?
        if (mCursor != NULL) {
            // loop while there is data in the buffer
            while ((mCursor->AtEnd(&atEnd), !atEnd)) {
                AutoPtr<IHeaderElement> e;
                mParser->ParseHeaderElement(mBuffer, mCursor, (IHeaderElement**)&e);
                String name, value;
                if (!((e->GetName(&name), name.GetLength() == 0) && (e->GetValue(&value), value.IsNull()))) {
                    // Found something
                    mCurrentElement = e;
                    return;
                }
            }
            // if at the end of the buffer
            if (mCursor->AtEnd(&atEnd), atEnd) {
                // discard it
                mCursor = NULL;
                mBuffer = NULL;
            }
        }
    }
}

ECode BasicHeaderElementIterator::HasNext(
    /* [out] */ Boolean* hasNext)
{
    VALIDATE_NOT_NULL(hasNext)
    if (mCurrentElement == NULL) {
        ParseNextElement();
    }
    *hasNext = mCurrentElement != NULL;
    return NOERROR;
}

ECode BasicHeaderElementIterator::NextElement(
    /* [out] */ IHeaderElement** element)
{
    VALIDATE_NOT_NULL(element)
    *element = NULL;

    if (mCurrentElement == NULL) {
        ParseNextElement();
    }

    if (mCurrentElement == NULL) {
        Logger::E("BasicHeaderElementIterator", "No more header elements available");
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }

    AutoPtr<IHeaderElement> e = mCurrentElement;
    mCurrentElement = NULL;
    *element = e;
    REFCOUNT_ADD(*element)
    return NOERROR;
}

ECode BasicHeaderElementIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IHeaderElement> e;
    NextElement((IHeaderElement**)&e);
    *object = e->Probe(EIID_IInterface);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode BasicHeaderElementIterator::Remove()
{
    Logger::E("BasicHeaderElementIterator", "Remove not supported");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
