
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/gesture/GestureStore.h"
#include "elastos/droid/gesture/GestureUtils.h"
#include "elastos/droid/gesture/GestureConstants.h"
#include "elastos/droid/gesture/InstanceLearner.h"
#include "elastos/droid/gesture/Instance.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Gesture::IGestureStore;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataOutput;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::EIID_IBufferedInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IFlushable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Gesture {

const Int16 GestureStore::FILE_FORMAT_VERSION = 1;
const Boolean GestureStore::PROFILE_LOADING_SAVING = FALSE;

CAR_INTERFACE_IMPL(GestureStore, Object, IGestureStore);

GestureStore::GestureStore()
    : mSequenceType(IGestureStore::SEQUENCE_SENSITIVE)
    , mOrientationStyle(IGestureStore::ORIENTATION_SENSITIVE)
    , mChanged(FALSE)

{}

GestureStore::~GestureStore()
{}


ECode GestureStore::constructor()
{
    mClassifier = new InstanceLearner();
    return NOERROR;
}

ECode GestureStore::SetOrientationStyle(
    /* [in] */ Int32 style)
{
    mOrientationStyle = style;
    return NOERROR;
}

ECode GestureStore::GetOrientationStyle(
    /* [out] */ Int32 *style)
{
    VALIDATE_NOT_NULL(style);
    *style = mOrientationStyle;
    return NOERROR;
}

ECode GestureStore::SetSequenceType(
    /* [in] */ Int32 type)
{
    mSequenceType = type;
    return NOERROR;
}

ECode GestureStore::GetSequenceType(
    /* [out] */ Int32 *type)
{
    VALIDATE_NOT_NULL(type);
    *type = mSequenceType;
    return NOERROR;
}

ECode GestureStore::GetGestureEntries(
    /* [out] */ IList** entries)
{
    VALIDATE_NOT_NULL(entries);

    AutoPtr<IList> container;
    HashMap<String, AutoPtr<GestureList> >::Iterator iter = mNamedGestures.Begin();
    for (; iter != mNamedGestures.End(); ++iter) {
        AutoPtr<ICharSequence> keystr = NULL;
        CString::New(iter->mFirst, (ICharSequence**)&keystr);
        container->Add(keystr);

    }
    *entries = container;
    REFCOUNT_ADD(*entries);
    return NOERROR;
}

