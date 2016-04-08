
#include "CCrashReporter.h"
#include <elastos/core/Math.h>
#include "GeckoRunnable.h"
#include "GeckoApp.h"
#include <stdio.h>
#include "Elastos.Core.h"
#include <os/Build.h>
#include <elastos/core/StringUtils.h>
#include <cmdef.h>

using Elastos::Core::CStringWrapper;
using Elastos::Core::Math;
using Elastos::Core::StringUtils;
using Elastos::Core::CThread;
using Elastos::Net::IURL;
using Elastos::Net::CURL;
using Elastos::Net::IURLConnection;
using Elastos::Net::IHttpURLConnection;
using Elastos::IO::CFile;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IFlushable;
using Elastos::IO::Channels::IFileChannel;
using Elastos::IO::Channels::IWritableByteChannel;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Content::CIntent;
//using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICheckable;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Snowfox {

#define MAX_INT_VALUE   2147483647
const String CCrashReporter::kMiniDumpPathKey("upload_file_minidump");
const String CCrashReporter::kPageURLKey("URL");
const String CCrashReporter::kNotesKey("Notes");

class CrashReporterClikListener : public ElRefBase
                                , public IViewOnClickListener
{
public:
    CrashReporterClikListener(
        /* [in] */ CCrashReporter* pHost)
        : mHost(pHost)
    {
    }

    UInt32 AddRef()
    {
        return ElRefBase::AddRef();
    }

    UInt32 Release()
    {
        return ElRefBase::Release();
    }

    PInterface Probe(
        /* [in] */ REIID riid)
    {
        if (riid == EIID_IInterface) {
            return (PInterface)(IViewOnClickListener*)this;
        }
        else if (riid == EIID_IViewOnClickListener) {
            return (IViewOnClickListener*)this;
        }

        return NULL;
    }

    ECode GetInterfaceID(
        /* [in] */ IInterface* pObject,
        /* [in] */ InterfaceID* pIID)
    {
        printf("CrashReporterClikListener E_NOT_IMPLEMENTED1\n");
        return E_NOT_IMPLEMENTED;
    }

    ECode OnClick(
        /* [in] */ IView* pView)
    {
        if (!CObject::Compare(pView, mHost->mCloseButton)) {
            return mHost->OnCloseClick(pView);
        }
        else if (!CObject::Compare(pView, mHost->mRestartButton)) {
            return mHost->OnRestartClick(pView);
        }
        return NOERROR;
    }

protected:
    CCrashReporter* mHost;
};


CCrashReporter::CCrashReporter()
    : mHandler(NULL)
    , mProgressDialog(NULL)
    , mPendingMinidumpFile(NULL)
    , mPendingExtrasFile(NULL)
    , mRestartButton(NULL)
    , mCloseButton(NULL)
{
}

CCrashReporter::~CCrashReporter()
{
}

ECode CCrashReporter::MoveFile(
    /* [in] */ IFile* pInFile,
    /* [in] */ IFile* pOutFile)
{
    Boolean succeeded;
    Int64 position = 0;
    ECode ec = pInFile->RenameTo(pOutFile, &succeeded);
    if (FAILED(ec)) return ec;
    if (succeeded) {
        return NOERROR;
    }
    //try {
    ec = pOutFile->CreateNewFile(&succeeded);
    if (FAILED(ec)) return ec;
    printf("GeckoCrashReporter couldn't rename minidump file\n");

    // so copy it instead
    AutoPtr<IFileInputStream> pIns;
    AutoPtr<IFileOutputStream> pOuts;
    AutoPtr<IFileChannel> pInChannel;
    AutoPtr<IFileChannel> pOutChannel;
    ec = CFileInputStream::New(pInFile, (IFileInputStream**)&pIns);
    if (FAILED(ec)) return ec;
//    ec = pIns->GetChannel((IFileChannel**)&pInChannel);
//    if (FAILED(ec)) return ec;

    ec = CFileOutputStream::New(pOutFile, (IFileOutputStream**)&pOuts);
    if (FAILED(ec)) return ec;
//    ec = pOuts->GetChannel((IFileChannel**)&pOutChannel);
//    if (FAILED(ec)) return ec;

    Int64 size, transferred;
    pInChannel->GetSize(&size);
    ec = pInChannel->TransferTo(position,
                size, IWritableByteChannel::Probe(pOutChannel), &transferred);
    if (FAILED(ec)) return ec;
    ICloseable::Probe(pInChannel)->Close();
    ICloseable::Probe(pOutChannel)->Close();

    if (transferred > 0) {
      pInFile->Delete(&succeeded);
    }
    //} catch (Exception e) {
    //  Log.e("GeckoCrashReporter",
    //        "exception while copying minidump file: ", e);
    //  return false;
    //}

    return ec;
}

