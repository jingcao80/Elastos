/*
 * Copyright (C) 2007-2008 Esmertec AG.
 * Copyright (C) 2007-2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.google.android.mms.pdu;

using Elastos::Droid::Net::IUri;

using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap;

/**
 * The pdu part.
 */
public class PduPart {
    /**
     * Well-Known Parameters.
     */
    public static const Int32 P_Q                  = 0x80;
    public static const Int32 P_CHARSET            = 0x81;
    public static const Int32 P_LEVEL              = 0x82;
    public static const Int32 P_TYPE               = 0x83;
    public static const Int32 P_DEP_NAME           = 0x85;
    public static const Int32 P_DEP_FILENAME       = 0x86;
    public static const Int32 P_DIFFERENCES        = 0x87;
    public static const Int32 P_PADDING            = 0x88;
    // This value of "TYPE" s used with Content-Type: multipart/related
    public static const Int32 P_CT_MR_TYPE         = 0x89;
    public static const Int32 P_DEP_START          = 0x8A;
    public static const Int32 P_DEP_START_INFO     = 0x8B;
    public static const Int32 P_DEP_COMMENT        = 0x8C;
    public static const Int32 P_DEP_DOMAIN         = 0x8D;
    public static const Int32 P_MAX_AGE            = 0x8E;
    public static const Int32 P_DEP_PATH           = 0x8F;
    public static const Int32 P_SECURE             = 0x90;
    public static const Int32 P_SEC                = 0x91;
    public static const Int32 P_MAC                = 0x92;
    public static const Int32 P_CREATION_DATE      = 0x93;
    public static const Int32 P_MODIFICATION_DATE  = 0x94;
    public static const Int32 P_READ_DATE          = 0x95;
    public static const Int32 P_SIZE               = 0x96;
    public static const Int32 P_NAME               = 0x97;
    public static const Int32 P_FILENAME           = 0x98;
    public static const Int32 P_START              = 0x99;
    public static const Int32 P_START_INFO         = 0x9A;
    public static const Int32 P_COMMENT            = 0x9B;
    public static const Int32 P_DOMAIN             = 0x9C;
    public static const Int32 P_PATH               = 0x9D;

    /**
     *  Header field names.
     */
     public static const Int32 P_CONTENT_TYPE       = 0x91;
     public static const Int32 P_CONTENT_LOCATION   = 0x8E;
     public static const Int32 P_CONTENT_ID         = 0xC0;
     public static const Int32 P_DEP_CONTENT_DISPOSITION = 0xAE;
     public static const Int32 P_CONTENT_DISPOSITION = 0xC5;
    // The next header is unassigned header, use reserved Header(0x48) value.
     public static const Int32 P_CONTENT_TRANSFER_ENCODING = 0xC8;

     /**
      * Content=Transfer-Encoding string.
      */
     public static const String CONTENT_TRANSFER_ENCODING =
             "Content-Transfer-Encoding";

     /**
      * Value of Content-Transfer-Encoding.
      */
     public static const String P_BINARY = "binary";
     public static const String P_7BIT = "7bit";
     public static const String P_8BIT = "8bit";
     public static const String P_BASE64 = "base64";
     public static const String P_QUOTED_PRINTABLE = "quoted-printable";

     /**
      * Value of disposition can be set to PduPart when the value is octet in
      * the PDU.
      * "from-data" instead of Form-data<Octet 128>.
      * "attachment" instead of Attachment<Octet 129>.
      * "inline" instead of Inline<Octet 130>.
      */
     static const Byte[] DISPOSITION_FROM_DATA = "from-data".GetBytes();
     static const Byte[] DISPOSITION_ATTACHMENT = "attachment".GetBytes();
     static const Byte[] DISPOSITION_INLINE = "inline".GetBytes();

     /**
      * Content-Disposition value.
      */
     public static const Int32 P_DISPOSITION_FROM_DATA  = 0x80;
     public static const Int32 P_DISPOSITION_ATTACHMENT = 0x81;
     public static const Int32 P_DISPOSITION_INLINE     = 0x82;

     /**
      * Header of part.
      */
     private Map<Integer, Object> mPartHeader = NULL;

