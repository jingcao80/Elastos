
#include "org/apache/http/message/BasicHeaderValueFormatter.h"
#include "org/apache/http/message/CBasicHeaderValueFormatter.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

static AutoPtr<IBasicHeaderValueFormatter> InitDefault()
{
    AutoPtr<CBasicHeaderValueFormatter> formatter;
    CBasicHeaderValueFormatter::NewByFriend((CBasicHeaderValueFormatter**)&formatter);
    return (IBasicHeaderValueFormatter*)formatter.Get();
}
const AutoPtr<IBasicHeaderValueFormatter> BasicHeaderValueFormatter::DEFAULT = InitDefault();

CAR_INTERFACE_IMPL_2(BasicHeaderValueFormatter, Object, IBasicHeaderValueFormatter, IHeaderValueFormatter)

ECode BasicHeaderValueFormatter::FormatElements(
    /* [in] */ ArrayOf<IHeaderElement*>* elems,
    /* [in] */ Boolean quote,
    /* [in] */ IHeaderValueFormatter* formatter,
    /* [out] */ String* elements)
{
    VALIDATE_NOT_NULL(elements)
    *elements = String(NULL);
    if (formatter == NULL) {
        formatter = IHeaderValueFormatter::Probe(BasicHeaderValueFormatter::DEFAULT);
    }
    AutoPtr<ICharArrayBuffer> buffer;
    FAIL_RETURN(formatter->FormatElements(NULL, elems, quote, (ICharArrayBuffer**)&buffer))
    IObject::Probe(buffer)->ToString(elements);
    return NOERROR;
}

