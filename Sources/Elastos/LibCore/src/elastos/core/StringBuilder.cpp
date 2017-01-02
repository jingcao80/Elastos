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

#include "StringBuilder.h"
#include "StringBuffer.h"
#include "CString.h"


namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL(StringBuilder, AbstractStringBuilder, IStringBuilder)

StringBuilder::StringBuilder()
    :AbstractStringBuilder()
{}

StringBuilder::StringBuilder(
    /* [in] */ Int32 capacity)
    :AbstractStringBuilder(capacity)
{
}

StringBuilder::StringBuilder(
    /* [in] */ const String& str)
    :AbstractStringBuilder(str)
{
}

StringBuilder::StringBuilder(
    /* [in] */ const char* str)
    :AbstractStringBuilder(String(str))
{
}

StringBuilder::~StringBuilder()
{}

ECode StringBuilder::ToASCIILowerCase()
{
    return AbstractStringBuilder::ToASCIICaseString(FALSE);
}

ECode StringBuilder::ToASCIIUpperCase()
{
    return AbstractStringBuilder::ToASCIICaseString(TRUE);
}

ECode StringBuilder::Reset()
{
    return SetLength(0);
}

ECode StringBuilder::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);
    *length = AbstractStringBuilder::GetLength();
    return NOERROR;
}

ECode StringBuilder::GetByteCount(
    /* [out] */ Int32* byteLength)
{
    VALIDATE_NOT_NULL(byteLength);
    *byteLength = AbstractStringBuilder::GetByteCount();
    return NOERROR;
}

ECode StringBuilder::GetCapacity(
    /* [out] */ Int32* capacity)
{
    VALIDATE_NOT_NULL(capacity);
    *capacity = AbstractStringBuilder::GetCapacity();
    return NOERROR;
}

ECode StringBuilder::EnsureCapacity(
    /* [in] */ Int32 min)
{
    return AbstractStringBuilder::EnsureCapacity(min);
}

ECode StringBuilder::TrimToSize()
{
    return AbstractStringBuilder::TrimToSize();
}

ECode StringBuilder::SetCharAt(
    /* [in] */ Int32 index,
    /* [in] */ Char32 ch)
{
    return AbstractStringBuilder::SetCharAt(index, ch);
}

Char32 StringBuilder::GetCharAt(
    /* [in] */ Int32 index)
{
    Char32 ch;
    AbstractStringBuilder::GetCharAt(index, &ch);
    return ch;
}

ECode StringBuilder::GetCharAt(
    /* [in] */ Int32 index,
    /* [out] */ Char32* c)
{
    return AbstractStringBuilder::GetCharAt(index, c);
}

ECode StringBuilder::GetChars(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in, out] */ ArrayOf<Char32>* dst,
    /* [in] */ Int32 dstStart)
{
    return AbstractStringBuilder::GetChars(start, end, dst, dstStart);
}

ECode StringBuilder::SetLength(
    /* [in] */ Int32 length)
{
    return AbstractStringBuilder::SetLength(length);
}

ECode StringBuilder::Substring(
    /* [in] */ Int32 start,
    /* [out] */ String* str)
{
    return AbstractStringBuilder::Substring(start, str);
}

ECode StringBuilder::Substring(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ String* str)
{
    return AbstractStringBuilder::Substring(start, end, str);
}

String StringBuilder::Substring(
    /* [in] */ Int32 start)
{
    String str;
    AbstractStringBuilder::Substring(start, &str);
    return str;
}

String StringBuilder::Substring(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    String str;
    AbstractStringBuilder::Substring(start, end, &str);
    return str;
}

ECode StringBuilder::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** seq)
{
    return AbstractStringBuilder::SubSequence(start, end, seq);
}

ECode StringBuilder::IndexOf(
    /* [in] */ const String& string,
    /* [out] */ Int32* index)
{
    return AbstractStringBuilder::IndexOf(string, index);
}

ECode StringBuilder::IndexOf(
    /* [in] */ const String& subString,
    /* [in] */ Int32 start,
    /* [out] */ Int32* index)
{
    return AbstractStringBuilder::IndexOf(subString, start, index);
}

ECode StringBuilder::LastIndexOf(
    /* [in] */ const String& string,
    /* [out] */ Int32* index)
{
    return AbstractStringBuilder::LastIndexOf(string, index);
}

ECode StringBuilder::LastIndexOf(
    /* [in] */ const String& subString,
    /* [in] */ Int32 start,
    /* [out] */ Int32* index)
{
    return AbstractStringBuilder::LastIndexOf(subString, start, index);
}

ECode StringBuilder::AppendNULL()
{
    return AbstractStringBuilder::AppendNULL();
}

ECode StringBuilder::Append(
    /* [in] */ const char* str)
{
    return AbstractStringBuilder::Append(str);
}

ECode StringBuilder::Append(
    /* [in] */ Boolean b)
{
    return AbstractStringBuilder::Append(b);
}

ECode StringBuilder::AppendChar(
    /* [in] */ Char32 c)
{
    return AbstractStringBuilder::AppendChar(c);
}

ECode StringBuilder::Append(
    /* [in] */ Int32 i)
{
    return AbstractStringBuilder::Append(i);
}

ECode StringBuilder::Append(
    /* [in] */ Int64 l)
{
    return AbstractStringBuilder::Append(l);
}

ECode StringBuilder::Append(
    /* [in] */ Float f)
{
    return AbstractStringBuilder::Append(f);
}

ECode StringBuilder::Append(
    /* [in] */ Double d)
{
    return AbstractStringBuilder::Append(d);
}