     /**
      * Data uri.
      */
     private Uri mUri = NULL;

     /**
      * Part data.
      */
     private Byte[] mPartData = NULL;

     private static const String TAG = "PduPart";

     /**
      * Empty Constructor.
      */
     public PduPart() {
         mPartHeader = new HashMap<Integer, Object>();
     }

     /**
      * Set part data. The data are stored as Byte array.
      *
      * @param data the data
      */
     CARAPI SetData(Byte[] data) {
         If(data == NULL) {
            return;
        }

         mPartData = new Byte[data.length];
         System->Arraycopy(data, 0, mPartData, 0, data.length);
     }

     /**
      * @return A copy of the part data or NULL if the data wasn't set or
      *         the data is stored as Uri.
      * @see #getDataUri
      */
     public Byte[] GetData() {
         If(mPartData == NULL) {
            return NULL;
         }

         Byte[] byteArray = new Byte[mPartData.length];
         System->Arraycopy(mPartData, 0, byteArray, 0, mPartData.length);
         return byteArray;
     }

    /**
     * @return The length of the data, if this object have data, else 0.
     */
     public Int32 GetDataLength() {
         If(mPartData != NULL){
             return mPartData.length;
         } else {
             return 0;
         }
     }


     /**
      * Set data uri. The data are stored as Uri.
      *
      * @param uri the uri
      */
     CARAPI SetDataUri(Uri uri) {
         mUri = uri;
     }

     /**
      * @return The Uri of the part data or NULL if the data wasn't set or
      *         the data is stored as Byte array.
      * @see #getData
      */
     public Uri GetDataUri() {
         return mUri;
     }

     /**
      * Set Content-id value
      *
      * @param contentId the content-id value
      * @throws NullPointerException if the value is NULL.
      */
     CARAPI SetContentId(Byte[] contentId) {
         If((contentId == NULL) || (contentId.length == 0)) {
             throw new IllegalArgumentException(
                     "Content-Id may not be NULL or empty.");
         }

         If ((contentId.length > 1)
                 && ((Char32) contentId[0] == '<')
                 && ((Char32) contentId[contentId.length - 1] == '>')) {
             mPartHeader->Put(P_CONTENT_ID, contentId);
             return;
         }

         // Insert beginning '<' and trailing '>' for Content-Id.
         Byte[] buffer = new Byte[contentId.length + 2];
         buffer[0] = (Byte) (0xff & '<');
         buffer[buffer.length - 1] = (Byte) (0xff & '>');
         System->Arraycopy(contentId, 0, buffer, 1, contentId.length);
         mPartHeader->Put(P_CONTENT_ID, buffer);
     }

     /**
      * Get Content-id value.
      *
      * @return the value
      */
     public Byte[] GetContentId() {
         Return (Byte[]) mPartHeader->Get(P_CONTENT_ID);
     }

     /**
      * Set Char-set value.
      *
      * @param charset the value
      */
     CARAPI SetCharset(Int32 charset) {
         mPartHeader->Put(P_CHARSET, charset);
     }

     /**
      * Get Char-set value
      *
      * @return the charset value. Return 0 if charset was not set.
      */
     public Int32 GetCharset() {
         Integer charset = (Integer) mPartHeader->Get(P_CHARSET);
         If(charset == NULL) {
             return 0;
         } else {
             return charset->IntValue();
         }
     }

     /**
      * Set Content-Location value.
      *
      * @param contentLocation the value
      * @throws NullPointerException if the value is NULL.
      */
     CARAPI SetContentLocation(Byte[] contentLocation) {
         If(contentLocation == NULL) {
             throw new NullPointerException("NULL content-location");
         }

         mPartHeader->Put(P_CONTENT_LOCATION, contentLocation);
     }

     /**
      * Get Content-Location value.
      *
      * @return the value
      *     return PduPart.disposition[0] instead of <Octet 128> (Form-data).
      *     return PduPart.disposition[1] instead of <Octet 129> (Attachment).
      *     return PduPart.disposition[2] instead of <Octet 130> (Inline).
      */
     public Byte[] GetContentLocation() {
         Return (Byte[]) mPartHeader->Get(P_CONTENT_LOCATION);
     }

