//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <sys/types.h>
#include <eltypes.h>
#include <_pubcrt.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void __cdecl __cfltcvt(double value, char *buffer, size_t buffersize, char fmt,
	int precision);
void __cdecl __cropzeros(char *buffer);
void __cdecl __forcdecpt(char *buffer);

_ELASTOS Boolean __cdecl _IsInfinity(double value, char *buf)
{
    //+ 7ff00000 00000000 ����
    //�C fff00000 00000000 ������
    //����7ff80000 00000000 NaN
    /* Infinity or NaN */
    fd_twoints u;
    u.d = value;
    if ((__HI(u) & EXP_MASK) == EXP_MASK) {
        if (!__LO(u) && !(__HI(u) & 0xFFFFF)) {
            if (value < 0) {
                strcpy(buf, "-Infinity");
            }
            else {
                strcpy(buf, "+Infinity");
            }
        }
        else if ((__HI(u) & 0xFFFFF) == 0x80000) {
            strcpy(buf, "NaN");
        }
        else {
            strcpy(buf, "Indefinite");
        }
        return TRUE;
    }

    return FALSE;
}

#ifdef __cplusplus
}
#endif

#include "convertstr.h"
#include <malloc.h>
#include <assert.h>
#include <stdio.h>

_ELASTOS_NAMESPACE_USING

EXTERN_C Boolean _IsInfinity(double value, char *buf);

static CAR_INLINE char *_ecvtbuf(double value, int ndigits, size_t buffersize,
    int *decpt, int *sign, char *buf)
{
#ifdef _MSVC
	return ecvtbuf(value, ndigits, decpt, sign, buf);
#else
    sprintf(buf, "%f", value);
    return buf;
#endif
}

static CAR_INLINE char *_fcvtbuf(double value, int ndigits, size_t buffersize,
    int *decpt, int *sign, char *buf)
{
#ifdef _MSVC
	return fcvtbuf(value, ndigits, decpt, sign, buf);
#else
    sprintf(buf, "%f", value);
    return buf;
#endif
}

#define FL_NEGATIVE   0x00001   /* value is negative */

typedef struct FormatInfo{
    Int32 width;
    UInt32 flag;
    Int32 precision;
    UInt32 type;
}FormatInfo, *PFormatInfo;

static CAR_INLINE void GetFormatInfo(Int32 fmt, PFormatInfo pFmtinfo)
{
    pFmtinfo->width = fmt & 0x000000FF;
    pFmtinfo->flag = fmt & NumberFormat_PrefixMask;
    pFmtinfo->precision = (fmt & 0x0000FF00) >> 8;
    pFmtinfo->type = fmt & NumberFormat_TypeMask;
}

#ifdef _MSC_VER
#pragma optimize( "", off )
#endif

