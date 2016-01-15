#include "elastos/droid/media/CTrackInfoVendor.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {


ECode CTrackInfoVendor::constructor(
    /* [in] */ ArrayOf<Byte>* name,
    /* [in] */ const String& charset)
{
    mName = name;
    mCharset = charset;
    return NOERROR;
}

ECode CTrackInfoVendor::SetName(
    /* [in] */ ArrayOf<Byte>* name)
{
    mName = name;
    return NOERROR;
}

ECode CTrackInfoVendor::GetName(
    /* [out, callee] */ ArrayOf<Byte>** name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    REFCOUNT_ADD(*name);
    return NOERROR;
}

ECode CTrackInfoVendor::SetCharset(
    /* [in] */ const String& charset)
{
    mCharset = charset;
    return NOERROR;
}

ECode CTrackInfoVendor::GetCharset(
    /* [out] */ String* charset)
{
    VALIDATE_NOT_NULL(charset);
    *charset = mCharset;
    return NOERROR;
}


} // namespace media
} // namepsace Droid
} // namespace Elastos
