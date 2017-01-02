//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Text.h"
#include "elastos/droid/content/CClipDataItem.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/Html.h"
#include "elastos/droid/text/style/CURLSpan.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Text::ISpanned;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::Html;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::Style::IURLSpan;
using Elastos::Droid::Text::Style::CURLSpan;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::IAppendable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IReader;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CClipDataItem, Object, IClipDataItem)

CAR_OBJECT_IMPL(CClipDataItem)

ECode CClipDataItem::GetText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = mText;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CClipDataItem::GetHtmlText(
    /* [out] */ String* htmlText)
{
    VALIDATE_NOT_NULL(htmlText)
    *htmlText = mHtmlText;
    return NOERROR;
}

ECode CClipDataItem::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = mIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CClipDataItem::GetUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mUri;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CClipDataItem::SetUri(
    /* [in] */ IUri* uri)
{
    mUri = uri;
    return NOERROR;
}

ECode CClipDataItem::CoerceToText(
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    // If this Item has an explicit textual value, simply return that.
    AutoPtr<ICharSequence> cs = mText;
    if (cs != NULL) {
        *text = cs;
        REFCOUNT_ADD(*text);
        return NOERROR;
    }

    // If this Item has a URI value, try using that.
    AutoPtr<IUri> uri = mUri;
    if (uri != NULL) {
        // First see if the URI can be opened as a plain text stream
        // (of any sub-type).  If so, this is the best textual
        // representation for it.
        AutoPtr<IFileInputStream> stream;
        // try {
        // Ask for a stream of the desired type.
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<IAssetFileDescriptor> descr;
        ECode ec = resolver->OpenTypedAssetFileDescriptor(uri, String("text/*"),
                NULL, (IAssetFileDescriptor**)&descr);
        if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
            String tmp = Object::ToString(uri);
            return CString::New(tmp, text);
        }

        FAIL_RETURN(descr->CreateInputStream((IFileInputStream**)&stream));
        AutoPtr<IInputStreamReader> reader;
        ec = CInputStreamReader::New(IInputStream::Probe(stream), String("UTF-8"), (IInputStreamReader**)&reader);
        if (FAILED(ec)) {
            if (stream != NULL) {
                ICloseable::Probe(stream)->Close();
            }
            return ec;
        }

        // Got it...  copy the stream into a local string and return it.
        StringBuilder builder(128);
        AutoPtr< ArrayOf<Char32> > buffer = ArrayOf<Char32>::Alloc(8192);
        Int32 len;
        while (IReader::Probe(reader)->Read(buffer, &len), len > 0) {
            builder.Append(*buffer, 0, len);
        }
        String str;
        builder.ToString(&str);
        AutoPtr<ICharSequence> seq = builder.ToCharSequence();
        *text = seq;
        REFCOUNT_ADD(*text);

        if (stream != NULL) {
            ICloseable::Probe(stream)->Close();
        }
        // } catch (FileNotFoundException e) {
            // Unable to open content URI as text...  not really an
            // error, just something to ignore.
        // } catch (IOException e) {
            // Something bad has happened.
            // Log.w("ClippedData", "Failure loading text", e);
            // return e.toString();
        // } finally {
            // if (stream != null) {
                // try {
                    // stream.close();
                // } catch (IOException e) {
                // }
            // }
        // }
        return NOERROR;
    }

    // Finally, if all we have is an Intent, then we can just turn that
    // into text.  Not the most user-friendly thing, but it's something.
    AutoPtr<IIntent> intent = mIntent;
    if (intent != NULL) {
        String str;
        intent->ToUri(IIntent::URI_INTENT_SCHEME, &str);
        return CString::New(str, text);
    }

    // Shouldn't get here, but just in case...
    return CString::New(String(""), text);
}