class FinishRunnable : public GeckoRunnable
{
public:
    FinishRunnable(
        /* [in] */ CCrashReporter* pHost)
        : GeckoRunnable((Void*)pHost)
    {
    }

    ECode Run()
    {
        ((CCrashReporter*)mHost)->Finish();
        return NOERROR;
    }
};

ECode CCrashReporter::DoFinish()
{
    if (!mHandler) {
        FinishRunnable* pRunnable = new FinishRunnable(this);
        if (!pRunnable) return E_OUT_OF_MEMORY;

        //return mHandler->Post(IRunnable::Probe(pRunnable));
    }

    return NOERROR;
}

ECode CCrashReporter::Finish()
{
    Boolean isShowing;
    mProgressDialog->IsShowing(&isShowing);
    if (isShowing) {
        FAIL_RETURN(mProgressDialog->Dismiss());
    }

    return Activity::Finish();
}

ECode CCrashReporter::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    // mHandler is created here so runnables can be run on the main thread
    ECode ec;
    //ec = CHandler::New((IHandler**)&mHandler);
    //if (FAILED(ec)) return ec;

    SetContentView(0x7f030000);//R.layout.crash_reporter

//    ec = CProgressDialog::New(this, (IProgressDialog**)&mProgressDialog);
//    if (FAILED(ec)) return ec;
    String report;
    GetString(0x7f05000c, &report); //R.string.sending_crash_report
    AutoPtr<ICharSequence> charS;
    ec = CStringWrapper::New(report, (ICharSequence**)&(charS));
    if (FAILED(ec)) return ec;
    mProgressDialog->SetMessage(charS);

    mRestartButton = FindViewById(0x7f070003);//R.id.restart
    assert(mRestartButton);
    mCloseButton = FindViewById(0x7f070002);//R.id.close
    assert(mCloseButton);
    CrashReporterClikListener* pListener = new CrashReporterClikListener(this);
    if (!pListener) return E_OUT_OF_MEMORY;
    mRestartButton->SetOnClickListener(IViewOnClickListener::Probe(pListener));
    mCloseButton->SetOnClickListener(IViewOnClickListener::Probe(pListener));

    AutoPtr<IIntent> pIIntent;
    GetIntent((IIntent**)&pIIntent);
    assert(pIIntent);
    String passedMinidumpPath;
    ec = pIIntent->GetStringExtra(String("minidumpPath"), &passedMinidumpPath);
    if (FAILED(ec)) return ec;

    AutoPtr<IFile> pPassedMinidumpFile;
    ec = CFile::New(passedMinidumpPath, (IFile**)&pPassedMinidumpFile);
    if (FAILED(ec)) return ec;

    String name;
    String path;
    AutoPtr<IFile> pPendingDir;
    ec = CFile::New(
        String("/data/data/com.elastos.runtime/elastos/Snowfox/mozilla/Crash Reports/pending"),
        (IFile**)&pPendingDir);
    if (FAILED(ec)) return ec;

    Boolean succeeded;
    pPendingDir->Mkdirs(&succeeded);

    pPassedMinidumpFile->GetName(&name);
    ec = CFile::New(pPendingDir, name, (IFile**)&mPendingMinidumpFile);
    if (FAILED(ec)) return ec;
    MoveFile(pPassedMinidumpFile, mPendingMinidumpFile);

    String tmp;
    StringUtils::ReplaceAll(passedMinidumpPath, String(".dmp"), String(".extra"), &tmp);
    AutoPtr<IFile> pExtrasFile;
    ec = CFile::New(tmp, (IFile**)&pExtrasFile);
    if (FAILED(ec)) return ec;
    pExtrasFile->GetName(&name);
    ec = CFile::New(pPendingDir, name, (IFile**)&mPendingExtrasFile);
    if (FAILED(ec)) return ec;

    MoveFile(pExtrasFile, mPendingExtrasFile);

    mPendingExtrasFile->GetPath(&path);
    ec = ReadStringsFromFile(path, &mExtrasStringMap);
    return ec;
}

