
#include "elastos/droid/media/CRingtoneManager.h"

#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/provider/CMediaStoreAudioMedia.h"
#include "elastos/droid/os/Environment.h"
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/media/CRingtone.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Provider::IMediaStoreAudioAudioColumns;
using Elastos::Droid::Provider::IMediaStoreAudioMedia;
using Elastos::Droid::Provider::CMediaStoreAudioMedia;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
//using Elastos::Droid::Provider::IDrmStoreAudio;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

const String CRingtoneManager::TAG("RingtoneManager");

static AutoPtr<ArrayOf<String> > InitINTERNAL_COLUMNS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(4);
    array->Set(0, IMediaStoreAudioMedia::ID);
    array->Set(1, IMediaStoreAudioMedia::TITLE);

    AutoPtr<IMediaStoreAudioMedia> am;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&am);
    String internalContentUri;
    AutoPtr<IUri> uri;
    am->GetINTERNAL_CONTENT_URI((IUri**)&uri);
    uri->ToString(&internalContentUri);

    String info;
    info.AppendFormat("\"%s\"", internalContentUri.string());
    array->Set(2, info);
    array->Set(3, IMediaStoreAudioMedia::TITLE_KEY);
    return array;
}

static AutoPtr<ArrayOf<String> > InitDRM_COLUMNS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(4);
    // array->Set(0, IDrmStoreAudio::ID);
    // array->Set(1, IDrmStoreAudio::TITLE);
    // String info;
    // info.Format("\"%s\"", IDrmStoreAudio::CONTENT_URI.string());
    // array->Set(2, info);
    // array->Set(3, IDrmStoreAudio::TITLE_KEY);
    return array;
}

static AutoPtr<ArrayOf<String> > InitMEDIA_COLUMNS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(4);
    array->Set(0, IMediaStoreAudioMedia::ID);
    array->Set(1, IMediaStoreAudioMedia::TITLE);

    AutoPtr<IMediaStoreAudioMedia> am;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&am);
    String externalContentUri;
    AutoPtr<IUri> uri;
    am->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
    uri->ToString(&externalContentUri);

    String info;
    info.AppendFormat("\"%s\"", externalContentUri.string());
    array->Set(2, info);
    array->Set(3, IMediaStoreAudioMedia::TITLE_KEY);
    return array;
}

AutoPtr< ArrayOf<String> > CRingtoneManager::INTERNAL_COLUMNS = InitINTERNAL_COLUMNS();
AutoPtr< ArrayOf<String> > CRingtoneManager::DRM_COLUMNS = InitDRM_COLUMNS();
AutoPtr< ArrayOf<String> > CRingtoneManager::MEDIA_COLUMNS = InitMEDIA_COLUMNS();

CRingtoneManager::CRingtoneManager()
    : mType(TYPE_RINGTONE)
    , mStopPreviousRingtone(TRUE)
{
}

ECode CRingtoneManager::constructor(
    /* [in] */ IActivity* activity)
{
    mContext = mActivity = activity;
    SetType(mType);
    return NOERROR;
}

ECode CRingtoneManager::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    SetType(mType);
    return NOERROR;
}

