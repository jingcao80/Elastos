
//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __ELSTRING_H__
#define __ELSTRING_H__

#include <elstringapi.h>
#include <elautoptr.h>
#include <elsharedbuf.h>
#include <elcstring.h>

#ifdef __cplusplus

_ELASTOS_NAMESPACE_BEGIN

template <class T> class ArrayOf;

//! This is a string holding UTF-8 characters. Does not allow the value more
// than 0x10FFFF, which is not valid unicode codepoint.
class String
{
public:
    String();
    String(const String& other);

    explicit String(const ArrayOf<Char32>& array, UInt32 offset = 0);
    explicit String(const ArrayOf<Char32>& array, UInt32 offset, UInt32 length);

    explicit String(const char* other);
    explicit String(const char* other, UInt32 numOfBytes);

    ~String();

    inline Int32 GetHashCode() const;

    inline Boolean IsNull() const;
    inline Boolean IsEmpty() const;
    inline Boolean IsNullOrEmpty() const;

    /**
     * @return the number of characters in this string.
     */
    UInt32 GetLength() const;

    /**
     * @return the number of bytes in this string.
     */
    inline UInt32 GetByteLength() const;

    /*
     * @returns the character at the specified offset in this string.
     */
    Char32 GetChar(UInt32 charIndex) const;

    /**
     * @param start
     *           the offset of the first character.
     * @return the characters from start to last character.
     */
    AutoPtr<ArrayOf<Char32> > GetChars(UInt32 start = 0) const;
    AutoPtr<ArrayOf<Char16> > GetChar16s(UInt32 start = 0) const;

    /**
     * @param start
     *           the offset of the first character.
     * @param end
     *           tthe offset one past the last character.
     * @return the characters from start to end - 1.
     */
    AutoPtr<ArrayOf<Char32> > GetChars(UInt32 start, UInt32 end) const;
    AutoPtr<ArrayOf<Char16> > GetChar16s(UInt32 start, UInt32 end) const;

    /**
     * @param start
     *           the offset of the first byte.
     * @return the bytes from start to last byte.
     */
    AutoPtr<ArrayOf<Byte> > GetBytes(UInt32 start = 0) const;

    /**
     * @param start
     *           the offset of the first byte.
     * @param end
     *           tthe offset one past the last byte.
     * @return the bytes from start to end - 1.
     */

    AutoPtr<ArrayOf<Byte> > GetBytes(UInt32 start, UInt32 end) const;

    /*
     * @returns the byte offset of a specified character offset in this string.
     */
    Int32 ToByteIndex(UInt32 charIndex) const;

    inline const char* string() const;
    inline operator const char*() const;
    inline operator CString() const;

    String Substring(UInt32 startChar) const;

    /**
     * @param start
     *           the offset of the first character.
     * @param end
     *           the offset one past the last character.
     * @return a new string containing the characters from start to end - 1.
     */
    String Substring(UInt32 startChar, UInt32 endChar) const;

    /**
     * Copies this string replacing occurrences of the specified character with
     * another character.
     *
     * @param oldChar
     *            the character to replace.
     * @param newChar
     *            the replacement character.
     * @return a new string with occurrences of oldChar replaced by newChar.
     */
    String Replace(Char32 oldChar, Char32 newChar) const;

    //---- Compare ----
    Int32 Compare(const char* other) const;
    Int32 CompareIgnoreCase(const char* other) const;
    inline Int32 Compare(const String& other) const;
    inline Int32 CompareIgnoreCase(const String& other) const;

    inline Boolean Equals(const String& other) const;
    inline Boolean Equals(const char* other) const;
    inline Boolean EqualsIgnoreCase(const String& other) const;
    inline Boolean EqualsIgnoreCase(const char* other) const;

    inline Boolean operator<(const String& other) const;
    inline Boolean operator<=(const String& other) const;
    inline Boolean operator==(const String& other) const;
    inline Boolean operator!=(const String& other) const;
    inline Boolean operator>=(const String& other) const;
    inline Boolean operator>(const String& other) const;

