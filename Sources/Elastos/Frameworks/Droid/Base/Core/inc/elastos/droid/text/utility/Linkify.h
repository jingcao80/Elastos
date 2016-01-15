#ifndef __ELASTOS_DROID_TEXT_UTILITY_Linkify_H__
#define __ELASTOS_DROID_TEXT_UTILITY_Linkify_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ICharSequence;
using Elastos::Core::IComparator;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

class LinkSpec
    : public Object
{
public:
    String url;
    Int32 start;
    Int32 end;
};

/**
 *  Linkify take a piece of text and a regular expression and turns all of the
 *  regex matches in the text into clickable links.  This is particularly
 *  useful for matching things like email addresses, web urls, etc. and making
 *  them actionable.
 *
 *  Alone with the pattern that is to be matched, a url scheme prefix is also
 *  required.  Any pattern match that does not begin with the supplied scheme
 *  will have the scheme prepended to the matched text when the clickable url
 *  is created.  For instance, if you are matching web urls you would supply
 *  the scheme <code>http://</code>.  If the pattern matches example.com, which
 *  does not have a url scheme prefix, the supplied scheme will be prepended to
 *  create <code>http://example.com</code> when the clickable url link is
 *  created.
 */
class Linkify
{
private:
    class MatchFilterUrl
        : public Object
        , public ILinkifyMatchFilter
    {
    public:
        CAR_INTERFACE_DECL()

        virtual CARAPI AcceptMatch(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [out] */ Boolean* ret);
    };

    class MatchFilterPhoneNumber
        : public Object
        , public ILinkifyMatchFilter
    {
    public:
        CAR_INTERFACE_DECL()

        virtual CARAPI AcceptMatch(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [out] */ Boolean* ret);
    };

    class TransformFilterPhoneNumber
        : public Object
        , public ILinkifyTransformFilter
    {
    public:
        CAR_INTERFACE_DECL()

        virtual CARAPI TransformUrl(
            /* [in] */ IMatcher* match,
            /* [in] */ const String & url,
            /* [out] */ String* ret);
    };

    class ComparatorLinkSpec
    {
    public:
        static Int32 ComparatorLinkSpecFunc(
            /* [in] */ LinkSpec* a,
            /* [in] */ LinkSpec* b);
    };

public:
    /**
     *  Scans the text of the provided Spannable and turns all occurrences
     *  of the link types indicated in the mask into clickable links.
     *  If the mask is nonzero, it also removes any existing URLSpans
     *  attached to the Spannable, to avoid problems if you call it
     *  repeatedly on the same text.
     */
    static CARAPI_(Boolean) AddLinks(
        /* [in] */ ISpannable* text,
        /* [in] */ Int32 mask);

    /**
     *  Scans the text of the provided TextView and turns all occurrences of
     *  the link types indicated in the mask into clickable links.  If matches
     *  are found the movement method for the TextView is set to
     *  LinkMovementMethod.
     */
    static CARAPI_(Boolean) AddLinks(
        /* [in] */ ITextView* text,
        /* [in] */ Int32 mask);

    /**
     *  Applies a regex to the text of a TextView turning the matches into
     *  links.  If links are found then UrlSpans are applied to the link
     *  text match areas, and the movement method for the text is changed
     *  to LinkMovementMethod.
     *
     *  @param text         TextView whose text is to be marked-up with links
     *  @param pattern      Regex pattern to be used for finding links
     *  @param scheme       Url scheme string (eg <code>http://</code> to be
     *                      prepended to the url of links that do not have
     *                      a scheme specified in the link text
     */
    static CARAPI AddLinks(
        /* [in] */ ITextView* text,
        /* [in] */ IPattern* pattern,
        /* [in] */ const String& scheme);

    /**
     *  Applies a regex to the text of a TextView turning the matches into
     *  links.  If links are found then UrlSpans are applied to the link
     *  text match areas, and the movement method for the text is changed
     *  to LinkMovementMethod.
     *
     *  @param text         TextView whose text is to be marked-up with links
     *  @param p            Regex pattern to be used for finding links
     *  @param scheme       Url scheme string (eg <code>http://</code> to be
     *                      prepended to the url of links that do not have
     *                      a scheme specified in the link text
     *  @param matchFilter  The filter that is used to allow the client code
     *                      additional control over which pattern matches are
     *                      to be converted into links.
     */
    static CARAPI AddLinks(
        /* [in] */ ITextView* text,
        /* [in] */ IPattern* p,
        /* [in] */ const String& scheme,
        /* [in] */ ILinkifyMatchFilter* matchFilter,
        /* [in] */ ILinkifyTransformFilter* transformFilter);

