
#include "Elastos.CoreLibrary.Utility.h"
#include "CaseMapper.h"
#include "ICUUtil.h"
#include "Character.h"
#include "CTransliterator.h"

using Libcore::ICU::ICUUtil;
using Libcore::ICU::CTransliterator;

namespace Elastos {
namespace Core {

// Char32[] sUpperValues = "SS\u0000\u02bcN\u0000J\u030c\u0000\u0399\u0308\u0301\u03a5\u0308\u0301\u0535\u0552\u0000H\u0331\u0000T\u0308\u0000W\u030a\u0000Y\u030a\u0000A\u02be\u0000\u03a5\u0313\u0000\u03a5\u0313\u0300\u03a5\u0313\u0301\u03a5\u0313\u0342\u1f08\u0399\u0000\u1f09\u0399\u0000\u1f0a\u0399\u0000\u1f0b\u0399\u0000\u1f0c\u0399\u0000\u1f0d\u0399\u0000\u1f0e\u0399\u0000\u1f0f\u0399\u0000\u1f08\u0399\u0000\u1f09\u0399\u0000\u1f0a\u0399\u0000\u1f0b\u0399\u0000\u1f0c\u0399\u0000\u1f0d\u0399\u0000\u1f0e\u0399\u0000\u1f0f\u0399\u0000\u1f28\u0399\u0000\u1f29\u0399\u0000\u1f2a\u0399\u0000\u1f2b\u0399\u0000\u1f2c\u0399\u0000\u1f2d\u0399\u0000\u1f2e\u0399\u0000\u1f2f\u0399\u0000\u1f28\u0399\u0000\u1f29\u0399\u0000\u1f2a\u0399\u0000\u1f2b\u0399\u0000\u1f2c\u0399\u0000\u1f2d\u0399\u0000\u1f2e\u0399\u0000\u1f2f\u0399\u0000\u1f68\u0399\u0000\u1f69\u0399\u0000\u1f6a\u0399\u0000\u1f6b\u0399\u0000\u1f6c\u0399\u0000\u1f6d\u0399\u0000\u1f6e\u0399\u0000\u1f6f\u0399\u0000\u1f68\u0399\u0000\u1f69\u0399\u0000\u1f6a\u0399\u0000\u1f6b\u0399\u0000\u1f6c\u0399\u0000\u1f6d\u0399\u0000\u1f6e\u0399\u0000\u1f6f\u0399\u0000\u1fba\u0399\u0000\u0391\u0399\u0000\u0386\u0399\u0000\u0391\u0342\u0000\u0391\u0342\u0399\u0391\u0399\u0000\u1fca\u0399\u0000\u0397\u0399\u0000\u0389\u0399\u0000\u0397\u0342\u0000\u0397\u0342\u0399\u0397\u0399\u0000\u0399\u0308\u0300\u0399\u0308\u0301\u0399\u0342\u0000\u0399\u0308\u0342\u03a5\u0308\u0300\u03a5\u0308\u0301\u03a1\u0313\u0000\u03a5\u0342\u0000\u03a5\u0308\u0342\u1ffa\u0399\u0000\u03a9\u0399\u0000\u038f\u0399\u0000\u03a9\u0342\u0000\u03a9\u0342\u0399\u03a9\u0399\u0000FF\u0000FI\u0000FL\u0000FFIFFLST\u0000ST\u0000\u0544\u0546\u0000\u0544\u0535\u0000\u0544\u053b\u0000\u054e\u0546\u0000\u0544\u053d\u0000".toCharArray();
// Char32[] sUpperValues2 = "\u000b\u0000\f\u0000\r\u0000\u000e\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u000f\u0010\u0011\u0012\u0013\u0014\u0015\u0016\u0017\u0018\u0019\u001a\u001b\u001c\u001d\u001e\u001f !\"#$%&'()*+,-./0123456789:;<=>\u0000\u0000?@A\u0000BC\u0000\u0000\u0000\u0000D\u0000\u0000\u0000\u0000\u0000EFG\u0000HI\u0000\u0000\u0000\u0000J\u0000\u0000\u0000\u0000\u0000KL\u0000\u0000MN\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000OPQ\u0000RS\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000TUV\u0000WX\u0000\u0000\u0000\u0000Y".toCharArray();
const Char32 CaseMapper::sUpperValues[306] = {
      0x0053, 0x0053, 0x0000, 0x02BC, 0x004E, 0x0000, 0x004A, 0x030C
    , 0x0000, 0x0399, 0x0308, 0x0301, 0x03A5, 0x0308, 0x0301, 0x0535
    , 0x0552, 0x0000, 0x0048, 0x0331, 0x0000, 0x0054, 0x0308, 0x0000
    , 0x0057, 0x030A, 0x0000, 0x0059, 0x030A, 0x0000, 0x0041, 0x02BE
    , 0x0000, 0x03A5, 0x0313, 0x0000, 0x03A5, 0x0313, 0x0300, 0x03A5
    , 0x0313, 0x0301, 0x03A5, 0x0313, 0x0342, 0x1F08, 0x0399, 0x0000
    , 0x1F09, 0x0399, 0x0000, 0x1F0A, 0x0399, 0x0000, 0x1F0B, 0x0399
    , 0x0000, 0x1F0C, 0x0399, 0x0000, 0x1F0D, 0x0399, 0x0000, 0x1F0E
    , 0x0399, 0x0000, 0x1F0F, 0x0399, 0x0000, 0x1F08, 0x0399, 0x0000
    , 0x1F09, 0x0399, 0x0000, 0x1F0A, 0x0399, 0x0000, 0x1F0B, 0x0399
    , 0x0000, 0x1F0C, 0x0399, 0x0000, 0x1F0D, 0x0399, 0x0000, 0x1F0E
    , 0x0399, 0x0000, 0x1F0F, 0x0399, 0x0000, 0x1F28, 0x0399, 0x0000
    , 0x1F29, 0x0399, 0x0000, 0x1F2A, 0x0399, 0x0000, 0x1F2B, 0x0399
    , 0x0000, 0x1F2C, 0x0399, 0x0000, 0x1F2D, 0x0399, 0x0000, 0x1F2E
    , 0x0399, 0x0000, 0x1F2F, 0x0399, 0x0000, 0x1F28, 0x0399, 0x0000
    , 0x1F29, 0x0399, 0x0000, 0x1F2A, 0x0399, 0x0000, 0x1F2B, 0x0399
    , 0x0000, 0x1F2C, 0x0399, 0x0000, 0x1F2D, 0x0399, 0x0000, 0x1F2E
    , 0x0399, 0x0000, 0x1F2F, 0x0399, 0x0000, 0x1F68, 0x0399, 0x0000
    , 0x1F69, 0x0399, 0x0000, 0x1F6A, 0x0399, 0x0000, 0x1F6B, 0x0399
    , 0x0000, 0x1F6C, 0x0399, 0x0000, 0x1F6D, 0x0399, 0x0000, 0x1F6E
    , 0x0399, 0x0000, 0x1F6F, 0x0399, 0x0000, 0x1F68, 0x0399, 0x0000
    , 0x1F69, 0x0399, 0x0000, 0x1F6A, 0x0399, 0x0000, 0x1F6B, 0x0399
    , 0x0000, 0x1F6C, 0x0399, 0x0000, 0x1F6D, 0x0399, 0x0000, 0x1F6E
    , 0x0399, 0x0000, 0x1F6F, 0x0399, 0x0000, 0x1FBA, 0x0399, 0x0000
    , 0x0391, 0x0399, 0x0000, 0x0386, 0x0399, 0x0000, 0x0391, 0x0342
    , 0x0000, 0x0391, 0x0342, 0x0399, 0x0391, 0x0399, 0x0000, 0x1FCA
    , 0x0399, 0x0000, 0x0397, 0x0399, 0x0000, 0x0389, 0x0399, 0x0000
    , 0x0397, 0x0342, 0x0000, 0x0397, 0x0342, 0x0399, 0x0397, 0x0399
    , 0x0000, 0x0399, 0x0308, 0x0300, 0x0399, 0x0308, 0x0301, 0x0399
    , 0x0342, 0x0000, 0x0399, 0x0308, 0x0342, 0x03A5, 0x0308, 0x0300
    , 0x03A5, 0x0308, 0x0301, 0x03A1, 0x0313, 0x0000, 0x03A5, 0x0342
    , 0x0000, 0x03A5, 0x0308, 0x0342, 0x1FFA, 0x0399, 0x0000, 0x03A9
    , 0x0399, 0x0000, 0x038F, 0x0399, 0x0000, 0x03A9, 0x0342, 0x0000
    , 0x03A9, 0x0342, 0x0399, 0x03A9, 0x0399, 0x0000, 0x0046, 0x0046
    , 0x0000, 0x0046, 0x0049, 0x0000, 0x0046, 0x004C, 0x0000, 0x0046
    , 0x0046, 0x0049, 0x0046, 0x0046, 0x004C, 0x0053, 0x0054, 0x0000
    , 0x0053, 0x0054, 0x0000, 0x0544, 0x0546, 0x0000, 0x0544, 0x0535
    , 0x0000, 0x0544, 0x053B, 0x0000, 0x054E, 0x0546, 0x0000, 0x0544
    , 0x053D, 0x0000
};

const Char32 CaseMapper::sUpperValues2[173] = {
      0x000B, 0x0000, 0x000C, 0x0000, 0x000D, 0x0000, 0x000E, 0x0000
    , 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
    , 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
    , 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
    , 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
    , 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
    , 0x000F, 0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016
    , 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E
    , 0x001F, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026
    , 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E
    , 0x002F, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036
    , 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E
    , 0x0000, 0x0000, 0x003F, 0x0040, 0x0041, 0x0000, 0x0042, 0x0043
    , 0x0000, 0x0000, 0x0000, 0x0000, 0x0044, 0x0000, 0x0000, 0x0000
    , 0x0000, 0x0000, 0x0045, 0x0046, 0x0047, 0x0000, 0x0048, 0x0049
    , 0x0000, 0x0000, 0x0000, 0x0000, 0x004A, 0x0000, 0x0000, 0x0000
    , 0x0000, 0x0000, 0x004B, 0x004C, 0x0000, 0x0000, 0x004D, 0x004E
    , 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
    , 0x0000, 0x0000, 0x004F, 0x0050, 0x0051, 0x0000, 0x0052, 0x0053
    , 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
    , 0x0000, 0x0000, 0x0054, 0x0055, 0x0056, 0x0000, 0x0057, 0x0058
    , 0x0000, 0x0000, 0x0000, 0x0000, 0x0059
};

const Char32 CaseMapper::LATIN_CAPITAL_I_WITH_DOT = 0x0130; //'\u0130';
const Char32 CaseMapper::GREEK_CAPITAL_SIGMA = 0x03a3;      //'\u03a3';
const Char32 CaseMapper::GREEK_SMALL_FINAL_SIGMA = 0x03c2;  //'\u03c2';

// static final ThreadLocal<Transliterator> EL_UPPER

pthread_key_t CaseMapper::sTlsKey;
pthread_once_t CaseMapper::sTlsKeyOnce = PTHREAD_ONCE_INIT;

static void TransliteratorDestructor(void* st)
{
    ITransliterator* obj = static_cast<ITransliterator*>(st);
    if (obj) {
        obj->Release();
    }
}

static void MakeKey()
{
    ASSERT_TRUE(pthread_key_create(&CaseMapper::sTlsKey, TransliteratorDestructor) == 0);
}

AutoPtr<ITransliterator> CaseMapper::GetEL_UPPER()
{
    pthread_once(&sTlsKeyOnce, MakeKey);

    AutoPtr<ITransliterator> tl = (ITransliterator*)pthread_getspecific(sTlsKey);
    if (tl == NULL) {
        CTransliterator::New(String("el-Upper"), (ITransliterator**)&tl);

        ASSERT_TRUE(pthread_setspecific(sTlsKey, tl.Get()) == 0);
        tl->AddRef();
        tl = (ITransliterator*)pthread_getspecific(sTlsKey);
    }
    assert(tl.Get() != NULL && "check Transliterator failed!");
    return tl;
}

String CaseMapper::ToLowerCase(
    /* [in] */ ILocale* locale,
    /* [in] */ const String& s,
    /* [in] */ ArrayOf<Char32>* value,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    assert(locale);
    assert(value);

    // Punt hard cases to ICU4C.
    // Note that Greek isn't a particularly hard case for toLowerCase, only toUpperCase.
    String languageCode;
    locale->GetLanguage(&languageCode);
    if (languageCode.Equals("tr") || languageCode.Equals("az") || languageCode.Equals("lt")) {
        return ICUUtil::ToLowerCase(s, locale);
    }

    AutoPtr<ArrayOf<Char32> > newValue;
    Int32 newCount = 0;
    Char32 ch;
    for (Int32 i = offset, end = offset + count; i < end; ++i) {
        ch = (*value)[i];
        Char32 newCh;
        if (ch == LATIN_CAPITAL_I_WITH_DOT || Character::IsHighSurrogate(ch)) {
            // Punt these hard cases.
            return ICUUtil::ToLowerCase(s, locale);
        }
        else if (ch == GREEK_CAPITAL_SIGMA && IsFinalSigma(value, offset, count, i)) {
            newCh = GREEK_SMALL_FINAL_SIGMA;
        }
        else {
            newCh = Character::ToLowerCase(ch);
        }
        if (newValue == NULL && ch != newCh) {
            newValue = ArrayOf<Char32>::Alloc(count); // The result can't be longer than the input.
            newCount = i - offset;
            newValue->Copy(value, offset, newCount);
        }
        if (newValue != NULL) {
            newValue->Set(newCount++, newCh);
        }
    }
    return newValue != NULL ? String(*newValue, 0, newCount) : s;
}

String CaseMapper::ToUpperCase(
    /* [in] */ ILocale* locale,
    /* [in] */ const String& s,
    /* [in] */ ArrayOf<Char32>* value,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    assert(locale);
    assert(value);

    pthread_once(&sTlsKeyOnce, MakeKey);
    pthread_setspecific(sTlsKey, NULL);

    String languageCode;
    locale->GetLanguage(&languageCode);
    if (languageCode.Equals("tr") || languageCode.Equals("az") || languageCode.Equals("lt")) {
        return ICUUtil::ToUpperCase(s, locale);
    }
    if (languageCode.Equals("el")) {
        AutoPtr<ITransliterator> tl = GetEL_UPPER();
        String upper;
        tl->Transliterate(s, &upper);
        return upper;
    }

    AutoPtr<ArrayOf<Char32> > output;
    Int32 i = 0;
    Char32 ch;
    for (Int32 o = offset, end = offset + count; o < end; o++) {
        ch = (*value)[o];
        if (Character::IsHighSurrogate(ch)) {
            return ICUUtil::ToUpperCase(s, locale);
        }

        Int32 index = UpperIndex(ch);
        if (index == -1) {
            if (output != NULL && i >= output->GetLength()) {
                AutoPtr<ArrayOf<Char32> > newoutput = ArrayOf<Char32>::Alloc(output->GetLength() + (count / 6) + 2);
                newoutput->Copy(output);
                output = newoutput;
            }

            Char32 upch = Character::ToUpperCase(ch);
            if (ch != upch) {
                if (output == NULL) {
                    output = ArrayOf<Char32>::Alloc(count);
                    i = o - offset;
                    output->Copy(value, offset, i);
                }
                output->Set(i++, upch);
            }
            else if (output != NULL) {
                output->Set(i++, ch);
            }
        }
        else {
            Int32 target = index * 3;
            Char32 val3 = sUpperValues[target + 2];
            if (output == NULL) {
                output = ArrayOf<Char32>::Alloc(count + (count / 6) + 2);
                i = o - offset;
                output->Copy(value, offset, i);
            }
            else if (i + (val3 == 0 ? 1 : 2) >= output->GetLength()) {
                AutoPtr<ArrayOf<Char32> > newoutput = ArrayOf<Char32>::Alloc(output->GetLength() + (count / 6) + 3);
                newoutput->Copy(output);
                output = newoutput;
            }

            Char32 val = sUpperValues[target];
            output->Set(i++, val);
            val = sUpperValues[target + 1];
            output->Set(i++, val);
            if (val3 != 0) {
                (*output)[i++] = val3;
            }
        }
    }
    if (output == NULL) {
        return s;
    }

    //return output.length == i || output.length - i < 8 ? new String(0, i, output) : new String(output, 0, i);
    //    public String(byte[] data, int offset, int byteCount) {
    //     this(data, offset, byteCount, Charset.defaultCharset());
    // }
    // /*
    //  * Internal version of the String(char[], int, int) constructor.
    //  * Does not range check, null check, or copy the array.
    //  */
    // String(int offset, int charCount, char[] chars) {
    //     this.value = chars;
    //     this.offset = offset;
    //     this.count = charCount;
    // }
    return output->GetLength() == i || output->GetLength() - i < 8
        ? String(*output, 0, i) : String(*output, 0, i);
}

Boolean CaseMapper::IsFinalSigma(
    /* [in] */ ArrayOf<Char32>* value,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ Int32 index)
{
    assert(value);
    // TODO: we don't skip case-ignorable sequences like we should.
    // TODO: we should add a more direct way to test for a cased letter.
    if (index <= offset) {
        return FALSE;
    }
    Char32 previous = (*value)[index - 1];
    if (!(Character::IsLowerCase(previous) || Character::IsUpperCase(previous) || Character::IsTitleCase(previous))) {
        return FALSE;
    }
    if (index + 1 >= offset + count) {
        return TRUE;
    }
    Char32 next = (*value)[index + 1];
    if (Character::IsLowerCase(next) || Character::IsUpperCase(next) || Character::IsTitleCase(next)) {
        return FALSE;
    }
    return TRUE;
}

Int32 CaseMapper::UpperIndex(
    /* [in] */ Int32 ch)
{
    Int32 index = -1;
    if (ch >= 0xdf) {
        if (ch <= 0x587) {
            switch (ch) {
            case 0xdf: return 0;
            case 0x149: return 1;
            case 0x1f0: return 2;
            case 0x390: return 3;
            case 0x3b0: return 4;
            case 0x587: return 5;
            }
        } else if (ch >= 0x1e96) {
            if (ch <= 0x1e9a) {
                index = 6 + ch - 0x1e96;
            } else if (ch >= 0x1f50 && ch <= 0x1ffc) {
                index = sUpperValues2[ch - 0x1f50];
                if (index == 0) {
                    index = -1;
                }
            } else if (ch >= 0xfb00) {
                if (ch <= 0xfb06) {
                    index = 90 + ch - 0xfb00;
                } else if (ch >= 0xfb13 && ch <= 0xfb17) {
                    index = 97 + ch - 0xfb13;
                }
            }
        }
    }
    return index;
}

} // namespace Core
} // namespace Elastos