     /**
      * Set Content-Disposition value.
      * Use PduPart.disposition[0] instead of <Octet 128> (Form-data).
      * Use PduPart.disposition[1] instead of <Octet 129> (Attachment).
      * Use PduPart.disposition[2] instead of <Octet 130> (Inline).
      *
      * @param contentDisposition the value
      * @throws NullPointerException if the value is NULL.
      */
     CARAPI SetContentDisposition(Byte[] contentDisposition) {
         If(contentDisposition == NULL) {
             throw new NullPointerException("NULL content-disposition");
         }

         mPartHeader->Put(P_CONTENT_DISPOSITION, contentDisposition);
     }

     /**
      * Get Content-Disposition value.
      *
      * @return the value
      */
     public Byte[] GetContentDisposition() {
         Return (Byte[]) mPartHeader->Get(P_CONTENT_DISPOSITION);
     }

     /**
      *  Set Content-Type value.
      *
      *  @param value the value
      *  @throws NullPointerException if the value is NULL.
      */
     CARAPI SetContentType(Byte[] contentType) {
         If(contentType == NULL) {
             throw new NullPointerException("NULL content-type");
         }

         mPartHeader->Put(P_CONTENT_TYPE, contentType);
     }

     /**
      * Get Content-Type value of part.
      *
      * @return the value
      */
     public Byte[] GetContentType() {
         Return (Byte[]) mPartHeader->Get(P_CONTENT_TYPE);
     }

     /**
      * Set Content-Transfer-Encoding value
      *
      * @param contentId the content-id value
      * @throws NullPointerException if the value is NULL.
      */
     CARAPI SetContentTransferEncoding(Byte[] contentTransferEncoding) {
         If(contentTransferEncoding == NULL) {
             throw new NullPointerException("NULL content-transfer-encoding");
         }

         mPartHeader->Put(P_CONTENT_TRANSFER_ENCODING, contentTransferEncoding);
     }

     /**
      * Get Content-Transfer-Encoding value.
      *
      * @return the value
      */
     public Byte[] GetContentTransferEncoding() {
         Return (Byte[]) mPartHeader->Get(P_CONTENT_TRANSFER_ENCODING);
     }

     /**
      * Set Content-type parameter: name.
      *
      * @param name the name value
      * @throws NullPointerException if the value is NULL.
      */
     CARAPI SetName(Byte[] name) {
         If(NULL == name) {
             throw new NullPointerException("NULL content-id");
         }

         mPartHeader->Put(P_NAME, name);
     }

     /**
      *  Get content-type parameter: name.
      *
      *  @return the name
      */
     public Byte[] GetName() {
         Return (Byte[]) mPartHeader->Get(P_NAME);
     }

     /**
      * Get Content-disposition parameter: filename
      *
      * @param fileName the filename value
      * @throws NullPointerException if the value is NULL.
      */
     CARAPI SetFilename(Byte[] fileName) {
         If(NULL == fileName) {
             throw new NullPointerException("NULL content-id");
         }

         mPartHeader->Put(P_FILENAME, fileName);
     }

     /**
      * Set Content-disposition parameter: filename
      *
      * @return the filename
      */
     public Byte[] GetFilename() {
         Return (Byte[]) mPartHeader->Get(P_FILENAME);
     }

    public String GenerateLocation() {
        // Assumption: At least one of the content-location / name / filename
        // or content-id should be set. This is guaranteed by the PduParser
        // for incoming messages and by MM composer for outgoing messages.
        Byte[] location = (Byte[]) mPartHeader->Get(P_NAME);
        If(NULL == location) {
            location = (Byte[]) mPartHeader->Get(P_FILENAME);

            If (NULL == location) {
                location = (Byte[]) mPartHeader->Get(P_CONTENT_LOCATION);
            }
        }

        If (NULL == location) {
            Byte[] contentId = (Byte[]) mPartHeader->Get(P_CONTENT_ID);
            return "cid:" + new String(contentId);
        } else {
            return new String(location);
        }
    }
}

