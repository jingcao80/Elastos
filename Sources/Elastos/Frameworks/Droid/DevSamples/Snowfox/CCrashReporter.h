
#ifndef __CCRASHREPORTER_H__
#define __CCRASHREPORTER_H__

#include <cmdef.h>
#include "elastos/droid/app/Activity.h"
#include "_CCrashReporter.h"
#include <elautoptr.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::IO::IOutputStream;
using Elastos::IO::IBufferedReader;
using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IProgressDialog;


namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

class CCrashReporter : public Activity
{
public:
    CCrashReporter();

    ~CCrashReporter();

    ECode MoveFile(
        /* [in] */ IFile* pInFile,
        /* [in] */ IFile* pOutFile);

    ECode DoFinish();

    CARAPI Finish();

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    ECode BackgroundSendReport();

    ECode OnCloseClick(
        /* [in] */ IView* pIView);

    ECode OnRestartClick(
        /* [in] */ IView* pIView);

    ECode ReadStringsFromFile(
        /* [in] */ const String& filePath,
        /* [in] */ HashMap<String, String>* stringMap);

    ECode ReadStringsFromReader(
        /* [in] */ IBufferedReader* pReader,
        /* [in] */ HashMap<String, String>* stringMap);

    ECode GenerateBoundary(
        /* [out] */ String* boundary);

    ECode SendPart(
        /* [in] */ IOutputStream* pOs,
        /* [in] */ const String& boundary,
        /* [in] */ const String& name,
        /* [in] */ const String& data);

    ECode SendFile(
        /* [in] */ IOutputStream* pOs,
        /* [in] */ const String& boundary,
        /* [in] */ const String& name,
        /* [in] */ IFile* pFile);

    ECode SendReport(
        /* [in] */ IFile* pMinidumpFile,
        /* [in] */ HashMap<String, String> extras,
        /* [in] */ IFile* pExtrasFile);

    ECode DoRestart();

    ECode Unescape(
        /* [in] */ const String& string,
        /* [out] */ String* out);

public:
    static const String kMiniDumpPathKey;
    static const String kPageURLKey;
    static const String kNotesKey;

    AutoPtr<IHandler> mHandler;
    AutoPtr<IProgressDialog> mProgressDialog;
    AutoPtr<IFile> mPendingMinidumpFile;
    AutoPtr<IFile> mPendingExtrasFile;
    HashMap<String, String> mExtrasStringMap;

    AutoPtr<IView> mRestartButton;
    AutoPtr<IView> mCloseButton;
};

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__CCRASHREPORTER_H__