ECode CRingtoneManager::SetType(
    /* [in] */ Int32 type)
{
    if (mCursor != NULL) {
        Logger::E(TAG, "Setting filter columns should be done before querying for ringtones.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mType = type;
    SetFilterColumnsList(type);

    return NOERROR;
}

ECode CRingtoneManager::InferStreamType(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);

    switch (mType) {
        case TYPE_ALARM:
        {
            *result = IAudioManager::STREAM_ALARM;
            return NOERROR;
        }
        case TYPE_NOTIFICATION:
        {
            *result = IAudioManager::STREAM_NOTIFICATION;
            return NOERROR;
        }
        default:
        {
            *result = IAudioManager::STREAM_RING;
            return NOERROR;
        }
    }
}

ECode CRingtoneManager::SetStopPreviousRingtone(
    /* [in] */ Boolean stopPreviousRingtone)
{
    mStopPreviousRingtone = stopPreviousRingtone;
    return NOERROR;
}

ECode CRingtoneManager::GetStopPreviousRingtone(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mStopPreviousRingtone;
    return NOERROR;
}

ECode CRingtoneManager::StopPreviousRingtone()
{
    if (mPreviousRingtone != NULL) {
        mPreviousRingtone->Stop();
    }
    return NOERROR;
}

ECode CRingtoneManager::GetIncludeDrm(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIncludeDrm;
    return NOERROR;
}

ECode CRingtoneManager::SetIncludeDrm(
    /* [in] */ Boolean includeDrm)
{
    mIncludeDrm = includeDrm;

    return NOERROR;
}

ECode CRingtoneManager::GetCursor(
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);

    Boolean tempState;
    if (mCursor != NULL && (mCursor->Requery(&tempState), tempState)) {
        *result = mCursor;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<ICursor> internalCursor = GetInternalRingtones();
    AutoPtr<ICursor> drmCursor = mIncludeDrm ? GetDrmRingtones() : NULL;
    AutoPtr<ICursor> mediaCursor = GetMediaRingtones();
/*Eddie(E_NOT_IMPLEMENTED)*/
//    mCursor = new SortCursor(new Cursor[] { internalCursor, drmCursor, mediaCursor },
//            IMediaStoreAudioMedia::DEFAULT_SORT_ORDER);

    return NOERROR;
}

ECode CRingtoneManager::GetRingtone(
    /* [in] */ Int32 position,
    /* [out] */ IRingtone** result)
{
    VALIDATE_NOT_NULL(result);

    if (mStopPreviousRingtone && mPreviousRingtone != NULL) {
        mPreviousRingtone->Stop();
    }

    AutoPtr<IUri> uri;
    GetRingtoneUri(position, (IUri**)&uri);
    Int32 tempValue;
    InferStreamType(&tempValue);
    GetRingtone(mContext, uri, (IRingtone**)&mPreviousRingtone);
    *result = mPreviousRingtone;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRingtoneManager::GetRingtoneUri(
    /* [in] */ Int32 position,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);

    // use cursor directly instead of requerying it, which could easily
    // cause position to shuffle.
    Boolean tempState;
    if (mCursor == NULL || !(mCursor->MoveToPosition(position, &tempState), tempState)) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IUri> temp = GetUriFromCursor(mCursor);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRingtoneManager::GetRingtonePosition(
    /* [in] */ IUri* ringtoneUri,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (ringtoneUri == NULL) {
        *result = -1;
        return NOERROR;
    }

    AutoPtr<ICursor> cursor;
    GetCursor((ICursor**)&cursor);
    Int32 cursorCount;
    cursor->GetCount(&cursorCount);

    Boolean tempState;
    cursor->MoveToFirst(&tempState);
    if (!tempState) {
        *result = -1;
        return NOERROR;
    }

    // Only create Uri objects when the actual URI changes
    AutoPtr<IUri> currentUri = NULL;
    String previousUriString; // = NULL;
    for (Int32 i = 0; i < cursorCount; i++) {
        String uriString;
        cursor->GetString(URI_COLUMN_INDEX, &uriString);
        if (currentUri == NULL || !uriString.Equals(previousUriString)) {
            AutoPtr<IUriHelper> uriHelper;
            CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
            currentUri = NULL;
            uriHelper->Parse(uriString, (IUri**)&currentUri);
        }

        AutoPtr<IContentUris> contentUris;
        Int64 tempValue;
        cursor->GetInt64(ID_COLUMN_INDEX, &tempValue);
        AutoPtr<IUri> uri;
        contentUris->WithAppendedId(currentUri, tempValue, (IUri**)&uri);
        ringtoneUri->Equals(uri, &tempState);
        if (tempState) {
            *result = i;
            return NOERROR;
        }

        cursor->Move(1, &tempState);

        previousUriString = uriString;
    }

    *result = -1;
    return NOERROR;
}

/*static*/
ECode CRingtoneManager::GetValidRingtoneUri(
    /* [in] */ IContext* context,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<CRingtoneManager> rm;
    CRingtoneManager::NewByFriend(context, (CRingtoneManager**)&rm);

    AutoPtr<IUri> uri = GetValidRingtoneUriFromCursorAndClose(context, rm->GetInternalRingtones());

    if (uri == NULL) {
        uri = GetValidRingtoneUriFromCursorAndClose(context, rm->GetMediaRingtones());
    }

    if (uri == NULL) {
        uri = GetValidRingtoneUriFromCursorAndClose(context, rm->GetDrmRingtones());
    }
    *result = uri;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*static*/
ECode CRingtoneManager::GetRingtone(
    /* [in] */ IContext* context,
    /* [in] */ IUri* ringtoneUri,
    /* [out] */ IRingtone** result)
{
    VALIDATE_NOT_NULL(result);

    // Don't set the stream type
    AutoPtr<IRingtone> temp = GetRingtone(context, ringtoneUri, -1);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*static*/
ECode CRingtoneManager::GetActualDefaultRingtoneUri(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result);

    String setting = GetSettingForType(type);
    if (setting == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    String uriString;
    FAIL_RETURN(Settings::System::GetString(contentResolver, setting, &uriString))
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> uri;
    uriHelper->Parse(uriString, (IUri**)&uri);

    *result = !uriString.IsNull() ? uri : NULL;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*static*/
ECode CRingtoneManager::SetActualDefaultRingtoneUri(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [in] */ IUri* ringtoneUri)
{
    String setting = GetSettingForType(type);
    if (setting == NULL) {
        return NOERROR;
    }
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    String tempText, tempNull;
    ringtoneUri->ToString(&tempText);
    Boolean restlt;
    return Settings::System::PutString(contentResolver, setting,
            ringtoneUri != NULL ? tempText : tempNull, &restlt);
}

/*static*/
ECode CRingtoneManager::IsDefault(
    /* [in] */ IUri* ringtoneUri,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 tempValue;
    GetDefaultType(ringtoneUri, &tempValue);
    *result =  tempValue != -1;
    return NOERROR;
}

/*static*/
ECode CRingtoneManager::GetDefaultType(
    /* [in] */ IUri* defaultRingtoneUri,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUri> uri;
    Boolean tempState;
    if (defaultRingtoneUri == NULL) {
        *result = -1;
    } else if (defaultRingtoneUri->Equals(Settings::System::DEFAULT_RINGTONE_URI, &tempState), tempState) {
        *result = TYPE_RINGTONE;
    } else if (defaultRingtoneUri->Equals(Settings::System::DEFAULT_NOTIFICATION_URI, &tempState), tempState) {
        *result = TYPE_NOTIFICATION;
    } else if (defaultRingtoneUri->Equals(Settings::System::DEFAULT_ALARM_ALERT_URI, &tempState), tempState) {
        *result = TYPE_ALARM;
    } else {
        *result = -1;
    }
    return NOERROR;
}

/*static*/
ECode CRingtoneManager::GetDefaultUri(
    /* [in] */ Int32 type,
    /* [out] */ IUri ** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUri> uri;
    if ((type & TYPE_RINGTONE) != 0) {
        *result = Settings::System::DEFAULT_RINGTONE_URI;
    } else if ((type & TYPE_NOTIFICATION) != 0) {
        *result = Settings::System::DEFAULT_NOTIFICATION_URI;
    } else if ((type & TYPE_ALARM) != 0) {
        *result = Settings::System::DEFAULT_ALARM_ALERT_URI;
    } else {
        *result = NULL;
    }
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*static*/
AutoPtr<IUri> CRingtoneManager::GetUriFromCursor(
    /* [in] */ ICursor* cursor)
{
    String tempText;
    cursor->GetString(URI_COLUMN_INDEX, &tempText);
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> uri;
    uriHelper->Parse(tempText, (IUri**)&uri);

    Int64 tempValue;
    cursor->GetInt64(ID_COLUMN_INDEX, &tempValue);
    AutoPtr<IContentUris> contentUris;
    AutoPtr<IUri> tempUri;
    contentUris->WithAppendedId(uri, tempValue, (IUri**)&tempUri);

    return tempUri;
}

/*static*/
AutoPtr<IUri> CRingtoneManager::GetValidRingtoneUriFromCursorAndClose(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IUri> uri;
    if (cursor != NULL) {
        Boolean tempState;
        cursor->MoveToFirst(&tempState);
        if (tempState) {
            uri = GetUriFromCursor(cursor);
        }
        cursor->Close();
    }

    return uri;
}

AutoPtr<ICursor> CRingtoneManager::GetInternalRingtones()
{
    AutoPtr<IMediaStoreAudioMedia> am;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&am);
    AutoPtr<IUri> uri;
    am->GetINTERNAL_CONTENT_URI((IUri**)&uri);
    return Query(uri, INTERNAL_COLUMNS,
        ConstructBooleanTrueWhereClause(&mFilterColumns, mIncludeDrm),
        NULL, IMediaStoreAudioMedia::DEFAULT_SORT_ORDER);
}

AutoPtr<ICursor> CRingtoneManager::GetDrmRingtones()
{
    assert(0);
    // DRM store does not have any columns to use for filtering
    // String nullStr;
    // return Query(IDrmStoreAudio::CONTENT_URI, DRM_COLUMNS, nullStr, NULL, IDrmStoreAudio::TITLE);
    return NULL;
}

AutoPtr<ICursor> CRingtoneManager::GetMediaRingtones()
{
     // Get the external media cursor. First check to see if it is mounted.
    String status = Environment::GetExternalStorageState();
    AutoPtr<IMediaStoreAudioMedia> am;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&am);
    AutoPtr<IUri> uri;
    am->GetEXTERNAL_CONTENT_URI((IUri**)&uri);

    return (status.Equals(IEnvironment::MEDIA_MOUNTED) || status.Equals(IEnvironment::MEDIA_MOUNTED_READ_ONLY))
        ? Query(uri, MEDIA_COLUMNS,
            ConstructBooleanTrueWhereClause(&mFilterColumns, mIncludeDrm), NULL,
            IMediaStoreAudioMedia::DEFAULT_SORT_ORDER)
        : NULL;
}

void CRingtoneManager::SetFilterColumnsList(
    /* [in] */ Int32 type)
{
    List<String> columns(mFilterColumns);
    mFilterColumns.Clear();

    if ((type & TYPE_RINGTONE) != 0) {
        columns.PushBack(IMediaStoreAudioAudioColumns::IS_RINGTONE);
    }

    if ((type & TYPE_NOTIFICATION) != 0) {
        columns.PushBack(IMediaStoreAudioAudioColumns::IS_NOTIFICATION);
    }

    if ((type & TYPE_ALARM) != 0) {
        columns.PushBack(IMediaStoreAudioAudioColumns::IS_ALARM);
    }
}

/*static*/
String CRingtoneManager::ConstructBooleanTrueWhereClause(
    /* [in] */ List<String>* columns,
    /* [in] */ Boolean includeDrm)
{
    String tempNull;
    if (columns == NULL) {
        return tempNull;
    }

    StringBuilder sb;
    sb += "(";

    for (Int32 i = columns->GetSize() - 1; i >= 0; i--) {
        sb += (*columns)[i];
        sb += "=1 or ";
    }

    if (columns->IsEmpty() == FALSE) {
        // Remove last ' or '
        Int32 tempValue;
        sb.GetLength(&tempValue);
        String tempText;
        sb.Substring(tempValue - 4, &tempText);
        sb.Reset();
        sb += tempText;
    }

    sb += ")";

    if (!includeDrm) {
        // If not DRM files should be shown, the where clause
        // will be something like "(is_notification=1) and is_drm=0"
        sb += " and ";
        sb += IMediaStoreMediaColumns::IS_DRM;
        sb += "=0";
    }
    String tempText;
    sb.ToString(&tempText);
    return tempText;
}

AutoPtr<ICursor> CRingtoneManager::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder)
{
    AutoPtr<ICursor> cursor;
    if (mActivity != NULL) {
        mActivity->ManagedQuery(uri, projection, selection, selectionArgs, sortOrder, (ICursor**)&cursor);
    }
    else {
        AutoPtr<IContentResolver> contentResolver;
        mContext->GetContentResolver((IContentResolver**)&contentResolver);
        contentResolver->Query(uri, projection, selection, selectionArgs, sortOrder, (ICursor**)&cursor);
    }
    return cursor;
}

/*static*/
AutoPtr<IRingtone> CRingtoneManager::GetRingtone(
    /* [in] */ IContext* context,
    /* [in] */ IUri* ringtoneUri,
    /* [in] */ Int32 streamType)
{
//    try {
    AutoPtr<IRingtone> r;
    ECode ec = CRingtone::New(context, TRUE, (IRingtone**)&r);
    if (FAILED(ec)) {
        goto exception;
    }
    if (streamType >= 0) {
        ec = r->SetStreamType(streamType);
        if (FAILED(ec)) {
            goto exception;
        }
    }
    ec = r->SetUri(ringtoneUri);
    FAIL_GOTO(ec, exception)

    return r;
//    } catch (Exception ex) {
exception:
        String tempText;
        ringtoneUri->ToString(&tempText);
        Logger::E(TAG, "Failed to open ringtone %s : %08x", tempText.string(), ec);
//    }
    return NULL;
}

/*static*/
String CRingtoneManager::GetSettingForType(
    /* [in] */ Int32 type)
{
    if ((type & TYPE_RINGTONE) != 0) {
        return ISettingsSystem::RINGTONE;
    } else if ((type & TYPE_NOTIFICATION) != 0) {
        return ISettingsSystem::NOTIFICATION_SOUND;
    } else if ((type & TYPE_ALARM) != 0) {
        return ISettingsSystem::ALARM_ALERT;
    } else {
        return String(NULL);
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