    /**
     *  Applies a regex to a Spannable turning the matches into
     *  links.
     *
     *  @param text         Spannable whose text is to be marked-up with
     *                      links
     *  @param pattern      Regex pattern to be used for finding links
     *  @param scheme       Url scheme string (eg <code>http://</code> to be
     *                      prepended to the url of links that do not have
     *                      a scheme specified in the link text
     */
    static CARAPI_(Boolean) AddLinks(
        /* [in] */ ISpannable* text,
        /* [in] */ IPattern* pattern,
        /* [in] */ const String& scheme);

    /**
     *  Applies a regex to a Spannable turning the matches into
     *  links.
     *
     *  @param s            Spannable whose text is to be marked-up with
     *                      links
     *  @param p            Regex pattern to be used for finding links
     *  @param scheme       Url scheme string (eg <code>http://</code> to be
     *                      prepended to the url of links that do not have
     *                      a scheme specified in the link text
     *  @param matchFilter  The filter that is used to allow the client code
     *                      additional control over which pattern matches are
     *                      to be converted into links.
     */
    static CARAPI_(Boolean) AddLinks(
        /* [in] */ ISpannable* s,
        /* [in] */ IPattern* p,
        /* [in] */ const String& scheme,
        /* [in] */ ILinkifyMatchFilter* matchFilter,
        /* [in] */ ILinkifyTransformFilter* transformFilter);

    static CARAPI_(AutoPtr<ILinkifyMatchFilter>) GetStaticUrlMatchFilter();

    static CARAPI_(AutoPtr<ILinkifyMatchFilter>) GetStaticPhoneNumberMatchFilter();

    static CARAPI_(AutoPtr<ILinkifyTransformFilter>) GetStaticPhoneNumberTransformFilter();

private:
    static CARAPI AddLinkMovementMethod(
        /* [in] */ ITextView* t);

    static CARAPI_(void) ApplyLink(
        /* [in] */ const String& url,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ISpannable* text);

    static CARAPI_(String) MakeUrl(
        /* [in] */ const String& url,
        /* [in] */ ArrayOf<String>* prefixes,
        /* [in] */ IMatcher* m,
        /* [in] */ ILinkifyTransformFilter* filter);

    static CARAPI_(void) GatherLinks(
        /* [in] */ List< AutoPtr<LinkSpec> >* links,
        /* [in] */ ISpannable* s,
        /* [in] */ IPattern* pattern,
        /* [in] */ ArrayOf<String>* schemes,
        /* [in] */ ILinkifyMatchFilter* matchFilter,
        /* [in] */ ILinkifyTransformFilter* transformFilter);

    static CARAPI_(void) GatherTelLinks(
        /* [in] */ List< AutoPtr<LinkSpec> >* links,
        /* [in] */ ISpannable* s);

    static CARAPI_(void) GatherMapLinks(
        /* [in] */ List< AutoPtr<LinkSpec> >* links,
        /* [in] */ ISpannable* s);

    static CARAPI_(void) PruneOverlaps(
        /* [in] */ List< AutoPtr<LinkSpec> >* links);

public:
    /**
     *  Filters out web URL matches that occur after an at-sign (@).  This is
     *  to prevent turning the domain name in an email address into a web link.
     */
    static const AutoPtr<ILinkifyMatchFilter> sUrlMatchFilter;

    /**
     *  Filters out URL matches that don't have enough digits to be a
     *  phone number.
     */
    static const AutoPtr<ILinkifyMatchFilter> sPhoneNumberMatchFilter;

    /**
     *  Transforms matched phone number text into something suitable
     *  to be used in a tel: URL.  It does this by removing everything
     *  but the digits and plus signs.  For instance:
     *  &apos;+1 (919) 555-1212&apos;
     *  becomes &apos;+19195551212&apos;
     */
    static const AutoPtr<ILinkifyTransformFilter> sPhoneNumberTransformFilter;

private:
    /**
     * Don't treat anything with fewer than this many digits as a
     * phone number.
     */
    static const Int32 PHONE_NUMBER_MINIMUM_DIGITS;// = 5;

private:
    Linkify();
    Linkify(const Linkify&);
};

} // namespace Utility
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_UTILITY_Linkify_H__
