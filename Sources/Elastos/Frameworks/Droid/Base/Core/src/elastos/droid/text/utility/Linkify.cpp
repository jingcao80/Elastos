
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/text/utility/Linkify.h"
#include "elastos/droid/text/CSpannableString.h"
#include "elastos/droid/text/method/LinkMovementMethod.h"
#include "elastos/droid/text/style/CURLSpan.h"
//#include "elastos/droid/webkit/CWebView.h"
#include "elastos/droid/utility/Patterns.h"
#include <elastos/core/Character.h>

using Elastos::Droid::Utility::Patterns;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::CSpannableString;
using Elastos::Droid::Text::Method::IMovementMethod;
using Elastos::Droid::Text::Method::ILinkMovementMethod;
using Elastos::Droid::Text::Method::EIID_ILinkMovementMethod;
using Elastos::Droid::Text::Method::LinkMovementMethod;
using Elastos::Droid::Text::Style::IURLSpan;
using Elastos::Droid::Text::Style::EIID_IURLSpan;
using Elastos::Droid::Text::Style::CURLSpan;

// import com.android.i18n.phonenumbers.PhoneNumberMatch;
// import com.android.i18n.phonenumbers.PhoneNumberUtil;
// import com.android.i18n.phonenumbers.PhoneNumberUtil.Leniency;

using Elastos::Core::Character;
using Elastos::Core::EIID_IComparator;
using Elastos::Net::CURLEncoder;
using Elastos::Net::CURLEncoder;
using Elastos::Net::IURLEncoder;
using Elastos::Utility::ILocale;
using Elastos::Utility::Regex::IMatchResult;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

const AutoPtr<ILinkifyMatchFilter> Linkify::sUrlMatchFilter = new Linkify::MatchFilterUrl();
const AutoPtr<ILinkifyMatchFilter> Linkify::sPhoneNumberMatchFilter = new Linkify::MatchFilterPhoneNumber();
const AutoPtr<ILinkifyTransformFilter> Linkify::sPhoneNumberTransformFilter = new Linkify::TransformFilterPhoneNumber();

const Int32 Linkify::PHONE_NUMBER_MINIMUM_DIGITS = 5;

/*****************************Linkify::MatchFilterUrl*****************************/
CAR_INTERFACE_IMPL(Linkify::MatchFilterUrl, Object, ILinkifyMatchFilter)

ECode Linkify::MatchFilterUrl::AcceptMatch(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = TRUE;
    VALIDATE_NOT_NULL(s);

    if (start == 0) {
        return NOERROR;
    }

    Char32 c;
    s->GetCharAt(start - 1, &c);
    if (c == '@') {
        *ret = FALSE;
        return NOERROR;
    }

    return NOERROR;
}

/*****************************Linkify::MatchFilterPhoneNumber*****************************/
CAR_INTERFACE_IMPL(Linkify::MatchFilterPhoneNumber, Object, ILinkifyMatchFilter)

ECode Linkify::MatchFilterPhoneNumber::AcceptMatch(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = FALSE;
    VALIDATE_NOT_NULL(s);

    Int32 digitCount = 0;
    Char32 c;
    for (Int32 i = start; i < end; i++) {
        s->GetCharAt(i, &c);
        if (Character::IsDigit(c)) {
            digitCount++;
            if (digitCount >= PHONE_NUMBER_MINIMUM_DIGITS) {
                *ret = TRUE;
                return NOERROR;
            }
        }
    }

    return NOERROR;
}

/*****************************Linkify::TransformFilterPhoneNumber*****************************/
CAR_INTERFACE_IMPL(Linkify::TransformFilterPhoneNumber, Object, ILinkifyTransformFilter)

ECode Linkify::TransformFilterPhoneNumber::TransformUrl(
    /* [in] */ IMatcher* match,
    /* [in] */ const String & url,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = Patterns::DigitsAndPlusOnly(match);
    return NOERROR;
}

/*****************************Linkify::ComparatorLinkSpec*****************************/

