
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/Media_Utils.h"
#include <elastos/utility/logging/Slogger.h>

#include <media/stagefright/MediaErrors.h>

using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::Core::CFloat;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IFloat;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IHashMap;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Media {

const String Media_Utils::TAG("Media_Utils");

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

android::status_t Media_Utils::ConvertMessageToMap(
   /* [in] */ android::sp<android::AMessage>& msg,
   /* [out] */ IMap** mymap)
{
    VALIDATE_NOT_NULL(mymap);
    *mymap = NULL;

    AutoPtr<IHashMap> returnMap;
    CHashMap::New( (IHashMap**)&returnMap);

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
                helper->Wrap(&bytes, (IByteBuffer**)&buffer);
                valueObj = buffer->Probe(EIID_IInterface);
                break;
            }
            case android::AMessage::kTypeRect: {
                Int32 left, top, right, bottom;
                msg->findRect(key, &left, &top, &right, &bottom);

                AutoPtr<IInteger32> tmpInt;

                String strLeft;
                strLeft.AppendFormat("%s-left", key);
                AutoPtr<ICharSequence> csLeft;
                CString::New(strLeft, (ICharSequence**)&csLeft);
                CInteger32::New(left, (IInteger32**)&tmpInt);
                returnMap->Put(csLeft ,tmpInt->Probe(EIID_IInterface));

                String strTop;
                strLeft.AppendFormat("%s-top", key);
                AutoPtr<ICharSequence> csTop;
                CString::New(strTop, (ICharSequence**)&csTop);
                tmpInt = NULL;
                CInteger32::New(top, (IInteger32**)&tmpInt);
                returnMap->Put(csTop ,tmpInt->Probe(EIID_IInterface));

                String strRight;
                strLeft.AppendFormat("%s-right", key);
                AutoPtr<ICharSequence> csRight;
                CString::New(strRight, (ICharSequence**)&csRight);
                tmpInt = NULL;
                CInteger32::New(right, (IInteger32**)&tmpInt);
                returnMap->Put(csRight ,tmpInt->Probe(EIID_IInterface));

                String strBottom;
                strLeft.AppendFormat("%s-bottom", key);
                AutoPtr<ICharSequence> csBottom;
                CString::New(strBottom, (ICharSequence**)&csBottom);
                tmpInt = NULL;
                CInteger32::New(bottom, (IInteger32**)&tmpInt);
                returnMap->Put(csBottom ,tmpInt->Probe(EIID_IInterface));
                break;
            }

            default:
                break;
        }//end switch

        if (valueObj != NULL)  {
            String keyObj(key);
            AutoPtr<ICharSequence> csKey;
            CString::New(keyObj, (ICharSequence**)&csKey);
            returnMap->Put(csKey ,valueObj);
        }
    }//end for

    *mymap = IMap::Probe(returnMap);
    REFCOUNT_ADD(*mymap);
    return android::OK;
}

android::status_t Media_Utils::ConvertKeyValueArraysToMessage(
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
            return -EINVAL;
        }

        numEntries = keys->GetLength();
        if (numEntries != values->GetLength()) {
            return E_INVALID_ARGUMENT;
        }
    }
    else if (values != NULL){
        return -EINVAL;
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
            IBuffer::Probe(b)->GetPosition(&position);
            IBuffer::Probe(b)->GetLimit(&limit);
            AutoPtr<ArrayOf<Byte> > array;
            b->GetArray((ArrayOf<Byte>**)&array);

            const uint8_t* data = (const uint8_t*)(array->GetPayload());
            android::sp<android::ABuffer> buffer = new android::ABuffer(limit - position);
            memcpy(buffer->data(), (const uint8_t*)(data + position), buffer->size());

            msg->setBuffer(key.c_str(),buffer);
        }
    }//for loop

    *out = msg;
    return android::OK;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