ECode CClipDataItem::CoerceToStyledText(
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    AutoPtr<ICharSequence> cs = mText;
    if (ISpanned::Probe(cs) != NULL) {
        *text = cs;
        REFCOUNT_ADD(*text);
        return NOERROR;
    }

    String htmlText = mHtmlText;
    if (!htmlText.IsNull()) {
        // try {
            AutoPtr<ICharSequence> newText;
            assert(0);
//            newText = Html.fromHtml(htmlText);
            if (newText != NULL) {
                *text = newText;
                REFCOUNT_ADD(*text);
                return NOERROR;
            }
        // } catch (RuntimeException e) {
            // If anything bad happens, we'll fall back on the plain text.
        // }
    }

    if (cs != NULL) {
        *text = cs;
        REFCOUNT_ADD(*text);
        return NOERROR;
    }

    CoerceToHtmlOrStyledText(context, TRUE, text);
    return NOERROR;
}

ECode CClipDataItem::CoerceToHtmlText(
    /* [in] */ IContext* context,
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);
    // If the item has an explicit HTML value, simply return that.
    String htmlText = mHtmlText;
    if (!htmlText.IsNull()) {
        *text = htmlText;
        return NOERROR;
    }

    // If this Item has a plain text value, return it as HTML.
    AutoPtr<ICharSequence> cs = mText;
    if (cs != NULL) {
        if (ISpanned::Probe(cs) != NULL) {
            // return Html.toHtml((Spanned)cs);
            assert(0);
            return E_NOT_IMPLEMENTED;
        }
        // return Html.escapeHtml(cs);
        assert(0);
        return E_NOT_IMPLEMENTED;
    }

    CoerceToHtmlOrStyledText(context, FALSE, (ICharSequence**)&cs);
    if (cs != NULL) {
        return cs->ToString(text);
    }
    *text = NULL;
    return NOERROR;
}

