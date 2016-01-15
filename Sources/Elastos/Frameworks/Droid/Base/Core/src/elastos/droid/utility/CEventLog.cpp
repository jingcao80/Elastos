#include "CEventLog.h"

namespace Elastos {
namespace Droid {
namespace App {

CEventLog::CEventLog()
    : mWallpaperXStep(-1)
    , mWallpaperYStep(-1)
{
}

CEventLog::~CEventLog()
{
}

ECode CEventLog::GetTagName(
    /* [in] */ Int32 tag,
    /* [out] */ String* name)
{
    ReadTagsFile();
    sTagNames->Get(tag, &name);
    return NOERROR;
}

ECode CEventLog::GetTagCode(
    /* [in] */ const String& name,
    /* [out] */ Int32* tagCode)
{
    ReadTagsFile();
    Int32 code;
    sTagCodes->Get(name, &code);
    if(code != NULL)
    {
        *tagCode = code;
    }
    else
    {
        *tagCode = -1;
    }
    return NOERROR;
}

void ReadTagsFile()
{
    if (sTagCodes != NULL && sTagNames != NULL) return;

    sTagCodes = new HashMap<String, Int32>();
    sTagNames = new HashMap<Int32, String>();

    AutoPtr<IPattern> comment = CPatternHelper::Compile(IPattern::COMMENT_PATTERN);
    AutoPtr<IPattern> tag = CPatternHelper::Compile(IPattern::TAG_PATTERN);
    AutoPtr<IBufferedReader> reader;
    String line;

    //try {
    AutoPtr<IFileReader> fr;
    CFileReader::New(TAGS_FILE, &fr);
    CBufferedReader::New(fr, 256, &reader);
    reader->ReadLine(&line);

    while (line != NULL) {
        AutoPtr<IMatcher> matcher;
        comment->Matcher(line, &matcher);
        Boolean result;
        matcher->Matches(&b);
        if (result) continue;

        AutoPtr<IMatcher> m;
        tag->Matcher(line, &m);
        Boolean b;
        m->Matches(&b);
        if (!b) {
            //Log.wtf(TAG, "Bad entry in " + TAGS_FILE + ": " + line);
            continue;
        }

        //try {
        Int32 num;
        String name;
        m->Group(1, &name);
        /* Int32::ParseInt32(name, &num);*/
        m->Group(2, &name);
        sTagCodes->Put(name, num);
        sTagNames->Put(num, name);
        //} catch (NumberFormatException e) {
        //    Log.wtf(TAG, "Error in " + TAGS_FILE + ": " + line, e);
        //}
        reader->ReadLine(&line);
    }
    //} catch (IOException e) {
    //    Log.wtf(TAG, "Error reading " + TAGS_FILE, e);
        // Leave the maps existing but unpopulated
    //} finally {
        //try {
    if (reader != NULL) reader->Close();
        //} catch (IOException e) {}
    //}
}

}
}
}
