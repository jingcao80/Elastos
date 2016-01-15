
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/Media_Utils.h"
#include <media/stagefright/MediaErrors.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IFloat;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CFloat;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Media {

const String Media_Utils::TAG("Media_Utils");

ECode Media_Utils::ThrowExceptionAsNecessary(
    /* [in] */ android::status_t err,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (err >= android::ERROR_DRM_WV_VENDOR_MIN
        && err <= android::ERROR_DRM_WV_VENDOR_MAX) {
        // We'll throw our custom MediaCodec.CryptoException
        Slogger::E(TAG, "MediaCodec.CryptoException!");
        return E_CRYPTO_EXCEPTION;
    }

    switch (err) {
        case android::OK:
            return NOERROR;

        case -EAGAIN:
            *result = DEQUEUE_INFO_TRY_AGAIN_LATER;
            return NOERROR; // DEQUEUE_INFO_TRY_AGAIN_LATER;

        case android::INFO_FORMAT_CHANGED:
            *result = DEQUEUE_INFO_OUTPUT_FORMAT_CHANGED;
            return NOERROR; //DEQUEUE_INFO_OUTPUT_FORMAT_CHANGED;

        case android::INFO_OUTPUT_BUFFERS_CHANGED:
            *result = DEQUEUE_INFO_OUTPUT_BUFFERS_CHANGED;
            return NOERROR; //DEQUEUE_INFO_OUTPUT_BUFFERS_CHANGED;

        default:
            Slogger::E(TAG, "Media IllegalStateException!");
            return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

ECode Media_Utils::ConvertKeyValueArraysToKeyedVector(
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<String>* values,
    /* [in] */ android::KeyedVector<android::String8,android::String8>* mykeyedVector)
{
    VALIDATE_NOT_NULL(mykeyedVector);

    Int32 nKeyValuePairs = 0;
    Boolean failed = FALSE;

    if (keys != NULL && values != NULL) {
        nKeyValuePairs = keys->GetLength();
        if (nKeyValuePairs != values->GetLength()) {
            failed = TRUE;
        }
    }

    if (!failed) {
        failed = ((keys != NULL && values == NULL)
            || (keys == NULL && values != NULL));
    }

    if (failed) {
        Slogger::E(TAG, "keys and values arrays have different length!");
        //should return fail
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    const char* keyStr;
    const char* valueStr;
    for (Int32 i = 0; i < nKeyValuePairs; ++i) {
        // No need to check on the ArrayIndexOutOfBoundsException, since
        // it won't happen here.
        keyStr = (*keys)[i].string();
        valueStr = (*values)[i].string();
        mykeyedVector->add(android::String8(keyStr),android::String8(valueStr));
    }
    return NOERROR;
}

ECode Media_Utils::ConvertMessageToMap(
   /* [in] */ android::sp<android::AMessage>& msg,
   /* [out] */ IObjectStringMap** mymap)
{
    VALIDATE_NOT_NULL(mymap);
    *mymap = NULL;

    AutoPtr<IObjectStringMap> returnMap;
    CObjectStringMap::New( (IObjectStringMap**)&returnMap);

    for (Int32 i = 0; i < msg->countEntries(); i++) {
        android::AMessage::Type valueType;
        const char *key = msg->getEntryNameAt(i,&valueType);

        AutoPtr<IInterface> valueObj;
        switch (valueType){
            case android::AMessage::kTypeInt32: {
                Int32 val;
                msg->findInt32(key, &val);
                AutoPtr<IInteger32> value;
                CInteger32::New(val, (IInteger32**)&value);
                valueObj = value->Probe(EIID_IInterface);
                break;
            }
            case android::AMessage::kTypeInt64: {
                Int64 val;
                msg->findInt64(key, &val);
                AutoPtr<IInteger64> value;
                CInteger64::New(val, (IInteger64**)&value);
                valueObj = value->Probe(EIID_IInterface);
                break;
            }
            case android::AMessage::kTypeFloat: {
                Float val;
                msg->findFloat(key, &val);
                AutoPtr<IFloat> value;
                CFloat::New(val, (IFloat**)&value);
                valueObj = value->Probe(EIID_IInterface);
                break;
            }
            case android::AMessage::kTypeString: {
                android::AString val;
                msg->findString(key, &val);
                AutoPtr<ICharSequence> value;
                CString::New(String(val.c_str()), (ICharSequence**)&value);
                valueObj = value->Probe(EIID_IInterface);
                break;
            }
            case android::AMessage::kTypeBuffer: {
                android::sp<android::ABuffer> val;
                msg->findBuffer(key, &val);
                Int32 size = val->size();
                ArrayOf<Byte> bytes((Byte*)val->data(), size);

                AutoPtr<IByteBufferHelper> helper;
                CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
                AutoPtr<IByteBuffer> buffer;
                helper->Allocate(size, (IByteBuffer**)&buffer);
                buffer->PutBytes(bytes);
                valueObj = buffer->Probe(EIID_IInterface);
                break;
            }
            case android::AMessage::kTypeRect: {
                Int32 left, top, right, bottom;
                msg->findRect(key, &left, &top, &right, &bottom);

                AutoPtr<IInteger32> tmpInt;

                String strLeft;
                strLeft.AppendFormat("%s-left", key);
                CInteger32::New(left, (IInteger32**)&tmpInt);
                returnMap->Put(strLeft ,tmpInt->Probe(EIID_IInterface));

                String strTop;
                strLeft.AppendFormat("%s-top", key);
                tmpInt = NULL;
                CInteger32::New(top, (IInteger32**)&tmpInt);
                returnMap->Put(strTop ,tmpInt->Probe(EIID_IInterface));

                String strRight;
                strLeft.AppendFormat("%s-right", key);
                tmpInt = NULL;
                CInteger32::New(right, (IInteger32**)&tmpInt);
                returnMap->Put(strRight ,tmpInt->Probe(EIID_IInterface));

                String strBottom;
                strLeft.AppendFormat("%s-bottom", key);
                tmpInt = NULL;
                CInteger32::New(bottom, (IInteger32**)&tmpInt);
                returnMap->Put(strBottom ,tmpInt->Probe(EIID_IInterface));
                break;
            }

            default:
                break;
        }//end switch

        if (valueObj != NULL)  {
            String keyObj(key);
            returnMap->Put(keyObj ,valueObj);
        }
    }//end for

    *mymap = returnMap;
    REFCOUNT_ADD(*mymap);
    return NOERROR;
}

ECode Media_Utils::ConvertKeyValueArraysToMessage(
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<IInterface*>* values,
    /* [in] */ android::sp<android::AMessage> * out)
{
    VALIDATE_NOT_NULL(out);
    *out = NULL;

    Int32 numEntries = 0;
    if (keys != NULL){
        if (values == NULL){
            //show error log
            return E_INVALID_ARGUMENT;
        }

        numEntries = keys->GetLength();
        if (numEntries != values->GetLength()) {
            return E_INVALID_ARGUMENT;
        }
    }
    else if (values!=NULL){
        return E_INVALID_ARGUMENT;
    }

    android::sp<android::AMessage> msg = new android::AMessage;

    for (Int32 i = 0; i < numEntries; i++) {
        android::AString key = (*keys)[i].string();
        IInterface* valueObj = (*values)[i];

        if (ICharSequence::Probe(valueObj) != NULL) {
            String tempvalue;
            ICharSequence::Probe(valueObj)->ToString(&tempvalue);
            msg->setString(key.c_str(), tempvalue.string());
        }
        else if (IInteger32::Probe(valueObj) != NULL){
            Int32 value;
            IInteger32::Probe(valueObj)->GetValue(&value);
            msg->setInt32(key.c_str(), value);
        }
        else if (IFloat::Probe(valueObj) != NULL){
            Float value;
            IFloat::Probe(valueObj)->GetValue(&value);
            msg->setFloat(key.c_str(), value);
        }
        else if (IByteBuffer::Probe(valueObj) != NULL){
            IByteBuffer* b = IByteBuffer::Probe(valueObj);
            Int32 position;
            Int32 limit;
            b->GetPosition(&position);
            b->GetLimit(&limit);
            AutoPtr<ArrayOf<Byte> > array;
            b->GetArray((ArrayOf<Byte>**)&array);

            const uint8_t* data = (const uint8_t*)(array->GetPayload());
            android::sp<android::ABuffer> buffer = new android::ABuffer(limit - position);
            memcpy(buffer->data(), (const uint8_t*)(data + position), buffer->size());

            msg->setBuffer(key.c_str(),buffer);
        }
    }//for loop

    *out = msg;
    return NOERROR;
}



} // namespace Media
} // namepsace Droid
} // namespace Elastos
