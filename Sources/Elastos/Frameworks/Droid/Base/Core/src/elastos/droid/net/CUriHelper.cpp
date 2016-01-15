
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/net/Uri.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CUriHelper, Singleton, IUriHelper)

CAR_SINGLETON_IMPL(CUriHelper)

ECode CUriHelper::Parse(
    /* [in] */ const String& uriString,
    /* [out] */ IUri** result)
{
    return Uri::Parse(uriString, result);
}

ECode CUriHelper::FromFile(
    /* [in] */ IFile* file,
    /* [out] */ IUri** result)
{
    return Uri::FromFile(file, result);
}

ECode CUriHelper::FromParts(
    /* [in] */ const String& scheme,
    /* [in] */ const String& ssp,
    /* [in] */ const String& fragment,
    /* [out] */ IUri** result)
{
    return Uri::FromParts(scheme, ssp, fragment, result);
}

ECode CUriHelper::Encode(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    return Uri::Encode(s, result);
}

ECode CUriHelper::Encode(
    /* [in] */ const String& s,
    /* [in] */ const String& allow,
    /* [out] */ String* result)
{
    return Uri::Encode(s, allow, result);
}

ECode CUriHelper::Decode(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    return Uri::Decode(s, result);
}

ECode CUriHelper::WithAppendedPath(
    /* [in] */ IUri* baseUri,
    /* [in] */ const String& pathSegment,
    /* [out] */ IUri** result)
{
    return Uri::WithAppendedPath(baseUri, pathSegment, result);
}

ECode CUriHelper::GetEMPTY(
    /* [out] */ IUri** result)
{
    return Uri::GetEMPTY(result);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos

