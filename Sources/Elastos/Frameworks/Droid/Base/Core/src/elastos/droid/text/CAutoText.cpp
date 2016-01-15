
#include "Elastos.Droid.View.h"
#include "elastos/droid/text/CAutoText.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Text {

const Int32 CAutoText::TRIE_C = 0;
const Int32 CAutoText::TRIE_OFF = 1;
const Int32 CAutoText::TRIE_CHILD = 2;
const Int32 CAutoText::TRIE_NEXT = 3;
const Int32 CAutoText::TRIE_SIZEOF = 4;
const Char32 CAutoText::TRIE_NULL = (Char32) -1;
const Int32 CAutoText::TRIE_ROOT = 0;
const Int32 CAutoText::INCREMENT = 1024;
const Int32 CAutoText::DEFAULT = 14337; // Size of the Trie 13 Aug 2007
const Int32 CAutoText::RIGHT = 9300; // Size of 'right' 13 Aug 2007
Object CAutoText::sLock;

Boolean CAutoText::sInited = FALSE;
AutoPtr<IAutoText> CAutoText::sInstance;

CAR_INTERFACE_IMPL(CAutoText, Object, IAutoText)

CAR_OBJECT_IMPL(CAutoText)

CAutoText::CAutoText()
    : mTrieUsed(TRIE_ROOT)
    , mSize(0)
{
}

ECode CAutoText::constructor()
{
    return NOERROR;
}

ECode CAutoText::constructor(
    /* [in] */ IResources* resources)
{
    AutoPtr<IConfiguration> cfg;
    resources->GetConfiguration((IConfiguration**)&cfg);
    cfg -> GetLocale((ILocale**)&mLocale);
    Init(resources);
    return NOERROR;
}

AutoPtr<IAutoText> CAutoText::GetStaticInstance()
{
    if (!sInited) {
        sInited = TRUE;

        AutoPtr<IResources> res = CResources::GetSystem();

        AutoPtr<CAutoText> instance;
        CAutoText::NewByFriend((CAutoText**)&instance);
        instance->constructor(res);
        sInstance = (IAutoText*)instance.Get();
    }
    return sInstance;
}

AutoPtr<IAutoText> CAutoText::GetInstance(
    /* [in] */ IView* view)
{
    AutoPtr<IContext> ctx;
    view->GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);

    AutoPtr<IConfiguration> cfg;
    res->GetConfiguration((IConfiguration**)&cfg);
    AutoPtr<ILocale> locale;
    cfg->GetLocale((ILocale**)&locale);
    AutoPtr<IAutoText> instance;

    {
        AutoLock lock(sLock);
        instance = GetStaticInstance();
        Boolean bEqual;
        if (!(locale->Equals((((CAutoText*)(instance.Get()))->mLocale).Get(), &bEqual), bEqual)) {
            instance = NULL;
            AutoPtr<CAutoText> instance;
            CAutoText::NewByFriend((CAutoText**)&instance);
            instance->constructor(res);
            sInstance = (IAutoText*)instance.Get();
        }
    }
    return instance;
}

String CAutoText::Get(
    /* [in] */ ICharSequence* src,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ IView* view)
{
    return ((CAutoText*)(GetInstance(view).Get()))->Lookup(src, start, end);
}

Int32 CAutoText::GetSize(
    /* [in] */ IView* view)
{
    return ((CAutoText*)(GetInstance(view).Get()))->GetSize();
}

Int32 CAutoText::GetSize()
{
    return mSize;
}

String CAutoText::Lookup(
        /* [in] */ ICharSequence* src,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end)
{
    Int32 here = (*mTrie)[TRIE_ROOT];

    for (Int32 i = start; i < end; i++) {
        Char32 c ;
        src->GetCharAt(i, &c);

        for (; here != TRIE_NULL; here = (*mTrie)[here + TRIE_NEXT]) {
            if (c == (*mTrie)[here + TRIE_C]) {
                if ((i == end - 1)
                            && ((*mTrie)[here + TRIE_OFF] != TRIE_NULL)) {
                    Int32 off = (*mTrie)[here + TRIE_OFF];
                    Int32 len = mText.GetChar(off);

                    return mText.Substring(off + 1, off + 1 + len);
                }

                here = (*mTrie)[here + TRIE_CHILD];
                break;
            }
        }

        if (here == TRIE_NULL) {
            return String(NULL);
        }
    }

    return String(NULL);
}

void CAutoText::Init(
    /* [in] */ IResources* r)
{
    AutoPtr<IXmlResourceParser> parser;
    r->GetXml(R::xml::autotext, (IXmlResourceParser**)&parser);

    AutoPtr<StringBuilder> right = new StringBuilder(RIGHT);
    mTrie = ArrayOf<Char32>::Alloc(DEFAULT);
    (*mTrie)[TRIE_ROOT] = TRIE_NULL;
    mTrieUsed = TRIE_ROOT + 1;

//    try {
        IXmlPullParser* xpp = IXmlPullParser::Probe(parser);
        XmlUtils::BeginDocument(xpp, String("words"));
        String odest("");
        Char32 ooff = 0;

        while (TRUE) {
            XmlUtils::NextElement(xpp);

            String element;
            xpp->GetName(&element);
            if (element.IsNull() || !(element.Equals("word"))) {
                break;
            }

            String src;
            xpp->GetAttributeValue(String(NULL), String("src"), &src);
            Int32 next;
            if ((xpp->Next(&next), next) == IXmlPullParser::TEXT) {
                String dest;
                xpp->GetText(&dest);
                Char32 off;

                if (dest.Equals(odest)) {
                    off = ooff;
                }
                else {
                    Int32 rightLen;
                    right->GetLength(&rightLen);
                    off = (Char32) rightLen;
                    right->AppendChar((Char32) dest.GetLength());
                    right->Append(dest);
                }

                Add(src, off);
            }
        }

        // Don't let Resources cache a copy of all these strings.
        r->FlushLayoutCache();
//    } catch (XmlPullParserException e) {
//            throw new RuntimeException(e);
//    } catch (IOException e) {
//            throw new RuntimeException(e);
//    } finally {
        parser->Close();
//    }

    right->ToString(&mText);
}

void CAutoText::Add(
    /* [in] */ const String& src,
    /* [in] */ Char32 off)
{
    AutoPtr<ArrayOf<Char32> > chars = src.GetChars();
    Int32 slen = chars->GetLength();
    Int32 herep = TRIE_ROOT;
    // Keep track of the size of the dictionary
    mSize++;
    Char32 c;
    for (Int32 i = 0; i < slen; i++) {
        c = (*chars)[i];
        Boolean found = FALSE;

        for (; (*mTrie)[herep] != TRIE_NULL; herep = (*mTrie)[herep] + TRIE_NEXT) {
            if (c == (*mTrie)[(*mTrie)[herep] + TRIE_C]) {
                // There is a node for this letter, and this is the
                // end, so fill in the right hand side fields.

                if (i == slen - 1) {
                    (*mTrie)[(*mTrie)[herep] + TRIE_OFF] = off;
                    return;
                }

                // There is a node for this letter, and we need
                // to go deeper into it to fill in the rest.

                herep = (*mTrie)[herep] + TRIE_CHILD;
                found = TRUE;
                break;
            }
        }

        if (!found) {
            // No node for this letter yet.  Make one.

            Char32 node = NewTrieNode();
            (*mTrie)[herep] = node;

            (*mTrie)[(*mTrie)[herep] + TRIE_C] = c;
            (*mTrie)[(*mTrie)[herep] + TRIE_OFF] = TRIE_NULL;
            (*mTrie)[(*mTrie)[herep] + TRIE_NEXT] = TRIE_NULL;
            (*mTrie)[(*mTrie)[herep] + TRIE_CHILD] = TRIE_NULL;

            // If this is the end of the word, fill in the offset.

            if (i == slen - 1) {
                (*mTrie)[(*mTrie)[herep] + TRIE_OFF] = off;
                return;
            }

            // Otherwise, step in deeper and go to the next letter.

            herep = (*mTrie)[herep] + TRIE_CHILD;
        }
    }
}

Char32 CAutoText::NewTrieNode()
{
    if (mTrieUsed + TRIE_SIZEOF > (mTrie->GetLength()) ) {
        AutoPtr< ArrayOf<Char32> > copy = ArrayOf<Char32>::Alloc(mTrie->GetLength() + INCREMENT);
        for (Int32 i = 0; i < (mTrie->GetLength()); ++i)
        {
            (*copy)[i] = (*mTrie)[i] ;
        }
        mTrie = copy;
    }

    Char32 ret = mTrieUsed;
    mTrieUsed += TRIE_SIZEOF;

    return ret;
}


} // namespace Text
} // namepsace Droid
} // namespace Elastos