ECode StringBuilder::Append(
    /* [in] */ IInterface* obj)
{
    return AbstractStringBuilder::Append(obj);
}

ECode StringBuilder::Append(
    /* [in] */ IObject* obj)
{
    return AbstractStringBuilder::Append(obj);
}

ECode StringBuilder::Append(
    /* [in] */ const String& str)
{
    return AbstractStringBuilder::Append(str);
}

ECode StringBuilder::Append(
    /* [in] */ IStringBuffer* sb)
{
    if (NULL == sb) {
        return AbstractStringBuilder::AppendNULL();
    }

    String str;
    ((StringBuffer*)sb)->ToString(&str);
    return AbstractStringBuilder::Append(str);
}

ECode StringBuilder::Append(
    /* [in] */ IStringBuilder* sb)
{
    if (NULL == sb) {
        return AbstractStringBuilder::AppendNULL();
    }

    String str;
    ((StringBuilder*)sb)->ToString(&str);
    return AbstractStringBuilder::Append(str);
}

ECode StringBuilder::Append(
    /* [in] */ const ArrayOf<Char32>& chars)
{
    return AbstractStringBuilder::Append(chars);
}

ECode StringBuilder::Append(
    /* [in] */ const ArrayOf<Char32>& chars,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    return AbstractStringBuilder::Append(chars, offset, length);
}

ECode StringBuilder::Append(
    /* [in] */ ICharSequence* csq)
{
    return AbstractStringBuilder::Append(csq);
}

ECode StringBuilder::Append(
    /* [in] */ ICharSequence* csq,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return AbstractStringBuilder::Append(csq, start, end);
}

ECode StringBuilder::InsertChar(
    /* [in] */ Int32 offset,
    /* [in] */ Char32 c)
{
    return AbstractStringBuilder::InsertChar(offset, c);
}

ECode StringBuilder::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean b)
{
    return AbstractStringBuilder::Insert(offset, b);
}

ECode StringBuilder::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 i)
{
    return AbstractStringBuilder::Insert(offset, i);
}

ECode StringBuilder::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ Int64 l)
{
    return AbstractStringBuilder::Insert(offset, l);
}

ECode StringBuilder::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ Float f)
{
    return AbstractStringBuilder::Insert(offset, f);
}

ECode StringBuilder::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ Double d)
{
    return AbstractStringBuilder::Insert(offset, d);
}

ECode StringBuilder::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ IInterface* obj)
{
    return AbstractStringBuilder::Insert(offset, obj);
}

ECode StringBuilder::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ const String& str)
{
    return AbstractStringBuilder::Insert(offset, str);
}

ECode StringBuilder::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ const ArrayOf<Char32>& chars)
{
    return AbstractStringBuilder::Insert(offset, chars);
}

ECode StringBuilder::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ const ArrayOf<Char32>& str,
    /* [in] */ Int32 strOffset,
    /* [in] */ Int32 strLen)
{
    return AbstractStringBuilder::Insert(offset, str, strOffset, strLen);
}

ECode StringBuilder::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ ICharSequence* s)
{
    return AbstractStringBuilder::Insert(offset, s);
}

ECode StringBuilder::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return AbstractStringBuilder::Insert(offset, s, start, end);
}

ECode StringBuilder::Replace(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const String& string)
{
    return AbstractStringBuilder::Replace(start, end, string);
}

ECode StringBuilder::Reverse()
{
    return AbstractStringBuilder::Reverse();
}

ECode StringBuilder::DeleteCharAt(
    /* [in] */ Int32 location)
{
    return AbstractStringBuilder::DeleteCharAt(location);
}

ECode StringBuilder::Delete(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return AbstractStringBuilder::Delete(start, end);
}

ECode StringBuilder::ToString(
    /* [out] */ String* str)
{
    return AbstractStringBuilder::Substring(0, str);
}

String StringBuilder::ToString()
{
   String str;
   ToString(&str);
   return str;
}

AutoPtr<ICharSequence> StringBuilder::ToCharSequence()
{
    String str;
    AbstractStringBuilder::Substring(0, &str);
    AutoPtr<ICharSequence> seq;
    CString::New(str, (ICharSequence**)&seq);
    return seq;
}

StringBuilder& StringBuilder::operator+=(const Boolean right)
{
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuilder& StringBuilder::operator+=(const char right)
{
    AbstractStringBuilder::AppendChar(right);
    return *this;
}

StringBuilder& StringBuilder::operator+=(const Char32 right)
{
    AbstractStringBuilder::AppendChar(right);
    return *this;
}

StringBuilder& StringBuilder::operator+=(const Int32 right)
{
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuilder& StringBuilder::operator+=(const Int64 right)
{
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuilder& StringBuilder::operator+=(const Float right)
{
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuilder& StringBuilder::operator+=(const Double right)
{
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuilder& StringBuilder::operator+=(const char* right)
{
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuilder& StringBuilder::operator+=(const String& right)
{
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuilder& StringBuilder::operator+=(IInterface* right)
{
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuilder& StringBuilder::operator+=(IObject* right)
{
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuilder& StringBuilder::operator+=(StringBuffer& right)
{
    String str;
    right.ToString(&str);
    AbstractStringBuilder::Append(str);
    return *this;
}

StringBuilder& StringBuilder::operator+=(StringBuilder& right)
{
    String str;
    right.ToString(&str);
    AbstractStringBuilder::Append(str);
    return *this;
}

} // namespace Core
} // namespace Elastos