ECode GestureStore::Recognize(
    /* [in] */ IGesture *gesture,
    /* [out] */ IArrayList** list)
{
    VALIDATE_NOT_NULL(list);

    AutoPtr<Instance> instance = Instance::CreateInstance(mSequenceType, mOrientationStyle,
        gesture, String(NULL));

    AutoPtr<IArrayList> tmpList;
    mClassifier->Classify(mSequenceType, mOrientationStyle, instance->mVector, (IArrayList**)&tmpList);
    *list = tmpList;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode GestureStore::AddGesture(
    /* [in] */ const String& entryName,
    /* [in] */ IGesture *gesture)
{
    if (entryName.IsNull() || entryName.GetLength() == 0) {
        return NOERROR;
    }

    AutoPtr<GestureList> gestures;
    HashMap<String, AutoPtr<GestureList> >::Iterator iter;
    for (iter = mNamedGestures.Begin(); iter != mNamedGestures.End(); ++iter) {
        if (iter->mFirst.Equals(entryName) ) {
            gestures = iter->mSecond;
        }
    }

    if (gestures == NULL) {
        gestures = new List<AutoPtr<IGesture> >();
        mNamedGestures[entryName] = gestures;
    }
    gestures->PushBack(gesture);
    AutoPtr<Instance> instance = Instance::CreateInstance(mSequenceType, mOrientationStyle,
        gesture, entryName);
    mClassifier->AddInstance(instance);
    mChanged = TRUE;

    return NOERROR;
}

ECode GestureStore::RemoveGesture(
    /* [in] */  const String& entryName,
    /* [in] */  IGesture *gesture)
{
    AutoPtr<GestureList> gestures;
    HashMap<String, AutoPtr<GestureList> >::Iterator iter;
    for (iter = mNamedGestures.Begin(); iter != mNamedGestures.End(); ++iter) {
        if ((iter->mFirst).Equals(entryName) ) {
            gestures = iter->mSecond;
        }
    }
    if (gestures == NULL) {
        return NOERROR;
    }

    gestures->Remove(gesture);

    // if there are no more samples, remove the entry automatically
    if (gestures->IsEmpty()) {
        mNamedGestures.Erase(entryName);
    }
    Int64 id;
    gesture->GetID(&id);
    mClassifier->RemoveInstance(id);

    mChanged = TRUE;
    return NOERROR;
}

ECode GestureStore::RemoveEntry(
    /* [in] */ const String& entryName)
{
    mNamedGestures.Erase(entryName);
    mClassifier->RemoveInstances(entryName);
    mChanged = TRUE;
    return NOERROR;
}

ECode GestureStore::GetGestures(
    /* [in] */ const String& entryName,
    /* [out] */ IArrayList **value)
{
    VALIDATE_NOT_NULL(value);
    *value = NULL;

    AutoPtr<GestureList> gestures;
    HashMap<String, AutoPtr<GestureList> >::Iterator iter = mNamedGestures.Find(entryName);
    if (iter != mNamedGestures.End())
        gestures = iter->mSecond;

    GestureList::Iterator it;
    if (gestures != NULL) {
        ECode ec = CArrayList::New(value);
        if (FAILED(ec)) {
            return E_OUT_OF_MEMORY;
        }

        for (it = gestures->Begin(); it != gestures->End(); ++it) {
            (*value)->Add(*it);
        }
    }
    return NOERROR;
}

ECode GestureStore::HasChanged(
    /* [out] */  Boolean *hasChanged)
{
    VALIDATE_NOT_NULL(hasChanged);
    *hasChanged = mChanged;
    return NOERROR;
}

ECode GestureStore::Save(
    /* [in] */ IOutputStream *stream)
{
    return Save(stream, FALSE);
}

ECode GestureStore::Save(
    /* [in] */ IOutputStream *stream,
    /* [in] */ Boolean closeStream)
{
    AutoPtr<IDataOutputStream> out;

    //try {
    Int64 start;
    if (PROFILE_LOADING_SAVING) {
        start = SystemClock::GetElapsedRealtime();
    }

    HashMap<String, AutoPtr<GestureList> >& maps = mNamedGestures;

    if (IBufferedOutputStream::Probe(stream) != NULL) {
        CDataOutputStream::New(stream, (IDataOutputStream**)&out);
    }
    else {
        AutoPtr<IBufferedOutputStream> bos;
        CBufferedOutputStream::New(stream,
            GestureConstants::IO_BUFFER_SIZE, (IBufferedOutputStream**)&bos);

        CDataOutputStream::New(IOutputStream::Probe(bos), (IDataOutputStream**)&out);
    }
    // Write version number
    IDataOutput::Probe(out)->WriteInt16(FILE_FORMAT_VERSION);
    // Write number of entries
    IDataOutput::Probe(out)->WriteInt32(maps.GetSize());

    HashMap<String, AutoPtr<GestureList> >::Iterator iter = maps.Begin();
    for (; iter != maps.End(); ++iter) {
        const String key = iter->mFirst;
        AutoPtr<List<AutoPtr<IGesture> > > examples = iter->mSecond;
        const Int32 count = examples->GetSize();

        // Write entry name
        IDataOutput::Probe(out)->WriteUTF(key);
        // Write number of examples for this entry
        IDataOutput::Probe(out)->WriteInt32(count);

        List<AutoPtr<IGesture> >::Iterator itList = examples->Begin();
        for (; itList != examples->End(); ++itList) {
            (*itList)->Serialize(out);
        }
    }

    IFlushable::Probe(out)->Flush();

    if (PROFILE_LOADING_SAVING) {
        Int64 end = SystemClock::GetElapsedRealtime();
        Int64 diff= end - start;
        String tmp = StringUtils::ToString(diff);
        String log = String("Saving gestures library = ") + tmp + String(" ms");
        Logger::D(GestureConstants::myLOG_TAG, log.string());
    }

    mChanged = FALSE;
    if (closeStream) {
        GestureUtils::CloseStream(ICloseable::Probe(out));
    }

    return NOERROR;
}

ECode GestureStore::Load(
    /* [in] */ IInputStream *stream)
{
    return Load(stream, FALSE);
}

ECode GestureStore::Load(
    /* [in] */ IInputStream *stream,
    /* [in] */ Boolean closeStream)
{
    AutoPtr<IDataInputStream> in = NULL;
    //try {

    if (stream->Probe(EIID_IBufferedInputStream) != NULL) {
        CDataInputStream::New(stream, (IDataInputStream**)&in);
    }
    else {
        AutoPtr<IBufferedInputStream> bis;
        CBufferedInputStream::New(stream,
            GestureConstants::IO_BUFFER_SIZE, (IBufferedInputStream**)&bis);

        CDataInputStream::New(IInputStream::Probe(bis), (IDataInputStream**)&in);
    }

    Int64 start;
    if (PROFILE_LOADING_SAVING) {
        start = SystemClock::GetElapsedRealtime();
    }

    // Read file format version number
    Int16 versionNumber;
    IDataInput::Probe(in)->ReadInt16(&versionNumber);
    switch (versionNumber) {
        case 1:
            ReadFormatV1(in);
            break;
    }

    if (PROFILE_LOADING_SAVING) {
        Int64 end = SystemClock::GetElapsedRealtime();
        Int64 diff= end - start;
        String tmp = StringUtils::ToString(diff);
        String log = String("Loading gestures library = ") + tmp + String(" ms");
        Logger::D(GestureConstants::myLOG_TAG, log.string());
    }

    if (closeStream) {
        GestureUtils::CloseStream(ICloseable::Probe(in));
    }

    return NOERROR;
}

void GestureStore::ReadFormatV1(
    /* [in] */ IDataInputStream *in)
{
    AutoPtr<Learner> classifier = mClassifier;
    mNamedGestures.Clear();

    // Number of entries in the library
    Int32 entriesCount;
    IDataInput::Probe(in)->ReadInt32(&entriesCount);

    for (Int32 i = 0; i < entriesCount; i++) {
        // Entry name
        String name;
        IDataInput::Probe(in)->ReadUTF(&name);
        // Number of gestures
        Int32 gestureCount;
        IDataInput::Probe(in)->ReadInt32(&gestureCount);

        AutoPtr<GestureList> gestures = new GestureList();
        for (Int32 j = 0; j < gestureCount; j++) {
            AutoPtr<IGesture> gesture;
            CGesture::Deserialize(in, (IGesture**)&gesture);
            gestures->PushBack(gesture);
            AutoPtr<Instance> instance = Instance::CreateInstance(mSequenceType, mOrientationStyle,
                gesture, name);
            classifier->AddInstance(instance);
        }

        mNamedGestures[name] = gestures;
    }
}

AutoPtr<Learner> GestureStore::GetLearner()
{
    return mClassifier;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