    inline Boolean operator<(const char* other) const;
    inline Boolean operator<=(const char* other) const;
    inline Boolean operator==(const char* other) const;
    inline Boolean operator!=(const char* other) const;
    inline Boolean operator>=(const char* other) const;
    inline Boolean operator>(const char* other) const;

    //---- Update ----
    inline String& operator=(const String& other);
    inline String& operator=(const char* other);

    inline String& operator+=(const String& other);
    inline String operator+(const String& other) const;

    inline String& operator+=(const char* other);
    inline String operator+(const char* other) const;

    inline String& operator+=(Char32 other);
    inline String operator+(Char32 other) const;

    ECode Append(const String& other);
    ECode Append(const String& other, UInt32 startChar);
    ECode Append(const String& other, UInt32 startChar, UInt32 numOfChars);
    ECode Append(const char* other);
    ECode Append(Char32 ch);
    ECode Append(const ArrayOf<Char32>& array, UInt32 offset = 0);
    ECode Append(const ArrayOf<Char32>& array, UInt32 offset, UInt32 length);

#ifdef _GNUC_
    ECode AppendFormat(const char* fmt, ...)
            __attribute__((format (printf, 2, 3)));
#else
    ECode AppendFormat(const char* fmt, ...);
#endif

    //---- Trim ----
    String Trim() const;
    String TrimStart() const;
    String TrimEnd() const;

    //---- Case ----
    String ToLowerCase() const;
    String ToLowerCase(UInt32 offset, UInt32 numChars) const;
    String ToUpperCase() const;
    String ToUpperCase(UInt32 offset, UInt32 numChars) const;

    //---- Contains ----
    Boolean Contains(const char* str) const;
    Boolean Contains(const String& str) const;
    Boolean StartWith(const char* str) const;
    Boolean StartWith(const String& str) const;
    Boolean EndWith(const char* str) const;
    Boolean EndWith(const String& str) const;

    Boolean ContainsIgnoreCase(const char* str) const;
    Boolean ContainsIgnoreCase(const String& str) const;
    Boolean StartWithIgnoreCase(const char* str) const;
    Boolean StartWithIgnoreCase(const String& str) const;
    Boolean EndWithIgnoreCase(const char* str) const;
    Boolean EndWithIgnoreCase(const String& str) const;

    //---- IndexOf ----
    /*
     * @returns the byte offset of a specified character in this string.
     */
    Int32 ByteIndexOf(Char32 ch) const;

    Int32 IndexOf(const char* str, UInt32 startChar = 0) const;
    Int32 IndexOf(Char32 ch, UInt32 startChar = 0) const;
    Int32 IndexOf(const String& str, UInt32 startChar = 0) const;

    Int32 IndexOfIgnoreCase(const char* str, UInt32 startChar = 0) const;
    Int32 IndexOfIgnoreCase(Char32 ch, UInt32 startChar = 0) const;
    Int32 IndexOfIgnoreCase(const String& str, UInt32 startChar = 0) const;

    Int32 LastIndexOf(Char32 ch) const;
    Int32 LastIndexOf(const char* str) const;
    Int32 LastIndexOf(const String& str) const;

    Int32 LastIndexOfIgnoreCase(Char32 ch) const;
    Int32 LastIndexOfIgnoreCase(const char* str) const;
    Int32 LastIndexOfIgnoreCase(const String& str) const;

    /**
     * Searches in this string for the index of the specified string. The search
     * for the string starts at the specified offset and moves towards the
     * beginning of this string.
     *
     * @param str
     *            the string to find.
     * @param start
     *            the starting offset.
     * @return the index of the first character of the specified string in this
     *         string , -1 if the specified string is not a substring.
     */
    Int32 LastIndexOf(Char32 ch, UInt32 start) const;
    Int32 LastIndexOf(const char* str, UInt32 start) const;
    Int32 LastIndexOf(const String& str, UInt32 start) const;