Int32 ConvertNumber(PNumberInfo pNumberInfo, Int32 bits)
{
    FormatInfo fmtinfo;
    Int32 digit;              /* ascii value of digit */
    UInt64 uValue;
    int  radix;      /* current conversion radix */
    char textbuf[TEXT_BUF_SIZE];
    char buf[TEXT_BUF_SIZE];
    char *ptext, *p = buf;

    int hexadd = 0;     /* offset to add to number to get 'a'..'f' */
    int prefixlen = 0;  /* length of prefix -- 0 means no prefix */
    char prefix[2];    /* numeric prefix -- up to two characters*/
    int textlen = 0;    /* length of the text to be printed.*/

    int i = 0;

    Boolean bSigned = TRUE;  /* signed data given */

    GetFormatInfo(pNumberInfo->fmt, &fmtinfo);

    if (pNumberInfo->type & NumberFormat_IntegerMask) {
        switch (fmtinfo.type & NumberFormat_IntegerMask) {
            case NumberFormat_Decimal:
                radix = 10;
                break;
            case NumberFormat_UsignedDecimal:
                radix = 10;
                bSigned = FALSE;
                break;
            case NumberFormat_BigHex:
                /* unsigned upper hex output */
                hexadd = 'A' - '9' - 1;     /* set hexadd for uppercase hex */
                goto COMMON_HEX;
            case NumberFormat_Hex:
                /* unsigned lower hex output */
                hexadd = 'a' - '9' - 1;     /* set hexadd for lowercase hex */
                /* DROP THROUGH TO COMMON_HEX */
            COMMON_HEX:
                radix = 16;
                if (fmtinfo.flag & NumberFormat_PoundSign) {
                    /* alternate form means '0x' prefix */
                    prefix[0] = '0';
                    prefix[1] = (char)('x' - 'a' + '9' + 1 + hexadd);
                    /* 'x' or 'X' */
                    prefixlen = 2;
                }
                break;
            case NumberFormat_Octal:
                /* unsigned octal output */
                radix = 8;
                break;
            default:
                return 0;
        }

        /* 2. check for negative; copy into number */
        if ((fmtinfo.type & NumberFormat_IntegerMask) == NumberFormat_Decimal
            && pNumberInfo->value.n < 0) {
            uValue = -pNumberInfo->value.n;
            fmtinfo.flag |= FL_NEGATIVE;   /* remember negative sign */
        }
        else {
            uValue = pNumberInfo->value.n;
        }

        switch (bits) {
            case 8:
                uValue &= 0xff;
                break;
            case 16:
                uValue &= 0xffff;
                break;
            case 32:
                uValue &= 0xffffffff;
                break;
        }

        /* 3. check precision value for default; non-default */
        /*    turns off 0 flag, according to ANSI. */
        if (fmtinfo.precision <= 0)
            fmtinfo.precision = 1; /* default precision */
        else {
            fmtinfo.flag &= ~NumberFormat_ZeroPrefixed;
            if (fmtinfo.precision > TEXT_BUF_SIZE)
                fmtinfo.precision = TEXT_BUF_SIZE;
        }

        /* 4. Check if data is 0; if so, turn off hex prefix */
        if (uValue == 0)
            prefixlen = 0;

        /* 5. Convert data to ASCII -- note if precision is zero */
        /*    and number is zero, we get no digits at all.       */

        ptext = &textbuf[TEXT_BUF_SIZE - 1];  /* last digit at end of buffer */

        while (fmtinfo.precision--> 0 || uValue != 0) {
            digit = (int)(uValue % radix) + '0';
            uValue /= radix;                /* reduce number */
            if (digit > '9') {
                /* a hex digit, make it a letter */
                digit += hexadd;
            }
            *ptext-- = (char)digit;       /* store the digit */
        }

        textlen = (int)(&textbuf[TEXT_BUF_SIZE - 1] - ptext);
        /* compute length of number */
        ++ptext;          /* text points to first digit now */

        /* 6. Force a leading zero if FORCEOCTAL flag set */
        if ((fmtinfo.type & NumberFormat_IntegerMask) == NumberFormat_Octal
            && (textlen == 0 || ptext[0] != '0')) {
            *--ptext = '0';
            ++textlen;      /* add a zero */
        }
    }
    else if (pNumberInfo->type & NumberFormat_DoubleMask) {
        char chfmt = 'f';
        switch (fmtinfo.type & NumberFormat_DoubleMask) {
            case NumberFormat_BigScientificDouble:
                chfmt = 'E';
                break;
            case NumberFormat_BigFlexScientificDouble:
                chfmt = 'G';
                break;
            case NumberFormat_Double:
                chfmt = 'f';
                break;
            case NumberFormat_ScientificDouble:
                chfmt = 'e';
                break;
            case NumberFormat_FlexScientificDouble:
                chfmt = 'g';
                break;
            break;
            default:
                return 0;
        }

        /* floating point conversion -- we call cfltcvt routines */
        /* to do the work for us.                                */
//        fmtinfo.flag |= NumberFormat_Signed;
        /* floating point is signed conversion */
        ptext = textbuf;        /* put result in buffer */

        /* compute the precision value */
        if (fmtinfo.precision <= 0)
            fmtinfo.precision = 6; /* default precision: 6 */
//        else if (fmtinfo.precision == 0
//            && (fmtinfo.type & NumberFormat_BigFlexScientificDouble
//            || fmtinfo.type & NumberFormat_FlexScientificDouble))
//            fmtinfo.precision = 1;          /* ANSI specified */
        else if (fmtinfo.precision > TEXT_BUF_SIZE)
            fmtinfo.precision = TEXT_BUF_SIZE;

    //    if (precision > BUFFERSIZE - CVTBUFSIZE) {
    //        /* conversion will potentially overflow local buffer */
    //        /* so we need to use a heap-allocated buffer.        */
    //        heapbuf = (char *)malloc(CVTBUFSIZE + precision);
    //        if (heapbuf != NULL)
    //        {
    //            text.sz = heapbuf;
    //            buffersize = CVTBUFSIZE + precision;
    //        }
    //        else
    //            /* malloc failed, cap precision further */
    //            precision = BUFFERSIZE - CVTBUFSIZE;
    //    }

    //    /* for safecrt, we pass along the FL_ALTERNATE flag
    //       to _safecrt_cfltcvt */
    //    if (fmtinfo.flag & NumberFormat_PoundSign) {
    //        capexp |= NumberFormat_PoundSign;
    //    }

        __cfltcvt(pNumberInfo->value.d, ptext, TEXT_BUF_SIZE, chfmt,
            fmtinfo.precision);

        /* '#' and precision == 0 means force a decimal point */
        if ((fmtinfo.flag & NumberFormat_PoundSign)
                && fmtinfo.precision == 0) {
            __forcdecpt(ptext);
        }

        /* 'g' format means crop zero unless '#' given */
        if ((fmtinfo.type & (NumberFormat_BigFlexScientificDouble
            | NumberFormat_FlexScientificDouble))
            && !(fmtinfo.flag & NumberFormat_PoundSign)) {
            __cropzeros(ptext);
        }

        /* check if result was negative, save '-' for later */
        /* and point to positive part (this is for '0' padding) */
        if (*ptext == '-') {
            fmtinfo.flag |= FL_NEGATIVE;
            ;
            ++ptext;
        }
        textlen = (int)strlen(ptext);     /* compute length of text */
    }
    else {
        return -1;
    }

    int padding;    /* amount of padding, negative means zero */

    if (bSigned) {
        if (fmtinfo.flag & FL_NEGATIVE) {
            /* prefix is a '-' */
            prefix[0] = '-';
            prefixlen = 1;
        }
        else if (fmtinfo.flag & NumberFormat_Signed) {
            /* prefix is '+' */
            prefix[0] = '+';
            prefixlen = 1;
        }
        else if (fmtinfo.flag & NumberFormat_BlankPrefixed) {
            /* prefix is ' ' */
            prefix[0] = ' ';
            prefixlen = 1;
        }
    }

    /* calculate amount of padding -- might be negative, */
    /* but this will just mean zero */
    padding = fmtinfo.width - textlen - prefixlen;

    /* put out the padding, prefix, and text, in the correct order */
    if (padding > 0 && !(fmtinfo.flag
        & (NumberFormat_LeftAlign | NumberFormat_ZeroPrefixed))) {
        /* pad on left with blanks */
        memset(p, ' ', padding);
        p += padding;
    }

    /* write prefix */
    for (i = 0; i < prefixlen; i++) {
        *p = prefix[i];
        p++;
    }

    if (padding > 0 && (fmtinfo.flag & NumberFormat_ZeroPrefixed)
        && !(fmtinfo.flag & NumberFormat_LeftAlign)) {
        /* write leading zeros */
        memset(p, '0', padding);
        p += padding;
    }

    /* write text */
    memcpy(p, ptext, textlen);
    p += textlen;

    if (padding > 0 && (fmtinfo.flag & NumberFormat_LeftAlign)) {
        /* pad on right with blanks */
        memset(p, ' ', padding);
        p += padding;
    }
    *p = '\0';

    int len = p - buf;
    int bytelen = 0;

    if ((pNumberInfo->pCq->m_flags & CarQuintetFlag_TypeMask)
            == CarQuintetFlag_Type_Char16) {
        bytelen = 2 * len;
    }
    else {
        bytelen = len;
    }

    if (pNumberInfo->pCq->m_used + bytelen > pNumberInfo->pCq->m_size) {
        return -1;
    }

    char *pBuf = (char *)pNumberInfo->pCq->m_pBuf + pNumberInfo->pCq->m_used;
    if ((pNumberInfo->pCq->m_flags & CarQuintetFlag_TypeMask)
            == CarQuintetFlag_Type_Char16) {
        pBuf -= sizeof(Char16);
        for (i = 0; i <= len; i++) {
            *(Char16 *)&pBuf[2 * i] = (Char16)buf[i];
        }
    }
    else {
        pBuf -=sizeof(Char8);
        for (i = 0; i <= len; i++) {
            pBuf[i] = buf[i];
        }
    }

    pNumberInfo->pCq->m_used += bytelen;

    return len;
}