Boolean Linkify::ComparatorLinkSpec::operator() (
    /* [in] */ LinkSpec* a,
    /* [in] */ LinkSpec* b)
{
    assert(a && b);
    LinkSpec* aT = (LinkSpec*)a;
    LinkSpec* bT = (LinkSpec*)b;

    if (aT->start < bT->start) {
        return TRUE;
    }

    if (aT->start > bT->start) {
        return FALSE;
    }

    if (aT->end < bT->end) {
        return TRUE;
    }

    if (aT->end > bT->end) {
        return FALSE;
    }
    return TRUE;
}

/*****************************Linkify*****************************/

Boolean Linkify::AddLinks(
    /* [in] */ ISpannable* text,
    /* [in] */ Int32 mask)
{
    if (mask == 0) {
        return FALSE;
    }

    ICharSequence* csq = ICharSequence::Probe(text);
    ISpanned* spanned = ISpanned::Probe(text);
    Int32 textLen;
    csq->GetLength(&textLen);
    AutoPtr< ArrayOf<IURLSpan*> > old;
    spanned->GetSpans(0, textLen, EIID_IURLSpan, (ArrayOf<IInterface*>**)&old);

    if (old != NULL) {
        for (Int32 i = old->GetLength() - 1; i >= 0; i--) {
            text->RemoveSpan((*old)[i]);
        }
    }

    List< AutoPtr<LinkSpec> > links;

    if ((mask & ILinkify::WEB_URLS) != 0) {
        AutoPtr< ArrayOf<String> > ary = ArrayOf<String>::Alloc(3);
        (*ary)[0] = String("http://");
        (*ary)[1] = String("https://");
        (*ary)[2] = String("rtsp://");
        GatherLinks(&links, text, Patterns::WEB_URL, ary, sUrlMatchFilter, NULL);
    }

    if ((mask & ILinkify::EMAIL_ADDRESSES) != 0) {
        AutoPtr< ArrayOf<String> > ary = ArrayOf<String>::Alloc(1);
        (*ary)[0] = String("mailto:");
        GatherLinks(&links, text, Patterns::EMAIL_ADDRESS, ary, NULL, NULL);
    }

    if ((mask & ILinkify::PHONE_NUMBERS) != 0) {
        GatherTelLinks(&links, text);
    }

    if ((mask & ILinkify::MAP_ADDRESSES) != 0) {
        GatherMapLinks(&links, text);
    }

    PruneOverlaps(&links);

    Int32 linksSize = links.GetSize();
    if (linksSize == 0) {
        return FALSE;
    }

    List< AutoPtr<LinkSpec> >::ReverseIterator it = links.RBegin();
    for (; it != links.REnd(); ++it) {
        AutoPtr<LinkSpec> link = *it;
        ApplyLink(link->url, link->start, link->end, text);
    }

    return TRUE;
}

Boolean Linkify::AddLinks(
    /* [in] */ ITextView* text,
    /* [in] */ Int32 mask)
{
    if (mask == 0) {
        return FALSE;
    }

    AutoPtr<ICharSequence> t;
    text->GetText((ICharSequence**)&t);

    AutoPtr<ISpannable> sp = ISpannable::Probe(t);

    if (sp != NULL) {
        if (AddLinks(sp, mask)) {
            AddLinkMovementMethod(text);
            return TRUE;
        }

        return FALSE;
    }
    else {
        AutoPtr<ISpannableString> s = CSpannableString::ValueOf(t);

        if (AddLinks(ISpannable::Probe(s), mask)) {
            AddLinkMovementMethod(text);
            text->SetText(ICharSequence::Probe(s));

            return TRUE;
        }

        return FALSE;
    }
}

ECode Linkify::AddLinkMovementMethod(
    /* [in] */ ITextView* t)
{
    AutoPtr<IMovementMethod> m;
    t->GetMovementMethod((IMovementMethod**)&m);

    if (ILinkMovementMethod::Probe(m) == NULL) {
        Boolean bLinksClickable;
        t->GetLinksClickable(&bLinksClickable);
        if (bLinksClickable) {
            assert(0 && "TODO");
            m = NULL;
            LinkMovementMethod::GetInstance((IMovementMethod**)&m);
            t->SetMovementMethod(m);
        }
    }

    return NOERROR;
}

ECode Linkify::AddLinks(
    /* [in] */ ITextView* text,
    /* [in] */ IPattern* pattern,
    /* [in] */ const String& scheme)
{
    return AddLinks(text, pattern, scheme, NULL, NULL);
}