ECode BasicHeaderValueFormatter::FormatElements(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ ArrayOf<IHeaderElement*>* elems,
    /* [in] */ Boolean quote,
    /* [out] */ ICharArrayBuffer** buf)
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;

    if (elems == NULL) {
        Logger::E("BasicHeaderValueFormatter", "Header element array must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 len;
    EstimateElementsLen(elems, &len);
    if (buffer == NULL) {
        CCharArrayBuffer::New(len, (ICharArrayBuffer**)&buffer);
    }
    else {
        buffer->EnsureCapacity(len);
    }

    for (Int32 i=0; i < elems->GetLength(); i++) {
        if (i > 0) {
            buffer->Append(String(", "));
        }
        AutoPtr<ICharArrayBuffer> b;
        FormatHeaderElement(buffer, (*elems)[i], quote, (ICharArrayBuffer**)&b);
    }
    *buf = buffer;
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode BasicHeaderValueFormatter::EstimateElementsLen(
    /* [in] */ ArrayOf<IHeaderElement*>* elems,
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    if ((elems == NULL) || (elems->GetLength() < 1)) {
        *length = 0;
        return NOERROR;
    }

    Int32 result = (elems->GetLength() - 1) * 2; // elements separated by ", "
    for (Int32 i = 0; i < elems->GetLength(); i++) {
        Int32 len;
        EstimateHeaderElementLen((*elems)[i], &len);
        result += len;
    }

    *length = result;
    return NOERROR;
}

ECode BasicHeaderValueFormatter::FormatHeaderElement(
    /* [in] */ IHeaderElement* elem,
    /* [in] */ Boolean quote,
    /* [in] */ IHeaderValueFormatter* formatter,
    /* [out] */ String* element)
{
    VALIDATE_NOT_NULL(element)
    *element = String(NULL);
    if (formatter == NULL) {
        formatter = IHeaderValueFormatter::Probe(BasicHeaderValueFormatter::DEFAULT);
    }
    AutoPtr<ICharArrayBuffer> buffer;
    FAIL_RETURN(formatter->FormatHeaderElement(NULL, elem, quote, (ICharArrayBuffer**)&buffer))
    IObject::Probe(buffer)->ToString(element);
    return NOERROR;
}

ECode BasicHeaderValueFormatter::FormatHeaderElement(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ IHeaderElement* elem,
    /* [in] */ Boolean quote,
    /* [out] */ ICharArrayBuffer** buf)
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;
    if (elem == NULL) {
        Logger::E("BasicHeaderValueFormatter", "Header element must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 len;
    EstimateHeaderElementLen(elem, &len);
    if (buffer == NULL) {
        CCharArrayBuffer::New(len, (ICharArrayBuffer**)&buffer);
    }
    else {
        buffer->EnsureCapacity(len);
    }

    String name;
    elem->GetName(&name);
    buffer->Append(name);
    String value;
    elem->GetValue(&value);
    if (!value.IsNull()) {
        buffer->Append('=');
        DoFormatValue(buffer, value, quote);
    }

    Int32 parcnt;
    elem->GetParameterCount(&parcnt);
    if (parcnt > 0) {
        for (Int32 i = 0; i < parcnt; i++) {
            buffer->Append(String("; "));
            AutoPtr<INameValuePair> pair;
            elem->GetParameter(i, (INameValuePair**)&pair);
            AutoPtr<ICharArrayBuffer> b;
            FormatNameValuePair(buffer, pair, quote, (ICharArrayBuffer**)&b);
        }
    }

    *buf = buffer;
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode BasicHeaderValueFormatter::EstimateHeaderElementLen(
    /* [in] */ IHeaderElement* elem,
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    if (elem == NULL) {
        *length = 0;
        return NOERROR;
    }

    String name;
    elem->GetName(&name);
    Int32 result = name.GetLength(); // name
    String value;
    elem->GetValue(&value);
    if (!value.IsNull()) {
        // assume quotes, but no escaped characters
        result += 3 + value.GetLength(); // ="value"
    }

    Int32 parcnt;
    elem->GetParameterCount(&parcnt);
    if (parcnt > 0) {
        for (Int32 i = 0; i < parcnt; i++) {
            AutoPtr<INameValuePair> pair;
            elem->GetParameter(i, (INameValuePair**)&pair);
            Int32 len;
            EstimateNameValuePairLen(pair, &len);
            result += 2 + len;
        }
    }

    *length = result;
    return NOERROR;
}

ECode BasicHeaderValueFormatter::FormatParameters(
    /* [in] */ ArrayOf<INameValuePair*>* nvps,
    /* [in] */ Boolean quote,
    /* [in] */ IHeaderValueFormatter* formatter,
    /* [out] */ String* parameters)
{
    VALIDATE_NOT_NULL(parameters)
    *parameters = String(NULL);
    if (formatter == NULL) {
        formatter = IHeaderValueFormatter::Probe(BasicHeaderValueFormatter::DEFAULT);
    }
    AutoPtr<ICharArrayBuffer> buffer;
    FAIL_RETURN(formatter->FormatParameters(NULL, nvps, quote, (ICharArrayBuffer**)&buffer))
    IObject::Probe(buffer)->ToString(parameters);
    return NOERROR;
}

ECode BasicHeaderValueFormatter::FormatParameters(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ ArrayOf<INameValuePair*>* nvps,
    /* [in] */ Boolean quote,
    /* [out] */ ICharArrayBuffer** buf)
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;

    if (nvps == NULL) {
        Logger::E("BasicHeaderValueFormatter", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 len;
    EstimateParametersLen(nvps, &len);
    if (buffer == NULL) {
        CCharArrayBuffer::New(len, (ICharArrayBuffer**)&buffer);
    }
    else {
        buffer->EnsureCapacity(len);
    }

    for (Int32 i = 0; i < nvps->GetLength(); i++) {
        if (i > 0) {
            buffer->Append(String("; "));
        }
        AutoPtr<ICharArrayBuffer> b;
        FormatNameValuePair(buffer, (*nvps)[i], quote, (ICharArrayBuffer**)&b);
    }

    *buf = buffer;
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode BasicHeaderValueFormatter::EstimateParametersLen(
    /* [in] */ ArrayOf<INameValuePair*>* nvps,
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    if ((nvps == NULL) || (nvps->GetLength() < 1)) {
        *length = 0;
        return NOERROR;
    }

    Int32 result = (nvps->GetLength() - 1) * 2; // "; " between the parameters
    for (Int32 i = 0; i < nvps->GetLength(); i++) {
        Int32 len;
        EstimateNameValuePairLen((*nvps)[i], &len);
        result += len;
    }

    *length = result;
    return NOERROR;
}

ECode BasicHeaderValueFormatter::FormatNameValuePair(
    /* [in] */ INameValuePair* nvp,
    /* [in] */ Boolean quote,
    /* [in] */ IHeaderValueFormatter* formatter,
    /* [out] */ String* pair)
{
    VALIDATE_NOT_NULL(pair)
    *pair = String(NULL);

    if (formatter == NULL) {
        formatter = IHeaderValueFormatter::Probe(BasicHeaderValueFormatter::DEFAULT);
    }
    AutoPtr<ICharArrayBuffer> buffer;
    FAIL_RETURN(formatter->FormatNameValuePair(NULL, nvp, quote, (ICharArrayBuffer**)&buffer))
    IObject::Probe(buffer)->ToString(pair);
    return NOERROR;
}

ECode BasicHeaderValueFormatter::FormatNameValuePair(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ INameValuePair* nvp,
    /* [in] */ Boolean quote,
    /* [out] */ ICharArrayBuffer** buf)
{
    VALIDATE_NOT_NULL(buf)
    *buf = NULL;

    if (nvp == NULL) {
        Logger::E("BasicHeaderValueFormatter", "NameValuePair must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 len;
    EstimateNameValuePairLen(nvp, &len);
    if (buffer == NULL) {
        CCharArrayBuffer::New(len, (ICharArrayBuffer**)&buffer);
    }
    else {
        buffer->EnsureCapacity(len);
    }

    String name;
    nvp->GetName(&name);
    buffer->Append(name);
    String value;
    nvp->GetValue(&value);
    if (!value.IsNull()) {
        buffer->Append('=');
        DoFormatValue(buffer, value, quote);
    }

    *buf = buffer;
    REFCOUNT_ADD(*buf)
    return NOERROR;
}

ECode BasicHeaderValueFormatter::EstimateNameValuePairLen(
    /* [in] */ INameValuePair* nvp,
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    if (nvp == NULL) {
        *length = 0;
        return NOERROR;
    }

    String name;
    nvp->GetName(&name);
    Int32 result = name.GetLength(); // name
    String value;
    nvp->GetValue(&value);
    if (!value.IsNull()) {
        // assume quotes, but no escaped characters
        result += 3 + value.GetLength(); // ="value"
    }
    *length = result;
    return NOERROR;
}

ECode BasicHeaderValueFormatter::DoFormatValue(
    /* [in] */ ICharArrayBuffer* buffer,
    /* [in] */ const String& value,
    /* [in] */ Boolean quote)
{
    if (!quote) {
        for (Int32 i = 0; (i < value.GetLength()) && !quote; i++) {
            IsSeparator(value.GetChar(i), &quote);
        }
    }

    if (quote) {
        buffer->Append('"');
    }
    for (Int32 i = 0; i < value.GetLength(); i++) {
        Char32 ch = value.GetChar(i);
        Boolean isUnsafe;
        if (IsUnsafe(ch, &isUnsafe), isUnsafe) {
            buffer->Append('\\');
        }
        buffer->Append(ch);
    }
    if (quote) {
        buffer->Append('"');
    }
    return NOERROR;
}

ECode BasicHeaderValueFormatter::IsSeparator(
    /* [in] */ Char32 ch,
    /* [out] */ Boolean* isSeparator)
{
    VALIDATE_NOT_NULL(isSeparator)
    *isSeparator = String(" ,;=()<>@:\\\"/[]?{}\t")/*SEPARATORS*/.IndexOf(ch) >= 0;
    return NOERROR;
}

ECode BasicHeaderValueFormatter::IsUnsafe(
    /* [in] */ Char32 ch,
    /* [out] */ Boolean* isUnsafe)
{
    VALIDATE_NOT_NULL(isUnsafe)
    *isUnsafe = String("\"\\")/*UNSAFE_CHARS*/.IndexOf(ch) >= 0;
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