class SendReportRunnable : public GeckoRunnable
{
public:
    SendReportRunnable(
        /* [in] */ CCrashReporter* pHost)
        : GeckoRunnable((Void*)pHost)
    {
    }

    ECode Run()
    {
        CCrashReporter* pHost = (CCrashReporter*)mHost;
        pHost->SendReport(pHost->mPendingMinidumpFile,
             pHost->mExtrasStringMap, pHost->mPendingExtrasFile);
        return NOERROR;
    }
};

ECode CCrashReporter::BackgroundSendReport()
{
    AutoPtr<IView> pView = FindViewById(
                                0x7f070000); //R.id.send_report
    assert(pView);

    ICheckBox* pSendReportCheckbox = ICheckBox::Probe(pView);

    Boolean isChecked;
    ICheckable::Probe(pSendReportCheckbox)->IsChecked(&isChecked);
    if (!isChecked) {
        return DoFinish();
    }

    mProgressDialog->Show();
    SendReportRunnable* pRunnable = new SendReportRunnable(this);
    if (!pRunnable) return E_OUT_OF_MEMORY;

    AutoPtr<IThread> pIThread;
    ECode ec = CThread::New(IRunnable::Probe(pRunnable), (IThread**)&pIThread);
    if (FAILED(ec)) {
        delete pRunnable;
        return ec;
    }
    pIThread->Start();
    return NOERROR;
}

ECode CCrashReporter::OnCloseClick(
    /* [in] */ IView* pIView)
{
    return BackgroundSendReport();
}

ECode CCrashReporter::OnRestartClick(
    /* [in] */ IView* pIView)
{
    DoRestart();
    return BackgroundSendReport();
}

ECode CCrashReporter::ReadStringsFromFile(
    /* [in] */ const String& filePath,
    /* [in] */ HashMap<String, String>* stringMap)
{
    //try {
    AutoPtr<IFileReader> pIFileReader;
    ECode ec = CFileReader::New(filePath, (IFileReader**)&pIFileReader);
    if (FAILED(ec)) return ec;

    AutoPtr<IBufferedReader> pReader;
    ec = CBufferedReader::New(pIFileReader, (IBufferedReader**)&pReader);
    if (FAILED(ec)) return ec;

    ec = ReadStringsFromReader(pReader, stringMap);
    return ec;
    //} catch (Exception e) {
    //  Log.e("GeckoCrashReporter", "exception while reading strings: ", e);
    //  return false;
    //}
}

ECode CCrashReporter::ReadStringsFromReader(
    /* [in] */ IBufferedReader* pReader,
    /* [in] */ HashMap<String, String>* stringMap)
{
    DBG_LOG("\t + CCrashReproter::ReadStringsFromReader");
    String line, key, val;
    ECode ec = pReader->ReadLine(&line);
    while (SUCCEEDED(ec) && !line.IsNull()) {
        Int32 equalsPos = -1;
        if ((equalsPos = line.IndexOf('=')) != -1) {
            key = line.Substring(0, equalsPos);
            Unescape(line.Substring(equalsPos + 1), &val);
            (*stringMap)[key] = val;
        }
        ec = pReader->ReadLine(&line);
    }
    ICloseable::Probe(pReader)->Close();
    DBG_LOG("\t - CCrashReproter::ReadStringsFromReader");
    return NOERROR;
}

