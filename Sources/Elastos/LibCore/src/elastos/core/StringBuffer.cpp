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

#include "StringBuffer.h"
#include "StringBuilder.h"
#include "Character.h"
#include "CString.h"
#include <stdio.h>
#include "AutoLock.h"

#define DEFAULT_STEP 16

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL(StringBuffer, AbstractStringBuilder, IStringBuffer)

StringBuffer::StringBuffer()
    :AbstractStringBuilder()
{}

StringBuffer::StringBuffer(
    /* [in] */ Int32 capacity)
    :AbstractStringBuilder(capacity)
{
}

StringBuffer::StringBuffer(
    /* [in] */ const String& str)
    :AbstractStringBuilder(str)
{
}

StringBuffer::StringBuffer(
    /* [in] */ const char* str)
    :AbstractStringBuilder(String(str))
{
}

StringBuffer::~StringBuffer()
{}

ECode StringBuffer::ToASCIILowerCase()
{
    AutoLock lock(this);
    return AbstractStringBuilder::ToASCIICaseString(FALSE);
}

ECode StringBuffer::ToASCIIUpperCase()
{
    AutoLock lock(this);
    return AbstractStringBuilder::ToASCIICaseString(TRUE);
}

ECode StringBuffer::Reset()
{
    AutoLock lock(this);
    return AbstractStringBuilder::SetLength(0);
}

ECode StringBuffer::GetLength(
    /* [out] */ Int32* length)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(length);
    *length = GetLength();
    return NOERROR;
}

ECode StringBuffer::GetByteCount(
    /* [out] */ Int32* byteLength)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(byteLength);
    *byteLength = GetByteCount();
    return NOERROR;
}

ECode StringBuffer::GetCapacity(
    /* [out] */ Int32* capacity)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(capacity);
    *capacity = GetCapacity();
    return NOERROR;
}

ECode StringBuffer::EnsureCapacity(
    /* [in] */ Int32 min)
{
    AutoLock lock(this);
    return AbstractStringBuilder::EnsureCapacity(min);
}

ECode StringBuffer::TrimToSize()
{
    AutoLock lock(this);
    return AbstractStringBuilder::TrimToSize();
}

ECode StringBuffer::SetCharAt(
    /* [in] */ Int32 index,
    /* [in] */ Char32 ch)
{
    AutoLock lock(this);
    return AbstractStringBuilder::SetCharAt(index, ch);
}

Char32 StringBuffer::GetCharAt(
    /* [in] */ Int32 index)
{
    AutoLock lock(this);
    Char32 ch;
    AbstractStringBuilder::GetCharAt(index, &ch);
    return ch;
}

ECode StringBuffer::GetCharAt(
    /* [in] */ Int32 index,
    /* [out] */ Char32* c)
{
    AutoLock lock(this);
    return AbstractStringBuilder::GetCharAt(index, c);
}

ECode StringBuffer::GetChars(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ArrayOf<Char32>* dst,
    /* [in] */ Int32 dstStart)
{
    AutoLock lock(this);
    return AbstractStringBuilder::GetChars(start, end, dst, dstStart);
}

ECode StringBuffer::Substring(
    /* [in] */ Int32 start,
    /* [out] */ String* str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Substring(start, str);
}

ECode StringBuffer::Substring(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ String* str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Substring(start, end, str);
}

String StringBuffer::Substring(
    /* [in] */ Int32 start)
{
    AutoLock lock(this);
    String str;
    AbstractStringBuilder::Substring(start, &str);
    return str;
}

String StringBuffer::Substring(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoLock lock(this);
    String str;
    AbstractStringBuilder::Substring(start, end, &str);
    return str;
}

ECode StringBuffer::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** seq)
{
    AutoLock lock(this);
    return AbstractStringBuilder::SubSequence(start, end, seq);
}

ECode StringBuffer::IndexOf(
    /* [in] */ const String& string,
    /* [out] */ Int32* index)
{
    AutoLock lock(this);
    return AbstractStringBuilder::IndexOf(string, index);
}

ECode StringBuffer::IndexOf(
    /* [in] */ const String& subString,
    /* [in] */ Int32 start,
    /* [out] */ Int32* index)
{
    AutoLock lock(this);
    return AbstractStringBuilder::IndexOf(subString, start, index);
}

ECode StringBuffer::LastIndexOf(
    /* [in] */ const String& string,
    /* [out] */ Int32* index)
{
    AutoLock lock(this);
    return AbstractStringBuilder::LastIndexOf(string, index);
}

ECode StringBuffer::LastIndexOf(
    /* [in] */ const String& subString,
    /* [in] */ Int32 start,
    /* [out] */ Int32* index)
{
    AutoLock lock(this);
    return AbstractStringBuilder::LastIndexOf(subString, start, index);
}

ECode StringBuffer::AppendNULL()
{
    AutoLock lock(this);
    return AbstractStringBuilder::AppendNULL();
}

ECode StringBuffer::Append(
    /* [in] */ const char* str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(str);
}

ECode StringBuffer::Append(
    /* [in] */ Boolean b)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(b);
}

ECode StringBuffer::AppendChar(
    /* [in] */ Char32 c)
{
    AutoLock lock(this);
    return AbstractStringBuilder::AppendChar(c);
}

ECode StringBuffer::Append(
    /* [in] */ Int32 i)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(i);
}

ECode StringBuffer::Append(
    /* [in] */ Int64 l)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(l);
}

ECode StringBuffer::Append(
    /* [in] */ Float f)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(f);
}

