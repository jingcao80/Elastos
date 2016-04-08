#include "CMainActivity.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::IO::CFile;
using Elastos::Core::CStringWrapper;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Media::CMediaRecorder;
using Elastos::Droid::Media::IAudioSource;
using Elastos::Droid::Media::IOutputFormat;
using Elastos::Droid::Media::IAudioEncoder;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendar;
using Elastos::Droid::Text::Format::IDateFormat;
using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace RecorderDemo {

CAR_INTERFACE_IMPL(CMainActivity::RecorderClickListener, IViewOnClickListener)

CAR_INTERFACE_IMPL(CMainActivity::StopClickListener, IViewOnClickListener)

ECode CMainActivity::RecorderClickListener::OnClick(
    /* [in] */ IView* v)
{
    Logger::D("CMainActivity::RecorderClickListener", "OnClick, mMediaRecorder = %p",mMediaRecorder);
    AutoPtr<IFile> file;


    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> locale;
    localeHelper->GetCHINA((ILocale**)&locale);
    AutoPtr<ICalendarHelper> calendarHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calendarHelper);
    AutoPtr<ICalendar> calendar;
    calendarHelper->GetInstance(locale, (ICalendar**)&calendar);
    AutoPtr<IDateFormat> df;
    CDateFormat::AcquireSingleton((IDateFormat**)&df);

    AutoPtr<ICharSequence> cs1;
    CStringWrapper::New(String("yyyyMMdd_hhmmss"), (ICharSequence**)&cs1);
    AutoPtr<ICharSequence> cs2;
    df->Format(cs1, calendar, (ICharSequence**)&cs2);

    String fileName;
    StringBuilder sb("/sdcard/");
    sb.AppendCharSequence(cs2);
    sb += ".amr";
    sb.ToString(&fileName);

    CFile::New(fileName, (IFile**)&file);
            // + new DateFormat().format("yyyyMMdd_hhmmss",
                    // Calendar.getInstance(Locale.CHINA)) + ".amr");
    // Toast.makeText(getApplicationContext(), "正在录音，录音文件在"+file.getAbsolutePath(), Toast.LENGTH_LONG).show();
    // 设置输出文件
    String path;
    file->GetAbsolutePath(&path);
    // 从麦克风源进行录音
    mMediaRecorder->SetAudioSource(IAudioSource::DEFAULT);
    // 设置输出格式
    mMediaRecorder->SetOutputFormat(IOutputFormat::DEFAULT);
    // 设置编码格式
    mMediaRecorder->SetAudioEncoder(IAudioEncoder::DEFAULT);

    mMediaRecorder->SetOutputFile(path);
    // try {
        // 创建文件
        Boolean b;
        file->CreateNewFile(&b);
        // 准备录制
    Logger::D("CMainActivity::RecorderClickListener::OnClick", "To Prepare");
        mMediaRecorder->Prepare();
    // } catch (IllegalStateException e) {
    //     e.printStackTrace();
    // } catch (IOException e) {
    //     e.printStackTrace();
    // }
    // 开始录制
    Logger::D("CMainActivity::RecorderClickListener::OnClick", "To Start");
    mMediaRecorder->Start();
    AutoPtr<ICharSequence> csq;
    CStringWrapper::New(String("录音中……"), (ICharSequence**)&csq);
    mRecordButton->SetText(csq);
    mRecordButton->SetEnabled(FALSE);
    return NOERROR;
}

ECode CMainActivity::StopClickListener::OnClick(
    /* [in] */ IView* v)
{
    Logger::D("CMainActivity::StopClickListener", "OnClick, mMediaRecorder = %p",mMediaRecorder);
    if (mMediaRecorder != NULL) {
        mMediaRecorder->Stop();
        // mMediaRecorder->Release();
        // mMediaRecorder = NULL;
        AutoPtr<ICharSequence> csq;
        CStringWrapper::New(String("录音"), (ICharSequence**)&csq);
        mRecordButton->SetText(csq);
        mRecordButton->SetEnabled(TRUE);
        // Toast.makeText(getApplicationContext(), "录音完毕", Toast.LENGTH_LONG).show();
    }
    return NOERROR;
}

ECode CMainActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D("CMainActivity", "OnCreate");

    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<IView> view;
    view = FindViewById(R::id::Button01);
    assert(view != NULL);
    mRecordButton = IButton::Probe(view);
    assert(mRecordButton != NULL);

    view = NULL;
    view = FindViewById(R::id::Button02);
    assert(view != NULL);
    mStopButton = IButton::Probe(view);
    assert(mStopButton != NULL);

    // 创建录音对象
    CMediaRecorder::New((IMediaRecorder**)&mMediaRecorder);

    Logger::D("CMainActivity::OnCreate", "mMediaRecorder = %p",mMediaRecorder.Get());
    AutoPtr<RecorderClickListener> rl = new RecorderClickListener(this, mMediaRecorder, mRecordButton);
    AutoPtr<StopClickListener> sl = new StopClickListener(this, mMediaRecorder, mRecordButton);
    mRecordButton->SetOnClickListener(rl);
    mStopButton->SetOnClickListener(sl);

    return NOERROR;
}

ECode CMainActivity::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CMainActivity::OnRestart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CMainActivity::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CMainActivity::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CMainActivity::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CMainActivity::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}


} // namespace RecorderDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