ECode CCrashReporter::GenerateBoundary(
    /* [out] */ String* boundary)
{
    // Generate some random numbers to fill out the boundary
    Int32 r0 = (Int32)((Double)MAX_INT_VALUE * Elastos::Core::Math::Random());
    Int32 r1 = (Int32)((Double)MAX_INT_VALUE * Elastos::Core::Math::Random());

    String result("---------------------------");
    result.AppendFormat("%08X", r0);
    result.AppendFormat("%08X", r1);
    return NOERROR;
}

ECode CCrashReporter::SendPart(
    /* [in] */ IOutputStream* pOs,
    /* [in] */ const String& boundary,
    /* [in] */ const String& name,
    /* [in] */ const String& data)
{
    //try {
    Int32 bufLen = boundary.GetByteLength()
                + name.GetByteLength() + data.GetByteLength();
    StringBuf* pStringBuf = StringBuf::Alloc(bufLen + 128);
    if (!pStringBuf) return E_OUT_OF_MEMORY;

    pStringBuf->Copy("--");
    pStringBuf->Append(boundary);
    pStringBuf->Append("\r\n");
    pStringBuf->Append("Content-Disposition: form-data; name=\"");
    pStringBuf->Append(name);
    pStringBuf->Append("\"\r\n\r\n");
    pStringBuf->Append(data);
    pStringBuf->Append("\r\n");

    ArrayOf<Byte> buf((Byte*)pStringBuf->GetPayload(), pStringBuf->GetLength());

    ECode ec = pOs->WriteBytes(buf);
    StringBuf::Free(pStringBuf);
    return ec;
    //} catch (Exception ex) {
    //  Log.e("GeckoCrashReporter", "Exception when sending \"" + name + "\"", ex);
    //}
}

ECode CCrashReporter::SendFile(
    /* [in] */ IOutputStream* pOs,
    /* [in] */ const String& boundary,
    /* [in] */ const String& name,
    /* [in] */ IFile* pFile)
{
    String fileName;
    pFile->GetName(&fileName);
    Int32 bufLen = boundary.GetByteLength()
                + name.GetByteLength() + fileName.GetByteLength();
    StringBuf* pStringBuf = StringBuf::Alloc(bufLen + 128);
    if (!pStringBuf) return E_OUT_OF_MEMORY;

    pStringBuf->Copy("--");
    pStringBuf->Append(boundary);
    pStringBuf->Append("\r\n");
    pStringBuf->Append("Content-Disposition: form-data; ");
    pStringBuf->Append("name=\"");
    pStringBuf->Append(name);
    pStringBuf->Append("\"; ");
    pStringBuf->Append("filename=\"");
    pStringBuf->Append(fileName);
    pStringBuf->Append("\"\r\n");
    pStringBuf->Append("Content-Type: application/octet-stream\r\n");
    pStringBuf->Append("\r\n");

    ArrayOf<Byte> buf((Byte*)pStringBuf->GetPayload(), pStringBuf->GetLength());
    ECode ec = pOs->WriteBytes(buf);
    StringBuf::Free(pStringBuf);
    if (FAILED(ec)) return ec;

    AutoPtr<IFileInputStream> pIns;
    ec = CFileInputStream::New(pFile, (IFileInputStream**)&pIns);
    if (FAILED(ec)) return ec;
    AutoPtr<IFileChannel> pInFc;
//    ec = pIns->GetChannel((IFileChannel**)&pInFc);
//    if (FAILED(ec)) return ec;

    assert(0);
    AutoPtr<IFileChannel> pOutFc;
    /*ec = Channels::NewChannel(pOs, (IFileChannel**)&pOutFc);
    if (FAILED(ec)) return ec;*/

    Int64 size, transfered, position = 0;
    pInFc->GetSize(&size);
    ec = pInFc->TransferTo(position,
            size, IWritableByteChannel::Probe(pOutFc), &transfered);
//    pInFc->Close();
//    pOutFc->Close();
    return ec;
}

