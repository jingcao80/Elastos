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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_WSPTYPEDECODER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_WSPTYPEDECODER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * Implement the WSP data type decoder.
 *
 * @hide
 */
class WspTypeDecoder
    : public Object
    , public IWspTypeDecoder
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pdu);

    /**
     * Decode the "Text-string" type for WSP pdu
     *
     * @param startIndex The starting position of the "Text-string" in this pdu
     *
     * @return FALSE when Error(not a Text-string) occur
     *         return value can be retrieved by GetValueString() method length of data in pdu can be
     *         retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeTextString(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "Token-text" type for WSP pdu
     *
     * @param startIndex The starting position of the "Token-text" in this pdu
     *
     * @return always TRUE
     *         return value can be retrieved by GetValueString() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeTokenText(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "Short-integer" type for WSP pdu
     *
     * @param startIndex The starting position of the "Short-integer" in this pdu
     *
     * @return FALSE when Error(not a Short-integer) occur
     *         return value can be retrieved by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeShortInteger(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "Long-integer" type for WSP pdu
     *
     * @param startIndex The starting position of the "Long-integer" in this pdu
     *
     * @return FALSE when Error(not a Long-integer) occur
     *         return value can be retrieved by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeLongInteger(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "Integer-Value" type for WSP pdu
     *
     * @param startIndex The starting position of the "Integer-Value" in this pdu
     *
     * @return FALSE when Error(not a Integer-Value) occur
     *         return value can be retrieved by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeIntegerValue(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "Uintvar-integer" type for WSP pdu
     *
     * @param startIndex The starting position of the "Uintvar-integer" in this pdu
     *
     * @return FALSE when Error(not a Uintvar-integer) occur
     *         return value can be retrieved by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeUintvarInteger(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "Value-length" type for WSP pdu
     *
     * @param startIndex The starting position of the "Value-length" in this pdu
     *
     * @return FALSE when Error(not a Value-length) occur
     *         return value can be retrieved by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeValueLength(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "Extension-media" type for WSP PDU.
     *
     * @param startIndex The starting position of the "Extension-media" in this PDU.
     *
     * @return FALSE on error, such as if there is no Extension-media at startIndex.
     *         Side-effects: updates StringValue (available with
     *         GetValueString()), which will be NULL on error. The length of the
     *         data in the PDU is available with GetValue32(), 0 on error.
     */
    CARAPI DecodeExtensionMedia(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "Constrained-encoding" type for WSP pdu
     *
     * @param startIndex The starting position of the "Constrained-encoding" in this pdu
     *
     * @return FALSE when Error(not a Constrained-encoding) occur
     *         return value can be retrieved first by GetValueString() and second by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeConstrainedEncoding(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "Content-type" type for WSP pdu
     *
     * @param startIndex The starting position of the "Content-type" in this pdu
     *
     * @return FALSE when Error(not a Content-type) occurs
     *         If a content type exists in the Headers (either as inline string, or as well-known
     *         value), GetValueString() will return it. If a 'well known value' is encountered that
     *         cannot be mapped to a string mime type, GetValueString() will return NULL, and
     *         GetValue32() will return the unknown content type value.
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     *         Any content type parameters will be accessible via GetContentParameters()
     */
    CARAPI DecodeContentType(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "Content length" type for WSP pdu
     *
     * @param startIndex The starting position of the "Content length" in this pdu
     *
     * @return FALSE when Error(not a Content length) occur
     *         return value can be retrieved by GetValue32() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeContentLength(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "Content location" type for WSP pdu
     *
     * @param startIndex The starting position of the "Content location" in this pdu
     *
     * @return FALSE when Error(not a Content location) occur
     *         return value can be retrieved by GetValueString() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeContentLocation(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "X-Wap-Application-Id" type for WSP pdu
     *
     * @param startIndex The starting position of the "X-Wap-Application-Id" in this pdu
     *
     * @return FALSE when Error(not a X-Wap-Application-Id) occur
     *         return value can be retrieved first by GetValueString() and second by GetValue32()
     *         method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeXWapApplicationId(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Seek for the "X-Wap-Application-Id" field for WSP pdu
     *
     * @param startIndex The starting position of seek pointer
     * @param endIndex Valid seek area end point
     *
     * @return FALSE when Error(not a X-Wap-Application-Id) occur
     *         return value can be retrieved by GetValue32()
     */
    CARAPI SeekXWapApplicationId(
        /* [in] */ Int32 startIndex,
        /* [in] */ Int32 endIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "X-Wap-Content-URI" type for WSP pdu
     *
     * @param startIndex The starting position of the "X-Wap-Content-URI" in this pdu
     *
     * @return FALSE when Error(not a X-Wap-Content-URI) occur
     *         return value can be retrieved by GetValueString() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeXWapContentURI(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * Decode the "X-Wap-Initiator-URI" type for WSP pdu
     *
     * @param startIndex The starting position of the "X-Wap-Initiator-URI" in this pdu
     *
     * @return FALSE when Error(not a X-Wap-Initiator-URI) occur
     *         return value can be retrieved by GetValueString() method
     *         length of data in pdu can be retrieved by GetDecodedDataLength() method
     */
    CARAPI DecodeXWapInitiatorURI(
        /* [in] */ Int32 startIndex,
        /* [out] */ Boolean* result);

    /**
     * The data length of latest operation.
     */
    CARAPI GetDecodedDataLength(
        /* [out] */ Int32* result);

    /**
     * The 32-bits result of latest operation.
     */
    CARAPI GetValue32(
        /* [out] */ Int64* result);

    /**
     * The String result of latest operation.
     */
    CARAPI GetValueString(
        /* [out] */ String* result);

    /**
     * Any parameters encountered as part of a DecodeContentType() invocation.
     *
     * @return a map of content parameters keyed by their names, or NULL if
     *         DecodeContentType() has not been called If any unassigned
     *         well-known parameters are encountered, the key of the map will be
     *         'unassigned/0x...', where '...' is the hex value of the
     *         unassigned parameter.  If a parameter has No-Value the value will be NULL.
     *
     */
    CARAPI GetContentParameters(
        /* [out] */ IHashMap** result);

private:
    CARAPI_(Boolean) ReadContentParameters(
        /* [in] */ Int32 startIndex,
        /* [in] */ Int32 leftToRead,
        /* [in] */ Int32 accumulator);

    /**
     * Check if the next Byte is No-Value
     *
     * @param startIndex The starting position of the "Content length" in this pdu
     *
     * @return TRUE if and only if the next Byte is 0x00
     */
    CARAPI_(Boolean) DecodeNoValue(
        /* [in] */ Int32 startIndex);

    /**
     * Populate stringValue with the mime type corresponding to the value in unsigned32bit
     *
     * Sets unsigned32bit to -1 if stringValue is already populated
     */
    CARAPI_(void) ExpandWellKnownMimeType();

public:
    AutoPtr<ArrayOf<Byte> > mWspData;
    Int32 mDataLength;
    Int64 mUnsigned32bit;
    String mStringValue;

    AutoPtr<IHashMap> mContentParameters;

private:
    static const Int32 WAP_PDU_SHORT_LENGTH_MAX;
    static const Int32 WAP_PDU_LENGTH_QUOTE;

    static const AutoPtr<IHashMap> WELL_KNOWN_MIME_TYPES;

    static const AutoPtr<IHashMap> WELL_KNOWN_PARAMETERS;

    static const Int32 Q_VALUE;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_WSPTYPEDECODER_H__
