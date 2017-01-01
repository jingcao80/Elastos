#ifndef __GUIDSEED_H__
#define __GUIDSEED_H__

#include <ctype.h>
#include <string.h>
#include <elatypes.h>
#include <clstype.h>
#include <stdint.h>

typedef unsigned char     BYTE;
typedef unsigned short int    WORD;
typedef unsigned int    DWORD;

const int MAX_SEED_SIZE = 255;

EXTERN_C int GuidFromSeedString(const char *, GUID *);
EXTERN_C int SeedStringFromGuid(REFGUID, char *);

typedef int (__stdcall *GuidEncodeFn_t)(void *, const char *, DWORD *);
typedef int (__stdcall *GuidDecodeFn_t)(void *, const DWORD *, char *);

#define DECL_GUID_ENCODER(En) \
    extern int __stdcall En##Encode(void *, const char *, DWORD *); \
    extern int __stdcall En##Decode(void *, const DWORD *, char *)

#define IMPL_GUID_ENCODE(Fn) int __stdcall Fn##Encode
#define IMPL_GUID_DECODE(Fn) int __stdcall Fn##Decode

struct GuidEncoder
{
    GuidEncodeFn_t  fnEncode;
    GuidDecodeFn_t  fnDecode;
    void *          pvArg;
};
typedef struct GuidEncoder GuidEncoder;

#define GUID_ENCODER_ENTRY(En, arg) { En##Encode, En##Decode, (void *)arg }

struct DictWord
{
    int             nSize;
    const char *    pszWord;
};
typedef struct DictWord DictWord;

#define DICT_WORD_ENTRY(s) { sizeof(#s) - 1, #s }
#define DICT_WORD_ENTRY_(s) { sizeof(s) - 1, s }
#define DICT_WORD_TERMINATOR { 0, NULL }

const int c_cMaxDictWords = 512 - 64 + 1;

inline DWORD EncodeCSymbol(int ch)
{
    if (isdigit(ch)) {
        return ch - '0';
    }
    else if (isupper(ch)) {
        return ch - 'A' + 10;
    }
    else if (islower(ch)) {
        return ch - 'a' + 36;
    }
    else if ('_' == ch) {
        return 62;
    }
    return 63;
}

inline DWORD DecodeCSymbol(int ch)
{
    if (ch < 10) {
        return ch + '0';
    }
    else if (ch < 36) {
        return ch + 'A' - 10;
    }
    else if (ch < 62) {
        return ch + 'a' - 36;
    }
    else if (62 == ch) {
        return '_';
    }
    return '/';
}

#endif // __GUIDSEED_H__