ECode CCrashReporter::SendReport(
    /* [in] */ IFile* pMinidumpFile,
    /* [in] */ HashMap<String, String> extras,
    /* [in] */ IFile* pExtrasFile)
{
    DBG_LOG("\t + CCrashReporter::SEndReport()");
    String path;
    pMinidumpFile->GetPath(&path);
    // printf("GeckoCrashReport sendReport: %s\n", path.string());

    String spec;
    HashMap<String, String>::Iterator it = extras.Find(String("ServerURL"));
    if (it != extras.End()) {
        spec = it->mSecond;
        if (spec.IsNull()) {
            DoFinish();
            return NOERROR;
        }
        printf("GeckoCrashReport server url: %s\n", spec.string());
    }

    //try {
    ECode ec = NOERROR;
    AutoPtr<IURL> pIUrl;
    String boundary;
    String notes;
    AutoPtr<IHttpURLConnection> pConn;
    HashMap<String, String> responseMap;
    AutoPtr<IOutputStream> pOs;
    AutoPtr<IInputStream> pIns;
    AutoPtr<IBufferedReader> pBr;
    AutoPtr<IInputStreamReader> pInSReader;
    ec = CURL::New(spec, (IURL**)&pIUrl);
    if (FAILED(ec)) goto exit;

    ec = pIUrl->OpenConnection((IURLConnection**)&pConn);
    if (FAILED(ec)) goto exit;

    ec = pConn->SetRequestMethod(String("POST"));
    if (FAILED(ec)) goto exit;

    ec = GenerateBoundary(&boundary);
    if (FAILED(ec)) goto exit;
    ec = pConn->SetDoOutput(TRUE);
    if (FAILED(ec)) goto exit;

    {
        String request("multipart/form-data; boundary=");
        ec = pConn->SetRequestProperty(String("Content-Type"),  request + boundary);
        if (FAILED(ec)) goto exit;
    }

    ec = pConn->GetOutputStream((IOutputStream**)&pOs);
    if (FAILED(ec)) goto exit;

    it = extras.Begin();
    for (; it != extras.End(); ++it) {
        String key = it->mFirst;
        if (key.Equals(kPageURLKey)) {
            Boolean isChecked;
            AutoPtr<IView> pView = FindViewById(0x7f070001);
            ICheckBox* pCheckBox = ICheckBox::Probe(pView);
            ICheckable::Probe(pCheckBox)->IsChecked(&isChecked); //R.id.include_url
            if (isChecked) {
                SendPart(pOs, boundary, key, it->mSecond);
            }
        }
        else if (!key.Equals("ServerURL") && !key.Equals(kNotesKey)) {
            SendPart(pOs, boundary, key, it->mSecond);
        }
    }

    // Add some extra information to notes so its displayed by
    // crash-stats.mozilla.org. Remove this when bug 607942 is fixed.
    it = extras.Find(kNotesKey);
    if (it != extras.End()) {
        notes = it->mSecond + "\n";
    }
    else {
        notes = "";
    }

    notes += Build::MANUFACTURER + " ";
    notes += Build::MODEL + "\n";
    notes += Build::FINGERPRINT;
    SendPart(pOs, boundary, kNotesKey, notes);

    SendPart(pOs, boundary, String("Min_ARM_Version"), String("7"));
    SendPart(pOs, boundary, String("Android_Manufacturer"), Build::MANUFACTURER);
    SendPart(pOs, boundary, String("Android_Model"), Build::MODEL);
    SendPart(pOs, boundary, String("Android_Board"), Build::BOARD);
    SendPart(pOs, boundary, String("Android_Brand"), Build::BRAND);
    SendPart(pOs, boundary, String("Android_Device"), Build::DEVICE);
    SendPart(pOs, boundary, String("Android_Display"), Build::DISPLAY);
    SendPart(pOs, boundary, String("Android_Fingerprint"), Build::FINGERPRINT);
    SendPart(pOs, boundary, String("Android_CPU_ABI"), Build::CPU_ABI);
    if (Build::VERSION::SDK_INT >= 8) {
        //try {
        SendPart(pOs, boundary, String("Android_CPU_ABI2"), Build::CPU_ABI2);
        SendPart(pOs, boundary, String("Android_Hardware"), Build::HARDWARE);
        //} catch (Exception ex) {
        //  Log.e("GeckoCrashReporter", "Exception while sending SDK version 8 keys", ex);
        //}
    }
    {
        String version(StringUtils::Int32ToString(Build::VERSION::SDK_INT));
        version += " (";
        version += Build::VERSION::CODENAME + ")";
        SendPart(pOs, boundary, String("Android_Version"), version);
    }

    SendFile(pOs, boundary, kMiniDumpPathKey, pMinidumpFile);
    {
        String writeStr("\r\n--");
        writeStr += boundary + "--\r\n";
        ArrayOf<Byte> buf((Byte*)writeStr.string(), writeStr.GetByteLength());
        ec = pOs->WriteBytes(buf);
        if (FAILED(ec)) goto exit;
        IFlushable::Probe(pOs)->Flush();
        ICloseable::Probe(pOs)->Close();
    }

    ec = pConn->GetInputStream((IInputStream**)&pIns);
    if (FAILED(ec)) goto exit;
    ec = CInputStreamReader::New(pIns, (IInputStreamReader**)&pInSReader);
    if (FAILED(ec)) goto exit;

    ec = CBufferedReader::New(pInSReader, (IBufferedReader**)&pBr);
    if (FAILED(ec)) goto exit;

    ec = ReadStringsFromReader(pBr, &responseMap);
    if (FAILED(ec)) goto exit;

    Int32 responseCode;
    pConn->GetResponseCode(&responseCode);
    if (responseCode == IHttpURLConnection::HTTP_OK) {
        AutoPtr<IFile> pSubmittedDir;
        String crashid = responseMap[String("CrashID")];
        AutoPtr<IFile> pFile;
        AutoPtr<IFileOutputStream> pFos;
        ec = CFile::New(
          String("/data/data/org.mozilla.fennec_unofficial/mozilla/Crash Reports/submitted"),
          (IFile**)&pSubmittedDir);
        if (FAILED(ec)) goto exit;
        Boolean succeeded;
        pSubmittedDir->Mkdirs(&succeeded);
        pMinidumpFile->Delete(&succeeded);
        pExtrasFile->Delete(&succeeded);

        ec = CFile::New(pSubmittedDir, crashid + ".txt", (IFile**)&pFile);
        if (FAILED(ec)) goto exit;

        ec = CFileOutputStream::New(pFile, (IFileOutputStream**)&pFos);
        if (FAILED(ec)) goto exit;
        ArrayOf_<Byte, 16> titleBuf;
        titleBuf.Copy((Byte*)"Crash ID: ", strlen("Crash ID: "));
        pFos->WriteBytes(titleBuf);
        ArrayOf<Byte> idBuf((Byte*)crashid.string(), crashid.GetByteLength());
        pFos->WriteBytes(idBuf);
        pFos->Close();
    }
    //} catch (IOException e) {
    //  Log.e("GeckoCrashReporter", "exception during send: ", e);
    //}

exit:
    DoFinish();
    DBG_LOG("\t - CCrashReporter::SEndReport()");
    return ec;
}

ECode CCrashReporter::DoRestart()
{
    //try {
    String action("android.intent.action.MAIN");
    AutoPtr<IIntent> pIIntent;
    ECode ec = CIntent::New(action, (IIntent**)&pIIntent);
    if (FAILED(ec)) return ec;
    pIIntent->SetClassName(String("org.mozilla.fennec_unofficial"),
                      String("org.mozilla.fennec_unofficial.App"));
    //printf("GeckoCrashReporter %s\n", pIIntent->ToString());
    ec = StartActivity(pIIntent);
    return ec;
    //} catch (Exception e) {
    //  Log.e("GeckoCrashReporter", "error while trying to restart", e);
    //}
}

ECode CCrashReporter::Unescape(
    /* [in] */ const String& string,
    /* [out] */ String* out)
{
    VALIDATE_NOT_NULL(out);
    String tmp;
    StringUtils::ReplaceAll(string, String("\\\\"), String("\\"), &tmp);
    StringUtils::ReplaceAll(tmp, String("\\n"), String("\n"), &tmp);
    StringUtils::ReplaceAll(tmp, String("\\t"), String("\t"), out);
    return NOERROR;
}

} // namespace Snowfox
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