ECode Linkify::AddLinks(
    /* [in] */ ITextView* text,
    /* [in] */ IPattern* p,
    /* [in] */ const String& scheme,
    /* [in] */ ILinkifyMatchFilter* matchFilter,
    /* [in] */ ILinkifyTransformFilter* transformFilter)
{
    AutoPtr<ICharSequence> cs;
    text->GetText((ICharSequence**)&cs);

    AutoPtr<ISpannableString> s = CSpannableString::ValueOf(cs);

    if (AddLinks(ISpannable::Probe(s), p, scheme, matchFilter, transformFilter)) {
        text->SetText(ICharSequence::Probe(s));
        return AddLinkMovementMethod(text);
    }

    return NOERROR;
}

Boolean Linkify::AddLinks(
    /* [in] */ ISpannable* text,
    /* [in] */ IPattern* pattern,
    /* [in] */ const String& scheme)
{
    return AddLinks(text, pattern, scheme, NULL, NULL);
}

Boolean Linkify::AddLinks(
    /* [in] */ ISpannable* s,
    /* [in] */ IPattern* p,
    /* [in] */ const String& scheme,
    /* [in] */ ILinkifyMatchFilter* matchFilter,
    /* [in] */ ILinkifyTransformFilter* transformFilter)
{
    ICharSequence* csq = ICharSequence::Probe(s);
    Boolean hasMatches = FALSE;
    String prefix("");
    if (!scheme.IsNull()) {
        prefix = scheme.ToLowerCase();//scheme.toLowerCase(Locale.ROOT);
    }

    AutoPtr<IMatcher> m;
    p->Matcher(csq, (IMatcher**)&m);
    IMatchResult* mr = IMatchResult::Probe(m);
    Boolean bFind, allowed;
    Int32 start, end;
    String group, url;

    while ((m->Find(&bFind), bFind)) {
        mr->Start(&start);
        mr->End(&end);
        allowed = TRUE;
        if (matchFilter != NULL) {
            matchFilter->AcceptMatch(csq, start, end, &allowed);
        }

        if (allowed) {
            AutoPtr< ArrayOf<String> > ary = ArrayOf<String>::Alloc(1);
            (*ary)[0] = prefix;

            mr->Group(0, &group);
            url = MakeUrl(group, ary, m, transformFilter);

            ApplyLink(url, start, end, s);
            hasMatches = TRUE;
        }
    }

    return hasMatches;
}

