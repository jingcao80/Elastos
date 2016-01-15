
#include "elastos/droid/media/CExifInterface.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.CoreLibrary.h>

#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <utils/Log.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <jhead.h>

#ifdef __cplusplus
}
#endif

using Elastos::Text::CParsePosition;
using Elastos::Text::IParsePosition;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::IDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::IDate;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {

//======================================================================================
// Statics
//======================================================================================
static int attributeCount;      // keep track of how many attributes we've added

static int loadExifInfo(const char* FileName, int readJPG) {
#ifdef SUPERDEBUG
    ALOGE("loadExifInfo");
#endif
    int Modified = FALSE;
    ReadMode_t ReadMode = (ReadMode_t)READ_METADATA;
    if (readJPG) {
        // Must add READ_IMAGE else we can't write the JPG back out.
        ReadMode = (ReadMode_t)(READ_METADATA | READ_IMAGE);
    }

#ifdef SUPERDEBUG
    ALOGE("ResetJpgfile");
#endif
    ResetJpgfile();

    // Start with an empty image information structure.
    memset(&ImageInfo, 0, sizeof(ImageInfo));
    ImageInfo.FlashUsed = -1;
    ImageInfo.MeteringMode = -1;
    ImageInfo.Whitebalance = -1;

    // Store file date/time.
    {
        struct stat st;
        if (stat(FileName, &st) >= 0) {
            ImageInfo.FileDateTime = st.st_mtime;
            ImageInfo.FileSize = st.st_size;
        }
    }

    strncpy(ImageInfo.FileName, FileName, PATH_MAX);
#ifdef SUPERDEBUG
    ALOGE("ReadJpegFile");
#endif
    return ReadJpegFile(FileName, ReadMode);
}

static void saveJPGFile(const char* filename) {
    char backupName[400];
    struct stat buf;

#ifdef SUPERDEBUG
    ALOGE("Modified: %s\n", filename);
#endif

    strncpy(backupName, filename, 395);
    strcat(backupName, ".t");

    // Remove any .old file name that may pre-exist
#ifdef SUPERDEBUG
    ALOGE("removing backup %s", backupName);
#endif
    unlink(backupName);

    // Rename the old file.
#ifdef SUPERDEBUG
    ALOGE("rename %s to %s", filename, backupName);
#endif
    rename(filename, backupName);

    // Write the new file.
#ifdef SUPERDEBUG
    ALOGE("WriteJpegFile %s", filename);
#endif
    if (WriteJpegFile(filename)) {

        // Copy the access rights from original file
#ifdef SUPERDEBUG
        ALOGE("stating old file %s", backupName);
#endif
        if (stat(backupName, &buf) == 0){
            // set Unix access rights and time to new file
            struct utimbuf mtime;
            chmod(filename, buf.st_mode);

            mtime.actime = buf.st_mtime;
            mtime.modtime = buf.st_mtime;

            utime(filename, &mtime);
        }

        // Now that we are done, remove original file.
#ifdef SUPERDEBUG
        ALOGE("unlinking old file %s", backupName);
#endif
        unlink(backupName);
#ifdef SUPERDEBUG
        ALOGE("returning from saveJPGFile");
#endif
    } else {
#ifdef SUPERDEBUG
        ALOGE("WriteJpegFile failed, restoring from backup file");
#endif
        // move back the backup file
        rename(backupName, filename);
    }
}

void copyThumbnailData(uchar* thumbnailData, int thumbnailLen) {
#ifdef SUPERDEBUG
    ALOGE("******************************** copyThumbnailData\n");
#endif
    Section_t* ExifSection = FindSection(M_EXIF);
    if (ExifSection == NULL) {
        return;
    }
    int NewExifSize = ImageInfo.ThumbnailOffset+8+thumbnailLen;
    ExifSection->Data = (uchar *)realloc(ExifSection->Data, NewExifSize);
    if (ExifSection->Data == NULL) {
        return;
    }
    uchar* ThumbnailPointer = ExifSection->Data+ImageInfo.ThumbnailOffset+8;

    memcpy(ThumbnailPointer, thumbnailData, thumbnailLen);

    ImageInfo.ThumbnailSize = thumbnailLen;

    Put32u(ExifSection->Data+ImageInfo.ThumbnailSizeOffset+8, thumbnailLen);

    ExifSection->Data[0] = (uchar)(NewExifSize >> 8);
    ExifSection->Data[1] = (uchar)NewExifSize;
    ExifSection->Size = NewExifSize;
}

// returns new buffer length
static int addKeyValueString(char** buf, int bufLen, const char* key, const char* value) {
    // Appends to buf like this: "ImageLength=4 1024"

    char valueLen[15];
    snprintf(valueLen, 15, "=%d ", (int)strlen(value));

    // check to see if buf has enough room to append
    int len = strlen(key) + strlen(valueLen) + strlen(value);
    int newLen = strlen(*buf) + len;
    if (newLen >= bufLen) {
#ifdef REALLOCTEST
        bufLen = newLen + 5;
        ALOGE("reallocing to %d", bufLen);
#else
        bufLen = newLen + 500;
#endif
        *buf = (char*)realloc(*buf, bufLen);
        if (*buf == NULL) {
            return 0;
        }
    }
    // append the new attribute and value
    snprintf(*buf + strlen(*buf), bufLen, "%s%s%s", key, valueLen, value);
#ifdef SUPERDEBUG
    ALOGE("buf %s", *buf);
#endif
    ++attributeCount;
    return bufLen;
}

// returns new buffer length
static int addKeyValueInt(char** buf, int bufLen, const char* key, int value) {
    char valueStr[20];
    snprintf(valueStr, 20, "%d", value);

    return addKeyValueString(buf, bufLen, key, valueStr);
}

// returns new buffer length
static int addKeyValueDouble(char** buf, int bufLen, const char* key, double value, const char* format) {
    char valueStr[30];
    snprintf(valueStr, 30, format, value);

    return addKeyValueString(buf, bufLen, key, valueStr);
}

// Returns new buffer length. Rational value will be appended as "numerator/denominator".
static int addKeyValueRational(char** buf, int bufLen, const char* key, rat_t value) {
    char valueStr[25];
    snprintf(valueStr, sizeof(valueStr), "%u/%u", value.num, value.denom);
    return addKeyValueString(buf, bufLen, key, valueStr);
}

//======================================================================================
// CExifInterface
//======================================================================================
AutoPtr<ISimpleDateFormat> CExifInterface::sFormatter;
Object CExifInterface::sLock;

CAR_INTERFACE_IMPL(CExifInterface, Object, IExifInterface)

CAR_OBJECT_IMPL(CExifInterface)

AutoPtr<ISimpleDateFormat> CExifInterface::GetFormatter()
{
    if (sFormatter == NULL) {
        CSimpleDateFormat::New(String("yyyy:MM:dd HH:mm:ss"), (ISimpleDateFormat**)&sFormatter);
        AutoPtr<ITimeZoneHelper> helper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
        AutoPtr<ITimeZone> tz;
        helper->GetTimeZone(String("UTC"), (ITimeZone**)&tz);
        IDateFormat::Probe(sFormatter)->SetTimeZone(tz);
    }
    return sFormatter;
}

CExifInterface::CExifInterface()
    : mHasThumbnail(FALSE)
{}

CExifInterface::~CExifInterface()
{
    mAttributes.Clear();
}

ECode CExifInterface::constructor( // throws IOException
    /* [in] */ const String& filename)
{
    if (filename == NULL) {
        // throw new IllegalArgumentException("filename cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mFilename = filename;
    LoadAttributes();
    return NOERROR;
}

ECode CExifInterface::GetAttribute(
    /* [in] */ const String& tag,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);

    HashMap<String, String>::Iterator it = mAttributes.Find(tag);
    if (it != mAttributes.End()) {
        *result = it->mSecond;
    }
    return NOERROR;
}

ECode CExifInterface::GetAttributeInt32(
    /* [in] */ const String& tag,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = defaultValue;

    HashMap<String, String>::Iterator it = mAttributes.Find(tag);
    if (it != mAttributes.End()) {
        if (it->mSecond.IsNull()) {
            return NOERROR;
        }

        *result = StringUtils::ParseInt32(it->mSecond, 10, defaultValue);
    }
    return NOERROR;
}

ECode CExifInterface::GetAttributeDouble(
    /* [in] */ const String& tag,
    /* [in] */ Double defaultValue,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    *result = defaultValue;

    HashMap<String, String>::Iterator it = mAttributes.Find(tag);
    if (it != mAttributes.End()) {
        String value = it->mSecond;
        if (value.IsNull()) {
            return NOERROR;
        }

        Int32 index = value.IndexOf("/");
        if (index == -1) {
            return NOERROR;
        }
        Double denom = StringUtils::ParseDouble(value.Substring(index + 1));
        if (denom == 0) {
            return NOERROR;
        }
        Double num = StringUtils::ParseDouble(value.Substring(0, index));
        *result = num / denom;
    }
    return NOERROR;
}

ECode CExifInterface::SetAttribute(
    /* [in] */ const String& tag,
    /* [in] */ const String& value)
{
    mAttributes[tag] = value;
    return NOERROR;
}

void CExifInterface::LoadAttributes() // throws IOException
{
    // format of string passed from native C code:
    // "attrCnt attr1=valueLen value1attr2=value2Len value2..."
    // example:
    // "4 attrPtr ImageLength=4 1024Model=6 FooImageWidth=4 1280Make=3 FOO"

    mAttributes.Clear();

    String attrStr;
    synchronized(sLock) {
        attrStr = GetAttributesNative(mFilename);
    }

    // get count
    Int32 ptr = attrStr.IndexOf(' ');
    Int32 count = StringUtils::ParseInt32(attrStr.Substring(0, ptr));
    // skip past the space between item count and the rest of the attributes
    ++ptr;

    Int32 equalPos, lenPos, attrLen;
    String attrName, attrValue;
    for (Int32 i = 0; i < count; i++) {
        // extract the attribute name
        equalPos = attrStr.IndexOf('=', ptr);
        attrName = attrStr.Substring(ptr, equalPos);
        ptr = equalPos + 1;     // skip past =

        // extract the attribute value length
        lenPos = attrStr.IndexOf(' ', ptr);
        attrLen = StringUtils::ParseInt32(attrStr.Substring(ptr, lenPos));
        ptr = lenPos + 1;       // skip pas the space

        // extract the attribute value
        attrValue = attrStr.Substring(ptr, ptr + attrLen);
        ptr += attrLen;

        if (attrName.Equals("hasThumbnail")) {
            mHasThumbnail = attrValue.EqualsIgnoreCase(String("true"));
        }
        else {
            mAttributes[attrName] = attrValue;
        }
    }
}

ECode CExifInterface::SaveAttributes() //throws IOException
{
    //format of string passed to native C code:
    //"attrCnt attr1=valueLen value1attr2=value2Len value2..."
    //example:
    // "4 attrPtr ImageLength=4 1024Model=6 FooImageWidth=4 1280Make=3 FOO"
    StringBuilder sb;
    Int32 size = mAttributes.GetSize();
    HashMap<String, String>::Iterator it = mAttributes.Find(String("hasThumbnail"));
    if (it != mAttributes.End()) {
        --size;
    }
    sb += size;
    sb += " ";

    String key, val;
    for (it = mAttributes.Begin(); it != mAttributes.End(); ++it) {
        key = it->mFirst;
        if (key.Equals("hasThumbnail")) {
            // this is a fake attribute not saved as an exif tag
            continue;
        }
        val = it->mSecond;
        sb += key;
        sb += "=";
        sb += val.GetLength();
        sb += " ";
        sb += val;
    }

    String s = sb.ToString();
    synchronized(sLock) {
        SaveAttributesNative(mFilename, s);
        CommitChangesNative(mFilename);
    }
    return NOERROR;
}

ECode CExifInterface::HasThumbnail(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mHasThumbnail;
    return NOERROR;
}

ECode CExifInterface::GetThumbnail(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(sLock) {
        AutoPtr<ArrayOf<Byte> > thumbnail = GetThumbnailNative(mFilename);
        *result = thumbnail;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
}

ECode CExifInterface::GetThumbnailRange(
    /* [out, callee] */ ArrayOf<Int64>** result)
{
    synchronized(sLock) {
        AutoPtr<ArrayOf<Int64> > range = GetThumbnailRangeNative(mFilename);
        *result = range;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

ECode CExifInterface::GetLatLong(
    /* [in] */ ArrayOf<Float>* output,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    HashMap<String, String>::Iterator itLatV = mAttributes.Find(IExifInterface::TAG_GPS_LATITUDE);
    HashMap<String, String>::Iterator itLatR = mAttributes.Find(IExifInterface::TAG_GPS_LATITUDE_REF);
    HashMap<String, String>::Iterator itlngV = mAttributes.Find(IExifInterface::TAG_GPS_LONGITUDE);
    HashMap<String, String>::Iterator itlngR = mAttributes.Find(IExifInterface::TAG_GPS_LONGITUDE_REF);

    if (itLatV != mAttributes.End() && itLatR != mAttributes.End()
        && itlngV != mAttributes.End() && itlngR != mAttributes.End()) {
        String latValue = itLatV->mSecond;
        String latRef = itLatR->mSecond;
        String lngValue = itlngV->mSecond;
        String lngRef = itlngR->mSecond;
        if (!latValue.IsNull() && !latRef.IsNull() && !lngValue.IsNull() && !lngRef.IsNull()) {
            Float f0, f1;
            FAIL_RETURN(ConvertRationalLatLonToFloat(latValue, latRef, &f0));
            FAIL_RETURN(ConvertRationalLatLonToFloat(lngValue, lngRef, &f1));
            (*output)[0] = f0;
            (*output)[1] = f1;
            *result = TRUE;
        }
    }

    return NOERROR;
}

ECode CExifInterface::GetAltitude(
    /* [in] */ Double defaultValue,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);

    Double altitude;
    GetAttributeDouble(IExifInterface::TAG_GPS_ALTITUDE, -1, &altitude);
    Int32 ref;
    GetAttributeInt32(IExifInterface::TAG_GPS_ALTITUDE_REF, -1, &ref);

    if (altitude >= 0 && ref >= 0) {
        *result = (Double) (altitude * ((ref == 1) ? -1 : 1));
    }
    else {
        *result = defaultValue;
    }
    return NOERROR;
}

ECode CExifInterface::GetDateTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    HashMap<String, String>::Iterator it = mAttributes.Find(IExifInterface::TAG_DATETIME);
    if (it == mAttributes.End()) {
        return NOERROR;
    }
    String dateTimeString = it->mSecond;
    if (dateTimeString.IsNull()) {
        return NOERROR;
    }

    AutoPtr<IParsePosition> pos;
    CParsePosition::New(0, (IParsePosition**)&pos);
    // try {
    AutoPtr<IDate> datetime;
    FAIL_RETURN(IDateFormat::Probe(GetFormatter())->Parse(dateTimeString, pos, (IDate**)&datetime));
    if (datetime != NULL) {
        datetime->GetTime(result);
    }
    // } catch (IllegalArgumentException ex) {
    //     *result = -1;
    //     return NOERROR;
    // }
    return NOERROR;
}

ECode CExifInterface::GetGpsDateTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    HashMap<String, String>::Iterator itTime = mAttributes.Find(IExifInterface::TAG_GPS_DATESTAMP);
    HashMap<String, String>::Iterator itDate = mAttributes.Find(IExifInterface::TAG_GPS_TIMESTAMP);
    if (itTime == mAttributes.End() || itDate == mAttributes.End()) {
        return NOERROR;
    }

    String date = itDate->mSecond;
    String time = itTime->mSecond;
    if (date.IsNull() || time.IsNull()) {
        return NOERROR;
    }

    String dateTimeString;
    dateTimeString.AppendFormat("%s %s", date.string(), time.string());

    AutoPtr<IParsePosition> pos;
    CParsePosition::New(0, (IParsePosition**)&pos);

    // try {
    AutoPtr<IDate> datetime;
    FAIL_RETURN(IDateFormat::Probe(GetFormatter())->Parse(dateTimeString, pos, (IDate**)&datetime));
    if (datetime != NULL) {
        datetime->GetTime(result);
    }
    // } catch (IllegalArgumentException ex) {
    //    *result = -1;
    //    return NOERROR;
    // }
    return NOERROR;
}

/*static*/
ECode CExifInterface::ConvertRationalLatLonToFloat(
    /* [in] */ const String& rationalString,
    /* [in] */ const String& ref,
    /* [in] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

//    try {
    AutoPtr< ArrayOf<String> > parts;
    StringUtils::Split(rationalString, String(","), (ArrayOf<String>**)&parts);
    if (parts->GetLength() < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Double v1, v2;

    AutoPtr< ArrayOf<String> > pair;
    StringUtils::Split((*parts)[0], String("/"), (ArrayOf<String>**)&pair);
    if (pair->GetLength() < 2) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    v1 = StringUtils::ParseDouble((*pair)[0].Trim());
    v2 = StringUtils::ParseDouble((*pair)[1].Trim());
    Double degrees = v1 / v2;

    pair = NULL;
    StringUtils::Split((*parts)[1], String("/"), (ArrayOf<String>**)&pair);
    if (pair->GetLength() < 2) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    v1 = StringUtils::ParseDouble((*pair)[0].Trim());
    v2 = StringUtils::ParseDouble((*pair)[1].Trim());
    Double minutes = v1 / v2;

    pair = NULL;
    StringUtils::Split((*parts)[2], String("/"), (ArrayOf<String>**)&pair);
    if (pair->GetLength() < 2) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    v1 = StringUtils::ParseDouble((*pair)[0].Trim());
    v2 = StringUtils::ParseDouble((*pair)[1].Trim());
    Double seconds = v1 / v2;

    Double r = degrees + (minutes / 60.0) + (seconds / 3600.0);
    if ((ref.Equals(String("S")) || ref.Equals(String("W")))) {
        *result = (Float) -r;
        return NOERROR;
    }

    *result = (Float) r;
    return NOERROR;
//    } catch (NumberFormatException e) {
//        // Some of the nubmers are not valid
//        throw new IllegalArgumentException();
//    } catch (ArrayIndexOutOfBoundsException e) {
//        // Some of the rational does not follow the correct format
//        throw new IllegalArgumentException();
//    }
}

//-----------------------------------------------------------
//    Native Method : amlogic_4.2\external\jhead.h and Main.c
//-----------------------------------------------------------

Boolean CExifInterface::AppendThumbnailNative(
    /* [in] */ const String& fileName,
    /* [in] */ const String& thumbnailFileName)
{
#ifdef SUPERDEBUG
    ALOGE("******************************** appendThumbnail\n");
#endif
    assert(0 && "TODO");
    if (fileName.IsNull() || thumbnailFileName.IsNull()) {
        return FALSE;
    }

#ifdef SUPERDEBUG
    ALOGE("*******before actual call to ReplaceThumbnail\n");
    ShowImageInfo(TRUE);
#endif
    ReplaceThumbnail(thumbnailFileName);
#ifdef SUPERDEBUG
    ShowImageInfo(TRUE);
#endif

    DiscardData();
    return TRUE;
}

void CExifInterface::SaveAttributesNative(
    /* [in] */ const String& jfilename,
    /* [in] */ const String& compressedAttributes)
{
#ifdef SUPERDEBUG
    ALOGE("******************************** saveAttributes\n");
#endif
    // format of attributes string passed from java:
    // "attrCnt attr1=valueLen value1attr2=value2Len value2..."
    // example input: "4 ImageLength=4 1024Model=6 FooImageWidth=4 1280Make=3 FOO"
    ExifElement_t* exifElementTable = NULL;
    const char* filename = NULL;
    char* attrPtr = NULL;
    uchar* thumbnailData = NULL;
    int attrCnt = 0;

    int i, thumbnailLength, valueLen;
    char tag[100];
    int hasDateTimeTag = FALSE;
    int gpsTagCount = 0;
    int exifTagCount = 0;

    const char* attributes = compressedAttributes.string();
    if (attributes == NULL) {
        goto _Exit_;
    }
#ifdef SUPERDEBUG
    ALOGE("attributes %s\n", attributes);
#endif

    // Get the number of attributes - it's the first number in the string.
    attrCnt = atoi(attributes);
    attrPtr = strchr(attributes, ' ') + 1;
#ifdef SUPERDEBUG
    ALOGE("attribute count %d attrPtr %s\n", attrCnt, attrPtr);
#endif

    // Load all the hash exif elements into a more c-like structure
    exifElementTable = (ExifElement_t*)malloc(sizeof(ExifElement_t) * attrCnt);
    if (exifElementTable == NULL) {
        goto _Exit_;
    }
#ifdef OUTOFMEMORYTEST1
    goto _Exit_;
#endif

    for (i = 0; i < attrCnt; i++) {
        // get an element from the attribute string and add it to the c structure
        // first, extract the attribute name
        char* tagEnd = strchr(attrPtr, '=');
        if (tagEnd == 0) {
#ifdef SUPERDEBUG
            ALOGE("saveAttributes: couldn't find end of tag");
#endif
            goto _Exit_;
        }

        if (tagEnd - attrPtr > 99) {
#ifdef SUPERDEBUG
            ALOGE("saveAttributes: attribute tag way too long");
#endif
            goto _Exit_;
        }
        memcpy(tag, attrPtr, tagEnd - attrPtr);
        tag[tagEnd - attrPtr] = 0;

        if (IsGpsTag(tag)) {
            exifElementTable[i].GpsTag = TRUE;
            exifElementTable[i].Tag = GpsTagNameToValue(tag);
            ++gpsTagCount;
        } else {
            exifElementTable[i].GpsTag = FALSE;
            exifElementTable[i].Tag = TagNameToValue(tag);
            ++exifTagCount;
        }
        attrPtr = tagEnd + 1;

        if (IsDateTimeTag(exifElementTable[i].Tag)) {
            hasDateTimeTag = TRUE;
        }

        // next get the length of the attribute value
        valueLen = atoi(attrPtr);
        attrPtr = strchr(attrPtr, ' ') + 1;
        if (attrPtr == 0) {
#ifdef SUPERDEBUG
            ALOGE("saveAttributes: couldn't find end of value len");
#endif
            goto _Exit_;
        }
        exifElementTable[i].Value = (char*)malloc(valueLen + 1);
        if (exifElementTable[i].Value == NULL) {
            goto _Exit_;
        }
        memcpy(exifElementTable[i].Value, attrPtr, valueLen);
        exifElementTable[i].Value[valueLen] = 0;
        exifElementTable[i].DataLength = valueLen;

        attrPtr += valueLen;

#ifdef SUPERDEBUG
        ALOGE("tag %s id %d value %s data length=%d isGps=%d", tag, exifElementTable[i].Tag,
            exifElementTable[i].Value, exifElementTable[i].DataLength, exifElementTable[i].GpsTag);
#endif
    }

    filename = jfilename.string();
#ifdef SUPERDEBUG
    ALOGE("Call loadAttributes() with filename is %s. Loading exif info\n", filename);
#endif
    loadExifInfo(filename, TRUE);

#ifdef SUPERDEBUG
//    DumpExifMap = TRUE;
    ShowTags = TRUE;
    ShowImageInfo(TRUE);
    ALOGE("create exif 2");
#endif

    // If the jpg file has a thumbnail, preserve it.
    thumbnailLength = ImageInfo.ThumbnailSize;
    if (ImageInfo.ThumbnailOffset) {
        Section_t* ExifSection = FindSection(M_EXIF);
        if (ExifSection) {
            uchar* thumbnailPointer = ExifSection->Data + ImageInfo.ThumbnailOffset + 8;
            thumbnailData = (uchar*)malloc(ImageInfo.ThumbnailSize);
            // if the malloc fails, we just won't copy the thumbnail
            if (thumbnailData) {
                memcpy(thumbnailData, thumbnailPointer, thumbnailLength);
            }
        }
    }

    create_EXIF(exifElementTable, exifTagCount, gpsTagCount, hasDateTimeTag);

    if (thumbnailData) {
        copyThumbnailData(thumbnailData, thumbnailLength);
    }

_Exit_:
#ifdef SUPERDEBUG
    ALOGE("cleaning up now in saveAttributes");
#endif
    // try to clean up resources
    if (exifElementTable) {
        // free the table
        for (i = 0; i < attrCnt; i++) {
            free(exifElementTable[i].Value);
        }
        free(exifElementTable);
    }
    if (thumbnailData) {
        free(thumbnailData);
    }
#ifdef SUPERDEBUG
    ALOGE("returning from saveAttributes");
#endif

// Temporarily saving these commented out lines because they represent a lot of figuring out
// patterns for JNI.
//    // Get link to Method "entrySet"
//    jmethodID entrySetMethod = (*env)->GetMethodID(env, jclass_of_hashmap, "entrySet", "()Ljava/util/Set;");
//
//    // Invoke the "entrySet" method on the HashMap object
//    jobject jobject_of_entryset = (*env)->CallObjectMethod(env, hashMap, entrySetMethod);
//
//    // Get the Set Class
//    jclass jclass_of_set = (*env)->FindClass(env, "java/util/Set");
//
//    if (jclass_of_set == 0) {
//        printf("java/util/Set lookup failed\n");
//        return;
//    }
//
//    // Get link to Method "iterator"
//    jmethodID iteratorMethod = (*env)->GetMethodID(env, jclass_of_set, "iterator", "()Ljava/util/Iterator;");
//
//    // Invoke the "iterator" method on the jobject_of_entryset variable of type Set
//    jobject jobject_of_iterator = (*env)->CallObjectMethod(env, jobject_of_entryset, iteratorMethod);
//
//    // Get the "Iterator" class
//    jclass jclass_of_iterator = (*env)->FindClass(env, "java/util/Iterator");
//
//    // Get link to Method "hasNext"
//    jmethodID hasNextMethod = (*env)->GetMethodID(env, jclass_of_iterator, "hasNext", "()Z");
//
//    // Invoke - Get the value hasNextMethod
//    jboolean bHasNext = (*env)->CallBooleanMethod(env, jobject_of_iterator, hasNextMethod);
//
//    // Get link to Method "hasNext"
//    jmethodID nextMethod = (*env)->GetMethodID(env, jclass_of_iterator, "next", "()Ljava/util/Map/Entry;");
//
//    jclass jclass_of_mapentry = (*env)->FindClass(env, "java/util/Map/Entry");
//
//    jmethodID getKeyMethod = (*env)->GetMethodID(env, jclass_of_mapentry, "getKey", "()Ljava/lang/Object");
//
//    jmethodID getValueMethod = (*env)->GetMethodID(env, jclass_of_mapentry, "getValue", "()Ljava/lang/Object");
}

String CExifInterface::GetAttributesNative(
    /* [in] */ const String& jfilename)
{
#ifdef SUPERDEBUG
    ALOGE("******************************** getAttributes\n");
#endif
    const char* filename = jfilename.string();
    loadExifInfo(filename, FALSE);
#ifdef SUPERDEBUG
    ShowImageInfo(TRUE);
#endif

    attributeCount = 0;
#ifdef REALLOCTEST
    int bufLen = 5;
#else
    int bufLen = 1000;
#endif
    char* buf = (char*)malloc(bufLen);
    if (buf == NULL) {
        return String(NULL);
    }
    *buf = 0;   // start the string out at zero length

    // save a fake "hasThumbnail" tag to pass to the java ExifInterface
    bufLen = addKeyValueString(&buf, bufLen, "hasThumbnail",
        ImageInfo.ThumbnailOffset == 0
            || ImageInfo.ThumbnailAtEnd == FALSE
            || ImageInfo.ThumbnailSize == 0
            ? "false" : "true");
    if (bufLen == 0) return String(NULL);

    if (ImageInfo.CameraMake[0]) {
       bufLen = addKeyValueString(&buf, bufLen, "Make", ImageInfo.CameraMake);
       if (bufLen == 0) return String(NULL);
    }
    if (ImageInfo.CameraModel[0]) {
       bufLen = addKeyValueString(&buf, bufLen, "Model", ImageInfo.CameraModel);
       if (bufLen == 0) return String(NULL);
    }
    if (ImageInfo.DateTime[0]) {
       bufLen = addKeyValueString(&buf, bufLen, "DateTime", ImageInfo.DateTime);
       if (bufLen == 0) return String(NULL);
    }
    bufLen = addKeyValueInt(&buf, bufLen, "ImageWidth", ImageInfo.Width);
    if (bufLen == 0) return String(NULL);

    bufLen = addKeyValueInt(&buf, bufLen, "ImageLength", ImageInfo.Height);
    if (bufLen == 0) return String(NULL);

    bufLen = addKeyValueInt(&buf, bufLen, "Orientation", ImageInfo.Orientation);
    if (bufLen == 0) return String(NULL);

    if (ImageInfo.FlashUsed >= 0) {
       bufLen = addKeyValueInt(&buf, bufLen, "Flash", ImageInfo.FlashUsed);
       if (bufLen == 0) return String(NULL);
    }

    if (ImageInfo.FocalLength.num != 0 && ImageInfo.FocalLength.denom != 0) {
        bufLen = addKeyValueRational(&buf, bufLen, "FocalLength", ImageInfo.FocalLength);
        if (bufLen == 0) return String(NULL);
    }

    if (ImageInfo.DigitalZoomRatio > 1.0){
        // Digital zoom used.  Shame on you!
        bufLen = addKeyValueDouble(&buf, bufLen, "DigitalZoomRatio", ImageInfo.DigitalZoomRatio, "%1.3f");
        if (bufLen == 0) return String(NULL);
    }

    if (ImageInfo.ExposureTime){
        const char* format;
        if (ImageInfo.ExposureTime < 0.010){
            format = "%6.4f";
        } else {
            format = "%5.3f";
        }

        bufLen = addKeyValueDouble(&buf, bufLen, "ExposureTime", (double)ImageInfo.ExposureTime, format);
        if (bufLen == 0) return String(NULL);
    }

    if (ImageInfo.ApertureFNumber){
        bufLen = addKeyValueDouble(&buf, bufLen, "FNumber", (double)ImageInfo.ApertureFNumber, "%3.1f");
        if (bufLen == 0) return String(NULL);
    }

    if (ImageInfo.Distance){
        bufLen = addKeyValueDouble(&buf, bufLen, "SubjectDistance", (double)ImageInfo.Distance, "%4.2f");
        if (bufLen == 0) return String(NULL);
    }

    if (ImageInfo.ISOequivalent){
        bufLen = addKeyValueInt(&buf, bufLen, "ISOSpeedRatings", ImageInfo.ISOequivalent);
        if (bufLen == 0) return String(NULL);
    }

    if (ImageInfo.ExposureBias){
        // If exposure bias was specified, but set to zero, presumably its no bias at all,
        // so only show it if its nonzero.
        bufLen = addKeyValueDouble(&buf, bufLen, "ExposureBiasValue", (double)ImageInfo.ExposureBias, "%4.2f");
        if (bufLen == 0) return String(NULL);
    }

    if (ImageInfo.Whitebalance >= 0) {
        bufLen = addKeyValueInt(&buf, bufLen, "WhiteBalance", ImageInfo.Whitebalance);
        if (bufLen == 0) return String(NULL);
    }

    bufLen = addKeyValueInt(&buf, bufLen, "LightSource", ImageInfo.LightSource);
    if (bufLen == 0) return String(NULL);

    if (ImageInfo.MeteringMode) {
        bufLen = addKeyValueInt(&buf, bufLen, "MeteringMode", ImageInfo.MeteringMode);
        if (bufLen == 0) return String(NULL);
    }

    if (ImageInfo.ExposureProgram) {
        bufLen = addKeyValueInt(&buf, bufLen, "ExposureProgram", ImageInfo.ExposureProgram);
        if (bufLen == 0) return String(NULL);
    }

    if (ImageInfo.ExposureMode) {
        bufLen = addKeyValueInt(&buf, bufLen, "ExposureMode", ImageInfo.ExposureMode);
        if (bufLen == 0) return String(NULL);
    }

    if (ImageInfo.GpsInfoPresent) {
        if (ImageInfo.GpsLatRaw[0]) {
            bufLen = addKeyValueString(&buf, bufLen, "GPSLatitude", ImageInfo.GpsLatRaw);
            if (bufLen == 0) return String(NULL);
        }
        if (ImageInfo.GpsLatRef[0]) {
            bufLen = addKeyValueString(&buf, bufLen, "GPSLatitudeRef", ImageInfo.GpsLatRef);
            if (bufLen == 0) return String(NULL);
        }
        if (ImageInfo.GpsLongRaw[0]) {
            bufLen = addKeyValueString(&buf, bufLen, "GPSLongitude", ImageInfo.GpsLongRaw);
            if (bufLen == 0) return String(NULL);
        }
        if (ImageInfo.GpsLongRef[0]) {
            bufLen = addKeyValueString(&buf, bufLen, "GPSLongitudeRef", ImageInfo.GpsLongRef);
            if (bufLen == 0) return String(NULL);
        }
        if (ImageInfo.GpsAlt[0]) {
            bufLen = addKeyValueRational(&buf, bufLen, "GPSAltitude", ImageInfo.GpsAltRaw);
            bufLen = addKeyValueInt(&buf, bufLen, "GPSAltitudeRef", ImageInfo.GpsAltRef);
            if (bufLen == 0) return String(NULL);
        }
        if (ImageInfo.GpsDateStamp[0]) {
            bufLen = addKeyValueString(&buf, bufLen, "GPSDateStamp", ImageInfo.GpsDateStamp);
            if (bufLen == 0) return String(NULL);
        }
        if (ImageInfo.GpsTimeStamp[0]) {
            bufLen = addKeyValueString(&buf, bufLen, "GPSTimeStamp", ImageInfo.GpsTimeStamp);
            if (bufLen == 0) return String(NULL);
        }
        if (ImageInfo.GpsProcessingMethod[0]) {
            bufLen = addKeyValueString(&buf, bufLen, "GPSProcessingMethod", ImageInfo.GpsProcessingMethod);
            if (bufLen == 0) return String(NULL);
        }
    }

    if (ImageInfo.Comments[0]) {
        bufLen = addKeyValueString(&buf, bufLen, "UserComment", ImageInfo.Comments);
        if (bufLen == 0) return String(NULL);
    }

    // put the attribute count at the beginnnig of the string
    int finalBufLen = strlen(buf) + 20;
    char* finalResult = (char*)malloc(finalBufLen);
    if (finalResult == NULL) {
        free(buf);
        return String(NULL);
    }
    snprintf(finalResult, finalBufLen, "%d %s", attributeCount, buf);
    int k;
    for (k = 0; k < finalBufLen; k++)
        if (!isascii(finalResult[k]))
           finalResult[k] = '?';
    free(buf);

#ifdef SUPERDEBUG
    ALOGE("*********Returning result \"%s\"", finalResult);
#endif
    String result(finalResult);
    free(finalResult);
    DiscardData();
    return result;
}

void CExifInterface::CommitChangesNative(
    /* [in] */ const String& fileName)
{
#ifdef SUPERDEBUG
   ALOGE("******************************** commitChanges\n");
#endif
    if (!fileName.IsNull()) {
        saveJPGFile(fileName.string());
        DiscardData();
    }
}

AutoPtr< ArrayOf<Byte> > CExifInterface::GetThumbnailNative(
    /* [in] */ const String& fileName)
{
#ifdef SUPERDEBUG
    ALOGE("******************************** getThumbnail\n");
#endif

    if (!fileName.IsNull()) {
        loadExifInfo(fileName.string(), FALSE);
        Section_t* ExifSection = FindSection(M_EXIF);
        if (ExifSection == NULL ||  ImageInfo.ThumbnailSize == 0) {
#ifdef SUPERDEBUG
            ALOGE("no exif section or size == 0, so no thumbnail\n");
#endif
            goto noThumbnail;
        }
        uchar* thumbnailPointer = ExifSection->Data + ImageInfo.ThumbnailOffset + 8;

        AutoPtr<ArrayOf<Byte> > byteArray = ArrayOf<Byte>::Alloc(ImageInfo.ThumbnailSize);
        if (byteArray == NULL) {
#ifdef SUPERDEBUG
            ALOGE("couldn't allocate thumbnail memory, so no thumbnail\n");
#endif
            goto noThumbnail;
        }

        byteArray->Copy((Byte*)thumbnailPointer, ImageInfo.ThumbnailSize);
#ifdef SUPERDEBUG
        ALOGE("thumbnail size %d\n", ImageInfo.ThumbnailSize);
#endif
        DiscardData();
        return byteArray;
   }

noThumbnail:
    DiscardData();
    return NULL;
}

AutoPtr< ArrayOf<Int64> > CExifInterface::GetThumbnailRangeNative(
    /* [in] */ const String& fileName)
{
    AutoPtr< ArrayOf<Int64> > resultArray;
    if (!fileName.IsNull()) {
        loadExifInfo(fileName, FALSE);
        Section_t* ExifSection = FindSection(M_EXIF);
        if (ExifSection == NULL || ImageInfo.ThumbnailSize == 0) {
            goto done;
        }

        resultArray = ArrayOf<Int64>::Alloc(2);
        (*resultArray)[0] = ExifSection->Offset + ImageInfo.ThumbnailOffset + 8;
        (*resultArray)[1] = ImageInfo.ThumbnailSize;
    }
done:
    DiscardData();
    return resultArray;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