    Int32 LastIndexOfIgnoreCase(Char32 ch, UInt32 start) const;
    Int32 LastIndexOfIgnoreCase(const char* str, UInt32 start) const;
    Int32 LastIndexOfIgnoreCase(const String& str, UInt32 start) const;

    Boolean RegionMatches(UInt32 thisStartChar,
        const String& otherStr, UInt32 startChar, UInt32 numOfChars) const;
    Boolean RegionMatchesIgnoreCase(UInt32 thisStartChar,
        const String& otherStr, UInt32 startChar, UInt32 numOfChars) const;

public:
    // see UTF8.cpp
    // static inline Boolean IsASCII(char c) { return !(c & ~0x7F); }
    static inline Boolean IsASCII(char c) { return (c & 0x80) == 0; }

    static inline Int32 UTF8SequenceLengthNonASCII(char b0)
    {
        if ((b0 & 0xC0) != 0xC0)
            return 0;
        if ((b0 & 0xE0) == 0xC0)
            return 2;
        if ((b0 & 0xF0) == 0xE0)
            return 3;
        if ((b0 & 0xF8) == 0xF0)
            return 4;
        return 0;
    }

    static inline Int32 UTF8SequenceLength(char b0)
    {
        return IsASCII(b0) ? 1 : UTF8SequenceLengthNonASCII(b0);
    }

    static inline Boolean IsValidChar(Char32 c)
    {
        return !((c > MAX_CODE_POINT) || (MIN_HIGH_SURROGATE <= c && c <= MAX_LOW_SURROGATE));
    }

    static inline UInt32 GetByteCount(Char32 c)
    {
        if ((c > MAX_CODE_POINT) || (MIN_HIGH_SURROGATE <= c && c <= MAX_LOW_SURROGATE)) {
            return 0;
        }

        Int32 byteCount = 4;

        // Figure out how many bytes the result will require.
        if (c < 0x00000080) {
            byteCount = 1;
        }
        else if (c < 0x00000800) {
            byteCount = 2;
        }
        else if (c < 0x00010000) {
            byteCount = 3;
        }

        return byteCount;
    }

    static AutoPtr<ArrayOf<Char8> > ToByteArray(Char32 c);
    static Char32 ToLowerCase(Char32 codePoint);
    static Char32 ToUpperCase(Char32 codePoint);

private:
    void SetCounted(UInt32 charCount) const;
    void ClearCounted() const;
    Boolean IsCounted() const;

    char* LockBuffer(UInt32 numOfBytes);
    void UnlockBuffer();
    ECode UnlockBuffer(UInt32 numOfBytes);

    ECode SetTo(const String& other);
    ECode SetTo(const char* other);
    ECode SetTo(const char* other, UInt32 numOfBytes);

    ECode Append(const char* other, UInt32 numOfBytes);
    ECode RealAppend(const char* other, UInt32 numOfBytes);

    static Char32 GetCharInternal(const char* cur, UInt32* numOfBytes);
    static void WriteUTFBytesToBuffer(Byte* dstP, Char32 srcChar, Int32 bytes);

    static Boolean InsensitiveStartWith(const char* string, const char* subString);

    static Int32 SensitiveIndexOf(const char* string, const char* subString, UInt32 startChar);
    static Int32 InsensitiveIndexOf(const char* string, const char* subString, UInt32 startChar);

    static Int32 CharIndexToByteIndex(const char* string, UInt32 charIndex);

public:
    static const Char32 INVALID_CHAR;

private:
    static const UInt32 MIN_CODE_POINT;
    static const UInt32 MAX_CODE_POINT;
    static const UInt32 MIN_HIGH_SURROGATE;
    static const UInt32 MAX_HIGH_SURROGATE;
    static const UInt32 MIN_LOW_SURROGATE;
    static const UInt32 MAX_LOW_SURROGATE;

private:
    /**
     * @brief The character pointer used to contain a string value.
     */
    const char* mString;