ECode StringBuffer::Append(
    /* [in] */ Double d)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(d);
}

ECode StringBuffer::Append(
    /* [in] */ IInterface* obj)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(obj);
}

ECode StringBuffer::Append(
    /* [in] */ const String& str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(str);
}

ECode StringBuffer::Append(
    /* [in] */ IStringBuffer* sb)
{
    AutoLock lock(this);
    if (NULL == sb) {
        return AbstractStringBuilder::AppendNULL();
    }

    String str;
    ((StringBuffer*)sb)->ToString(&str);
    return AbstractStringBuilder::Append(str);
}

ECode StringBuffer::Append(
    /* [in] */ IStringBuilder* sb)
{
    AutoLock lock(this);
    if (NULL == sb) {
        return AbstractStringBuilder::AppendNULL();
    }

    String str;
    ((StringBuilder*)sb)->ToString(&str);
    return AbstractStringBuilder::Append(str);
}

ECode StringBuffer::Append(
    /* [in] */ const ArrayOf<Char32>& chars)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(chars);
}

ECode StringBuffer::Append(
    /* [in] */ const ArrayOf<Char32>& chars,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(chars, offset, length);
}

ECode StringBuffer::Append(
    /* [in] */ ICharSequence* csq)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(csq);
}

ECode StringBuffer::Append(
    /* [in] */ ICharSequence* csq,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Append(csq, start, end);
}

ECode StringBuffer::InsertChar(
    /* [in] */ Int32 offset,
    /* [in] */ Char32 c)
{
    AutoLock lock(this);
    return AbstractStringBuilder::InsertChar(offset, c);
}

ECode StringBuffer::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean b)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, b);
}

ECode StringBuffer::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ Int32 i)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, i);
}

ECode StringBuffer::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ Int64 l)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, l);
}

ECode StringBuffer::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ Float f)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, f);
}

ECode StringBuffer::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ Double d)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, d);
}

ECode StringBuffer::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ IInterface* obj)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, obj);
}

ECode StringBuffer::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ const String& str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, str);
}

ECode StringBuffer::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ const ArrayOf<Char32>& chars)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, chars);
}

ECode StringBuffer::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ const ArrayOf<Char32>& str,
    /* [in] */ Int32 strOffset,
    /* [in] */ Int32 strLen)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, str, strOffset, strLen);
}

ECode StringBuffer::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ ICharSequence* s)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, s);
}

ECode StringBuffer::Insert(
    /* [in] */ Int32 offset,
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Insert(offset, s, start, end);
}

ECode StringBuffer::Replace(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const String& string)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Replace(start, end, string);
}

ECode StringBuffer::Reverse()
{
    AutoLock lock(this);
    return AbstractStringBuilder::Reverse();
}

ECode StringBuffer::DeleteCharAt(
    /* [in] */ Int32 location)
{
    AutoLock lock(this);
    return AbstractStringBuilder::DeleteCharAt(location);
}

ECode StringBuffer::Delete(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Delete(start, end);
}

ECode StringBuffer::ToString(
    /* [out] */ String* str)
{
    AutoLock lock(this);
    return AbstractStringBuilder::Substring(0, str);
}

String StringBuffer::ToString()
{
   String str;
   ToString(&str);
   return str;
}

AutoPtr<ICharSequence> StringBuffer::ToCharSequence()
{
    AutoLock lock(this);
    String str;
    AbstractStringBuilder::Substring(0, &str);
    AutoPtr<ICharSequence> seq;
    CString::New(str, (ICharSequence**)&seq);
    return seq;
}

Int32 StringBuffer::GetLength()
{
    AutoLock lock(this);
    return AbstractStringBuilder::GetLength();
}

Int32 StringBuffer::GetByteCount()
{
    AutoLock lock(this);
    return AbstractStringBuilder::GetByteCount();
}

StringBuffer& StringBuffer::operator+=(const Boolean right)
{
    AutoLock lock(this);
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuffer& StringBuffer::operator+=(const char right)
{
    AutoLock lock(this);
    AbstractStringBuilder::AppendChar(right);
    return *this;
}

StringBuffer& StringBuffer::operator+=(const Char32 right)
{
    AutoLock lock(this);
    AbstractStringBuilder::AppendChar(right);
    return *this;
}

StringBuffer& StringBuffer::operator+=(const Int32 right)
{
    AutoLock lock(this);
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuffer& StringBuffer::operator+=(const Int64 right)
{
    AutoLock lock(this);
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuffer& StringBuffer::operator+=(const Float right)
{
    AutoLock lock(this);
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuffer& StringBuffer::operator+=(const Double right)
{
    AutoLock lock(this);
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuffer& StringBuffer::operator+=(const char* right)
{
    AutoLock lock(this);
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuffer& StringBuffer::operator+=(const String& right)
{
    AutoLock lock(this);
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuffer& StringBuffer::operator+=(IInterface* right)
{
    AutoLock lock(this);
    AbstractStringBuilder::Append(right);
    return *this;
}

StringBuffer& StringBuffer::operator+=(StringBuffer& right)
{
    AutoLock lock(this);
    String str;
    right.ToString(&str);
    AbstractStringBuilder::Append(str);
    return *this;
}

StringBuffer& StringBuffer::operator+=(StringBuilder& right)
{
    AutoLock lock(this);
    String str;
    right.ToString(&str);
    AbstractStringBuilder::Append(str);
    return *this;
}

} // namespace Core
} // namespace Elastos