ECode CClipDataItem::CoerceToHtmlOrStyledText(
    /* [in] */ IContext* context,
    /* [in] */ Boolean styled,
    /* [out] */ ICharSequence** cs)
{
    VALIDATE_NOT_NULL(cs);
    *cs = NULL;
    // If this Item has a URI value, try using that.
    if (mUri != NULL) {
        // Check to see what data representations the content
        // provider supports.  We would like HTML text, but if that
        // is not possible we'll live with plan text.
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr< ArrayOf<String> > types;
        resolver->GetStreamTypes(mUri, String("text/*"), (ArrayOf<String>**)&types);
        Boolean hasHtml = FALSE;
        Boolean hasText = FALSE;
        if (types != NULL) {
            for (Int32 i = 0; i < types->GetLength(); ++i) {
                if ((*types)[i].Equals("text/html")) {
                    hasHtml = TRUE;
                }
                else if ((*types)[i].StartWith("text/")) {
                    hasText = TRUE;
                }
            }
        }

        // If the provider can serve data we can use, open and load it.
        if (hasHtml || hasText) {
            AutoPtr<IFileInputStream> stream;
            // BEGIN FROM THIS
            // try {
            // Ask for a stream of the desired type.
            AutoPtr<IAssetFileDescriptor> descr;
            AutoPtr<IInputStreamReader> reader;
            // Got it...  copy the stream into a local string and return it.
            AutoPtr<IStringBuilder> builder = new StringBuilder(128);
            AutoPtr<ArrayOf<Char32> > buffer = ArrayOf<Char32>::Alloc(8192);
            Int32 len = 0;
            String text;
            AutoPtr<ICharSequence> newText;
            ECode ec = resolver->OpenTypedAssetFileDescriptor(mUri, hasHtml ? String("text/html") : String("text/plain"),
                (IBundle*)NULL, (IAssetFileDescriptor**)&descr);
            FAIL_GOTO(ec, EXIT);
            ec = descr->CreateInputStream((IFileInputStream**)&stream);
            FAIL_GOTO(ec, EXIT);
            ec = CInputStreamReader::New(IInputStream::Probe(stream), String("UTF-8"), (IInputStreamReader**)&reader);
            FAIL_GOTO(ec, EXIT);

            while ((IReader::Probe(reader)->Read(buffer, &len), len) > 0) {
                ec = builder->Append(*buffer, 0, len);
                FAIL_GOTO(ec, EXIT);
            }

            text = Object::ToString(builder);

            if (hasHtml) {
                if (styled) {
                    // We loaded HTML formatted text and the caller
                    // want styled text, convert it.
                    newText = ICharSequence::Probe(Html::FromHtml(text));

                    if (newText != NULL) {
                        *cs = newText;
                        REFCOUNT_ADD(*cs);
                        if (stream != NULL) {
                            FAIL_RETURN(ICloseable::Probe(stream)->Close());
                        }
                        return NOERROR;
                    }
                    else {
                        return CString::New(text, cs);
                    }
                }
                else {
                    // We loaded HTML formatted text and that is what
                    // the caller wants, just return it.
                    return CString::New(text, cs);
                }
            }

            if (styled) {
                // We loaded plain text and the caller wants styled
                // text, that is all we have so return it.
                return CString::New(text, cs);
            }
            else {
                // We loaded plain text and the caller wants HTML
                // text, escape it for HTML.
                if (stream != NULL) {
                    FAIL_RETURN(ICloseable::Probe(stream)->Close());
                }

                AutoPtr<ICharSequence> tmp = CoreUtils::Convert(text);
                String value = Html::EscapeHtml(tmp);
                return CString::New(value, cs);
            }

            // } catch (FileNotFoundException e) {
            //     // Unable to open content URI as text...  not really an
            //     // error, just something to ignore.

            // } catch (IOException e) {
            //     // Something bad has happened.
            //     Log.w("ClippedData", "Failure loading text", e);
            //     return Html.escapeHtml(e.toString());

            // } finally {
            //     if (stream != null) {
            //         try {
            //             stream.close();
            //         } catch (IOException e) {
            //         }
            //     }
            // }
EXIT:
            if (stream != NULL) {
                FAIL_RETURN(ICloseable::Probe(stream)->Close());
            }
            if (ec == (ECode)E_IO_EXCEPTION) {
                // Something bad has happened.
//                Html.escapeHtml(e.toString());
                return E_NOT_IMPLEMENTED;
            }
            else if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
                // Unable to open content URI as text...  not really an
                // error, just something to ignore.
            }
            else {
                return ec;
            }

        }

        // If we couldn't open the URI as a stream, then we can build
        // some HTML text with the URI itself.
        // probably serves fairly well as a textual representation.
        String str = Object::ToString(mUri);
        if (styled) {
            return UriToStyledText(str, cs);
        }
        else {
            String htmlText;
            FAIL_RETURN(UriToHtml(str, &htmlText));
            return CString::New(htmlText, cs);
        }
    }

    // Finally, if all we have is an Intent, then we can just turn that
    // into text.  Not the most user-friendly thing, but it's something.
    if (mIntent != NULL) {
        String uri;
        FAIL_RETURN(mIntent->ToUri(IIntent::URI_INTENT_SCHEME, &uri));
        if (styled) {
            return UriToStyledText(uri, cs);
        }
        else {
            String htmlText;
            FAIL_RETURN(UriToHtml(uri, &htmlText));
            return CString::New(htmlText, cs);
        }
    }

    // Shouldn't get here, but just in case...
    return CString::New(String(""), cs);
}

ECode CClipDataItem::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    AutoPtr<IStringBuilder> builder = new StringBuilder(128);
    builder->Append(String("ClipData.Item { "));
    FAIL_RETURN(ToShortString(builder));
    builder->Append(String(" }"));
    *str = Object::ToString(builder);
    return NOERROR;
}