    /**
     * @brief The number of characters.
     */
    mutable UInt32 mCharCount;   // lazy-count, the highest bit is used as a counted-flag.
};

//=============================================================================
//              Getter
//=============================================================================

inline Int32 String::GetHashCode() const
{
    return _String_GetHashCode(mString);
}

inline Boolean String::IsNull() const
{
    return mString == NULL;
}

inline Boolean String::IsEmpty() const
{
    assert(mString);
    return mString[0] == '\0';
}

inline Boolean String::IsNullOrEmpty() const
{
    return (mString == NULL || mString[0] == '\0');
}

inline UInt32 String::GetByteLength() const
{
    if (IsNullOrEmpty()) return 0;
    return (Int32)SharedBuffer::GetSizeFromData(mString) - 1;
}

inline const char* String::string() const
{
    return mString;
}

inline String::operator const char*() const
{
    return mString;
}

inline String::operator CString() const
{
    return mString;
}

//=============================================================================
//              Compare
//=============================================================================

inline Int32 String::Compare(const String& other) const
{
    return Compare(other.mString);
}

inline Int32 String::CompareIgnoreCase(const String& other) const
{
    return CompareIgnoreCase(other.mString);
}

inline Boolean String::Equals(const String& str) const
{
    return Compare(str) == 0;
}

inline Boolean String::Equals(const char* str) const
{
    return Compare(str) == 0;
}

inline Boolean String::EqualsIgnoreCase(const String& str) const
{
    return CompareIgnoreCase(str) == 0;
}

inline Boolean String::EqualsIgnoreCase(const char* str) const
{
    return CompareIgnoreCase(str) == 0;
}

inline Boolean String::operator<(const String& other) const
{
    return Compare(other) < 0;
}

inline Boolean String::operator<=(const String& other) const
{
    return Compare(other) <= 0;
}

inline Boolean String::operator==(const String& other) const
{
    return Compare(other) == 0;
}

inline Boolean String::operator!=(const String& other) const
{
    return Compare(other) != 0;
}

inline Boolean String::operator>=(const String& other) const
{
    return Compare(other) >= 0;
}

inline Boolean String::operator>(const String& other) const
{
    return Compare(other) > 0;
}

inline Boolean String::operator<(const char* other) const
{
    return Compare(other) < 0;
}

inline Boolean String::operator<=(const char* other) const
{
    return Compare(other) <= 0;
}

inline Boolean String::operator==(const char* other) const
{
    return Compare(other) == 0;
}

inline Boolean String::operator!=(const char* other) const
{
    return Compare(other) != 0;
}

inline Boolean String::operator>=(const char* other) const
{
    return Compare(other) >= 0;
}

inline Boolean String::operator>(const char* other) const
{
    return Compare(other) > 0;
}

//=============================================================================
//              Update
//=============================================================================

inline String& String::operator=(const String& other)
{
    SetTo(other);
    return *this;
}

inline String& String::operator=(const char* other)
{
    SetTo(other);
    return *this;
}

inline String& String::operator+=(const String& other)
{
    Append(other);
    return *this;
}

inline String String::operator+(const String& other) const
{
    String tmp(*this);
    tmp += other;
    return tmp;
}

inline String& String::operator+=(const char* other)
{
    Append(other);
    return *this;
}

inline String String::operator+(const char* other) const
{
    String tmp(*this);
    tmp += other;
    return tmp;
}

inline String& String::operator+=(Char32 other)
{
    Append(other);
    return *this;
}

inline String String::operator+(Char32 other) const
{
    String tmp(*this);
    tmp += other;
    return tmp;
}

_ELASTOS_NAMESPACE_END


#else //!__cplusplus

typedef const char* String;

#endif //__cplusplus

#endif // __ELSTRING_H__