void Linkify::ApplyLink(
    /* [in] */ const String& url,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ISpannable* text)
{
    AutoPtr<IURLSpan> span;
    CURLSpan::New(url, (IURLSpan**)&span);

    text->SetSpan(span, start, end, ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
}

String Linkify::MakeUrl(
    /* [in] */ const String& _url,
    /* [in] */ ArrayOf<String>* prefixes,
    /* [in] */ IMatcher* m,
    /* [in] */ ILinkifyTransformFilter* filter)
{
    String url(_url);
    if (filter != NULL) {
        filter->TransformUrl(m, url, &url);
    }

    Boolean hasPrefix = FALSE;

    for (Int32 i = 0; i < prefixes->GetLength(); i++) {
        if ( url.StartWith( (*prefixes)[i])) {
            hasPrefix = TRUE;

            // Fix capitalization if necessary
            if ( !(url.StartWith( (*prefixes)[i]))) {
                url = (*prefixes)[i] + url.Substring(((*prefixes)[i]).GetLength());
            }

            break;
        }
    }

    if (!hasPrefix) {
        url = (*prefixes)[0] + url;
    }

    return url;
}

void Linkify::GatherLinks(
    /* [in] */ List< AutoPtr<LinkSpec> >* links,
    /* [in] */ ISpannable* s,
    /* [in] */ IPattern* pattern,
    /* [in] */ ArrayOf<String>* schemes,
    /* [in] */ ILinkifyMatchFilter* matchFilter,
    /* [in] */ ILinkifyTransformFilter* transformFilter)
{
    ICharSequence* csq = ICharSequence::Probe(s);
    AutoPtr<IMatcher> m;
    pattern->Matcher(csq, (IMatcher**)&m);
    IMatchResult* mr = IMatchResult::Probe(m);

    Boolean mFind, bAcceptMatch;
    Int32 start, end;
    String group, url;
    while ((m->Find(&mFind), mFind)) {
        mr->Start(&start);
        mr->End(&end);

        if (matchFilter == NULL || (matchFilter->AcceptMatch(csq, start, end, &bAcceptMatch), bAcceptMatch)) {
            mr->Group(0, &group);
            url = MakeUrl(group, schemes, m, transformFilter);

            AutoPtr<LinkSpec> spec = new LinkSpec();
            spec->url = url;
            spec->start = start;
            spec->end = end;

            links->PushBack(spec);
        }
    }
}

void Linkify::GatherTelLinks(
    /* [in] */ List< AutoPtr<LinkSpec> >* links,
    /* [in] */ ISpannable* s)
{
    assert(0 && "TODO");
    // PhoneNumberUtil phoneUtil = PhoneNumberUtil.getInstance();
    // Iterable<PhoneNumberMatch> matches = phoneUtil.findNumbers(s.toString(),
    //         Locale.getDefault().getCountry(), Leniency.POSSIBLE, Long.MAX_VALUE);
    // for (PhoneNumberMatch match : matches) {
    //     LinkSpec spec = new LinkSpec();
    //     spec.url = "tel:" + PhoneNumberUtils.normalizeNumber(match.rawString());
    //     spec.start = match.start();
    //     spec.end = match.end();
    //     links.add(spec);
    // }
}

void Linkify::GatherMapLinks(
    /* [in] */ List< AutoPtr<LinkSpec> >* links,
    /* [in] */ ISpannable* s)
{
    String string = Object::ToString(s);
    String address;
    Int32 base = 0;
    Int32 length, end;

    String encodedAddress;

//    address = CWebView::FindAddress(string);
    while (!address.IsNull()) {
        Int32 start = string.IndexOf(address);

        if (start < 0) {
            break;
        }

        AutoPtr<LinkSpec> spec = new LinkSpec();
        length = address.GetLength();
        end = start + length;

        spec->start = base + start;
        spec->end = base + end;
        string = string.Substring(end);
        base += end;

        //try {
            AutoPtr<IURLEncoder> sUrlEncoder;
            CURLEncoder::AcquireSingleton((IURLEncoder**)&sUrlEncoder);
            sUrlEncoder->Encode(address, String("UTF-8"), &encodedAddress);
        //} catch (UnsupportedEncodingException e) {
            //continue;
        //}

        spec->url = String("geo:0,0?q=") + encodedAddress;
        links->PushBack(spec);
    }
}

void Linkify::PruneOverlaps(
    /* [in] */ List< AutoPtr<LinkSpec> >* links)
{
//    AutoPtr<ComparatorLinkSpec> c = new ComparatorLinkSpec();
    Linkify::ComparatorLinkSpec func;
    links->Sort(func);

    Int32 len = links->GetSize();
    Int32 i = 0;
    Int32 remove;

    while (i < len - 1) {
        LinkSpec* a = ( (*links)[i] );
        LinkSpec* b = ( (*links)[i + 1] );
        remove = -1;

        if ((a->start <= b->start) && (a->end > b->start)) {
            if (b->end <= a->end) {
                remove = i + 1;
            }
            else if ((a->end - a->start) > (b->end - b->start)) {
                remove = i + 1;
            }
            else if ((a->end - a->start) < (b->end - b->start)) {
                remove = i;
            }

            if (remove != -1) {
                links->Remove(remove);
                len--;
                continue;
            }

        }

        i++;
    }
}

AutoPtr<ILinkifyMatchFilter> Linkify::GetStaticUrlMatchFilter()
{
    return sUrlMatchFilter;
}

AutoPtr<ILinkifyMatchFilter> Linkify::GetStaticPhoneNumberMatchFilter()
{
    return sPhoneNumberMatchFilter;
}

AutoPtr<ILinkifyTransformFilter> Linkify::GetStaticPhoneNumberTransformFilter()
{
    return sPhoneNumberTransformFilter;
}

} // namespace Utility
} // namespace Text
} // namepsace Droid
} // namespace Elastos