#ifdef _MSC_VER
#pragma optimize( "", on )
#endif

extern "C" void __cdecl __cfltcvt(double value, char *buffer, size_t buffersize,
    char fmt, int precision)
{
    int decpt = 0, sign = 0, exp = 0, pos = 0;
    char *digits = NULL;
    int capexp = 0;
    int magnitude = 0;
    char *cvtbuf = NULL;

    assert(buffer && (buffersize >= 18));

    if (precision < 0) precision = 0;
    if (precision >= (int)(buffersize - 1)) precision = (int)(buffersize - 2);

    cvtbuf = (char *)alloca(buffersize);
    if(cvtbuf == NULL) {
        *buffer = '\0';
        return;
    }

    if (_IsInfinity(value, buffer)) {
        return;
    }

    if (fmt == 'G' || fmt == 'E') {
        capexp = 1;
        fmt += 'a' - 'A';
    }

    if (fmt == 'g') {
        digits = _ecvtbuf(value, precision, buffersize, &decpt, &sign, cvtbuf);
        magnitude = decpt - 1;
        if (magnitude < -4 || magnitude > precision - 1) {
            fmt = 'e';
            precision -= 1;
        }
        else {
            fmt = 'f';
            precision -= decpt;
        }
    }

    if (fmt == 'e') {
        digits = _ecvtbuf(value, precision + 1, buffersize, &decpt, &sign, cvtbuf);
        if (sign) *buffer++= '-';
        *buffer++ = *digits;
        if (precision > 0) *buffer++= '.';
        memcpy(buffer, digits + 1, precision);
        buffer += precision;
        *buffer++ = capexp ? 'E' : 'e';

        if (decpt == 0) {
            if (value == 0.0)
                exp = 0;
            else
                exp = -1;
        }
        else
            exp = decpt - 1;

        if (exp < 0) {
            *buffer++ = '-';
            exp = -exp;
        }
        else
            *buffer++ = '+';

        buffer[2] = (exp % 10) + '0';
        exp = exp / 10;
        buffer[1] = (exp % 10) + '0';
        exp = exp / 10;
        buffer[0] = (exp % 10) + '0';
        buffer += 3;
    }
    else if (fmt == 'f') {
        digits = _fcvtbuf(value, precision, buffersize, &decpt, &sign, cvtbuf);
#ifdef _MSVC
        if (sign) *buffer++= '-';
        if (*digits) {
            if (decpt <= 0) {
                *buffer++ = '0';
                *buffer++ = '.';
                for (pos = 0; pos < -decpt; pos++) *buffer++= '0';
                while (*digits) *buffer++= *digits++;
            }
            else {
                pos = 0;
                while (*digits) {
                    if (pos++== decpt) *buffer++= '.';
                    *buffer++ = *digits++;
                }
            }
        }
        else {
            *buffer++ = '0';
            if (precision > 0) {
                *buffer++ = '.';
                for (pos = 0; pos < precision; pos++) *buffer++= '0';
            }
        }
#else
        while (*digits) *buffer++= *digits++;
#endif
    }
    *buffer = '\0';
}

extern "C" void __cdecl __cropzeros(char *buffer)
{
    char *stop;

    while (*buffer && *buffer != '.') buffer++;
    if (*buffer++) {
        while (*buffer && *buffer != 'e' && *buffer != 'E') buffer++;
        stop = buffer--;
        while (*buffer == '0') buffer--;
        if (*buffer == '.') buffer--;
        while ((*++buffer = *stop++) != '\0');
    }
}

extern "C" void __cdecl __forcdecpt(char *buffer)
{
    while (*buffer) {
        if (*buffer == '.') return;
        if (*buffer == 'e' || *buffer == 'E') break;
        buffer++;
    }

    if (*buffer) {
        int n = strlen(buffer) + 1;
        while (n > 0) {
            buffer[n] = buffer[n - 1];
            n--;
        }

        *buffer = '.';
    }
    else {
        *buffer++ = '.';
        *buffer = '\0';
    }
}