ECode CClipDataItem::ToShortString(
    /* [in] */ IStringBuilder* sb)
{
    if (!mHtmlText.IsNull()) {
        sb->Append(String("H:"));
        sb->Append(mHtmlText);
    }
    else if (mText != NULL) {
        sb->Append(String("T:"));
        String str;
        FAIL_RETURN(mText->ToString(&str));
        sb->Append(str);
    }
    else if (mUri != NULL) {
        sb->Append(String("U:"));
        String str = Object::ToString(mUri);
        sb->Append(str);
    }
    else if (mIntent != NULL) {
        sb->Append(String("I:"));
        FAIL_RETURN(mIntent->ToShortString(sb, TRUE, TRUE, TRUE, TRUE));
    }
    else {
        sb->Append(String("NULL"));
    }

    return NOERROR;
}

ECode CClipDataItem::constructor(
    /* [in] */ ICharSequence* text)
{
    mText = text;
    mHtmlText = NULL;
    mIntent = NULL;
    mUri = NULL;
    return NOERROR;
}

ECode CClipDataItem::constructor(
    /* [in] */ ICharSequence* text,
    /* [in] */ const String& htmlText)
{
    mText = text;
    mHtmlText = htmlText;
    mIntent = NULL;
    mUri = NULL;
    return NOERROR;
}

ECode CClipDataItem::constructor(
    /* [in] */ IIntent* intent)
{
    mText = NULL;
    mHtmlText = NULL;
    mIntent = intent;
    mUri = NULL;
    return NOERROR;
}

ECode CClipDataItem::constructor(
    /* [in] */ IUri* uri)
{
    mText = NULL;
    mHtmlText = NULL;
    mIntent = NULL;
    mUri = uri;
    return NOERROR;
}

ECode CClipDataItem::constructor(
    /* [in] */ ICharSequence* text,
    /* [in] */ IIntent* intent,
    /* [in] */ IUri* uri)
{
    mText = text;
    mHtmlText = NULL;
    mIntent = intent;
    mUri = uri;
    return NOERROR;
}

ECode CClipDataItem::constructor(
    /* [in] */ ICharSequence* text,
    /* [in] */ const String& htmlText,
    /* [in] */ IIntent* intent,
    /* [in] */ IUri* uri)
{
    if (!htmlText.IsNull() && NULL == text) {
        //throw new IllegalArgumentException(
        //        "Plain text must be supplied if HTML text is supplied");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mText = text;
    mHtmlText = htmlText;
    mIntent = intent;
    mUri = uri;
    return NOERROR;
}

ECode CClipDataItem::UriToHtml(
    /* [in] */ const String& uri,
    /* [out] */ String* htmlText)
{
    VALIDATE_NOT_NULL(htmlText)
    AutoPtr<ICharSequence> uriSeq = CoreUtils::Convert(uri);
    StringBuilder builder(256);
    builder += "<a href=\"";
    builder += Html::EscapeHtml(uriSeq);
    builder += uri;
    builder += "\">";
    builder += Html::EscapeHtml(uriSeq);
    builder += uri;
    builder += "</a>";
    return builder.ToString(htmlText);
}

ECode CClipDataItem::UriToStyledText(
    /* [in] */ const String& uri,
    /* [out] */ ICharSequence** cs)
{
    VALIDATE_NOT_NULL(cs)
    AutoPtr<ISpannableStringBuilder> builder;
    FAIL_RETURN(CSpannableStringBuilder::New((ISpannableStringBuilder**)&builder));
    AutoPtr<ICharSequence> tmpUri;
    FAIL_RETURN(CString::New(uri, (ICharSequence**)&tmpUri));
    FAIL_RETURN(IAppendable::Probe(builder)->Append(tmpUri));

    AutoPtr<IURLSpan> span;
    CURLSpan::New(uri, (IURLSpan**)&span);
    Int32 len = 0;
    ICharSequence::Probe(builder)->GetLength(&len);
    ISpannable::Probe(builder)->SetSpan(span, 0, len, ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
    String str = Object::ToString(builder);
    *cs = ICharSequence::Probe(builder.Get());
    REFCOUNT_ADD(*cs);
    return NOERROR;
}

}
}
}

