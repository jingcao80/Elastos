
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/mtp/CMtpPropertyGroup.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include "elastos/droid/mtp/CMtpPropertyList.h"
//TODO: Need provider/CMediaStoreFiles
// #include "elastos/droid/provider/CMediaStoreFiles.h"
//TODO: Need provider/CMediaStoreAudioMedia
// #include "elastos/droid/provider/CMediaStoreAudioMedia.h"
//TODO: Need provider/CMediaStoreAudioGenres
// #include "elastos/droid/provider/CMediaStoreAudioGenres.h"

#include <stdio.h>
#include <time.h>
#include <cutils/tztime.h>

using Elastos::IO::ICloseable;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Mtp::CMtpPropertyList;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IMediaStoreFilesFileColumns;
using Elastos::Droid::Provider::IMediaStoreFiles;
//TODO: Need provider/CMediaStoreFiles
// using Elastos::Droid::Provider::CMediaStoreFiles;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::Provider::IMediaStoreAudioAudioColumns;
using Elastos::Droid::Provider::IMediaStoreAudioPlaylistsColumns;
using Elastos::Droid::Provider::IMediaStoreImagesImageColumns;
using Elastos::Droid::Provider::IMediaStoreAudioGenresColumns;
using Elastos::Droid::Provider::IMediaStoreAudioMedia;
//TODO: Need provider/CMediaStoreAudioMedia
// using Elastos::Droid::Provider::CMediaStoreAudioMedia;
using Elastos::Droid::Provider::IMediaStoreAudioGenres;
//TODO: Need provider/CMediaStoreAudioGenres
// using Elastos::Droid::Provider::CMediaStoreAudioGenres;

