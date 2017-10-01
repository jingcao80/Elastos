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

#ifndef __ELASTOS_TEXT_MESSAGEFORMAT_H__
#define __ELASTOS_TEXT_MESSAGEFORMAT_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "FormatBase.h"
#include "NumberFormat.h"
#include "DateFormat.h"
#include "StringBuffer.h"
#include <elastos/utility/etl/List.h>
#include "Character.h"

using Elastos::Utility::Etl::List;
using Elastos::Text::IAttributedCharacterIteratorAttribute;
using Elastos::Text::IFieldPosition;
using Elastos::Text::IParsePosition;

namespace Elastos {
namespace Text {

class MessageFormat
    : public FormatBase
    , public IMessageFormat
{
public:

    class MessageFormatField
        : public FormatBase::Field
        , public IMessageFormatField
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ const String& fieldName);

        //public static final Field ARGUMENT = new Field("message argument field");
        const static AutoPtr<IMessageFormatField> ARGUMENT;

    private:
        //const static Int64 serialVersionUID = 7899943957617360810L;
    };

private:
    /**
     * An inner class to store attributes, values, start and end indices.
     * Instances of this inner class are used as elements for the fields vector
     */
    /*static*/ class FieldContainer : public ElRefBase {
    public:

        FieldContainer(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [in] */ IAttributedCharacterIteratorAttribute* attribute,
            /* [in] */ IInterface* value);

    public:
        Int32 mStart;
        Int32 mEnd;
        AutoPtr<IAttributedCharacterIteratorAttribute> mAttribute;
        AutoPtr<IInterface> mValue;
    };

public:
    CAR_INTERFACE_DECL()

    virtual ~MessageFormat();

    CARAPI constructor(
        /* [in] */ const String& templateName,
        /* [in] */ ILocale* locale);

    CARAPI constructor(
        /* [in] */ const String& templateName);

    CARAPI ApplyPattern(
        /* [in] */ const String& templateName);

    // @Override
    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI CloneImpl(
        /* [in] */ IMessageFormat* format);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI FormatToCharacterIterator(
        /* [in] */ IInterface* object,
        /* [out] */ IAttributedCharacterIterator** characterIterator);

    using FormatBase::Format;

    CARAPI Format(
        /* [in] */ ArrayOf< IInterface* >* objects,
        /* [in] */ IStringBuffer * buffer,
        /* [in] */ IFieldPosition* field);

    //@Override
    CARAPI Format(
        /* [in] */ IInterface* object,
        /* [in] */ IStringBuffer * buffer,
        /* [in] */ IFieldPosition* field);

//    public static String format(String format, Object... args);

    CARAPI GetFormats(
        /* [out] */ ArrayOf< IFormat* >** arrayOfInstances);

    CARAPI GetFormatsByArgumentIndex(
        /* [out] */ ArrayOf< IFormat* >** arrayOfInstances);

    CARAPI SetFormatByArgumentIndex(
        /* [in] */ Int32 argIndex,
        /* [in] */ IFormat* format);

    CARAPI SetFormatsByArgumentIndex(
        /* [in] */ ArrayOf< IFormat* >* formats);

    CARAPI GetLocale(
        /* [out] */ ILocale** locale);

    CARAPI GetHashCode(
        /* [out] */ Int32* value);

    CARAPI Parse(
        /* [in] */ const String& string,
        /* [out, callee] */ ArrayOf< IInterface* >** results);

    CARAPI Parse(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out, callee] */ ArrayOf< IInterface* >** result);

    //@Override
    CARAPI ParseObject(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out] */ IInterface** arrayOfObjects);

    CARAPI SetFormat(
        /* [in] */ Int32 offset,
        /* [in] */ IFormat* format);

    CARAPI SetFormats(
        /* [in] */ ArrayOf< IFormat* >* formats);

    CARAPI SetLocale(
        /* [in] */ ILocale* locale);

    CARAPI ToPattern(
        /* [out] */ String* value);

    CARAPI GetMaxOffset(
        /* [out] */ Int32* maxOffset);

    CARAPI GetArgumentNumbers(
        /* [out, callee] */ ArrayOf<Int32>** argumentNumbers);

    CARAPI GetStrings(
        /* [out, callee] */ ArrayOf<String>** strings);

private:

    CARAPI FormatImpl(
        /* [in] */ ArrayOf< IInterface* >* objects,
        /* [in] */ IStringBuffer* inbuffer,
        /* [in] */ IFieldPosition* field,
        /* [in] */ List<AutoPtr<FieldContainer> >* fields);

    CARAPI HandleArgumentField(
        /* [in] */ Int32 begin,
        /* [in] */ Int32 end,
        /* [in] */ Int32 argIndex,
        /* [in] */ IFieldPosition* position,
        /* [in] */ List<AutoPtr<FieldContainer> >* fields);

    CARAPI HandleFormat(
        /* [in] */ IFormat* format,
        /* [in] */ IInterface* arg,
        /* [in] */ Int32 begin,
        /* [in] */ List<AutoPtr<FieldContainer> >* fields);

    CARAPI Match(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [in] */ Boolean last,
        /* [in] */ ArrayOf<String>* tokens,
        /* [out] */ Int32* value);

    CARAPI ParseVariable(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out] */ IFormat** value);

    CARAPI DecodeDecimalFormat(
        /* [in] */ StringBuffer* buffer,
        /* [in] */ IFormat* format,
        /* [out] */ String* value);

    CARAPI DecodeSimpleDateFormat(
        /* [in] */ StringBuffer* buffer,
        /* [in] */ IFormat* format,
        /* [out] */ String* value);

    CARAPI AppendQuoted(
        /* [in] */ StringBuffer* buffer,
        /* [in] */ const String& string);

/*    private static final ObjectStreamField[] serialPersistentFields = {
            new ObjectStreamField("argumentNumbers", int[].class),
            new ObjectStreamField("formats", Format[].class),
            new ObjectStreamField("locale", Locale.class),
            new ObjectStreamField("maxOffset", int.class),
            new ObjectStreamField("offsets", int[].class),
            new ObjectStreamField("pattern", String.class), };*/

    // private void writeObject(ObjectOutputStream stream);

    // private void readObject(ObjectInputStream stream);

private:
    AutoPtr<ILocale> mLocale;

    mutable AutoPtr<ArrayOf<String> > mStrings;

    AutoPtr<ArrayOf<Int32> > mArgumentNumbers;

    AutoPtr<ArrayOf<IFormat*> > mFormats;

    Int32 mMaxOffset;

    mutable Int32 mMaxArgumentIndex;
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_MESSAGEFORMAT_H__
