
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/content/CClipDataHelper.h"
#include "elastos/droid/content/CClipData.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CClipDataHelper, Singleton, IClipDataHelper)

CAR_SINGLETON_IMPL(CClipDataHelper)

ECode CClipDataHelper::NewPlainText(
    /* [in] */ ICharSequence* label,
    /* [in] */ ICharSequence* text,
    /* [out] */ IClipData** clipData)
{
    return CClipData::NewPlainText(label, text, clipData);
}

ECode CClipDataHelper::NewHtmlText(
    /* [in] */ ICharSequence* label,
    /* [in] */ ICharSequence* text,
    /* [in] */ const String& htmlText,
    /* [out] */ IClipData** clipData)
{
    return CClipData::NewHtmlText(label, text, htmlText, clipData);
}

ECode CClipDataHelper::NewIntent(
    /* [in] */ ICharSequence* label,
    /* [in] */ IIntent* intent,
    /* [out] */ IClipData** clipData)
{
    return CClipData::NewIntent(label, intent, clipData);
}

ECode CClipDataHelper::NewUri(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ ICharSequence* label,
    /* [in] */ IUri* uri,
    /* [out] */ IClipData** clipData)
{
    return CClipData::NewUri(resolver, label, uri, clipData);
}

ECode CClipDataHelper::NewRawUri(
    /* [in] */ ICharSequence* label,
    /* [in] */ IUri* uri,
    /* [out] */ IClipData** clipData)
{
    return CClipData::NewRawUri(label, uri, clipData);
}

ECode CClipDataHelper::GetMimeTypes(
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<String>** types)
{
    VALIDATE_NOT_NULL(types);
    *types = NULL;

    if (name.Equals("plain")) {
        *types = CClipData::MIMETYPES_TEXT_PLAIN;
    }
    else if (name.Equals("html")) {
        *types = CClipData::MIMETYPES_TEXT_HTML;
    }
    else if (name.Equals("uriList")) {
        *types = CClipData::MIMETYPES_TEXT_URILIST;
    }
    else if (name.Equals("intent")) {
        *types = CClipData::MIMETYPES_TEXT_INTENT;
    }

    REFCOUNT_ADD(*types);
    return NOERROR;
}

} // Content
} // Droid
} // Elastos