namespace Elastos {
namespace Droid {
namespace Mtp {

const String CMtpPropertyGroup::TAG("MtpPropertyGroup");
const String CMtpPropertyGroup::ID_WHERE = IBaseColumns::ID + "=?";
const String CMtpPropertyGroup::FORMAT_WHERE = IMediaStoreFilesFileColumns::FORMAT + "=?";
const String CMtpPropertyGroup::ID_FORMAT_WHERE = CMtpPropertyGroup::ID_WHERE + " AND " + CMtpPropertyGroup::FORMAT_WHERE;
const String CMtpPropertyGroup::PARENT_WHERE = IMediaStoreFilesFileColumns::PARENT + "=?";
const String CMtpPropertyGroup::PARENT_FORMAT_WHERE = CMtpPropertyGroup::PARENT_WHERE + " AND " + CMtpPropertyGroup::FORMAT_WHERE;

CMtpPropertyGroup::Property::Property(
    /* [in] */ Int32 code,
    /* [in] */ Int32 type,
    /* [in] */ Int32 column)
    : mCode(code)
    , mType(type)
    , mColumn(column)
{}

CMtpPropertyGroup::CMtpPropertyGroup()
    : mVolumeName(NULL)
{}

CAR_INTERFACE_IMPL(CMtpPropertyGroup, Object, IMtpPropertyGroup)

CAR_OBJECT_IMPL(CMtpPropertyGroup)

ECode CMtpPropertyGroup::constructor(
    /* [in] */ IMtpDatabase* database,
    /* [in] */ IIContentProvider* provider,
    /* [in] */ const String& packageName,
    /* [in] */ const String& volume,
    /* [in] */ ArrayOf<Int32>* properties)
{
    mDatabase = database;
    mProvider = provider;
    mPackageName = packageName;
    mVolumeName = volume;
    AutoPtr<IMediaStoreFiles> files;
//TODO: Need provider/CMediaStoreFiles
    // CMediaStoreFiles::AcquireSingleton((IMediaStoreFiles**)&files);
    files->GetMtpObjectsUri(volume, (IUri**)&mUri);

    Int32 count = properties->GetLength();
    AutoPtr<List<String> > columns = new List<String>();
    columns->PushBack(IBaseColumns::ID);

    mProperties = ArrayOf<Property*>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        mProperties->Set(i, CreateProperty((*properties)[i], columns));
    }
    count = columns->GetSize();
    mColumns = ArrayOf<String>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        mColumns->Set(i, (*columns)[i]);
    }
    return NOERROR;
}

ECode CMtpPropertyGroup::GetPropertyList(
    /* [in] */ Int32 handle,
    /* [in] */ Int32 format,
    /* [in] */ Int32 depth,
    /* [out] */ IMtpPropertyList** list)
{
    //Log.d(TAG, "getPropertyList handle: " + handle + " format: " + format + " depth: " + depth);
    if (depth > 1) {
        // we only support depth 0 and 1
        // depth 0: single object, depth 1: immediate children
        AutoPtr<IMtpPropertyList> list;
        return CMtpPropertyList::New(0, IMtpConstants::RESPONSE_SPECIFICATION_BY_DEPTH_UNSUPPORTED, (IMtpPropertyList**)&list);
    }

    String where;
    AutoPtr<ArrayOf<String> > whereArgs;
    if (format == 0) {
        if (handle == 0xFFFFFFFF) {
            // select all objects
            where = NULL;
            whereArgs = NULL;
        }
        else {
            whereArgs = ArrayOf<String>::Alloc(1);
            whereArgs->Set(0, StringUtils::ToString(handle));
            if (depth == 1) {
                where = PARENT_WHERE;
            } else {
                where = ID_WHERE;
            }
        }
    }
    else {
        if (handle == 0xFFFFFFFF) {
            // select all objects with given format
            where = FORMAT_WHERE;
            whereArgs = ArrayOf<String>::Alloc(1);
            whereArgs->Set(0, StringUtils::ToString(format));
        }
        else {
            whereArgs = ArrayOf<String>::Alloc(2);
            whereArgs->Set(0, StringUtils::ToString(handle));
            whereArgs->Set(1, StringUtils::ToString(format));
            if (depth == 1) {
                where = PARENT_FORMAT_WHERE;
            }
            else {
                where = ID_FORMAT_WHERE;
            }
        }
    }

    AutoPtr<ICursor> c = NULL;
    //try {
        // don't query if not necessary
        if (depth > 0 || handle == 0xFFFFFFFF || mColumns->GetLength() > 1) {
            mProvider->Query(mPackageName, mUri, mColumns, where, whereArgs, String(NULL), NULL, (ICursor**)&c);
            if (c == NULL) {
                return CMtpPropertyList::New(0, IMtpConstants::RESPONSE_INVALID_OBJECT_HANDLE, (IMtpPropertyList**)&list);
            }
        }

        Int32 count;
        if(c == NULL) {
            count = 1;
        }
        else {
            c->GetCount(&count);
        }
        AutoPtr<IMtpPropertyList> result;
        CMtpPropertyList::New(count * mProperties->GetLength(), IMtpConstants::RESPONSE_OK, (IMtpPropertyList**)&result);

        // iterate over all objects in the query
        for (Int32 objectIndex = 0; objectIndex < count; objectIndex++) {
            if (c != NULL) {
                Boolean b;
                c->MoveToNext(&b);
                c->GetInt64(0, (Int64*)&handle);
            }

            // iterate over all properties in the query for the given object
            for (Int32 propertyIndex = 0; propertyIndex < mProperties->GetLength(); propertyIndex++) {
                AutoPtr<Property> property = (*mProperties)[propertyIndex];
                Int32 propertyCode = property->mCode;
                Int32 column = property->mColumn;

                String str;
                String name;
                String dateTime;
                String genre;
                Int32 value;
                Int32 year;
                Int64 puid;
                Int64 l;
                // handle some special cases
                switch (propertyCode) {
                    case IMtpConstants::PROPERTY_PROTECTION_STATUS:
                        // protection status is always 0
                        result->Append(handle, propertyCode, IMtpConstants::TYPE_UINT16, 0);
                        break;
                    case IMtpConstants::PROPERTY_OBJECT_FILE_NAME:
                        // special case - need to extract file name from full path
                        c->GetString(column, &str);
                        if (str != NULL) {
                            result->Append(handle, propertyCode, NameFromPath(str));
                        } else {
                            result->SetResult(IMtpConstants::RESPONSE_INVALID_OBJECT_HANDLE);
                        }
                        break;
                    case IMtpConstants::PROPERTY_NAME:
                        // first try title
                        c->GetString(column, &name);
                        // then try name
                        if (name == NULL) {
                            name = QueryString(handle, IMediaStoreAudioPlaylistsColumns::NAME);
                        }
                        // if title and name fail, extract name from full path
                        if (name == NULL) {
                            name = QueryString(handle, IMediaStoreMediaColumns::DATA);
                            if (name != NULL) {
                                name = NameFromPath(name);
                            }
                        }
                        if (name != NULL) {
                            result->Append(handle, propertyCode, name);
                        } else {
                            result->SetResult(IMtpConstants::RESPONSE_INVALID_OBJECT_HANDLE);
                        }
                        break;
                    case IMtpConstants::PROPERTY_DATE_MODIFIED:
                    case IMtpConstants::PROPERTY_DATE_ADDED:
                        // convert from seconds to DateTime
                        c->GetInt32(column, &value);
                        result->Append(handle, propertyCode, NativeFormatDateTime(value));
                        break;
                    case IMtpConstants::PROPERTY_ORIGINAL_RELEASE_DATE:
                        // release date is stored internally as just the year
                        c->GetInt32(column, &year);
                        dateTime = StringUtils::ToString(year) + "0101T000000";
                        result->Append(handle, propertyCode, dateTime);
                        break;
                    case IMtpConstants::PROPERTY_PERSISTENT_UID:
                        // PUID is concatenation of storageID and object handle
                        c->GetInt64(column, &puid);
                        puid <<= 32;
                        puid += handle;
                        result->Append(handle, propertyCode, IMtpConstants::TYPE_UINT128, puid);
                        break;
                    case IMtpConstants::PROPERTY_TRACK:
                        c->GetInt32(column, &value);
                        value = value % 1000;
                        result->Append(handle, propertyCode, IMtpConstants::TYPE_UINT16, value);
                        break;
                    case IMtpConstants::PROPERTY_ARTIST:
                        result->Append(handle, propertyCode,
                                QueryAudio(handle, IMediaStoreAudioAudioColumns::ARTIST));
                        break;
                    case IMtpConstants::PROPERTY_ALBUM_NAME:
                        result->Append(handle, propertyCode,
                                QueryAudio(handle, IMediaStoreAudioAudioColumns::ALBUM));
                        break;
                    case IMtpConstants::PROPERTY_GENRE:
                        genre = QueryGenre(handle);
                        if (genre != NULL) {
                            result->Append(handle, propertyCode, genre);
                        } else {
                            result->SetResult(IMtpConstants::RESPONSE_INVALID_OBJECT_HANDLE);
                        }
                        break;
                    default:
                        if (property->mType == IMtpConstants::TYPE_STR) {
                            c->GetString(column, &str);
                            result->Append(handle, propertyCode, str);
                        } else if (property->mType == IMtpConstants::TYPE_UNDEFINED) {
                            result->Append(handle, propertyCode, property->mType, 0);
                        } else {
                            c->GetInt64(column, &l);
                            result->Append(handle, propertyCode, property->mType, l);
                        }
                        break;
                }
            }
        }

        *list = result;
        REFCOUNT_ADD(*list);
        return NOERROR;
    //} catch (RemoteException e) {
    //  return new MtpPropertyList(0, IMtpConstants::RESPONSE_GENERAL_ERROR);
    //} finally {
    //    if (c != NULL) {
    //        c.close();
    //    }
    //}
    // impossible to get here, so no return statement
}

AutoPtr<Elastos::Droid::Mtp::CMtpPropertyGroup::Property> CMtpPropertyGroup::CreateProperty(
    /* [in] */ Int32 code,
    /* [in] */ List<String>* columns)
{
    String column;
    Int32 type;

     switch (code) {
        case IMtpConstants::PROPERTY_STORAGE_ID:
            column = IMediaStoreFilesFileColumns::STORAGE_ID;
            type = IMtpConstants::TYPE_UINT32;
            break;
        case IMtpConstants::PROPERTY_OBJECT_FORMAT:
            column = IMediaStoreFilesFileColumns::FORMAT;
            type = IMtpConstants::TYPE_UINT16;
            break;
        case IMtpConstants::PROPERTY_PROTECTION_STATUS:
            // protection status is always 0
            type = IMtpConstants::TYPE_UINT16;
            break;
        case IMtpConstants::PROPERTY_OBJECT_SIZE:
            column = IMediaStoreMediaColumns::SIZE;
            type = IMtpConstants::TYPE_UINT64;
            break;
        case IMtpConstants::PROPERTY_OBJECT_FILE_NAME:
            column = IMediaStoreMediaColumns::DATA;
            type = IMtpConstants::TYPE_STR;
            break;
        case IMtpConstants::PROPERTY_NAME:
            column = IMediaStoreMediaColumns::TITLE;
            type = IMtpConstants::TYPE_STR;
            break;
        case IMtpConstants::PROPERTY_DATE_MODIFIED:
            column = IMediaStoreMediaColumns::DATE_MODIFIED;
            type = IMtpConstants::TYPE_STR;
            break;
        case IMtpConstants::PROPERTY_DATE_ADDED:
            column = IMediaStoreMediaColumns::DATE_ADDED;
            type = IMtpConstants::TYPE_STR;
            break;
        case IMtpConstants::PROPERTY_ORIGINAL_RELEASE_DATE:
            column = IMediaStoreAudioAudioColumns::YEAR;
            type = IMtpConstants::TYPE_STR;
            break;
        case IMtpConstants::PROPERTY_PARENT_OBJECT:
            column = IMediaStoreFilesFileColumns::PARENT;
            type = IMtpConstants::TYPE_UINT32;
            break;
        case IMtpConstants::PROPERTY_PERSISTENT_UID:
            // PUID is concatenation of storageID and object handle
            column = IMediaStoreFilesFileColumns::STORAGE_ID;
            type = IMtpConstants::TYPE_UINT128;
            break;
        case IMtpConstants::PROPERTY_DURATION:
            column = IMediaStoreAudioAudioColumns::DURATION;
            type = IMtpConstants::TYPE_UINT32;
            break;
        case IMtpConstants::PROPERTY_TRACK:
            column = IMediaStoreAudioAudioColumns::TRACK;
            type = IMtpConstants::TYPE_UINT16;
            break;
        case IMtpConstants::PROPERTY_DISPLAY_NAME:
            column = IMediaStoreMediaColumns::DISPLAY_NAME;
            type = IMtpConstants::TYPE_STR;
            break;
        case IMtpConstants::PROPERTY_ARTIST:
            type = IMtpConstants::TYPE_STR;
            break;
        case IMtpConstants::PROPERTY_ALBUM_NAME:
            type = IMtpConstants::TYPE_STR;
            break;
        case IMtpConstants::PROPERTY_ALBUM_ARTIST:
            column = IMediaStoreAudioAudioColumns::ALBUM_ARTIST;
            type = IMtpConstants::TYPE_STR;
            break;
        case IMtpConstants::PROPERTY_GENRE:
            // genre requires a special query
            type = IMtpConstants::TYPE_STR;
            break;
        case IMtpConstants::PROPERTY_COMPOSER:
            column = IMediaStoreAudioAudioColumns::COMPOSER;
            type = IMtpConstants::TYPE_STR;
            break;
        case IMtpConstants::PROPERTY_DESCRIPTION:
            column = IMediaStoreImagesImageColumns::DESCRIPTION;
            type = IMtpConstants::TYPE_STR;
            break;
        default:
            type = IMtpConstants::TYPE_UNDEFINED;
            Logger::E(TAG, "unsupported property %d", code);
            break;
        }

    if (column != NULL) {
        columns->PushBack(column);
        AutoPtr<Property> p = new Property(code, type, columns->GetSize() - 1);
        return p;
    }
    else {
        AutoPtr<Property> p = new Property(code, type, -1);
        return p;
    }
}

String CMtpPropertyGroup::QueryString(
        /* [in] */ Int32 id,
        /* [in] */ const String& column)
{
    AutoPtr<ICursor> c;
    //try {
        // for now we are only reading properties from the "objects" table
        AutoPtr<ArrayOf<String> > s1 = ArrayOf<String>::Alloc(2);
        s1->Set(0, IBaseColumns::ID);
        s1->Set(1, column);
        AutoPtr<ArrayOf<String> > s2 = ArrayOf<String>::Alloc(1);
        s1->Set(0, StringUtils::ToString(id));
        mProvider->Query(mPackageName, mUri, s1, ID_WHERE, s2, String(NULL), NULL, (ICursor**)&c);
        Boolean b;
        if (c != NULL && (c->MoveToNext(&b),b)) {
            String s;
            c->GetString(1, &s);
            return s;
        }
        else {
            return String("");
        }
    //} catch (Exception e) {
    //    return NULL;
    //} finally {
    //    if (c != NULL) {
    //        c.close();
    //    }
    //}
}

String CMtpPropertyGroup::QueryAudio(
        /* [in] */ Int32 id,
        /* [in] */ const String& column)
{
    AutoPtr<ICursor> c;
    //try {
        AutoPtr<ArrayOf<String> > s1 = ArrayOf<String>::Alloc(2);
        s1->Set(0, IBaseColumns::ID);
        s1->Set(1, column);
        AutoPtr<ArrayOf<String> > s2 = ArrayOf<String>::Alloc(1);
        s1->Set(0, StringUtils::ToString(id));
        AutoPtr<IMediaStoreAudioMedia> am;
//TODO: Need provider/CMediaStoreAudioMedia
        // CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&am);
        AutoPtr<IUri> uri;
        am->GetContentUri(mVolumeName, (IUri**)&uri);
        mProvider->Query(mPackageName, uri, s1, ID_WHERE, s2, String(NULL), NULL, (ICursor**)&c);
        Boolean b;
        if (c != NULL && (c->MoveToNext(&b),b)) {
            String s;
            c->GetString(1, &s);
            return s;
        }
        else {
            return String("");
        }
    //} catch (Exception e) {
    //    return NULL;
    //} finally {
    //    if (c != NULL) {
    //        c.close();
    //    }
    //}
}

String CMtpPropertyGroup::QueryGenre(
        /* [in] */ Int32 id)
{
    AutoPtr<ICursor> c;
    //try {
        AutoPtr<IUri> uri;
        AutoPtr<IMediaStoreAudioGenres> ag;
//TODO: Need provider/CMediaStoreAudioGenres
        // CMediaStoreAudioGenres::AcquireSingleton((IMediaStoreAudioGenres**)&ag);
        ag->GetContentUriForAudioId(mVolumeName, id, (IUri**)&uri);
        AutoPtr<ArrayOf<String> > s1 = ArrayOf<String>::Alloc(2);
        s1->Set(0, IBaseColumns::ID);
        s1->Set(1, IMediaStoreAudioGenresColumns::NAME);
        mProvider->Query(mPackageName, uri, s1, String(NULL), NULL, String(NULL), NULL, (ICursor**)&c);
        Boolean b;
        if (c != NULL && (c->MoveToNext(&b),b)) {
            String s;
            c->GetString(1, &s);
            return s;
        }
        else {
            return String("");
        }
    //} catch (Exception e) {
    //    Log.e(TAG, "queryGenre exception", e);
    //    return NULL;
    //} finally {
    //    if (c != NULL) {
    //        c.close();
    //    }
    //}
}

Int64 CMtpPropertyGroup::QueryInt64(
        /* [in] */ Int32 id,
        /* [in] */ const String& column)
{
    AutoPtr<ICursor> c;
    //try {
        // for now we are only reading properties from the "objects" table
        AutoPtr<ArrayOf<String> > s1 = ArrayOf<String>::Alloc(2);
        s1->Set(0, IBaseColumns::ID);
        s1->Set(1, column);
        AutoPtr<ArrayOf<String> > s2 = ArrayOf<String>::Alloc(1);
        s1->Set(0, StringUtils::ToString(id));
        mProvider->Query(mPackageName, mUri, s1, ID_WHERE, s2, String(NULL), NULL, (ICursor**)&c);
        Boolean b;
        if (c != NULL && (c->MoveToNext(&b),b)) {
            Int64 value;
            c->GetInt64(1, &value);
            ICloseable::Probe(c)->Close();
            return value;
        }
    //} catch (Exception e) {
    //} finally {
        if (c != NULL) {
            ICloseable::Probe(c)->Close();
        }
    //}
    return 0;
}

String CMtpPropertyGroup::NameFromPath(
        /* [in] */ const String& path)
{
    // extract name from full path
    Int32 start = 0;
    Int32 lastSlash = path.LastIndexOf('/');
    if (lastSlash >= 0) {
        start = lastSlash + 1;
    }
    Int32 end = path.GetLength();
    if (end - start > 255) {
        end = start + 255;
    }
    return path.Substring(start, end);
}

String CMtpPropertyGroup::NativeFormatDateTime(
        /* [in] */ Int64 seconds)
{
    char date[20];
    //// FormatDateTime(seconds, date, Sizeof(date));

    struct tm tm;
    localtime_r((time_t*)&seconds, &tm);
    snprintf(date, sizeof(date), "%04d%02d%02dT%02d%02d%02d",
        tm.tm_year + 1900,
        tm.tm_mon + 1, // localtime_r uses months in 0 - 11 range
        tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return String(date);
}

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
