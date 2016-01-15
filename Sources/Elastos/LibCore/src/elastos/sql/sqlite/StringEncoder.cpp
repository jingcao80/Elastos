
#include "StringEncoder.h"
#include "StringBuffer.h"

using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Sql {
namespace SQLite {

Boolean StringEncoder::Init()
{
    xdigits = ArrayOf<Char32>::Alloc(16);
    (*xdigits)[0] = '0';
    (*xdigits)[1] = '1';
    (*xdigits)[2] = '2';
    (*xdigits)[3] = '3';
    (*xdigits)[4] = '4';
    (*xdigits)[5] = '5';
    (*xdigits)[6] = '6';
    (*xdigits)[7] = '7';
    (*xdigits)[8] = '8';
    (*xdigits)[9] = '9';
    (*xdigits)[10] = 'A';
    (*xdigits)[11] = 'B';
    (*xdigits)[12] = 'C';
    (*xdigits)[13] = 'D';
    (*xdigits)[14] = 'E';
    (*xdigits)[15] = 'F';
    return FALSE;
}

Boolean StringEncoder::initflag = Init();
AutoPtr<ArrayOf<Char32> > StringEncoder::xdigits;

String StringEncoder::Encode(
    /* [in] */ ArrayOf<Byte> * a)
{
    // check input
    if (a == NULL || a->GetLength() ==0) {
        return String("x");
    }

    // determine count
    AutoPtr<ArrayOf<Int32> > cnt = ArrayOf<Int32>::Alloc(256);
    for (Int32 i = 0; i < a->GetLength(); ++i)
    {
        (*cnt)[ (*a)[i] & 0xff ]++;
    }
    // determine shift for minimum number of escapes
    Int32 shift = 1;
    Int32 nEscapes = a->GetLength();
    for (Int32 i = 1; i < 256; i++) {
        if (i == '\'') {
        continue;
        }
        Int32 sum = (*cnt)[i] + (*cnt)[(i + 1) & 0xff] + (*cnt)[(i + '\'') & 0xff];
        if (sum < nEscapes) {
        nEscapes = sum;
        shift = i;
        if (nEscapes == 0) {
            // cannot become smaller
            break;
        }
        }
    }
    // construct encoded output
    Int32 outLen = a->GetLength() + nEscapes + 1;
    StringBuffer out(outLen);
    out.AppendChar((Char32)shift);
    for (Int32 i = 0; i < a->GetLength(); i++) {
        // apply shift
        Char32 c = (Char32)(((*a)[i] - shift)&0xff);
        // insert escapes
        if (c == 0) { // forbidden
        out.AppendChar((Char32)1);
        out.AppendChar((Char32)1);
        } else if (c == 1) { // escape character
        out.AppendChar((Char32)1);
        out.AppendChar((Char32)2);
        } else if (c == '\'') { // forbidden
        out.AppendChar((Char32)1);
        out.AppendChar((Char32)3);
        } else {
        out.AppendChar(c);
        }
    }
    return out.ToString();
}

String StringEncoder::EncodeX(
    /* [in] */ ArrayOf<Byte> * a)
{
    // check input
    if (a == NULL || a->GetLength() ==0) {
        return String("X''");
    }

    Int32 outLen = a->GetLength() * 2 + 3;
    StringBuffer out(outLen);
    out.AppendChar('X');
    out.AppendChar('\'');
    for (Int32 i = 0; i < a->GetLength(); i++) {
        out.AppendChar((*xdigits)[((*a)[i] >> 4) & 0x0F]);
        out.AppendChar((*xdigits)[(*a)[i] & 0x0F]);
    }
    out.AppendChar('\'');
    return out.ToString();
}

AutoPtr<ArrayOf<Byte> > StringEncoder::Decode(
    /* [in] */ const String& s)
{
    AutoPtr<ArrayOf<Char32> > a = s.GetChars();
    if (a->GetLength() > 2 && (*a)[0] == 'X' &&
        (*a)[1] == '\'' && (*a)[a->GetLength()-1] == '\'') {
        Byte tmp;
        // SQLite3 BLOB syntax
        AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc((a->GetLength()-3)/2);
        for (Int32 i = 2, k = 0; i < a->GetLength() - 1; i += 2, k++) {
            switch ((*a)[i]) {
                case '0': tmp = 0; break;
                case '1': tmp = 1; break;
                case '2': tmp = 2; break;
                case '3': tmp = 3; break;
                case '4': tmp = 4; break;
                case '5': tmp = 5; break;
                case '6': tmp = 6; break;
                case '7': tmp = 7; break;
                case '8': tmp = 8; break;
                case '9': tmp = 9; break;
                case 'A':
                case 'a': tmp = 10; break;
                case 'B':
                case 'b': tmp = 11; break;
                case 'C':
                case 'c': tmp = 12; break;
                case 'D':
                case 'd': tmp = 13; break;
                case 'E':
                case 'e': tmp = 14; break;
                case 'F':
                case 'f': tmp = 15; break;
                default:  tmp = 0; break;
            }
            (*result)[k] = (byte) (tmp << 4);
            switch ((*a)[i+1]) {
                case '0': tmp = 0; break;
                case '1': tmp = 1; break;
                case '2': tmp = 2; break;
                case '3': tmp = 3; break;
                case '4': tmp = 4; break;
                case '5': tmp = 5; break;
                case '6': tmp = 6; break;
                case '7': tmp = 7; break;
                case '8': tmp = 8; break;
                case '9': tmp = 9; break;
                case 'A':
                case 'a': tmp = 10; break;
                case 'B':
                case 'b': tmp = 11; break;
                case 'C':
                case 'c': tmp = 12; break;
                case 'D':
                case 'd': tmp = 13; break;
                case 'E':
                case 'e': tmp = 14; break;
                case 'F':
                case 'f': tmp = 15; break;
                default:  tmp = 0; break;
            }
            (*result)[k] |= tmp;
        }
        return result;
    }
    // first element is the shift
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(a->GetLength()-1);
    Int32 i = 0;
    Int32 shift = (*a)[i++];
    Int32 j = 0;
    Int32 c;
    while (i < a->GetLength()) {
        if ((c = (*a)[i++]) == 1) { // escape character found
            if ((c = (*a)[i++]) == 1) {
                c = 0;
            }
            else if (c == 2) {
                c = 1;
            }
            else if (c == 3) {
                c = '\'';
            }
            else {
                break;
              /*  throw new IllegalArgumentException(
                "invalid string passed to decoder: " + j);*/
            }
        }
        // do shift
        (*result)[j++] = (byte)((c + shift) & 0xff);
    }
    Int32 outLen = j;
    // provide array of correct length
    if (result->GetLength() != outLen) {
        result = ByteCopy(result, 0, outLen, ArrayOf<Byte>::Alloc(outLen));
    }
    return result;
}

AutoPtr<ArrayOf<Byte> > StringEncoder::ByteCopy(
    /* [in] */ ArrayOf<Byte> * source,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Byte> * target)
{
    for (Int32 i = offset, j = 0; i < offset + count; i++, j++) {
        (*target)[j] = (*source)[i];
    }
    return target;
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
