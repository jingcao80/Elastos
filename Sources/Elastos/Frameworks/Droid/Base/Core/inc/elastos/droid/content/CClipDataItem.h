
#ifndef __ELASTOS_DROID_CONTENT_CCLIPDATAITEM_H__
#define __ELASTOS_DROID_CONTENT_CCLIPDATAITEM_H__

#include "Elastos.Droid.Net.h"
#include "_Elastos_Droid_Content_CClipDataItem.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::IStringBuilder;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Content {

class CClipData;

CarClass(CClipDataItem)
    , public Object
    , public IClipDataItem
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Retrieve the raw text contained in this Item.
     */
    CARAPI GetText(
        /* [out] */ ICharSequence** text);

    /**
     * Retrieve the raw HTML text contained in this Item.
     */
    CARAPI GetHtmlText(
        /* [out] */ String* htmlText);

    /**
     * Retrieve the raw Intent contained in this Item.
     */
    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

    /**
     * Retrieve the raw URI contained in this Item.
     */
    CARAPI GetUri(
        /* [out] */ IUri** uri);

    /**
     * Turn this item into text, regardless of the type of data it
     * actually contains.
     *
     * <p>The algorithm for deciding what text to return is:
     * <ul>
     * <li> If {@link #getText} is non-null, return that.
     * <li> If {@link #getUri} is non-null, try to retrieve its data
     * as a text stream from its content provider.  If this succeeds, copy
     * the text into a String and return it.  If it is not a content: URI or
     * the content provider does not supply a text representation, return
     * the raw URI as a string.
     * <li> If {@link #getIntent} is non-null, convert that to an intent:
     * URI and return it.
     * <li> Otherwise, return an empty string.
     * </ul>
     *
     * @param context The caller's Context, from which its ContentResolver
     * and other things can be retrieved.
     * @return Returns the item's textual representation.
     */
    CARAPI CoerceToText(
        /* [in] */ IContext* context,
        /* [out] */ ICharSequence** text);

    /**
     * Like {@link #coerceToHtmlText(Context)}, but any text that would
     * be returned as HTML formatting will be returned as text with
     * style spans.
     * @param context The caller's Context, from which its ContentResolver
     * and other things can be retrieved.
     * @return Returns the item's textual representation.
     */
    CARAPI CoerceToStyledText(
        /* [in] */ IContext* context,
        /* [out] */ ICharSequence** text);

    /**
     * Turn this item into HTML text, regardless of the type of data it
     * actually contains.
     *
     * <p>The algorithm for deciding what text to return is:
     * <ul>
     * <li> If {@link #getHtmlText} is non-null, return that.
     * <li> If {@link #getText} is non-null, return that, converting to
     * valid HTML text.  If this text contains style spans,
     * {@link Html#toHtml(Spanned) Html.toHtml(Spanned)} is used to
     * convert them to HTML formatting.
     * <li> If {@link #getUri} is non-null, try to retrieve its data
     * as a text stream from its content provider.  If the provider can
     * supply text/html data, that will be preferred and returned as-is.
     * Otherwise, any text data will be returned and escaped to HTML.
     * If it is not a content: URI or the content provider does not supply
     * a text representation, HTML text containing a link to the URI
     * will be returned.
     * <li> If {@link #getIntent} is non-null, convert that to an intent:
     * URI and return as an HTML link.
     * <li> Otherwise, return an empty string.
     * </ul>
     *
     * @param context The caller's Context, from which its ContentResolver
     * and other things can be retrieved.
     * @return Returns the item's representation as HTML text.
     */
    CARAPI CoerceToHtmlText(
        /* [in] */ IContext* context,
        /* [out] */ String* text);

    CARAPI ToString(
        /* [out] */ String* str);

    /** @hide */
    CARAPI ToShortString(
        /* [in] */ IStringBuilder* sb);

    /**
     * Create an Item consisting of a single block of (possibly styled) text.
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* text);

    /**
     * Create an Item consisting of a single block of (possibly styled) text,
     * with an alternative HTML formatted representation.  You <em>must</em>
     * supply a plain text representation in addition to HTML text; coercion
     * will not be done from HTML formated text into plain text.
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* text,
        /* [in] */ const String& htmlText);

    /**
     * Create an Item consisting of an arbitrary Intent.
     */
    CARAPI constructor(
        /* [in] */ IIntent* intent);

    /**
     * Create an Item consisting of an arbitrary URI.
     */
    CARAPI constructor(
        /* [in] */ IUri* uri);

    /**
     * Create a complex Item, containing multiple representations of
     * text, Intent, and/or URI.
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* text,
        /* [in] */ IIntent* intent,
        /* [in] */ IUri* uri);

    /**
     * Create a complex Item, containing multiple representations of
     * text, HTML text, Intent, and/or URI.  If providing HTML text, you
     * <em>must</em> supply a plain text representation as well; coercion
     * will not be done from HTML formated text into plain text.
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* text,
        /* [in] */ const String& htmlText,
        /* [in] */ IIntent* intent,
        /* [in] */ IUri* uri);

private:
    CARAPI CoerceToHtmlOrStyledText(
        /* [in] */ IContext* context,
        /* [in] */ Boolean styled,
        /* [out] */ ICharSequence** cs);

    CARAPI UriToHtml(
        /* [in] */ const String& uri,
        /* [out] */ String* str);

    CARAPI UriToStyledText(
        /* [in] */ const String& uri,
        /* [out] */ ICharSequence** cs);

private:
    AutoPtr<ICharSequence> mText;
    String mHtmlText;
    AutoPtr<IIntent> mIntent;
    AutoPtr<IUri> mUri;

private:
    friend class CClipData;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCLIPDATAITEM_H__
