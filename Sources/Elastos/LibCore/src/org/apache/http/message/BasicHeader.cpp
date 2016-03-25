
#include "org/apache/http/message/BasicHeader.h"
#include "org/apache/http/message/BasicLineFormatter.h"
#include "org/apache/http/message/BasicHeaderValueParser.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

CAR_INTERFACE_IMPL_2(BasicHeader, Object, IHeader, ICloneable)

ECode BasicHeader::Init(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (name.IsNull()) {
        Logger::E("BasicHeader", "Name may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mName = name;
    mValue = value;
    return NOERROR;
}

ECode BasicHeader::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode BasicHeader::GetValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mValue;
    return NOERROR;
}

ECode BasicHeader::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    // no need for non-default formatting in toString()
    AutoPtr<ICharArrayBuffer> buf;
    ILineFormatter::Probe(BasicLineFormatter::DEFAULT)->FormatHeader(
            NULL, this, (ICharArrayBuffer**)&buf);
    return IObject::Probe(buf)->ToString(str);
}

ECode BasicHeader::GetElements(
    /* [out, callee] */ ArrayOf<IHeaderElement*>** elements)
{
    VALIDATE_NOT_NULL(elements)
    if (!mValue.IsNull()) {
        // result intentionally not cached, it's probably not used again
        return BasicHeaderValueParser::ParseElements(mValue, NULL, elements);
    }
    else {
        *elements = ArrayOf<IHeaderElement*>::Alloc(0);
        REFCOUNT_ADD(*elements)
        return NOERROR;
    }
}

void BasicHeader::CloneImpl(
    /* [in] */ BasicHeader* obj)
{
    obj->mName = mName;
    obj->mValue = mValue;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
