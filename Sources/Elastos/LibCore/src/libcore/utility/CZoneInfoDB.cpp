
#include "CZoneInfoDB.h"
#include "CSystem.h"
#include "CString.h"
#include "ZoneInfo.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/etl/List.h>
#include "io/CMemoryMappedFile.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Libcore::IO::IBufferIterator;
using Elastos::Utility::Etl::List;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::CMemoryMappedFile;
using Libcore::IO::IMemoryMappedFile;

namespace Libcore {
namespace Utility {

//===============================================================
// ZoneInfoDB
//===============================================================
CAR_SINGLETON_IMPL(CZoneInfoDB)

CAR_INTERFACE_IMPL(CZoneInfoDB, Object, IZoneInfoDB)

static AutoPtr<TzData> InitDATA()
{
    AutoPtr<CSystem> system;
    CSystem::AcquireSingletonByFriend((CSystem**)&system);
    String data, root;
    system->GetEnv(String("ANDROID_DATA"), &data);
    system->GetEnv(String("ANDROID_ROOT"), &root);
    data += "/misc/zoneinfo/tzdata";
    root += "/usr/share/zoneinfo/tzdata";

    AutoPtr<ArrayOf<String> > paths = ArrayOf<String>::Alloc(2);
    paths->Set(0, data);
    paths->Set(1, root);
    AutoPtr<TzData> tz = new TzData(paths);
    return tz;
}

INIT_PROI_4 AutoPtr<TzData> CZoneInfoDB::DATA = InitDATA();

AutoPtr<TzData> CZoneInfoDB::GetInstance()
{
    return DATA;
}

ECode CZoneInfoDB::GetInstance(
    /* [out] */ ITzData** data)
{
    VALIDATE_NOT_NULL(data)
    *data = DATA;
    REFCOUNT_ADD(*data)
    return NOERROR;
}

//===============================================================
// TzData::MyBasicLruCache
//===============================================================
TzData::MyBasicLruCache::MyBasicLruCache(TzData* host) {
    BasicLruCache::constructor(CACHE_SIZE);
    mHost = host;
}

AutoPtr<IZoneInfo> TzData::MyBasicLruCache::Create(
    /* [in] */ const String& id)
{
    AutoPtr<ICharSequence> csq;
    CString::New(id, (ICharSequence**)&csq);
    AutoPtr<IInterface> obj = Create(TO_IINTERFACE(csq));
    AutoPtr<IZoneInfo> zi = IZoneInfo::Probe(obj);
    return zi;
}

AutoPtr<IInterface> TzData::MyBasicLruCache::Create(
    /* [in] */ IInterface* key)
{
    // Work out where in the big data file this time zone is.
    AutoPtr<ICharSequence> csq = ICharSequence::Probe(key);
    if (csq == NULL)
    {
        return NULL;
    }
    String str;
    csq->ToString(&str);
    Int32 index = mHost->mIds->IndexOf(str);
    if (index < 0) {
      return NULL;
    }

    AutoPtr<IBufferIterator> it;
    mHost->mMappedFile->BigEndianIterator((IBufferIterator**)&it);
    it->Skip((*mHost->mByteOffsets)[index]);

    AutoPtr<IZoneInfo> rst = ZoneInfo::MakeTimeZone(str, it);
    return rst.Get();
}

//===============================================================
// TzData
//===============================================================
const Int32 TzData::CACHE_SIZE;

CAR_INTERFACE_IMPL(TzData, Object, ITzData)

TzData::TzData(
    /* [in] */ ArrayOf<String>* paths)
{
    mCache = new MyBasicLruCache(this);

    for (Int32 i = 0; i < paths->GetLength(); ++i) {
        if (LoadData((*paths)[i])) {
            return;
        }
    }

    // We didn't find any usable tzdata on disk, so let's just hard-code knowledge of "GMT".
    // This is actually implemented in TimeZone itself, so if this is the only time zone
    // we report, we won't be asked any more questions.
    ALOGE("Couldn't find any tzdata!");
    mVersion = String("missing");
    mZoneTab = String("# Emergency fallback data.\n");
    mIds = ArrayOf<String>::Alloc(1);
    mIds->Set(0, String("GMT"));
    mByteOffsets = ArrayOf<Int32>::Alloc(1);
    mRawUtcOffsetsCache = mByteOffsets;
}

Boolean TzData::LoadData(
    /* [in] */ const String& path)
{
      // try {
    mMappedFile = NULL;
    ECode ec = CMemoryMappedFile::MmapRO(path, (IMemoryMappedFile**)&mMappedFile);
    if (ec == (ECode)E_ERRNO_EXCEPTION) {
        return FALSE;
    }
    // } catch (ErrnoException errnoException) {
        // return false;
    // }

    // try {
    if (FAILED(ReadHeader())) {
        Logger::E("TzData", "tzdata file %s was present but invalid!", path.string());
        return FALSE;
    }
    else {
        return TRUE;
    }
    // } catch (Exception ex) {
        // Something's wrong with the file.
        // Log the problem and return false so we try the next choice.
        // return false;
    // }
}

ECode TzData::ReadHeader()
{
    assert(mMappedFile != NULL);

    // byte[12] tzdata_version  -- "tzdata2012f\0"
    // int index_offset
    // int data_offset
    // int zonetab_offset
    AutoPtr<IBufferIterator> it;
    mMappedFile->BigEndianIterator((IBufferIterator**)&it);

    AutoPtr<ArrayOf<Byte> > tzdata_version = ArrayOf<Byte>::Alloc(12);
    it->ReadByteArray(tzdata_version, 0, tzdata_version->GetLength());
    AutoPtr<ArrayOf<Char32> > charArray = ArrayOf<Char32>::Alloc(tzdata_version->GetLength());
    for (Int32 i = 0; i < tzdata_version->GetLength(); i++) {
        (*charArray)[i] = (*tzdata_version)[i];
    }
    String magic = String(*charArray, 0, 6);
    if (!magic.Equals(String("tzdata")) || (*tzdata_version)[11] != 0) {
        // throw new RuntimeException("bad tzdata magic: " + Arrays.toString(tzdata_version));
        return E_RUNTIME_EXCEPTION;
    }
    mVersion = String(*charArray, 6, 5);

    Int32 index_offset;
    it->ReadInt32(&index_offset);
    Int32 data_offset;
    it->ReadInt32(&data_offset);
    Int32 zonetab_offset;
    it->ReadInt32(&zonetab_offset);

    FAIL_RETURN(ReadIndex(it, index_offset, data_offset))
    Int64 size;
    mMappedFile->Size(&size);
    FAIL_RETURN(ReadZoneTab(it, zonetab_offset, (Int32)size - zonetab_offset))
    return NOERROR;
}

ECode TzData::ReadZoneTab(
    /* [in] */ IBufferIterator* it,
    /* [in] */ Int32 zoneTabOffset,
    /* [in] */ Int32 zoneTabSize)
{
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(zoneTabSize);
    FAIL_RETURN(it->Seek(zoneTabOffset))
    FAIL_RETURN(it->ReadByteArray(bytes, 0, bytes->GetLength()))
    AutoPtr<ArrayOf<Char32> > charArray = ArrayOf<Char32>::Alloc(bytes->GetLength());
    for (Int32 i = 0; i < bytes->GetLength(); i++) {
        (*charArray)[i] = (*bytes)[i];
    }
    mZoneTab = String(*charArray, 0, charArray->GetLength());
    return NOERROR;
}

ECode TzData::ReadIndex(
    /* [in] */ IBufferIterator* it,
    /* [in] */ Int32 indexOffset,
    /* [in] */ Int32 dataOffset)
{
    FAIL_RETURN(it->Seek(indexOffset))

    // The database reserves 40 bytes for each id.
    Int32 SIZEOF_TZNAME = 40;
    // The database uses 32-bit (4 byte) integers.
    Int32 SIZEOF_TZINT = 4;

    AutoPtr<ArrayOf<Byte> > idBytes = ArrayOf<Byte>::Alloc(SIZEOF_TZNAME);
    Int32 indexSize = (dataOffset - indexOffset);
    Int32 entryCount = indexSize / (SIZEOF_TZNAME + 3*SIZEOF_TZINT);

    AutoPtr<ArrayOf<Char32> > idChars = ArrayOf<Char32>::Alloc(entryCount * SIZEOF_TZNAME);
    AutoPtr<ArrayOf<Int32> > idEnd = ArrayOf<Int32>::Alloc(entryCount);

    Int32 idOffset = 0;

    mByteOffsets = ArrayOf<Int32>::Alloc(entryCount);

    for (Int32 i = 0; i < entryCount; i++) {
        FAIL_RETURN(it->ReadByteArray(idBytes, 0, idBytes->GetLength()))

        Int32 iValue;
        FAIL_RETURN(it->ReadInt32(&iValue))
        (*mByteOffsets)[i] = iValue;
        (*mByteOffsets)[i] += dataOffset; // TODO: change the file format so this is included.

        Int32 length;
        it->ReadInt32(&length);
        if (length < 44) {
            return E_ASSERTION_ERROR;
          // throw new AssertionError("length in index file < sizeof(tzhead)");
        }
        FAIL_RETURN(it->Skip(4)) // Skip the unused 4 bytes that used to be the raw offset.

        // Don't include null chars in the String
        Int32 len = idBytes->GetLength();
        for (Int32 j = 0; j < len; j++) {
          if ((*idBytes)[j] == 0) {
            break;
          }
          (*idChars)[idOffset++] = (Char32) ((*idBytes)[j] & 0xFF);
        }

        (*idEnd)[i] = idOffset;
    }

    // We create one string containing all the ids, and then break that into substrings.
    // This way, all ids share a single char[] on the heap.
    String allIds = String(*idChars, 0, idOffset);
    mIds = ArrayOf<String>::Alloc(entryCount);
    for (Int32 i = 0; i < entryCount; i++) {
        (*mIds)[i] = allIds.Substring(i == 0 ? 0 : (*idEnd)[i - 1], (*idEnd)[i]);
    }
    return NOERROR;
}

ECode TzData::GetAvailableIDs(
    /* [out, callee] */ ArrayOf<String>** ids)
{
    VALIDATE_NOT_NULL(ids)
    AutoPtr<ArrayOf<String> > clone = mIds->Clone();
    *ids = clone;
    return NOERROR;
}

AutoPtr<ArrayOf<String> > TzData::GetAvailableIDs()
{
    AutoPtr<ArrayOf<String> > clone = mIds->Clone();
    return clone;
}

ECode TzData::GetAvailableIDs(
    /* [in] */ Int32 rawUtcOffset,
    /* [out, callee] */ ArrayOf<String>** ids)
{
    VALIDATE_NOT_NULL(ids)

    List<String> matches;
    AutoPtr<ArrayOf<Int32> > rawUtcOffsets = GetRawUtcOffsets();
    for (Int32 i = 0; i < rawUtcOffsets->GetLength(); ++i) {
        if ((*rawUtcOffsets)[i] == rawUtcOffset) {
          matches.PushBack((*mIds)[i]);
        }
    }
    *ids = ArrayOf<String>::Alloc(matches.GetSize());
    List<String>::Iterator it = matches.Begin();
    for (Int32 i = 0; it != matches.End(); it++, i++) {
        (*ids)->Set(i, *it);
    }
    REFCOUNT_ADD(*ids)
    return NOERROR;
}

AutoPtr<ArrayOf<String> > TzData::GetAvailableIDs(
    /* [in] */ Int32 rawUtcOffset)
{
    AutoPtr<ArrayOf<String> > copy;
    GetAvailableIDs(rawUtcOffset, (ArrayOf<String>**)&copy);
    return copy;
}

ECode TzData::GetVersion(
    /* [out] */ String* version)
{
    VALIDATE_NOT_NULL(version)
    *version = mVersion;
    return NOERROR;
}

ECode TzData::GetZoneTab(
    /* [out] */ String* tab)
{
    VALIDATE_NOT_NULL(tab)
    *tab = mZoneTab;
    return NOERROR;
}

ECode TzData::MakeTimeZone(
    /* [in] */ const String& id,
    /* [out] */ IZoneInfo** zone)
{
    AutoPtr<ICharSequence> cId;
    CString::New(id, (ICharSequence**)&cId);
    AutoPtr<IInterface> tmp;
    mCache->Get(cId, (IInterface**)&tmp);
    AutoPtr<IZoneInfo> zoneInfo = IZoneInfo::Probe(tmp);
    // The object from the cache is cloned because TimeZone / ZoneInfo are mutable.
    if (zoneInfo == NULL) {
        *zone = NULL;
    } else {
        AutoPtr<ITimeZone> tmp;
        AutoPtr<ITimeZone> cloneObj = ITimeZone::Probe(zoneInfo);
        cloneObj->Clone((ITimeZone**)&tmp);
        *zone = IZoneInfo::Probe(tmp);
        REFCOUNT_ADD(*zone)
    }
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > TzData::GetRawUtcOffsets()
{
    if (mRawUtcOffsetsCache != NULL) {
        return mRawUtcOffsetsCache;
    }
    mRawUtcOffsetsCache = ArrayOf<Int32>::Alloc(mIds->GetLength());
    for (Int32 i = 0; i < mIds->GetLength(); ++i) {
        // This creates a TimeZone, which is quite expensive. Hence the cache.
        // Note that icu4c does the same (without the cache), so if you're
        // switching this code over to icu4j you should check its performance.
        // Telephony shouldn't care, but someone converting a bunch of calendar
        // events might.
        AutoPtr<ICharSequence> id;
        CString::New((*mIds)[i], (ICharSequence**)&id);
        AutoPtr<IInterface> tmp;
        mCache->Get(id, (IInterface**)&tmp);
        AutoPtr<ITimeZone> info = ITimeZone::Probe(tmp);
        if (info == NULL) {
            continue;
        }
        Int32 rawOffset;
        info->GetRawOffset(&rawOffset);
        (*mRawUtcOffsetsCache)[i] = rawOffset;
    }
    return mRawUtcOffsetsCache;
}

} // Utility
} // Libcore
