
#include "CZigbeeService.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include "zigbee/ZigbeeDoorSensor.h"
#include <hardware/zigbee/zigbee_hal.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;
using Elastos::IO::ICloseable;
using Elastos::IO::IWriter;
using Elastos::IO::IFileWriter;
using Elastos::IO::CFileWriter;
using Elastos::IO::IBufferedWriter;
using Elastos::IO::CBufferedWriter;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IParcelStringList;
using Elastos::Droid::Os::CParcelStringList;
using Elastos::Droid::Os::IZigbeeManager;
using Elastos::Droid::Server::Zigbee::ZigbeeDoorSensor;
using Elastos::Droid::Server::Zigbee::ZigbeeHumiditySensor;
using Elastos::Droid::Server::Zigbee::ZigbeeTHSensor;
using Elastos::Droid::Server::Zigbee::ZigbeeTemperatureSensor;
using Elastos::Utility::CDate;
using Elastos::Utility::Logging::Slogger;
using Org::Kxml2::IO::IKXmlParser;
using Org::Kxml2::IO::CKXmlParser;
using Org::Kxml2::IO::CKXmlSerializer;
using Org::Xmlpull::V1::IXmlPullParser;
// using Org::Xmlpull::V1::IXmlPullParserFactory;
// using Org::Xmlpull::V1::IXmlPullParserFactoryHelper;
// using Org::Xmlpull::V1::CXmlPullParserFactoryHelper;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {

/************************************************************************************
 *  HistoryRecordThread
 ************************************************************************************/

CZigbeeService::HistoryRecordThread::HistoryRecordThread(
    /* [in] */ const String& deviceName,
    /* [in] */ Int32 shortAddr,
    /* [in] */ IDate* date,
    /* [in] */ const String& record,
    /* [in] */ const String& filename,
    /* [in] */ CZigbeeService* host)
    : mDeviceName(deviceName)
    , mShortAddr(shortAddr)
    , mDate(date)
    , mRecord (record)
    , mFilename(filename)
    , mHost(host)
{
    Thread::constructor();
}

ECode CZigbeeService::HistoryRecordThread::Run()
{
    // date for record
    AutoPtr<ISimpleDateFormat> sDateFormat;
    CSimpleDateFormat::New(String("yyyy-MM-dd-H-mm-ss"), (ISimpleDateFormat**)&sDateFormat);
    String fullTime;
    sDateFormat->FormatDate(mDate, &fullTime);

    if (DBG)
        Slogger::D(TAG, "HistoryRecordThread: run(%s, 0x%04x, %s, %s): Start HistoryRecordThread: -thread()",
            mDeviceName.string(), mShortAddr, fullTime.string(), mRecord.string());

    //final record content
    String writeData = fullTime + mHost->mDataSpliter + mRecord;
    String savePath = mHost->mHistoryRecord_InteralSavePath;   //default path
    AutoPtr<IFile> folder;

    //decide target path
    ECode ec = NOERROR;
    do {
        ec = CFile::New(mHost->mHistoryRecord_SataSavePath, (IFile**)&folder);    //check sata availability
        if (FAILED(ec))
            break;
        Boolean isExists;
        if (!(folder->Exists(&isExists), isExists)) {
            Boolean res;
            if (folder->Mkdirs(&res), res) {    //create sata folder
                if (DBG)
                    Slogger::D(TAG, "HistoryRecordThread: run(): \"%s\": create directory \"%s\"",
                        mDeviceName.string(), mHost->mHistoryRecord_SataSavePath.string());
                savePath = mHost->mHistoryRecord_SataSavePath;
            }
            else{
                Slogger::E(TAG, "HistoryRecordThread: run(): \"%s\": \"%s\" is not available, try second path",
                    mDeviceName.string(), mHost->mHistoryRecord_SataSavePath.string());
                folder = NULL;
                ec = CFile::New(mHost->mHistoryRecord_InteralSavePath, (IFile**)&folder); //check internal storage availability
                if (FAILED(ec))
                    break;
                if (!(folder->Exists(&isExists), isExists)) {
                    if (folder->Mkdirs(&res), res) {    //create internal storage folder
                        if (DBG)
                            Slogger::D(TAG, "HistoryRecordThread: run(): \"%s\": create directory \"%s\"",
                                mDeviceName.string(), mHost->mHistoryRecord_InteralSavePath.string());
                        savePath = mHost->mHistoryRecord_InteralSavePath;
                    }
                    else{
                        Slogger::E(TAG, "HistoryRecordThread: run(): \"%s\": can't make directory \"%s\"",
                            mDeviceName.string(), mHost->mHistoryRecord_InteralSavePath.string());
                        Slogger::E(TAG, "HistoryRecordThread: run(): No avaialbe path for history record, Exit");
                        return NOERROR;
                    }
                }
                else{
                    savePath = mHost->mHistoryRecord_InteralSavePath;
                }
            }
        }
        else{   //sata path available
            savePath = mHost->mHistoryRecord_SataSavePath;
        }
        Slogger::I(TAG, "HistoryRecordThread: run(): Target history record save path: \"%s\"", savePath.string());
    } while (0);

    if (FAILED(ec)) {
        // e.printStackTrace();
        Slogger::E(TAG, "HistoryRecordThread: run(): \"%s\": check history record save path exception, Exit", mDeviceName.string());
        return NOERROR;
    }

    String saveFileName = savePath + "/" + mFilename + mHost->mFilenameExtension;
    if (DBG)
        Slogger::D(TAG, "HistoryRecordThread: run(): \"%s\" target save file: %s", mDeviceName.string(), saveFileName.string());
    AutoPtr<IFile> file;
    CFile::New(saveFileName, (IFile**)&file);
    do {
        AutoPtr<IFileWriter> fileWriter;
        ec = CFileWriter::New(file, TRUE, (IFileWriter**)&fileWriter);
        if (FAILED(ec))
            break;
        AutoPtr<IBufferedWriter> writer;
        ec = CBufferedWriter::New(IWriter::Probe(fileWriter), (IBufferedWriter**)&writer);
        if (FAILED(ec))
            break;
        ec = writer->WriteString(writeData + "\n");
        if (FAILED(ec))
            break;
        ICloseable::Probe(writer)->Close();
    } while (0);

    if (FAILED(ec)) {
        // e.printStackTrace();
        Slogger::E(TAG, "HistoryRecordThread: run(): \"%s\": Write history record file exception", mDeviceName.string());
    }
    return NOERROR;
}

/************************************************************************************
 *  BulbScenarioModeThread
 ************************************************************************************/

const Int32 CZigbeeService::BulbScenarioModeThread::SM_RANDOM_COLOR = 1;
const Int32 CZigbeeService::BulbScenarioModeThread::SM_RGB_COLOR = 2;

//mode parameters
const Int32 CZigbeeService::BulbScenarioModeThread::levelStep = 3;
const Int32 CZigbeeService::BulbScenarioModeThread::SMdelayTime = 700;

CZigbeeService::BulbScenarioModeThread::BulbScenarioModeThread(
    /* [in] */ Int32 shortAddr,
    /* [in] */ Int32 mode,
    /* [in] */ CZigbeeService* host)
    : mDestory(FALSE)
    , mMode(mode)
    , mShortAddr(shortAddr)
    , mHost(host)
{
    Thread::constructor();
}

ECode CZigbeeService::BulbScenarioModeThread::Destroy()
{
    mDestory = TRUE;
    return NOERROR;
}

ECode CZigbeeService::BulbScenarioModeThread::Run()
{
    if (DBG)
        Slogger::D(TAG, "BulbScenarioModeThread: run(0x%04x): Start BulbScenarioModeThread: %s-thread() , Destroy Tag = %d",
            mShortAddr, mDeviceName.string(), mDestory);
    Int32 R = 0;
    Int32 G = 0;
    Int32 B = 0;
    Int32 level = 1;
    //Int32[] levelList = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
    //Int32[] levelList = {2,5,7,9,11,13,15,14,13,12,11,10,9,8,7,6,5};
    //Int32[] levelList = {2,5,7,9,11,13,15,13,11,9,7,5};
    Int32 levelList[] = {3,6,8,11,15,11,8,5};
    Int32 levelListLength = 8;
    Int32 RGBList[][3] = {{98,58,230}, {84,136,232}, {56,238,224}, {25,224,125}, {93,181,72}, {160,164,53},
                        {242,154,68}, {249,19,21}, {247,18,123}, {239,0,242}};
    Int32 RGBListLength = 10;

    switch(mMode) {
        case SM_RGB_COLOR:
            // ******************************************* Type A *****************************************************
            R = 0;
            G = 0;
            B = 0;
            level = 0;

            while (TRUE) {
                if (mDestory || mHost->Native_GetDeviceOnlineStatus(mShortAddr) == IZigbeeManager::STATE_OFF) {
                    if (DBG) Slogger::D(TAG, "BulbScenarioModeThread: run(0x%04x): Exit SM_RGB_COLOR thread - while", mShortAddr);
                    return NOERROR;
                }

                for (Int32 i = 0; i < levelListLength; i++) {
                    if (mDestory || mHost->Native_GetDeviceOnlineStatus(mShortAddr) == IZigbeeManager::STATE_OFF) {
                        if (DBG) Slogger::D(TAG, "BulbScenarioModeThread: run(0x%04x): Exit SM_RGB_COLOR thread - for loop", mShortAddr);
                        return NOERROR;
                    }

                    level = levelList[i];
                    if (DBG) Slogger::D(TAG, "BulbScenarioModeThread: run(0x%04x): SM_RGB_COLOR thread: level = %d", mShortAddr, level);
                    mHost->Native_LightSetDim(level, mShortAddr);

                    // try{
                        if (mDestory || mHost->Native_GetDeviceOnlineStatus(mShortAddr) == IZigbeeManager::STATE_OFF) {
                            if (DBG) Slogger::D(TAG, "BulbScenarioModeThread: run(0x%04x):  Exit SM_RGB_COLOR thread - sleep", mShortAddr);
                            return NOERROR;
                        }
                        Thread::Sleep(350);
                    // }
                    // catch (InterruptedException e) {
                    //     e.printStackTrace();
                    // }
                }
            }
            break;

        // ******************************************* Type B *****************************************************
        case SM_RANDOM_COLOR:
            R = 0;
            G = 0;
            B = 0;
            level = 1;

            while (TRUE) {
                if (mDestory || mHost->Native_GetDeviceOnlineStatus(mShortAddr) == IZigbeeManager::STATE_OFF) {
                    if (DBG) Slogger::D(TAG, "BulbScenarioModeThread: run(0x%04x):  Exit SM_RGB_COLOR thread - while", mShortAddr);
                    return NOERROR;
                }

                for(Int32 i = 0; i < RGBListLength; i++) {
                    R = RGBList[i][0];
                    G = RGBList[i][1];
                    B = RGBList[i][2];
                    if (DBG) Slogger::D(TAG, "BulbScenarioModeThread: run(0x%04x):  SM_RGB_COLOR thread: R = %d G = %d B = %d", mShortAddr, R, G, B);

                    //Int32 rgb = (R & 0x00ff0000) + (G & 0x0000ff00) + (B & 0x000000ff);
                    Int32 rgb = ((R << 16) & 0x00ff0000) + ((G << 8) & 0x0000ff00) + (B & 0x000000ff);
                    mHost->Native_LightSetColor(rgb, mShortAddr);

                    // delay
                    // try{
                        if (mDestory || mHost->Native_GetDeviceOnlineStatus(mShortAddr) == IZigbeeManager::STATE_OFF) {
                            if (DBG) Slogger::D(TAG, "BulbScenarioModeThread: run(0x%04x):  Exit SM_RGB_COLOR thread - sleep", mShortAddr);
                            return NOERROR;
                        }
                        Thread::Sleep(2500);
                    // }
                    // catch (InterruptedException e) {
                    //     e.printStackTrace();
                    // }
                }
            }
            break;
    }
    return NOERROR;
}

// *************************************************************************************************************************** //
// Handle callback funtion to application
// Temporal solution: broadcast intent to nofity application as callback function
// *************************************************************************************************************************** //

//[short-term solution for sync state between remote client and local client]
void CZigbeeService::CallbackManager::NotifyDeviceStateChanged()
{
    //update CZigbeeService device data strcuture
    //UpdateZigbeeDevices();

    //setup intent and data in bundle
    AutoPtr<IIntent> intent;
    CIntent::New(IZigbeeManager::ACTION_DEVICE_STATE_CHANGED, (IIntent**)&intent);
    if (mHost->mContext != NULL) {
        if (DBG) Slogger::D(TAG, "CallbackManager: NotifyDeviceStateChanged(): sendBroadcast()");
        mHost->mContext->SendBroadcast(intent);
        //mHost->mContext->SendBroadcastAsUser(intent, new UserHandle(UserHandle.USER_CURRENT));
    }
    else {
        Slogger::E(TAG, "CallbackManager: NotifyDeviceStateChanged(): CZigbeeService's context is NULL");
    }

    if (DBG) Slogger::D(TAG, "CallbackManager: NotifyDeviceStateChanged()");
}

void CZigbeeService::CallbackManager::NotifyDeviceCountChanged()
{
    //update CZigbeeService device data strcuture
    mHost->UpdateZigbeeDevices();

    //setup intent and data in bundle
    AutoPtr<IIntent> intent;
    CIntent::New(IZigbeeManager::ACTION_DEVICE_COUNT_CHANGED, (IIntent**)&intent);
    if (mHost->mContext != NULL) {
        if (DBG) Slogger::D(TAG, "CallbackManager: NotifyDeviceCountChanged(): sendBroadcast()");
        mHost->mContext->SendBroadcast(intent);
        //mHost->mContext->SendBroadcastAsUser(intent, new UserHandle(UserHandle.USER_CURRENT));
    }
    else {
        Slogger::E(TAG, "CallbackManager: NotifyDeviceCountChanged(): CZigbeeService's context is NULL");
    }

    if (DBG) Slogger::D(TAG, "CallbackManager: NotifyDeviceCountChanged()");
}

void CZigbeeService::CallbackManager::NotifyDeviceOnlineStatusChanged(
    /* [in] */ const String& deviceName)
{
    AutoPtr<ZigbeeDevice> zigbeeDevice = mHost->mDevices[deviceName];
    // get updated temperature
    Int32 deviceShortAddr = zigbeeDevice->GetShortAddr();
    Int32 onlineStatus = mHost->Native_GetDeviceOnlineStatus(deviceShortAddr);

    //update device online status
    if (zigbeeDevice->SetOnlineStatus(onlineStatus) == 0) {
        //setup intent and data in bundle
        AutoPtr<IIntent> intent;
        CIntent::New(IZigbeeManager::ACTION_ONLINE_STATUS_CHANGED, (IIntent**)&intent);
        AutoPtr<IBundle> bundle;
        CBundle::New((IBundle**)&bundle);
        bundle->PutString(IZigbeeManager::EXTRA_NAME, deviceName);
        bundle->PutInt32(IZigbeeManager::EXTRA_STATUS, onlineStatus);
        intent->PutExtras(bundle);

        if (mHost->mContext != NULL) {
            if (DBG) Slogger::D(TAG, "CallbackManager: NotifyDeviceOnlineStatusChanged(): sendBroadcast()");
            mHost->mContext->SendBroadcast(intent);
            //mHost->mContext->SendBroadcastAsUser(intent, new UserHandle(UserHandle.USER_CURRENT));
        }
        else {
            Slogger::E(TAG, "CallbackManager: NotifyDeviceOnlineStatusChanged(): CZigbeeService's context is NULL");
        }
    }
    else {
        Slogger::E(TAG, "CallbackManager: NotifyDeviceOnlineStatusChanged(0x%04x), deviceName: %s, get illegal status from HAL: %d",
            deviceShortAddr, deviceName.string(), onlineStatus);
    }

    if (DBG) Slogger::D(TAG, "CallbackManager: NotifyDeviceOnlineStatusChanged(0x%04x), deviceName: %s, online status: %d",
                deviceShortAddr, deviceName.string(), onlineStatus);
}

void CZigbeeService::CallbackManager::NotifySensorHumidityReport(
    /* [in] */ const String& deviceName)
{
    AutoPtr<ZigbeeHumiditySensor> humiditySensor;
    Boolean isUpperWarning = FALSE;
    Boolean isLowerWarning = FALSE;
    Boolean isActionSet = FALSE;
    AutoPtr<IIntent> intent;
    AutoPtr<IBundle> bundle;

    //get current system time
    AutoPtr<IDate> date;
    CDate::New((IDate**)&date);
    AutoPtr<ISimpleDateFormat> sDateFormat;
    CSimpleDateFormat::New(String("yyyy-MM-dd-H-mm-ss"), (ISimpleDateFormat**)&sDateFormat);
    String time;
    sDateFormat->FormatDate(date, &time);
    if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorHumidityReport(): time = %s", time.string());

    if (mHost->mDevices[deviceName]->GetClass() == "ZigbeeHumiditySensor") {
        humiditySensor = (ZigbeeHumiditySensor*)mHost->mDevices[deviceName].Get();
    }
    else if (mHost->mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorHumidityReport(): ZigbeeTHSensor");
        humiditySensor = ((ZigbeeTHSensor*)mHost->mDevices[deviceName].Get())->GetHumiditySensor();
    }
    else {
        Slogger::E(TAG, "CallbackManager: NotifySensorHumidityReport(): device \"%s\" is not a ZigbeeHumiditySensor", deviceName.string());
        return;
    }

    // get updated temperature
    Int32 deviceShortAddr = humiditySensor->GetShortAddr();
    Int32 humidity = mHost->Native_GetSensorHumidity(deviceShortAddr);
    if (humidity == -1) {
        Slogger::E(TAG, "CallbackManager: NotifySensorHumidityReport(): get invalid humidity: %d", humidity );
        return;
    }

    //History Record
    if (humiditySensor->GetHistorySetting()) {
        String record = StringUtils::ToString(humidity);
        AutoPtr<HistoryRecordThread> humidityHistoryRecordThread = new HistoryRecordThread(
            deviceName, deviceShortAddr, date, record, humiditySensor->GetHistoryFileName(), mHost);
        humidityHistoryRecordThread->Start();
    }

    //History Record - short-term solution for history record display
    humiditySensor->AddHistoryRecord(date, humidity);

    //notify application the warning humidity if warning setting is enabled
    if (humiditySensor->GetLowerLimitWarningSetting() && !humiditySensor->GetUpperLimitWarningSetting()) {   //check if lower limit warning is enabled
        isLowerWarning = humiditySensor->IsLowerLimitWarning(humidity);
        if (isLowerWarning) {
            // [short-term solution]for remote client warning , due to remote client can't get callback warning message
            humiditySensor->SetLastAlertTime(time);
            humiditySensor->SetLastAlertHumidity(humidity);

            CIntent::New(IZigbeeManager::ACTION_SENSOR_HUMIDITY_LOWER_LIMIT_WARNING, (IIntent**)&intent);
            isActionSet = TRUE;
            if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorHumidityReport(): send ACTION_SENSOR_HUMIDITY_LOWER_LIMIT_WARNING");
        }
    }
    else if (!humiditySensor->GetLowerLimitWarningSetting() && humiditySensor->GetUpperLimitWarningSetting()) {  //check if upper limit warning is enabled
        isUpperWarning = humiditySensor->IsUpperLimitWarning(humidity);
        if (isUpperWarning) {
            //[short-term solution]for remote client warning , due to remote client can't get callback warning message
            humiditySensor->SetLastAlertTime(time);
            humiditySensor->SetLastAlertHumidity(humidity);

            CIntent::New(IZigbeeManager::ACTION_SENSOR_HUMIDITY_UPPER_LIMIT_WARNING, (IIntent**)&intent);
            isActionSet = TRUE;
            if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorHumidityReport(): send ACTION_SENSOR_HUMIDITY_UPPER_LIMIT_WARNING");
        }
    }
    else if (humiditySensor->GetLowerLimitWarningSetting() && humiditySensor->GetUpperLimitWarningSetting()) {

        isLowerWarning = humiditySensor->IsLowerLimitWarning(humidity);
        isUpperWarning = humiditySensor->IsUpperLimitWarning(humidity);
        if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorHumidityReport(): isLowerWarning/isUpperWarning = %d / %d",
                    isLowerWarning, isUpperWarning);
        if (isLowerWarning && isUpperWarning) {
            // [short-term solution]for remote client warning , due to remote client can't get callback warning message
            humiditySensor->SetLastAlertTime(time);
            humiditySensor->SetLastAlertHumidity(humidity);

            CIntent::New(IZigbeeManager::ACTION_SENSOR_HUMIDITY_WARNING, (IIntent**)&intent);
            isActionSet = TRUE;
            if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorHumidityReport(): send ACTION_SENSOR_HUMIDITY_WARNING");
        }

    }

    if (!isActionSet) {   //normal case: no warning
        CIntent::New(IZigbeeManager::ACTION_SENSOR_HUMIDITY_CHANGED, (IIntent**)&intent);
        if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorHumidityReport(): send ACTION_SENSOR_HUMIDITY_CHANGED");
    }

    //general field
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(IZigbeeManager::EXTRA_NAME, deviceName);
    bundle->PutString(IZigbeeManager::EXTRA_TIME, time);
    bundle->PutInt32(IZigbeeManager::EXTRA_HUMIDITY, humidity);

    if (intent != NULL) {
        intent->PutExtras(bundle);

        if (mHost->mContext != NULL) {
            if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorHumidityReport(): sendBroadcast()");
            mHost->mContext->SendBroadcast(intent);
            //mHost->mContext->SendBroadcastAsUser(intent, new UserHandle(UserHandle.USER_CURRENT));
        }
        else {
            Slogger::E(TAG, "CallbackManager: NotifySensorHumidityReport(): CZigbeeService's context is NULL");
            return;
        }
    }
    else {
        Slogger::E(TAG, "CallbackManager: NotifySensorHumidityReport(): intent is NULL");
        return;
    }


    if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorHumidityReport(0x%04x), deviceName: %s, humidity: %d",
                deviceShortAddr, deviceName.string(), humidity);
}

void CZigbeeService::CallbackManager::NotifySensorTemperatureReport(
    /* [in] */ const String& deviceName)
{
    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    Boolean isUpperWarning = FALSE;
    Boolean isLowerWarning = FALSE;
    Boolean isActionSet = FALSE;
    AutoPtr<IIntent> intent;
    AutoPtr<IBundle> bundle;

    //get current system time
    AutoPtr<IDate> date;
    CDate::New((IDate**)&date);
    AutoPtr<ISimpleDateFormat> sDateFormat;
    CSimpleDateFormat::New(String("yyyy-MM-dd-H-mm-ss"), (ISimpleDateFormat**)&sDateFormat);
    String time;
    sDateFormat->FormatDate(date, &time);
    if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorTemperatureReport(): time %s", time.string());

    if (mHost->mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mHost->mDevices[deviceName].Get();
    }
    else if (mHost->mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorTemperatureReport(): ZigbeeTHSensor");
        tempSensor = ((ZigbeeTHSensor*)mHost->mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "CallbackManager: NotifySensorTemperatureReport(): device \"%s\" is not a ZigbeeTemperatureSensor", deviceName.string());
        return;
    }

    // get updated temperature
    Int32 deviceShortAddr = tempSensor->GetShortAddr();
    Int32 temperature = mHost->Native_GetSensorTemperature(deviceShortAddr);
    if (temperature == 9999) {
        Slogger::E(TAG, "CallbackManager: NotifySensorTemperatureReport(): get invalid temperature: %d", temperature);
        return;
    }

    //History Record
    if (tempSensor->GetHistorySetting()) {
        String record = StringUtils::ToString(temperature);
        AutoPtr<HistoryRecordThread> temperatureHistoryRecordThread = new HistoryRecordThread(
                deviceName, deviceShortAddr, date, record, tempSensor->GetHistoryFileName(), mHost);
        temperatureHistoryRecordThread->Start();
    }

    //History Record - short-term solution for history record display
    tempSensor->AddHistoryRecord(date, temperature);

    //notify application the warning temperature if warning setting is enabled
    if (tempSensor->GetLowerLimitWarningSetting() && !tempSensor->GetUpperLimitWarningSetting()) {   //check if lower limit warning is enabled
        isLowerWarning = tempSensor->IsLowerLimitWarning(temperature);
        if (isLowerWarning) {
            // [short-term solution]for remote client warning , due to remote client can't get callback warning message
            tempSensor->SetLastAlertTime(time);
            tempSensor->SetLastAlertTemperature(temperature);

            CIntent::New(IZigbeeManager::ACTION_SENSOR_TEMPERATURE_LOWER_LIMIT_WARNING, (IIntent**)&intent);
            isActionSet = TRUE;
            if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorTemperatureReport(): send ACTION_SENSOR_TEMPERATURE_LOWER_LIMIT_WARNING");
        }
    }
    else if (!tempSensor->GetLowerLimitWarningSetting() && tempSensor->GetUpperLimitWarningSetting()) {  //check if upper limit warning is enabled
        isUpperWarning = tempSensor->IsUpperLimitWarning(temperature);
        if (isUpperWarning) {
            //[short-term solution]for remote client warning , due to remote client can't get callback warning message
            tempSensor->SetLastAlertTime(time);
            tempSensor->SetLastAlertTemperature(temperature);

            CIntent::New(IZigbeeManager::ACTION_SENSOR_TEMPERATURE_UPPER_LIMIT_WARNING, (IIntent**)&intent);
            isActionSet = TRUE;
            if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorTemperatureReport(): send ACTION_SENSOR_TEMPERATURE_UPPER_LIMIT_WARNING");
        }
    }
    else if (tempSensor->GetLowerLimitWarningSetting() && tempSensor->GetUpperLimitWarningSetting()) {
        isLowerWarning = tempSensor->IsLowerLimitWarning(temperature);
        isUpperWarning = tempSensor->IsUpperLimitWarning(temperature);
        if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorTemperatureReport(): isLowerWarning/isUpperWarning = %d / %d",
                    isLowerWarning, isUpperWarning);
        if (isLowerWarning && isUpperWarning) {
            // [short-term solution]for remote client warning , due to remote client can't get callback warning message
            tempSensor->SetLastAlertTime(time);
            tempSensor->SetLastAlertTemperature(temperature);

            CIntent::New(IZigbeeManager::ACTION_SENSOR_TEMPERATURE_WARNING, (IIntent**)&intent);
            isActionSet = TRUE;
            if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorTemperatureReport(): send ACTION_SENSOR_TEMPERATURE_WARNING");
        }

    }

    if (!isActionSet) {   //normal case: no warning
        CIntent::New(IZigbeeManager::ACTION_SENSOR_TEMPERATURE_CHANGED, (IIntent**)&intent);
        if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorTemperatureReport(): send ACTION_SENSOR_TEMPERATURE_CHANGED");
    }

    //general field
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(IZigbeeManager::EXTRA_NAME, deviceName);
    bundle->PutString(IZigbeeManager::EXTRA_TIME, time);
    bundle->PutInt32(IZigbeeManager::EXTRA_TEMPERATURE, temperature);

    if (intent != NULL) {
        intent->PutExtras(bundle);

        if (mHost->mContext != NULL) {
            if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorTemperatureReport(): sendBroadcast()");
            mHost->mContext->SendBroadcast(intent);
            //mHost->mContext->SendBroadcastAsUser(intent, new UserHandle(UserHandle.USER_CURRENT));
        }
        else {
            Slogger::E(TAG, "CallbackManager: NotifySensorTemperatureReport(): CZigbeeService's context is NULL");
            return;
        }
    }
    else {
        Slogger::E(TAG, "CallbackManager: NotifySensorTemperatureReport(): intent is NULL");
        return;
    }

    if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorTemperatureReport(0x%04x), deviceName: %s, temprature: %d",
                deviceShortAddr, deviceName.string(), temperature);
}

void CZigbeeService::CallbackManager::NotifyDoorOnOffStateChanged(
    /* [in] */ const String& deviceName)
{
    AutoPtr<ZigbeeDoorSensor> doorSensor;
    Boolean isDoorWarning = FALSE;
    AutoPtr<IIntent> intent;
    AutoPtr<IBundle> bundle;

    //get current system time
    AutoPtr<IDate> date;
    CDate::New((IDate**)&date);
    AutoPtr<ISimpleDateFormat> sDateFormat;
    CSimpleDateFormat::New(String("yyyy-MM-dd-H-mm-ss"), (ISimpleDateFormat**)&sDateFormat);
    String time;
    sDateFormat->FormatDate(date, &time);
    if (DBG) Slogger::D(TAG, "CallbackManager: NotifyDoorOnOffStateChanged(): time %s", time.string());

    if (mHost->mDevices[deviceName]->GetClass() == "ZigbeeDoorSensor") {
        doorSensor = (ZigbeeDoorSensor*)mHost->mDevices[deviceName].Get();
    }
    else {
        Slogger::E(TAG, "CallbackManager: NotifyDoorOnOffStateChanged(): device \"%s\" is not a ZigbeeDoorSensor", deviceName.string());
        return;
    }

    // get updated door state
    Int32 deviceShortAddr = doorSensor->GetShortAddr();
    Int32 doorState;
    mHost->GetDoorOnOffState(deviceName, &doorState);
    if (doorState != IZigbeeManager::STATE_ON && doorState != IZigbeeManager::STATE_OFF) {
        Slogger::E(TAG, "CallbackManager: NotifyDoorOnOffStateChanged(): device \"%s\" get invalid door on/off state: %d",
            deviceName.string(), doorState);
        return;
    }

    //History Record
    if (doorSensor->GetHistorySetting()) {
        String record = StringUtils::ToString(doorState);
        AutoPtr<HistoryRecordThread> doorStateHistoryRecordThread = new HistoryRecordThread(
            deviceName, deviceShortAddr, date, record, doorSensor->GetHistoryFileName(), mHost);
        doorStateHistoryRecordThread->Start();
    }

    //History Record - short-term solution for history record display
    doorSensor->AddHistoryRecord(date, doorState);

    // check if door warning is enable and alert
    if (doorSensor->GetWarningSetting()) {
        isDoorWarning = doorSensor->IsDoorWarning(doorState);
        if (isDoorWarning) {
            doorSensor->SetLastAlertTime(time);
            doorSensor->SetLastAlertState(doorState);
            CIntent::New(IZigbeeManager::ACTION_SENSOR_DOOR_WARNING, (IIntent**)&intent);
            if (DBG) Slogger::D(TAG, "CallbackManager: NotifyDoorOnOffStateChanged(): setup action: ACTION_SENSOR_DOOR_WARNING");
        }
        if (DBG) Slogger::D(TAG, "CallbackManager: NotifyDoorOnOffStateChanged(): Warning is enabled, and door warning is NOT triggered.");
    }

    // other case
    if (!(doorSensor->GetWarningSetting() && isDoorWarning)) {
        CIntent::New(IZigbeeManager::ACTION_DOOR_ONOFF_STATE_CHANGED, (IIntent**)&intent);
        if (DBG) Slogger::D(TAG, "CallbackManager: NotifyDoorOnOffStateChanged(): setup action: ACTION_DOOR_ONOFF_STATE_CHANGED");
    }

     //setup intent and data in bundle
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(IZigbeeManager::EXTRA_NAME, deviceName);
    bundle->PutInt32(IZigbeeManager::EXTRA_STATE, doorState);
    bundle->PutString(IZigbeeManager::EXTRA_TIME, time);

    if (intent != NULL) {
        intent->PutExtras(bundle);

        if (mHost->mContext != NULL) {
            if (DBG) Slogger::D(TAG, "CallbackManager: NotifyDoorOnOffStateChanged(): sendBroadcast()");
            mHost->mContext->SendBroadcast(intent);
            //mHost->mContext->SendBroadcastAsUser(intent, new UserHandle(UserHandle.USER_CURRENT));
        }
        else {
            Slogger::E(TAG, "CallbackManager: NotifyDoorOnOffStateChanged(): CZigbeeService's context is NULL");
            return;
        }
    }
    else {
        Slogger::E(TAG, "CallbackManager: NotifyDoorOnOffStateChanged(): intent is NULL");
        return;
    }

    if (DBG) Slogger::D(TAG, "CallbackManager: NotifyDoorOnOffStateChanged(0x%04x), deviceName: %s, door state: %d",
        deviceShortAddr, deviceName.string(), doorState);
}

void CZigbeeService::CallbackManager::NotifySensorVoltageChanged(
    /* [in] */ const String& deviceName)
{
    AutoPtr<ZigbeeDevice> zigbeeDevice = mHost->mDevices[deviceName];
    // get updated voltage
    Int32 deviceShortAddr = zigbeeDevice->GetShortAddr();
    Int32 voltage;
    mHost->GetSensorVoltage(deviceName, &voltage);
    /*
    if (doorState != IZigbeeManager::STATE_ON && doorState != IZigbeeManager::STATE_OFF) {
        Slogger::E(TAG, "CallbackManager: NotifySensorVoltageChanged(): get invalid door on/off state");
        return;
    }
    */

    //setup intent and data in bundle
    AutoPtr<IIntent> intent;
    CIntent::New(IZigbeeManager::ACTION_SENSOR_VOLTAGE_CHANGED, (IIntent**)&intent);
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(IZigbeeManager::EXTRA_NAME, deviceName);
    bundle->PutInt32(IZigbeeManager::EXTRA_VOLTAGE, voltage);
    intent->PutExtras(bundle);

    if (mHost->mContext != NULL) {
        if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorVoltageChanged(): sendBroadcast()");
        mHost->mContext->SendBroadcast(intent);
        //mHost->mContext->SendBroadcastAsUser(intent, new UserHandle(UserHandle.USER_CURRENT));
    }
    else {
        Slogger::E(TAG, "CallbackManager: NotifySensorVoltageChanged(): CZigbeeService's context is NULL");
    }

    if (DBG) Slogger::D(TAG, "CallbackManager: NotifySensorVoltageChanged(0x%04x), deviceName: %s, voltage: %d",
                deviceShortAddr, deviceName.string(), voltage);
}

// ***************************************************************************************************** //
// Callback funtion from JNI
//
// ***************************************************************************************************** //

CZigbeeService::CallbackThread::CallbackThread(
    /* [in] */ Int32 deviceShortAddr,
    /* [in] */ Int32 msgID,
    /* [in] */ CZigbeeService* host)
    : mDeviceShortAddr(deviceShortAddr)
    , mMsgID(msgID)
    , mHost(host)
{
    Thread::constructor();
}

ECode CZigbeeService::CallbackThread::Run()
{
    if (DBG) Slogger::D(TAG, "------------------ JNI_Callback Start ----------------");
    AutoPtr<CallbackManager> callbackMgr;
    String deviceName;
    //if (!Arrays.equals(mDeviceShortAddr, new byte[] {(byte)0xFF, (byte)0xFF})) {   //accept brocast type 0x ff ff
    if (mDeviceShortAddr != StringUtils::ParseInt32(String("0000ffff"), 16)) {   //accept brocast type 0x ff ff
        if (DBG) Slogger::D(TAG, "JNI_Callback(device short address: 0x%04x", mDeviceShortAddr);
        //get device name by short address
        deviceName = mHost->GetNameByShortAddr(mDeviceShortAddr);
        if (deviceName == NULL) {
            Slogger::E(TAG, "JNI_Callback(): no such device of short address: 0x%04x", mDeviceShortAddr);
            if (DBG) Slogger::D(TAG, "------------------ JNI_Callback End ----------------");
            return NOERROR;
        }
    }

    // handle callback messages
    switch(mMsgID) {
        case MSG_ID_NOTHING:    //door closed
            if (DBG) Slogger::D(TAG, "JNI_Callback(device short address: 0x%04x, msg ID: MSG_ID_NOTHING", mDeviceShortAddr);
            callbackMgr = new CallbackManager(mHost);
            callbackMgr->NotifyDoorOnOffStateChanged(deviceName);
            break;

        case MSG_ID_DEVICE_COUNT_CHANGE:    //short address: 0xff ff => broadcast
            if (deviceName == NULL && mDeviceShortAddr == StringUtils::ParseInt32(String("0000ffff"), 16)) {
                if (DBG) Slogger::D(TAG, "JNI_Callback(device short address: 0x%04x, msg ID: MSG_ID_DEVICE_COUNT_CHANGE",mDeviceShortAddr);
                callbackMgr = new CallbackManager(mHost);
                callbackMgr->NotifyDeviceCountChanged();
            }
            break;

        case MSG_ID_ONLINE_COUNT_CHANGE:    //short address: 0xff ff => broadcast
            if (deviceName == NULL && mDeviceShortAddr == StringUtils::ParseInt32(String("0000ffff"), 16)) {
                if (DBG) Slogger::D(TAG, "[JNI_Callback]msg id:  MSG_ID_ONLINE_COUNT_CHANGE");
            }
            break;

        case MSG_ID_DEVICE_ONLINE_CHANGE:
            if (DBG) Slogger::D(TAG, "JNI_Callback(device short address: 0x%04x, msg ID: MSG_ID_DEVICE_ONLINE_CHANGE", mDeviceShortAddr);
            callbackMgr = new CallbackManager(mHost);
            callbackMgr->NotifyDeviceOnlineStatusChanged(deviceName);
            break;

        case MSG_ID_TEMPERATURE_REPORT:
            if (DBG) Slogger::D(TAG, "JNI_Callback(device short address: 0x%04x, msg ID: MSG_ID_TEMPERATURE_REPORT", mDeviceShortAddr);
            callbackMgr = new CallbackManager(mHost);
            callbackMgr->NotifySensorTemperatureReport(deviceName);
            break;

        case MSG_ID_HIMIDITY_REPORT:
            if (DBG) Slogger::D(TAG, "JNI_Callback(device short address: 0x%04x, msg ID: MSG_ID_HIMIDITY_REPORT", mDeviceShortAddr);
            callbackMgr = new CallbackManager(mHost);
            callbackMgr->NotifySensorHumidityReport(deviceName);
            break;

        case MSG_ID_VOLTAGE_REPORT:
            if (DBG) Slogger::D(TAG, "JNI_Callback(device short address: 0x%04x, msg ID: MSG_ID_VOLTAGE_REPORT", mDeviceShortAddr);
            callbackMgr = new CallbackManager(mHost);
            callbackMgr->NotifySensorVoltageChanged(deviceName);
            break;

        case MSG_ID_OPENED_OR_ALARM_1:  //door opened
            if (DBG) Slogger::D(TAG, "JNI_Callback(device short address: 0x%04x, msg ID: MSG_ID_OPENED_OR_ALARM_1", mDeviceShortAddr);
            callbackMgr = new CallbackManager(mHost);
            callbackMgr->NotifyDoorOnOffStateChanged(deviceName);
            break;

        case MSG_ID_OPENED_OR_ALARM_2:
            if (DBG) Slogger::D(TAG, "JNI_Callback(device short address: 0x%04x, msg ID: MSG_ID_OPENED_OR_ALARM_2", mDeviceShortAddr);
            break;

        case MSG_ID_TAMPERED:
            Slogger::D(TAG, "[JNI_Callback]msg id:  MSG_ID_TAMPERED");
            break;

        case MSG_ID_LOW_BATTERY:
            Slogger::D(TAG, "[JNI_Callback]msg id:  MSG_ID_LOW_BATTERY");
            break;

        case MSG_ID_REPORTS:
            Slogger::D(TAG, "[JNI_Callback]msg id:  MSG_ID_REPORTS");
            break;

        case MSG_ID_REPORT_RESTORE:
            Slogger::D(TAG, "[JNI_Callback]msg id:  MSG_ID_REPORT_RESTORE");
            break;

        case MSG_ID_TROUBLE_OR_FAILURE:
            Slogger::D(TAG, "[JNI_Callback]msg id:  MSG_ID_TROUBLE_OR_FAILURE");
            break;

        case MSG_ID_AC_MAINS_FAULT:
            Slogger::D(TAG, "[JNI_Callback]msg id:  MSG_ID_AC_MAINS_FAULT");
            break;

        case MSG_ID_ALARM_FILTER:
            Slogger::D(TAG, "[JNI_Callback]msg id:  MSG_ID_ALARM_FILTER");
            break;
    }

    if (DBG) Slogger::D(TAG, "message ID: 0x%04x", mMsgID);
    if (DBG) Slogger::D(TAG, "---------JNI_Callback(device short address: 0x%04x, Function end -------------------", mDeviceShortAddr);
    if (DBG) Slogger::D(TAG, "------------------ JNI_Callback End ----------------");

    return NOERROR;
}

/************************************************************************************
 *  CZigbeeService
 ***********************************************************************************/

const String CZigbeeService::TAG("CZigbeeService");
//debug
const Boolean CZigbeeService::DBG = FALSE;

static AutoPtr<ArrayOf<String> > StringListToStringArray(
    /* [in] */ List<String>* list)
{
    if (list == NULL)
        return NULL;

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(list->GetSize());
    List<String>::Iterator iter = list->Begin();
    for (Int32 i = 0; iter != list->End(); ++iter, ++i)
        (*array)[i] = *iter;

    return array;
}

CZigbeeService::CZigbeeService()
    : mIsZigbeeServiceReady(FALSE)
    , mDefaultDeviceName("zigbee")
    , NOT_AVAILABLE("NA")
    , mDataSpliter(",")
    , mInfoSpliter("/")
    , mSavePath("/data/kortideZigbee")
    , mHistoryRecord_SataSavePath("/mnt/sata/kortideZigbee")
    , mHistoryRecord_InteralSavePath("/data/kortideZigbee")
    , mFilenameExtension(".csv")
    , mConfigFileName("zigbeeDeviceConfig")
    , mConfigFileExtName(".xml")
    , mConfigGroupSpliter(":")
{
}

ECode CZigbeeService::constructor(
    /* [in] */ IContext* context)
{
    if (DBG)
        Slogger::D(TAG, "init native!!");
    mContext = context;

    //scan all current devices
    InitZigbeeDevices();
    return NOERROR;
}

//check device exist or not
// return value: TRUE - device exist, FALSE - device not exist
Boolean CZigbeeService::IsDeviceExist(
    /* [in] */ const String& deviceName)
{
    if (mDevices.Find(deviceName) != mDevices.End()) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

//check multiple devices exist or not
// return value: TRUE - all devices exist, FALSE - at least one device not exist
Boolean CZigbeeService::IsDevicesExist(
    /* [in] */ const ArrayOf<String>& deviceNameList)
{
    Boolean allFound = TRUE;
    for (Int32 i = 0; i < deviceNameList.GetLength(); i++) {
        if (mDevices.Find(deviceNameList[i]) == mDevices.End()) {
            if (DBG) Slogger::D(TAG, "IsDeviceExist(): device \"%s\" not exist", deviceNameList[i].string());
            allFound = FALSE;
            break;
        }
    }

    return allFound;
}

//check group exist or not
// return value: TRUE - group exist, FALSE - group not exist
Boolean CZigbeeService::IsGroupExist(
    /* [in] */ const String& groupName)
{
    if (mGroups.Find(groupName) != mGroups.End()) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/*
   * Remove a device and the device in all groups(if device belongs to some groups)
   * Parameters: device name
   * Returns: void
*/
void CZigbeeService::RemoveDevice(
    /* [in] */ const String& deviceName)
{
    //remove the device
    if (IsDeviceExist(deviceName)) {
        //stop bulb scenario mode thread if needed
        StopBulbScenarioModeThread(deviceName);

        //remove device
        mDevices.Erase(deviceName);

        //remove device from groups
        HashMap<String, AutoPtr<ZigbeeGroup> >::Iterator iter = mGroups.Begin();
        for (; iter != mGroups.End(); ++iter) {
            AutoPtr<ZigbeeGroup> zigbeeGroup = iter->mSecond;
            if (zigbeeGroup->IsContainDevice(deviceName))
                zigbeeGroup->RemoveDevice(deviceName);
        }
    }
}

/*
   * Rename a device and rename the  device name in all groups(if device belongs to some groups)
   * Parameters: device name, new device name
   * Returns: void
*/
void CZigbeeService::RenameDevice(
    /* [in] */ const String& deviceName,
    /* [in] */ const String& newDeviceName)
{
    //rename the device
    if (IsDeviceExist(deviceName)) {
        mDevices[newDeviceName] = mDevices[deviceName];
        mDevices.Erase(deviceName);
    }

    //rename device from groups
    HashMap<String, AutoPtr<ZigbeeGroup> >::Iterator iter = mGroups.Begin();
    for (; iter != mGroups.End(); ++iter) {
        AutoPtr<ZigbeeGroup> zigbeeGroup = iter->mSecond;
        if (zigbeeGroup->IsContainDevice(deviceName)) {
            zigbeeGroup->RemoveDevice(deviceName);
            zigbeeGroup->AddDevice(newDeviceName);
        }
    }
}


/*
   * Check if device type and group type is the same
   * Argument: (device name, group name)
   * Return value: TRUE - device type equals to group type, FALSE - device type does not equal to group type
*/
Boolean CZigbeeService::CompareType(
    /* [in] */ const String& groupName,
    /* [in] */ const String& deviceName)
{
    if (mDevices.Find(deviceName) == mDevices.End() || mGroups.Find(groupName) == mGroups.End())
        return FALSE;

    Boolean result = TRUE;
    //check if device type is the same with group type
    Int32 deviceType = mDevices[deviceName]->GetType();
    Int32 groupType = mGroups[groupName]->GetType();
    if (deviceType != groupType) {    //compare device type and group type
        if (DBG) Slogger::D(TAG, "compareType(): compare type: device(%s, %d), group(%s, %d): type is not the same",
                    deviceName.string(), deviceType, groupName.string(), groupType);
        result = FALSE;
    }
    else {
        //if device type is SENSOR type => compare device zone type and group zone type
        if (deviceType == IZigbeeManager::ZIGBEE_DEV_TYPE_SENSOR) {
            Int32 deviceZoneType = mDevices[deviceName]->GetZoneType();
            Int32 groupZoneType = mGroups[groupName]->GetZoneType();
            if (deviceZoneType != groupZoneType) {
                if (DBG) Slogger::D(TAG, "compareType(): compare zone type: device(%s, %d), group(%s, %d): type is not the same",
                            deviceName.string(), deviceZoneType, groupName.string(), groupZoneType);
                result = FALSE;
            }
            else {
                if (DBG) Slogger::D(TAG, "compareType(): compare zone type: device(%s, %d), group(%s, %d): type is the same",
                            deviceName.string(), deviceZoneType, groupName.string(), groupZoneType);
            }
        }
        else {
            if (DBG) Slogger::D(TAG, "compareType(): compare type: device(%s, %d), group(%s, %d): type is the same",
                        deviceName.string(), deviceType, groupName.string(), groupType);
        }
    }

    return result;
}

/*
 *  Check if devices' type and group type are the same
 *  Argument: (device name list, group name)
 *  Returns: TRUE - devices' type all equal to group type, FALSE - one or more devices' type does not equal to group type
*/
Boolean CZigbeeService::CompareTypes(
    /* [in] */ const String& groupName,
    /* [in] */ const ArrayOf<String>& deviceNameList)
{
    Boolean result = TRUE;

    for (Int32 i = 0; i < deviceNameList.GetLength(); i++) {
        if (!CompareType(groupName, deviceNameList[i])) {
            result = FALSE;
            break;
        }
    }

    return result;
}

/*
 *  Stop if bulb scenario mode thread is running
 *  Parameters: device name
 *  Returns: void
*/
void CZigbeeService::StopBulbScenarioModeThread(
    /* [in] */ const String& deviceName)
{
    // kill SM thread if exist
    HashMap<String, AutoPtr<BulbScenarioModeThread> >::Iterator find = mBulbSMThreads.Find(deviceName);
    if (find != mBulbSMThreads.End()) {
        if (DBG) Slogger::D(TAG, "StopBulbScenarioModeThread(): kill device \"%s\"- SM thread", deviceName.string());
        find->mSecond->Destroy();
        mBulbSMThreads.Erase(find);
    }
}

void CZigbeeService::DumpDeviceInfo()
{

    if (DBG) Slogger::D(TAG, "=================  dumpDeviceInfo() ================ ");
    HashMap<String, AutoPtr<ZigbeeDevice> >::Iterator iter = mDevices.Begin();
    for (; iter != mDevices.End(); ++iter) {
        AutoPtr<ZigbeeDevice> zigbeeDevice = iter->mSecond;
        //String deviceCategory = "";

        //get all general device information
        String deviceName = zigbeeDevice->GetName();
        Int32 deviceStatus = zigbeeDevice->GetOnlineStatus();
        Int32 shortAddr = zigbeeDevice->GetShortAddr();
        Int64 macAddr = zigbeeDevice->GetMacAddr();
        Int32 deviceType = zigbeeDevice->GetType();
        Int32 deviceZoneType = zigbeeDevice->GetZoneType();

        Slogger::D(TAG, "   device name: %s", deviceName.string());
        Slogger::D(TAG, "   device onlineStatus: %d", deviceStatus);
        Slogger::D(TAG, "   device short address: 0x%04x", shortAddr);
        Slogger::D(TAG, "   device MAC address: 0x%08x", macAddr);
        Slogger::D(TAG, "   device type: 0x%04x", deviceType);
        Slogger::D(TAG, "   device zone type: 0x%04x", deviceZoneType);

        //get other device information based on device type
        if (zigbeeDevice->GetClass() == "ZigbeeTemperatureSensor") {   //temperature sensor
            AutoPtr<ZigbeeTemperatureSensor> tempSensor = (ZigbeeTemperatureSensor*)zigbeeDevice.Get();
            Slogger::D(TAG, "   device lower/upper limit temperature warning setting: %d / %d",
                tempSensor->GetLowerLimitWarningSetting(), tempSensor->GetUpperLimitWarningSetting());
            Slogger::D(TAG, "   device lower/upper limit warning temperature: %d / %d",
                tempSensor->GetWarningTemperatureLowerLimit(), tempSensor->GetWarningTemperatureUpperLimit());
            Slogger::D(TAG, "   device last alert time/temperature: %s / %d",
                tempSensor->GetLastAlertTime().string(), tempSensor->GetLastAlertTemperature());
            Slogger::D(TAG, "   device history record setting: %d", tempSensor->GetHistorySetting());
        }
        else if (zigbeeDevice->GetClass() == "ZigbeeDoorSensor") {   //door sensor
            AutoPtr<ZigbeeDoorSensor> doorSensor = (ZigbeeDoorSensor*)zigbeeDevice.Get();
            Slogger::D(TAG, "   device door warning setting: %d", doorSensor->GetWarningSetting());
            Slogger::D(TAG, "   device door warning state: %d", doorSensor->GetWarningState());
            Slogger::D(TAG, "   device last alert time/alert door state: %s / %d",
                doorSensor->GetLastAlertTime().string(), doorSensor->GetLastAlertState());
            Slogger::D(TAG, "   device history record setting: %d", doorSensor->GetHistorySetting());
        }
        else if (zigbeeDevice->GetClass() == "ZigbeeHumiditySensor") {   //Humidity sensor
            AutoPtr<ZigbeeHumiditySensor> humiditySensor = ((ZigbeeHumiditySensor*)zigbeeDevice.Get());
            Slogger::D(TAG, "   device lower/upper limit humidity warning setting: %d / %d",
                humiditySensor->GetLowerLimitWarningSetting(), humiditySensor->GetUpperLimitWarningSetting());
            Slogger::D(TAG, "   device lower/upper limit warning humidity: %d / %d",
                humiditySensor->GetWarningHumidityLowerLimit(), humiditySensor->GetWarningHumidityUpperLimit());
            Slogger::D(TAG, "   device last alert time/humidity: %s / %d",
                humiditySensor->GetLastAlertTime().string(), humiditySensor->GetLastAlertHumidity());
            Slogger::D(TAG, "   device history record setting: %d", humiditySensor->GetHistorySetting());
        }
        else if (zigbeeDevice->GetClass() == "ZigbeeTHSensor") {   //TH sensor
            AutoPtr<ZigbeeTemperatureSensor> tempSensor = ((ZigbeeTHSensor*)zigbeeDevice.Get())->GetTemperatureSensor();
            Slogger::D(TAG, "   device lower/upper limit temperature warning setting: %d / %d",
                tempSensor->GetLowerLimitWarningSetting(), tempSensor->GetUpperLimitWarningSetting());
            Slogger::D(TAG, "   device lower/upper limit warning temperature: %d / %d",
                tempSensor->GetWarningTemperatureLowerLimit(), tempSensor->GetWarningTemperatureUpperLimit());
            Slogger::D(TAG, "   device last alert time/temperature: %s / %d",
                tempSensor->GetLastAlertTime().string(), tempSensor->GetLastAlertTemperature());
            Slogger::D(TAG, "   device history record setting: %d", tempSensor->GetHistorySetting());

            AutoPtr<ZigbeeHumiditySensor> humiditySensor = ((ZigbeeTHSensor*)zigbeeDevice.Get())->GetHumiditySensor();
            Slogger::D(TAG, "   device lower/upper limit humidity warning setting: %d / %d",
                humiditySensor->GetLowerLimitWarningSetting(), humiditySensor->GetUpperLimitWarningSetting());
            Slogger::D(TAG, "   device lower/upper limit warning humidity: %d / %d",
                humiditySensor->GetWarningHumidityLowerLimit(), humiditySensor->GetWarningHumidityUpperLimit());
            Slogger::D(TAG, "   device last alert time/humidity: %s / %d",
                humiditySensor->GetLastAlertTime().string(), humiditySensor->GetLastAlertHumidity());
        }

        Slogger::D(TAG, " ------------------");
    }

    List<String>::Iterator iter2 = mDeviceNameList->Begin();
    for (; iter2 != mDeviceNameList->End(); ++iter2) {
        Slogger::D(TAG, "   device name List: \"%s\"", iter2->string());
    }
}

void CZigbeeService::DumpGroupInfo()
{
    Slogger::D(TAG, "=================  dumpGroupInfo() ================ ");
    HashMap<String, AutoPtr<ZigbeeGroup> >::Iterator iter = mGroups.Begin();
    for (; iter != mGroups.End(); ++iter) {
        AutoPtr<ZigbeeGroup> zigbeeGroup = iter->mSecond;

        //get all members
        String groupName = zigbeeGroup->GetName();
        Int32 groupType = zigbeeGroup->GetType();
        Int32 groupZoneType = zigbeeGroup->GetZoneType();
        AutoPtr<List<String> > deviceNameList = zigbeeGroup->GetAllDevices();

        Slogger::D(TAG, "   group name: %s", groupName.string());
        Slogger::D(TAG, "   group type: %d", groupType);
        Slogger::D(TAG, "   group zone type: %d", groupZoneType);
        if (deviceNameList->GetSize() == 0) {
            Slogger::D(TAG, "   no device in this group");
        }
        else {
            List<String>::Iterator iter2 = deviceNameList->Begin();
            for (; iter2 != deviceNameList->End(); ++iter2) {
                Slogger::D(TAG, "   device in group: %s", iter2->string());
            }
        }

        Slogger::D(TAG, " ------------------");
    }
}

Int32 CZigbeeService::InitZigbeeDevices()
{
    if (Init_JNI() == FALSE) {
        mIsZigbeeServiceReady = FALSE;
        return 1;
    }
    else {
        mIsZigbeeServiceReady = TRUE;
    }

    mCallbackMgr = new CallbackManager(this);

    Int32 res;
    if ((ReadDeviceConfig(&res), res) == 0) {
        Slogger::I(TAG, "InitZigbeeDevices():  ReadDeviceConfig success, skip init process, do update process");
        UpdateZigbeeDevices();
        return 0;
    }

    Int32 deviceNum = Native_GetDeviceNumber() - 1;   //Art Test: Temp solution
    AutoPtr<ArrayOf<Int32> > shortAddrList = Native_GetAllDeviceShortAddr();
    if (DBG) Slogger::D(TAG, "InitZigbeeDevices():  device count: %d", deviceNum);
    for (Int32 i = 1; i <= deviceNum; i++) {    // Art Test start from 1, 0 is coordinator
        //setup zigbee device general informantion
        StringBuilder sb(mDefaultDeviceName); //device name, default naming: zigbee1, zigbee2 ...
        sb.Append(i);
        String name = sb.ToString();
        Int32 onlineStatus = Native_GetDeviceOnlineStatus((*shortAddrList)[i]);  //device online status
        //Int32 shortAddr = Native_GetDeviceShortAddr(i); //device short address
        Int64 macAddr = Native_GetDeviceMacAddr((*shortAddrList)[i]); //device MAC address
        Int32 type = Native_GetDeviceType((*shortAddrList)[i]); //device type
        Int32 zoneType = Native_GetDeviceZoneType((*shortAddrList)[i]); //device zone type

        if (DBG) {
            Slogger::D(TAG, "InitZigbeeDevices():  device name: %s", name.string());
            Slogger::D(TAG, "InitZigbeeDevices():  device onlineStatus: %d", onlineStatus );
            Slogger::D(TAG, "InitZigbeeDevices():  device short address: 0x%04x", (*shortAddrList)[i]);
            Slogger::D(TAG, "InitZigbeeDevices():  device MAC address: 0x%16X", macAddr);
            Slogger::D(TAG, "InitZigbeeDevices():  device type: 0x%04x", type);
            Slogger::D(TAG, "InitZigbeeDevices():  device zone type: 0x%04x", zoneType);
        }


        // Setup zigbee device extra information based on different device type
        AutoPtr<ZigbeeDevice> zbeDev;
        if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_SENSOR) {
            if (zoneType == IZigbeeManager::ZIGBEE_ZONE_TYPE_TEMPERATURE) { //Temperature sensor
                zbeDev = new ZigbeeTemperatureSensor(name, onlineStatus, (*shortAddrList)[i], macAddr, type, zoneType);
                mDevices[name] = zbeDev;
                if (DBG) Slogger::D(TAG, "InitZigbeeDevices():  add this device to table(temperature sensor type)");
            }
            else if (zoneType == IZigbeeManager::ZIGBEE_ZONE_TYPE_DOOR) {   //Door sensor
                zbeDev = new ZigbeeDoorSensor(name, onlineStatus, (*shortAddrList)[i], macAddr, type, zoneType);
                mDevices[name] = zbeDev;
                if (DBG) Slogger::D(TAG, "InitZigbeeDevices():  add this device to table(door sensor type)");
            }
            else {
                if (DBG) Slogger::D(TAG, "InitZigbeeDevices():  Does not add this device to table(invalid device zone type)");
            }
        }
        else if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_BULB) {    //Bulb
            zbeDev = new ZigbeeDevice(name, onlineStatus, (*shortAddrList)[i], macAddr, type, zoneType);
            mDevices[name] = zbeDev;
            if (DBG) Slogger::D(TAG, "InitZigbeeDevices():  add this device to table(bulb type)");
        }
        else if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_SWITCH) {    //Switch
            zbeDev = new ZigbeeDevice(name, onlineStatus, (*shortAddrList)[i], macAddr, type, zoneType);
            mDevices[name] = zbeDev;
            if (DBG) Slogger::D(TAG, "InitZigbeeDevices():  add this device to table(switch type)");
        }
        else if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_NHR_SENSOR) {    //NHR Sensor
            zbeDev = new ZigbeeTHSensor(name, onlineStatus, (*shortAddrList)[i], macAddr, type, zoneType);
            mDevices[name] = zbeDev;
            if (DBG) Slogger::D(TAG, "InitZigbeeDevices():  add this device to table(NHR sensor type)");
        }
        else {   //invalid type/zone type
            if (DBG) Slogger::D(TAG, "InitZigbeeDevices():  Does not add this device to table(invalid device type)");
        }

    }

    mDeviceNameList = new List<String>();
    HashMap<String, AutoPtr<ZigbeeDevice> >::Iterator iter = mDevices.Begin();
    for (; iter != mDevices.End(); ++iter)
        mDeviceNameList->PushBack(iter->mFirst);

    //dumpLog();

    return 0;
}

/* update Zigbee device from HAL */
Int32 CZigbeeService::UpdateZigbeeDevices()
{
    if (DBG) Slogger::D(TAG, "=================== UpdateZigbeeDevices =======================");
    if (DBG) Slogger::D(TAG, "mIsZigbeeServiceReady == %d", mIsZigbeeServiceReady);
    if (!mIsZigbeeServiceReady) {
        if (DBG) Slogger::D(TAG, "mIsZigbeeServiceReady == FALSE(not ready)");
        if (InitZigbeeDevices() == 1) {
            if (DBG) Slogger::D(TAG, "call InitZigbeeDevices , return 1 failed");
            return 1;
        }
    }
    Boolean isFound = FALSE;
    AutoPtr<List<String> > checkedList = new List<String>(); //matched devices in old device hash map
    Int32 deviceNum = Native_GetDeviceNumber() - 1;   //Art Test: Temp solution  => including coordinator, ex one bulb: device number = 2(one is coordinator)
    if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices():  HAL device count: %d", deviceNum);
    if (deviceNum == -1) {    //if no coordinator
        return 1;
    }

    AutoPtr<ArrayOf<Int32> > shortAddrList = Native_GetAllDeviceShortAddr();


    /*********  compare zigbee service saved device and HAL device************/
    for (Int32 i = 1; i <= deviceNum; i++) {   //Art Test
        isFound = FALSE;

        //setup zigbee device informantion
        Int32 HALShortAddr = (*shortAddrList)[i]; //get device short address from HAL


        HashMap<String, AutoPtr<ZigbeeDevice> >::Iterator iter = mDevices.Begin();
        for (; iter != mDevices.End(); ++iter) {
            String deviceName = iter->mFirst;
            AutoPtr<ZigbeeDevice> zigbeeDevice = iter->mSecond;

            /************* CASE1: device is already in zigbee service **************/
            if (zigbeeDevice->CompareShortAddr(HALShortAddr)) {
                isFound = TRUE;
                //update device information
                zigbeeDevice->SetOnlineStatus(Native_GetDeviceOnlineStatus((*shortAddrList)[i]));  //device online status
                zigbeeDevice->SetMacAddr(Native_GetDeviceMacAddr((*shortAddrList)[i])); //device MAC address
                zigbeeDevice->SetType(Native_GetDeviceType((*shortAddrList)[i])); //device type
                zigbeeDevice->SetZoneType(Native_GetDeviceZoneType((*shortAddrList)[i])); //device type

                checkedList->PushBack(deviceName);
                if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices():  [CASE1]HAL device \"%s\" is already in CZigbeeService", deviceName.string());
                break;
            }

        }

        /***********  CASE2: detect new device **************/
        if (!isFound) {
            if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices():  [CASE2]detected new device from HAL");

            //decide new device name
            String name("");
            Int32 index = 1;
            while(TRUE) {
                StringBuilder sb(mDefaultDeviceName); //device name, default naming: zigbee1, zigbee2 ...
                sb.Append(index);
                String tempName = sb.ToString();
                if (!IsDeviceExist(tempName)) {
                    name = tempName;
                    break;
                }
                index++;
                //if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices(): [CASE2] try device name: " + tempName + ", => used ");

            }
            //if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices():  [CASE2] decide new device name: " + name);

            Int32 onlineStatus = Native_GetDeviceOnlineStatus((*shortAddrList)[i]);  //device online status
            Int64 macAddr = Native_GetDeviceMacAddr((*shortAddrList)[i]); //device MAC address
            Int32 type = Native_GetDeviceType((*shortAddrList)[i]); //device type
            Int32 zoneType = Native_GetDeviceZoneType((*shortAddrList)[i]); //device zone type

            if (DBG) {
                Slogger::D(TAG, "UpdateZigbeeDevices():  ---------- [CASE2] new device info --------");
                Slogger::D(TAG, "UpdateZigbeeDevices():  device name: %s", name.string());
                Slogger::D(TAG, "UpdateZigbeeDevices():  device onlineStatus: %d", onlineStatus );
                Slogger::D(TAG, "UpdateZigbeeDevices():  device short address: 0x%04x", HALShortAddr);
                Slogger::D(TAG, "UpdateZigbeeDevices():  device MAC address: 0x%16X", macAddr);
                Slogger::D(TAG, "UpdateZigbeeDevices():  device type: 0x%04x", type);
                Slogger::D(TAG, "UpdateZigbeeDevices():  device zone type: 0x%04x", zoneType);
            }

            // Setup zigbee device extra information based on different device type
            AutoPtr<ZigbeeDevice> zbeDev;
            if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_SENSOR) {
                if (zoneType == IZigbeeManager::ZIGBEE_ZONE_TYPE_TEMPERATURE) { //Temperature sensor
                    zbeDev = new ZigbeeTemperatureSensor(name, onlineStatus, HALShortAddr, macAddr, type, zoneType);
                    mDevices[name] = zbeDev;
                    if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices():  add this device to table(temperature sensor type)");
                }
                else if (zoneType == IZigbeeManager::ZIGBEE_ZONE_TYPE_DOOR) {   //Door sensor
                    zbeDev = new ZigbeeDoorSensor(name, onlineStatus, HALShortAddr, macAddr, type, zoneType);
                    mDevices[name] = zbeDev;
                    if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices():  add this device to table(door sensor type)");
                }
                else {
                    if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices():  Does not add this device to table(invalid device zone type)");
                }
            }
            else if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_BULB) {    //Bulb
                zbeDev = new ZigbeeDevice(name, onlineStatus, HALShortAddr, macAddr, type, zoneType);
                mDevices[name] = zbeDev;
                if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices():  add this device to table(bulb type)");
            }
            else if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_SWITCH) {    //Switch
                zbeDev = new ZigbeeDevice(name, onlineStatus, HALShortAddr, macAddr, type, zoneType);
                mDevices[name] = zbeDev;
                if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices():  add this device to table(switch type)");
            }
            else if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_NHR_SENSOR) {    //NHR Sensor
                zbeDev = new ZigbeeTHSensor(name, onlineStatus, (*shortAddrList)[i], macAddr, type, zoneType);
                mDevices[name] = zbeDev;
                if (DBG) Slogger::D(TAG, "InitZigbeeDevices():  add this device to table(NHR sensor type)");
            }
            else {   //invalid type/zone type
                if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices():  Does not add this device to table(invalid device type)");
            }

            checkedList->PushBack(name);
        }

    }

    /***********  CASE3: detect if device is reset/lost in HAL***/
    Boolean isExist = FALSE;
    HashMap<String, AutoPtr<ZigbeeDevice> >::Iterator iter = mDevices.Begin();
    while (iter != mDevices.End()) {
        isExist = FALSE;
        String deviceName = iter->mFirst;

        // check if current device list is still exist in HAL
        if (Find(checkedList->Begin(), checkedList->End(), deviceName) != checkedList->End()) {
            isExist = TRUE;
            if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices():  [CASE3] device \"%s\" is checked, no actions.", deviceName.string());
            break;
        }

        //device is no longer exist, remove device and device in group if needed
        if (!isExist) {
            //remove device from groups
            HashMap<String, AutoPtr<ZigbeeGroup> >::Iterator groupIt = mGroups.Begin();
            for (; groupIt != mGroups.End(); ++groupIt) {
                AutoPtr<ZigbeeGroup> zigbeeGroup = groupIt->mSecond;
                if (zigbeeGroup->IsContainDevice(deviceName)) {
                    zigbeeGroup->RemoveDevice(deviceName);
                }
            }

            //remove non exist device
            StopBulbScenarioModeThread(deviceName);
            mDevices.Erase(iter++);

            if (DBG) Slogger::D(TAG, "UpdateZigbeeDevices():  [CASE3] remove device \"%s\"", deviceName.string());
        }
        else {
            ++iter;
        }
    }

    //update DeviceNameList
    mDeviceNameList->Clear();
    iter = mDevices.Begin();
    for (; iter != mDevices.End(); ++iter)
        mDeviceNameList->PushBack(iter->mFirst);

    if (DBG) Slogger::D(TAG, "===================end of UpdateZigbeeDevices =======================");

    return 0;
}

//return value: success - short address of the device
//return value: fail - NULL
Int32 CZigbeeService::GetShortAddrByName(
    /* [in] */ const String& name)
{
    HashMap<String, AutoPtr<ZigbeeDevice> >::Iterator find = mDevices.Find(name);
    if (find != mDevices.End()) {
        return find->mSecond->GetShortAddr();
    }
    else {
        return -1;
    }
}

//Get device name by short address
//Return value: device name- success, NULL - no match short address
String CZigbeeService::GetNameByShortAddr(
    /* [in] */ Int32 shortAddr)
{
    String deviceName;
    HashMap<String, AutoPtr<ZigbeeDevice> >::Iterator iter = mDevices.Begin();
    for (; iter != mDevices.End(); ++iter) {
        AutoPtr<ZigbeeDevice> zigbeeDevice = iter->mSecond;
        if (zigbeeDevice->CompareShortAddr(shortAddr)) {
            deviceName = zigbeeDevice->GetName();
            break;
        }

    }

    if (DBG) Slogger::D(TAG, "GetNameByShortAddr(0x%04x) return %s", shortAddr, deviceName.string());
    return deviceName;
}

/* -------------------------------------- API for Application   ------------------------------------- */

/*
 *  Save device/group configuration
 *  Parameters:
 *  Returns: 0 - success, 1 - fail
*/
ECode CZigbeeService::SaveDeviceConfig(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = 1;
    //return if no device
    if (mDevices.GetSize() == 0) {
        Slogger::I(TAG, "saveDeviceInfo(): no device to record");
        *res = 0;
        return NOERROR;
    }

    AutoPtr<IFile> folder;
    CFile::New(mSavePath, (IFile**)&folder);
    Boolean isExist;
    if (!(folder->Exists(&isExist), isExist)) {
        Boolean ret;
        if (folder->Mkdirs(&ret), ret) {
            if (DBG) Slogger::D(TAG, "saveDeviceInfo(): create directory \"%s\"", mSavePath.string());
        }
        else {
            Slogger::E(TAG, "saveDeviceInfo(): can't make directory \"%s\"", mSavePath.string());
            return NOERROR;
        }
    }

    String targetFile = mSavePath + "/" + mConfigFileName + mConfigFileExtName;

    AutoPtr<IFile> newxmlfile;
    CFile::New(targetFile, (IFile**)&newxmlfile);
    if (!(newxmlfile->Exists(&isExist), isExist)) {
        Boolean ret;
        if (FAILED(newxmlfile->CreateNewFile(&ret))) {
            Slogger::E(TAG, "create new device information xml file exception: %s", targetFile.string());
            return NOERROR;
        }
    }

    //bind the new file with a FileOutputStream
    AutoPtr<IFileOutputStream> fileos;
    if (FAILED(CFileOutputStream::New(newxmlfile, (IFileOutputStream**)&fileos))) {
        Slogger::E(TAG, "Can't write device information xml: %s", targetFile.string());
        return NOERROR;
    }
    Slogger::I(TAG, "save device information xml file: %s", targetFile.string());

    //create a XmlSerializer in order to write xml data
    AutoPtr<IXmlSerializer> serializer ;// serializer = Xml::NewSerializer();
    CKXmlSerializer::New((IXmlSerializer**)&serializer);
    ECode ec;
    do {
        //set the FileOutputStream as output for the serializer, using UTF-8 encoding
        ec = serializer->SetOutput(fileos, String("UTF-8"));
        if (FAILED(ec))
            break;
        //Write <?xml declaration with encoding (if encoding not NULL) and standalone flag (if standalone not NULL)
        AutoPtr<IBoolean> bv;
        CBoolean::New(TRUE, (IBoolean**)&bv);
        ec = serializer->StartDocument(String(NULL), bv);
        if (FAILED(ec))
            break;
        //set indentation option
        ec = serializer->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE);
        if (FAILED(ec))
            break;

        //Device information
        ec = serializer->WriteStartTag(String(NULL), String("ZigbeeDevice"));
        if (FAILED(ec))
            break;

        HashMap<String, AutoPtr<ZigbeeDevice> >::Iterator iter = mDevices.Begin();
        for (; iter != mDevices.End(); ++iter) {
            AutoPtr<ZigbeeDevice> zigbeeDevice = iter->mSecond;
            Int32 type = -1;
            Int32 zoneType = -1;

            ec = serializer->WriteStartTag(String(NULL), String("Device"));
            if (FAILED(ec))
                break;

            ec = serializer->WriteStartTag(String(NULL), String("Name"));
            if (FAILED(ec))
                break;
            ec = serializer->WriteText(zigbeeDevice->GetName());
            if (FAILED(ec))
                break;
            ec = serializer->WriteEndTag(String(NULL), String("Name"));
            if (FAILED(ec))
                break;

            ec = serializer->WriteStartTag(String(NULL), String("OnlineStatus"));
            if (FAILED(ec))
                break;
            ec = serializer->WriteText(StringUtils::Int32ToString(zigbeeDevice->GetOnlineStatus()));
            if (FAILED(ec))
                break;
            ec = serializer->WriteEndTag(String(NULL), String("OnlineStatus"));
            if (FAILED(ec))
                break;

            ec = serializer->WriteStartTag(String(NULL), String("ShortAddress"));
            if (FAILED(ec))
                break;
            ec = serializer->WriteText(StringUtils::Int32ToString(zigbeeDevice->GetShortAddr()));
            if (FAILED(ec))
                break;
            ec = serializer->WriteEndTag(String(NULL), String("ShortAddress"));
            if (FAILED(ec))
                break;

            ec = serializer->WriteStartTag(String(NULL), String("MacAddress"));
            if (FAILED(ec))
                break;
            ec = serializer->WriteText(StringUtils::Int64ToString(zigbeeDevice->GetMacAddr()));
            if (FAILED(ec))
                break;
            ec = serializer->WriteEndTag(String(NULL), String("MacAddress"));
            if (FAILED(ec))
                break;

            ec = serializer->WriteStartTag(String(NULL), String("Type"));
            if (FAILED(ec))
                break;
            type = zigbeeDevice->GetType();
            ec = serializer->WriteText(StringUtils::Int32ToString(type));
            if (FAILED(ec))
                break;
            ec = serializer->WriteEndTag(String(NULL), String("Type"));
            if (FAILED(ec))
                break;

            ec = serializer->WriteStartTag(String(NULL), String("ZoneType"));
            if (FAILED(ec))
                break;
            zoneType = zigbeeDevice->GetZoneType();
            ec = serializer->WriteText(StringUtils::Int32ToString(zoneType));
            if (FAILED(ec))
                break;
            ec = serializer->WriteEndTag(String(NULL), String("ZoneType"));
            if (FAILED(ec))
                break;

            if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_SENSOR) {
                if (zoneType == IZigbeeManager::ZIGBEE_ZONE_TYPE_TEMPERATURE) { //Temperature sensor
                    if (DBG) Slogger::D(TAG, "saveDeviceConfig():  detect this device : temperature sensor type");
                    AutoPtr<ZigbeeTemperatureSensor> temp = (ZigbeeTemperatureSensor*)zigbeeDevice.Get();

                    //Warning Setting
                    ec = serializer->WriteStartTag(String(NULL), String("LowerLimitWarningEnableSetting"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(StringUtils::BooleanToString(temp->GetLowerLimitWarningSetting()));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("LowerLimitWarningEnableSetting"));
                    if (FAILED(ec))
                        break;

                    ec = serializer->WriteStartTag(String(NULL), String("UpperLimitWarningEnableSetting"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(StringUtils::BooleanToString(temp->GetUpperLimitWarningSetting()));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("UpperLimitWarningEnableSetting"));
                    if (FAILED(ec))
                        break;

                    ec = serializer->WriteStartTag(String(NULL), String("LowerLimitWarningTemperature"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(StringUtils::Int32ToString(temp->GetWarningTemperatureLowerLimit()));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("LowerLimitWarningTemperature"));
                    if (FAILED(ec))
                        break;

                    ec = serializer->WriteStartTag(String(NULL), String("UpperLimitWarningTemperature"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(StringUtils::Int32ToString(temp->GetWarningTemperatureUpperLimit()));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("UpperLimitWarningTemperature"));
                    if (FAILED(ec))
                        break;

                    ec = serializer->WriteStartTag(String(NULL), String("LastWarningTime"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(temp->GetLastAlertTime());
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("LastWarningTime"));
                    if (FAILED(ec))
                        break;

                    ec = serializer->WriteStartTag(String(NULL), String("LastWarningTemperature"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(StringUtils::Int32ToString(temp->GetLastAlertTemperature()));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("LastWarningTemperature"));
                    if (FAILED(ec))
                        break;

                    //History Record Setting
                    ec = serializer->WriteStartTag(String(NULL), String("HistoryRecordEnableSetting"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(StringUtils::BooleanToString(temp->GetHistorySetting()));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("HistoryRecordEnableSetting"));
                    if (FAILED(ec))
                        break;

                    ec = serializer->WriteStartTag(String(NULL), String("HistoryRecordFile"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(temp->GetHistoryFileName());
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("HistoryRecordFile"));
                    if (FAILED(ec))
                        break;

                }
                else if (zoneType == IZigbeeManager::ZIGBEE_ZONE_TYPE_DOOR) {   //Door sensor
                    if (DBG) Slogger::D(TAG, "saveDeviceConfig():  add this device to table(door sensor type)");
                    AutoPtr<ZigbeeDoorSensor> door = (ZigbeeDoorSensor*)zigbeeDevice.Get();

                    //Warning Setting
                    ec = serializer->WriteStartTag(String(NULL), String("WarningEnableSetting"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(StringUtils::BooleanToString(door->GetWarningSetting()));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("WarningEnableSetting"));
                    if (FAILED(ec))
                        break;

                    ec = serializer->WriteStartTag(String(NULL), String("WarningState"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(StringUtils::Int32ToString(door->GetWarningState()));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("WarningState"));
                    if (FAILED(ec))
                        break;

                    ec = serializer->WriteStartTag(String(NULL), String("LastWarningTime"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(door->GetLastAlertTime());
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("LastWarningTime"));
                    if (FAILED(ec))
                        break;

                    ec = serializer->WriteStartTag(String(NULL), String("LastWarningState"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(StringUtils::Int32ToString(door->GetLastAlertState()));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("LastWarningState"));
                    if (FAILED(ec))
                        break;

                    //History Record Setting
                    ec = serializer->WriteStartTag(String(NULL), String("HistoryRecordEnableSetting"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(StringUtils::BooleanToString(door->GetHistorySetting()));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("HistoryRecordEnableSetting"));
                    if (FAILED(ec))
                        break;

                    ec = serializer->WriteStartTag(String(NULL), String("HistoryRecordFile"));
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteText(door->GetHistoryFileName());
                    if (FAILED(ec))
                        break;
                    ec = serializer->WriteEndTag(String(NULL), String("HistoryRecordFile"));
                    if (FAILED(ec))
                        break;
                }
                else {
                    if (DBG) Slogger::D(TAG, "saveDeviceConfig():  Does not add this device to table(invalid device zone type)");
                }
            }
            else if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_BULB) {    //Bulb
                if (DBG) Slogger::D(TAG, "saveDeviceConfig():  add this device to table(bulb type)");
            }
            else if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_SWITCH) {    //Switch
                if (DBG) Slogger::D(TAG, "saveDeviceConfig():  add this device to table(switch type)");
            }
            else if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_NHR_SENSOR) {    //TH Sensor
                if (DBG) Slogger::D(TAG, "saveDeviceConfig():  detect this device : TH sensor type");

                //--- Temperature Warning Setting ---
                AutoPtr<ZigbeeTemperatureSensor> temp = ((ZigbeeTHSensor*)zigbeeDevice.Get())->GetTemperatureSensor();

                ec = serializer->WriteStartTag(String(NULL), String("LowerLimitTemperatureWarningEnableSetting"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::BooleanToString(temp->GetLowerLimitWarningSetting()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("LowerLimitTemperatureWarningEnableSetting"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("UpperLimitTemperatureWarningEnableSetting"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::BooleanToString(temp->GetUpperLimitWarningSetting()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("UpperLimitTemperatureWarningEnableSetting"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("LowerLimitWarningTemperature"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::Int32ToString(temp->GetWarningTemperatureLowerLimit()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("LowerLimitWarningTemperature"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("UpperLimitWarningTemperature"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::Int32ToString(temp->GetWarningTemperatureUpperLimit()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("UpperLimitWarningTemperature"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("LastTemperatureWarningTime"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(temp->GetLastAlertTime());
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("LastTemperatureWarningTime"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("LastWarningTemperature"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::Int32ToString(temp->GetLastAlertTemperature()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("LastWarningTemperature"));
                if (FAILED(ec))
                    break;

                //Temperature History Record Setting
                ec = serializer->WriteStartTag(String(NULL), String("TemperatureHistoryRecordEnableSetting"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::BooleanToString(temp->GetHistorySetting()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("TemperatureHistoryRecordEnableSetting"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("TemperatureHistoryRecordFile"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(temp->GetHistoryFileName());
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("TemperatureHistoryRecordFile"));
                if (FAILED(ec))
                    break;

                //--- Humidity Warning Setting ---
                AutoPtr<ZigbeeHumiditySensor> humidity = ((ZigbeeTHSensor*)zigbeeDevice.Get())->GetHumiditySensor();

                ec = serializer->WriteStartTag(String(NULL), String("LowerLimitHumidityWarningEnableSetting"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::BooleanToString(humidity->GetLowerLimitWarningSetting()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("LowerLimitHumidityWarningEnableSetting"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("UpperLimitHumidityWarningEnableSetting"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::BooleanToString(humidity->GetUpperLimitWarningSetting()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("UpperLimitHumidityWarningEnableSetting"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("LowerLimitWarningHumidity"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::Int32ToString(humidity->GetWarningHumidityLowerLimit()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("LowerLimitWarningHumidity"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("UpperLimitWarningHumidity"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::Int32ToString(humidity->GetWarningHumidityUpperLimit()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("UpperLimitWarningHumidity"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("LastHumidityWarningTime"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(humidity->GetLastAlertTime());
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("LastHumidityWarningTime"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("LastWarningHumidity"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::Int32ToString(humidity->GetLastAlertHumidity()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("LastWarningHumidity"));
                if (FAILED(ec))
                    break;

                //Humidity History Record Setting
                ec = serializer->WriteStartTag(String(NULL), String("HumidityHistoryRecordEnableSetting"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::BooleanToString(humidity->GetHistorySetting()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("HumidityHistoryRecordEnableSetting"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("HumidityHistoryRecordFile"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(humidity->GetHistoryFileName());
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("HumidityHistoryRecordFile"));
                if (FAILED(ec))
                    break;
            }
            else {   //invalid type/zone type
                if (DBG) Slogger::D(TAG, "saveDeviceConfig():  Does not add this device to table(invalid device type)");
            }

            ec = serializer->WriteEndTag(String(NULL), String("Device"));
        }

        if (FAILED(ec))
            break;

        ec = serializer->WriteEndTag(String(NULL), String("ZigbeeDevice"));
        if (FAILED(ec))
            break;

        //Group information
        if (mGroups.GetSize() > 0) {
            ec = serializer->WriteStartTag(String(NULL), String("ZigbeeGroup"));
            if (FAILED(ec))
                break;
            HashMap<String, AutoPtr<ZigbeeGroup> >::Iterator groupIt = mGroups.Begin();
            for (; groupIt != mGroups.End(); ++groupIt) {
                AutoPtr<ZigbeeGroup> zigbeeGroup = groupIt->mSecond;
                ec = serializer->WriteStartTag(String(NULL), String("Group"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("Name"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(zigbeeGroup->GetName());
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("Name"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("Type"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::Int32ToString(zigbeeGroup->GetType()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("Type"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("ZoneType"));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteText(StringUtils::Int32ToString(zigbeeGroup->GetZoneType()));
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("ZoneType"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteStartTag(String(NULL), String("DeviceInGroup"));
                if (FAILED(ec))
                    break;
                String deviceList("");
                AutoPtr<List<String> > devices = zigbeeGroup->GetAllDevices();
                List<String>::Iterator listIter = devices->Begin();
                while (listIter != devices->End()) {
                    deviceList += *listIter;
                    ++listIter;
                    if (listIter != devices->End()) {
                        deviceList += mConfigGroupSpliter;
                    }
                }
                ec = serializer->WriteText(deviceList);
                if (FAILED(ec))
                    break;
                ec = serializer->WriteEndTag(String(NULL), String("DeviceInGroup"));
                if (FAILED(ec))
                    break;

                ec = serializer->WriteEndTag(String(NULL), String("Group"));
                if (FAILED(ec))
                    break;
            }
            if (FAILED(ec))
                break;

            ec = serializer->WriteEndTag(String(NULL), String("ZigbeeGroup"));
            if (FAILED(ec))
                break;
        }

        ec = serializer->EndDocument();
        if (FAILED(ec))
            break;
        //write xml data into the FileOutputStream
        ec = serializer->Flush();
        if (FAILED(ec))
            break;
        //finally we close the file stream
        ec = fileos->Close();
    } while(0);

    if (FAILED(ec)) {
        // e.printStackTrace();
        Slogger::E(TAG,"error occurred while writing xml file");
        return NOERROR;
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Read device/group configuration
 *  Parameters:
 *  Returns: 0 - success, 1 - fail
*/
ECode CZigbeeService::ReadDeviceConfig(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = 1;
    ECode ec = NOERROR;
    do {
        // AutoPtr<IXmlPullParserFactoryHelper> factoryHelper;
        // CXmlPullParserFactoryHelper::AcquireSingleton((IXmlPullParserFactoryHelper**)&factoryHelper)
        // AutoPtr<IXmlPullParserFactory> factory;
        // factoryHelper->NewInstance((IXmlPullParserFactory**)&factory);
        // factory->SetNamespaceAware(TRUE);
        // AutoPtr<IXmlPullParser> xpp;
        // ec = factory->NewPullParser((IXmlPullParser**)&xpp);
        AutoPtr<IKXmlParser> xpp;
        ec = CKXmlParser::New((IKXmlParser**)&xpp);
        if (FAILED(ec))
            break;

        String targetFile = mSavePath + "/" + mConfigFileName + mConfigFileExtName;
        AutoPtr<IFile> xmlfile;
        ec = CFile::New(targetFile, (IFile**)&xmlfile);
        if (FAILED(ec))
            break;
        Boolean isExist;
        if (!(xmlfile->Exists(&isExist), isExist)) {
            Slogger::E(TAG, "File is not exist: %s", targetFile.string());
            return NOERROR;
        }
        AutoPtr<IFileInputStream> fileis;
        if (FAILED(CFileInputStream::New(xmlfile, (IFileInputStream**)&fileis))) {
            // e.printStackTrace();
            Slogger::E(TAG, "Can't read device information xml: %s", targetFile.string());
            return NOERROR;
        }
        Slogger::I(TAG, "read device information xml file: %s", targetFile.string());


        xpp->SetInput(fileis, String("UTF-8"));
        if (FAILED(ec))
            break;

        //clean deivce list
        mDevices.Clear();
        mGroups.Clear();
        AutoPtr<ArrayOf<String> > splitStr;

        //Check if no device info
        Int32 next;
        if ((xpp->Next(&next), next) != IXmlPullParser::START_TAG) {
            Slogger::I(TAG, "No device info in zigbee info xml: Exit");
            *res = 0;
            return NOERROR;
        }

        ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("ZigbeeDevice"));
        if (FAILED(ec))
            break;
        Int32 nextTag;
        String nextText;
        while ((xpp->NextTag(&nextTag), nextTag) == IXmlPullParser::START_TAG) {
            ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("Device"));
            if (FAILED(ec))
                break;

            //Name
            ec = xpp->NextTag(&nextTag);
            if (FAILED(ec))
                break;
            ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("Name"));
            if (FAILED(ec))
                break;
            ec = xpp->NextText(&nextText);
            if (FAILED(ec))
                break;
            String name = nextText;
            if (DBG) Slogger::D(TAG, "Name: %s", name.string());
            ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("Name"));
            if (FAILED(ec))
                break;

            //Online status
            ec = xpp->NextTag(&nextTag);
            if (FAILED(ec))
                break;
            ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("OnlineStatus"));
            if (FAILED(ec))
                break;
            ec = xpp->NextText(&nextText);
            if (FAILED(ec))
                break;
            Int32 onlineStatus = StringUtils::ParseInt32(nextText);
            if (DBG) Slogger::D(TAG, "OnlineStatus: %d", onlineStatus);
            ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("OnlineStatus"));
            if (FAILED(ec))
                break;

            //Short address
            ec = xpp->NextTag(&nextTag);
            if (FAILED(ec))
                break;
            ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("ShortAddress"));
            if (FAILED(ec))
                break;
            ec = xpp->NextText(&nextText);
            if (FAILED(ec))
                break;
            Int32 shortAddr = StringUtils::ParseInt32(nextText);
            if (DBG) Slogger::D(TAG, "ShortAddr: 0x%04X", shortAddr);
            ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("ShortAddress"));
            if (FAILED(ec))
                break;

            //Mac address
            ec = xpp->NextTag(&nextTag);
            if (FAILED(ec))
                break;
            ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("MacAddress"));
            if (FAILED(ec))
                break;
            ec = xpp->NextText(&nextText);
            if (FAILED(ec))
                break;
            Int64 macAddr = StringUtils::ParseInt32(nextText);
            if (DBG) Slogger::D(TAG, "MacAddr: 0x%16X", macAddr);
            ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("MacAddress"));
            if (FAILED(ec))
                break;

            //Type
            ec = xpp->NextTag(&nextTag);
            if (FAILED(ec))
                break;
            ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("Type"));
            if (FAILED(ec))
                break;
            ec = xpp->NextText(&nextText);
            if (FAILED(ec))
                break;
            Int32 type = StringUtils::ParseInt32(nextText);
            if (DBG) Slogger::D(TAG, "Type: 0x%04X", type);
            ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("Type"));
            if (FAILED(ec))
                break;

            //Zone Type
            ec = xpp->NextTag(&nextTag);
            if (FAILED(ec))
                break;
            ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("ZoneType"));
            if (FAILED(ec))
                break;
            ec = xpp->NextText(&nextText);
            if (FAILED(ec))
                break;
            Int32 zoneType = StringUtils::ParseInt32(nextText);
            if (DBG) Slogger::D(TAG, "ZoneType: 0x%04X", zoneType);
            ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("ZoneType"));
            if (FAILED(ec))
                break;


            //Create device object
            AutoPtr<ZigbeeDevice> zbeDev;
            if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_SENSOR) {
                if (zoneType == IZigbeeManager::ZIGBEE_ZONE_TYPE_TEMPERATURE) { //Temperature sensor
                    // warning setting
                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LowerLimitWarningEnableSetting"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    Boolean lowerLimitWarningSetting = nextText.EqualsIgnoreCase("TRUE");
                    if (DBG) Slogger::D(TAG, "LowerLimitWarningEnableSetting: %d", lowerLimitWarningSetting);
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LowerLimitWarningEnableSetting"));
                    if (FAILED(ec))
                        break;

                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("UpperLimitWarningEnableSetting"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    Boolean upperLimitWarningSetting = nextText.EqualsIgnoreCase("TRUE");
                    if (DBG) Slogger::D(TAG, "UpperLimitWarningEnableSetting: %d", upperLimitWarningSetting);
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("UpperLimitWarningEnableSetting"));
                    if (FAILED(ec))
                        break;

                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LowerLimitWarningTemperature"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    Int32 lowerLimitWarningTemperature = StringUtils::ParseInt32(nextText);
                    if (DBG) Slogger::D(TAG, "LowerLimitWarningTemperature: %d", lowerLimitWarningTemperature);
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LowerLimitWarningTemperature"));
                    if (FAILED(ec))
                        break;

                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("UpperLimitWarningTemperature"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    Int32 upperLimitWarningTemperature = StringUtils::ParseInt32(nextText);
                    if (DBG) Slogger::D(TAG, "UpperLimitWarningTemperature: %d", upperLimitWarningTemperature);
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("UpperLimitWarningTemperature"));
                    if (FAILED(ec))
                        break;

                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LastWarningTime"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    String lastWarningTime = nextText;
                    if (DBG) Slogger::D(TAG, "LastWarningTime: %s", lastWarningTime.string());
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LastWarningTime"));
                    if (FAILED(ec))
                        break;

                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LastWarningTemperature"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    Int32 lastWarningTemperature = StringUtils::ParseInt32(nextText);
                    if (DBG) Slogger::D(TAG, "LastWarningTemperature: %d", lastWarningTemperature);
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LastWarningTemperature"));
                    if (FAILED(ec))
                        break;

                    //History Setting
                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("HistoryRecordEnableSetting"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    Boolean historyRecordSetting = nextText.EqualsIgnoreCase("TRUE");
                    if (DBG) Slogger::D(TAG, "HistoryRecordEnableSetting: %d", historyRecordSetting);
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("HistoryRecordEnableSetting"));
                    if (FAILED(ec))
                        break;

                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("HistoryRecordFile"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    String histoyrRecordFile = nextText;
                    if (DBG) Slogger::D(TAG, "HistoryRecordFile: %s", histoyrRecordFile.string());
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("HistoryRecordFile"));
                    if (FAILED(ec))
                        break;

                    zbeDev = new ZigbeeTemperatureSensor(name, onlineStatus, shortAddr, macAddr, type, zoneType);
                    mDevices[name] = zbeDev;

                    //restore temperature warning/history setting
                    AutoPtr<ZigbeeTemperatureSensor> tempSensor = (ZigbeeTemperatureSensor*)zbeDev.Get();
                    tempSensor->SetLowerLimitWarningSetting(lowerLimitWarningSetting);
                    tempSensor->SetUpperLimitWarningSetting(upperLimitWarningSetting);
                    tempSensor->SetWarningTemperatureLowerLimit(lowerLimitWarningTemperature);
                    tempSensor->SetWarningTemperatureUpperLimit(upperLimitWarningTemperature);
                    tempSensor->SetLastAlertTime(lastWarningTime);
                    tempSensor->SetLastAlertTemperature(lastWarningTemperature);

                    tempSensor->SetHistorySetting(historyRecordSetting);
                    tempSensor->SetHistoryFileName(histoyrRecordFile);

                    if (DBG) Slogger::D(TAG, "ReadDeviceConfig():  add this device to table(temperature sensor type)");
                }
                else if (zoneType == IZigbeeManager::ZIGBEE_ZONE_TYPE_DOOR) {   //Door sensor
                    if (DBG) Slogger::D(TAG, "ReadDeviceConfig():  add this device to table(door sensor type)");

                    // warning setting
                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("WarningEnableSetting"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    Boolean warningSetting = nextText.EqualsIgnoreCase("TRUE");
                    if (DBG) Slogger::D(TAG, "WarningEnableSetting: %d", warningSetting);
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("WarningEnableSetting"));
                    if (FAILED(ec))
                        break;

                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("WarningState"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    Int32 warningState = StringUtils::ParseInt32(nextText);
                    if (DBG) Slogger::D(TAG, "WarningState: %d", warningState);
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("WarningState"));
                    if (FAILED(ec))
                        break;

                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LastWarningTime"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    String lastWarningTime = nextText;
                    if (DBG) Slogger::D(TAG, "LastWarningTime: %s", lastWarningTime.string());
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LastWarningTime"));
                    if (FAILED(ec))
                        break;

                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LastWarningState"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    Int32 lastWarningState = StringUtils::ParseInt32(nextText);
                    if (DBG) Slogger::D(TAG, "LastWarningState: %d", lastWarningState);
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LastWarningState"));
                    if (FAILED(ec))
                        break;

                    //History Setting
                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("HistoryRecordEnableSetting"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    Boolean historyRecordSetting = nextText.EqualsIgnoreCase("TRUE");
                    if (DBG) Slogger::D(TAG, "HistoryRecordEnableSetting: %d", historyRecordSetting);
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("HistoryRecordEnableSetting"));
                    if (FAILED(ec))
                        break;

                    ec = xpp->NextTag(&nextTag);
                    if (FAILED(ec))
                        break;
                    ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("HistoryRecordFile"));
                    if (FAILED(ec))
                        break;
                    ec = xpp->NextText(&nextText);
                    if (FAILED(ec))
                        break;
                    String histoyrRecordFile = nextText;
                    if (DBG) Slogger::D(TAG, "HistoryRecordFile: %s", histoyrRecordFile.string());
                    ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("HistoryRecordFile"));
                    if (FAILED(ec))
                        break;

                    zbeDev = new ZigbeeDoorSensor(name, onlineStatus, shortAddr, macAddr, type, zoneType);
                    mDevices[name] = zbeDev;

                    //restore door warning/history setting
                    AutoPtr<ZigbeeDoorSensor> doorSensor = (ZigbeeDoorSensor*)zbeDev.Get();
                    doorSensor->SetWarningSetting(warningSetting);
                    doorSensor->SetWarningState(warningState);
                    doorSensor->SetLastAlertTime(lastWarningTime);
                    doorSensor->SetLastAlertState(lastWarningState);

                    doorSensor->SetHistorySetting(historyRecordSetting);
                    doorSensor->SetHistoryFileName(histoyrRecordFile);

                }
                else {
                    if (DBG) Slogger::D(TAG, "ReadDeviceConfig():  Does not add this device to table(invalid device zone type)");
                }
            }
            else if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_BULB) {    //Bulb
                zbeDev = new ZigbeeDevice(name, onlineStatus, shortAddr, macAddr, type, zoneType);
                mDevices[name] = zbeDev;
                if (DBG) Slogger::D(TAG, "ReadDeviceConfig():  add this device to table(bulb type)");
            }
            else if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_SWITCH) {    //Switch
                zbeDev = new ZigbeeDevice(name, onlineStatus, shortAddr, macAddr, type, zoneType);
                mDevices[name] = zbeDev;
                if (DBG) Slogger::D(TAG, "ReadDeviceConfig():  add this device to table(switch type)");
            }
            else if (type == IZigbeeManager::ZIGBEE_DEV_TYPE_NHR_SENSOR) {    //TH Sensor
                if (DBG) Slogger::D(TAG, "ReadDeviceConfig():  add this device to table(TH sensor type)");

                zbeDev = new ZigbeeTHSensor(name, onlineStatus, shortAddr, macAddr, type, zoneType);
                mDevices[name] = zbeDev;

                //Temperature Warning setting
                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LowerLimitTemperatureWarningEnableSetting"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                Boolean lowerLimitTemperatureWarningSetting = nextText.EqualsIgnoreCase("TRUE");
                if (DBG) Slogger::D(TAG, "LowerLimitTemperatureWarningEnableSetting: %d", lowerLimitTemperatureWarningSetting);
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LowerLimitTemperatureWarningEnableSetting"));
                if (FAILED(ec))
                    break;

                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("UpperLimitTemperatureWarningEnableSetting"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                Boolean upperLimitTemperatureWarningSetting = nextText.EqualsIgnoreCase("TRUE");
                if (DBG) Slogger::D(TAG, "UpperLimitTemperatureWarningEnableSetting: %d", upperLimitTemperatureWarningSetting);
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("UpperLimitTemperatureWarningEnableSetting"));
                if (FAILED(ec))
                    break;

                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LowerLimitWarningTemperature"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                Int32 lowerLimitWarningTemperature = StringUtils::ParseInt32(nextText);
                if (DBG) Slogger::D(TAG, "LowerLimitWarningTemperature: %d", lowerLimitWarningTemperature);
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LowerLimitWarningTemperature"));
                if (FAILED(ec))
                    break;

                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("UpperLimitWarningTemperature"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                Int32 upperLimitWarningTemperature = StringUtils::ParseInt32(nextText);
                if (DBG) Slogger::D(TAG, "UpperLimitWarningTemperature: %d", upperLimitWarningTemperature);
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("UpperLimitWarningTemperature"));
                if (FAILED(ec))
                    break;

                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LastTemperatureWarningTime"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                String lastTemperatureWarningTime = nextText;
                if (DBG) Slogger::D(TAG, "LastTemperatureWarningTime: %s", lastTemperatureWarningTime.string());
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LastTemperatureWarningTime"));
                if (FAILED(ec))
                    break;

                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LastWarningTemperature"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                Int32 lastWarningTemperature = StringUtils::ParseInt32(nextText);
                if (DBG) Slogger::D(TAG, "LastWarningTemperature: %d", lastWarningTemperature);
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LastWarningTemperature"));
                if (FAILED(ec))
                    break;

                //Temperature History Setting
                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("TemperatureHistoryRecordEnableSetting"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                Boolean temperatureHistoryRecordSetting = nextText.EqualsIgnoreCase("TRUE");
                if (DBG) Slogger::D(TAG, "TemperatureHistoryRecordEnableSetting: %d", temperatureHistoryRecordSetting);
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("TemperatureHistoryRecordEnableSetting"));
                if (FAILED(ec))
                    break;

                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("TemperatureHistoryRecordFile"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                String temperatureHistoyrRecordFile = nextText;
                if (DBG) Slogger::D(TAG, "TemperatureHistoryRecordFile: %s", temperatureHistoyrRecordFile.string());
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("TemperatureHistoryRecordFile"));
                if (FAILED(ec))
                    break;

                AutoPtr<ZigbeeTemperatureSensor> tempSensor = ((ZigbeeTHSensor*)zbeDev.Get())->GetTemperatureSensor();
                //restore temperature warning/history setting
                tempSensor->SetLowerLimitWarningSetting(lowerLimitTemperatureWarningSetting);
                tempSensor->SetUpperLimitWarningSetting(upperLimitTemperatureWarningSetting);
                tempSensor->SetWarningTemperatureLowerLimit(lowerLimitWarningTemperature);
                tempSensor->SetWarningTemperatureUpperLimit(upperLimitWarningTemperature);
                tempSensor->SetLastAlertTime(lastTemperatureWarningTime);
                tempSensor->SetLastAlertTemperature(lastWarningTemperature);

                tempSensor->SetHistorySetting(temperatureHistoryRecordSetting);
                tempSensor->SetHistoryFileName(temperatureHistoyrRecordFile);

                //---- Humidity Sensor ----
                //Humidity Warning setting
                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LowerLimitHumidityWarningEnableSetting"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                Boolean lowerLimitHumidityWarningSetting = nextText.EqualsIgnoreCase("TRUE");
                if (DBG) Slogger::D(TAG, "LowerLimitHumidityWarningEnableSetting: %d", lowerLimitHumidityWarningSetting);
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LowerLimitHumidityWarningEnableSetting"));
                if (FAILED(ec))
                    break;

                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("UpperLimitHumidityWarningEnableSetting"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                Boolean upperLimitHumidityWarningSetting = nextText.EqualsIgnoreCase("TRUE");
                if (DBG) Slogger::D(TAG, "UpperLimitHumidityWarningEnableSetting: %d", upperLimitHumidityWarningSetting);
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("UpperLimitHumidityWarningEnableSetting"));
                if (FAILED(ec))
                    break;

                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LowerLimitWarningHumidity"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                Int32 lowerLimitWarningHumidity = StringUtils::ParseInt32(nextText);
                if (DBG) Slogger::D(TAG, "LowerLimitWarningHumidity: %d", lowerLimitWarningHumidity);
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LowerLimitWarningHumidity"));
                if (FAILED(ec))
                    break;

                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("UpperLimitWarningHumidity"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                Int32 upperLimitWarningHumidity = StringUtils::ParseInt32(nextText);
                if (DBG) Slogger::D(TAG, "UpperLimitWarningHumidity: %d", upperLimitWarningHumidity);
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("UpperLimitWarningHumidity"));
                if (FAILED(ec))
                    break;

                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LastHumidityWarningTime"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                String lastHumidityWarningTime = nextText;
                if (DBG) Slogger::D(TAG, "LastHumidityWarningTime: %s", lastHumidityWarningTime.string());
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LastHumidityWarningTime"));
                if (FAILED(ec))
                    break;

                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("LastWarningHumidity"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                Int32 lastWarningHumidity = StringUtils::ParseInt32(nextText);
                if (DBG) Slogger::D(TAG, "LastWarningHumidity: %d", lastWarningHumidity);
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("LastWarningHumidity"));
                if (FAILED(ec))
                    break;

                //Humidity History Setting
                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("HumidityHistoryRecordEnableSetting"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                Boolean humidityHistoryRecordSetting = nextText.EqualsIgnoreCase("TRUE");
                if (DBG) Slogger::D(TAG, "HumidityHistoryRecordEnableSetting: %d", humidityHistoryRecordSetting);
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("HumidityHistoryRecordEnableSetting"));
                if (FAILED(ec))
                    break;

                ec = xpp->NextTag(&nextTag);
                if (FAILED(ec))
                    break;
                ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("HumidityHistoryRecordFile"));
                if (FAILED(ec))
                    break;
                ec = xpp->NextText(&nextText);
                if (FAILED(ec))
                    break;
                String humidityHistoyrRecordFile = nextText;
                if (DBG) Slogger::D(TAG, "HumidityHistoryRecordFile: %s", humidityHistoyrRecordFile.string());
                ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("HumidityHistoryRecordFile"));
                if (FAILED(ec))
                    break;

                AutoPtr<ZigbeeHumiditySensor> humiditySensor = ((ZigbeeTHSensor*)zbeDev.Get())->GetHumiditySensor();
                //restore Humidity warning/history setting
                humiditySensor->SetLowerLimitWarningSetting(lowerLimitHumidityWarningSetting);
                humiditySensor->SetUpperLimitWarningSetting(upperLimitHumidityWarningSetting);
                humiditySensor->SetWarningHumidityLowerLimit(lowerLimitWarningHumidity);
                humiditySensor->SetWarningHumidityUpperLimit(upperLimitWarningHumidity);
                humiditySensor->SetLastAlertTime(lastHumidityWarningTime);
                humiditySensor->SetLastAlertHumidity(lastWarningHumidity);

                humiditySensor->SetHistorySetting(humidityHistoryRecordSetting);
                humiditySensor->SetHistoryFileName(humidityHistoyrRecordFile);

            }
            else {   //invalid type/zone type
                if (DBG) Slogger::D(TAG, "ReadDeviceConfig():  Does not add this device to table(invalid device type)");
            }

            ec = xpp->NextTag(&nextTag);
            if (FAILED(ec))
                break;
            ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("Device"));
            if (FAILED(ec))
                break;
        }
        if (FAILED(ec))
            break;

        ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("ZigbeeDevice"));
        if (FAILED(ec))
            break;

        mDeviceNameList = new List<String>();
        HashMap<String, AutoPtr<ZigbeeDevice> >::Iterator iter = mDevices.Begin();
        for (; iter != mDevices.End(); ++iter)
            mDeviceNameList->PushBack(iter->mFirst);

        //Check if no group info
        if ((xpp->Next(&next), next) != IXmlPullParser::START_TAG) {
            Slogger::I(TAG, "No group Info in zigbee info xml: Exit");
            *res = 0;
            return NOERROR;
        }

        //Group information
        ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("ZigbeeGroup"));
        if (FAILED(ec))
            break;
        while ((xpp->NextTag(&nextTag), nextTag) == IXmlPullParser::START_TAG) {
            ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("Group"));
            if (FAILED(ec))
                break;

            //Name
            ec = xpp->NextTag(&nextTag);
            if (FAILED(ec))
                break;
            ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("Name"));
            if (FAILED(ec))
                break;
            ec = xpp->NextText(&nextText);
            if (FAILED(ec))
                break;
            String name = nextText;
            if (DBG) Slogger::D(TAG, "Group Name: %s", name.string());
            ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("Name"));
            if (FAILED(ec))
                break;

            //Type
            ec = xpp->NextTag(&nextTag);
            if (FAILED(ec))
                break;
            ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("Type"));
            if (FAILED(ec))
                break;
            ec = xpp->NextText(&nextText);
            if (FAILED(ec))
                break;
            Int32 type = StringUtils::ParseInt32(nextText);
            if (DBG) Slogger::D(TAG, "Group Type: 0x%04x", type);
            ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("Type"));
            if (FAILED(ec))
                break;

            //Zone Type
            ec = xpp->NextTag(&nextTag);
            if (FAILED(ec))
                break;
            ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("ZoneType"));
            if (FAILED(ec))
                break;
            ec = xpp->NextText(&nextText);
            if (FAILED(ec))
                break;
            Int32 zoneType = StringUtils::ParseInt32(nextText);
            if (DBG) Slogger::D(TAG, "Group ZoneType: 0x%04x", zoneType);
            ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("ZoneType"));
            if (FAILED(ec))
                break;

            //Device in group
            ec = xpp->NextTag(&nextTag);
            if (FAILED(ec))
                break;
            ec = xpp->Require(IXmlPullParser::START_TAG, String(NULL), String("DeviceInGroup"));
            if (FAILED(ec))
                break;
            ec = xpp->NextText(&nextText);
            if (FAILED(ec))
                break;
            String deviceName = nextText;
            splitStr = NULL;
            StringUtils::Split(deviceName, mConfigGroupSpliter, (ArrayOf<String>**)&splitStr);
            AutoPtr<List<String> > deviceList = new List<String>();
            for (Int32 i = 0; i < splitStr->GetLength(); i++) {
                if (DBG) Slogger::D(TAG, "device in group[%d]: %s", i, (*splitStr)[i].string());
                deviceList->PushBack((*splitStr)[i]);
            }
            ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("DeviceInGroup"));
            if (FAILED(ec))
                break;

            ec = xpp->NextTag(&nextTag);
            if (FAILED(ec))
                break;
            ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("Group"));
            if (FAILED(ec))
                break;

            //Create group object
            AutoPtr<ZigbeeGroup> zbeGrp = new ZigbeeGroup(name, type, zoneType, deviceList);
            mGroups[name] = zbeGrp;
        }
        ec = xpp->Require(IXmlPullParser::END_TAG, String(NULL), String("ZigbeeGroup"));
    } while(0);

    if (FAILED(ec)) {
        // e.printStackTrace();
        Slogger::E(TAG,"error occurred while reading xml file");
        return NOERROR;
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Thing-to-Thing profile loading function
 *  Parameters: XML file path
 *  Returns: TRUE - success, FALSE - fail
*/
/*
Boolean LoadT2TRules(String file) {
    String file = "/mnt/sdcard/t2t.xml";
}
*/


/********************************* Device Management API ******************************************/
/*
 *  Get all device information
 *  Parameters: void
 *  Returns: device information in 2-D array(List<List<String>>)
 *
 *  First array format: device counts, group counts
 *  Device format:
 *  (a) Bulb: (1)device name, (2)device online status, (3)device type, (4)device zone type, (5)onOffState
 *  (b) Temperature Sensor: (1)device name, (2)device online status, (3)device type, (4)device zone type, (5)sensor temperature, (6) is temperature warning
 *                          (7)warning enable setting_lower limit, (8)warning enable setting_upper limit, (9)lower limit warning temperature, (10)upper limit warning temperature,
 *                          (11)last alert time, (12)last alert temperature, (13)history record enable setting, (14)history record start time, (15)history record end time, (16)history average temperature,
 *                          (17)history minimum temperature occurred time, (18)history minimum temperature,
 *                          (19)history maximum temperature occurred time, (20)history maximum temperature
 *  (c) Door Sensor:    (1)device name, (2)device online status, (3)device type, (4)device zone type, (5)door on/off state, (6) is door state warning
 *                      (7)warning enable setting, (8)warning door state, (9)last alert time, (10)last alert door state, (11)history record enable setting
 *                      (12)history door state changed occured time, (13)history door state, ... (Max 5 records)
 *  (d) TH Sensor:  Temperature/Humidity
 *                  (1)device name, (2)device online status, (3)device type, (4)device zone type,
 *                  (5)sensor temperature, (6) is temperature warning
 *                  (7)warning enable setting_lower limit, (8)warning enable setting_upper limit, (9)lower limit warning temperature, (10)upper limit warning temperature,
 *                  (11)last alert time, (12)last alert temperature, (13)history record enable setting, (14)history record start time, (15)history record end time, (16)history average temperature,
 *                  (17)history minimum temperature occurred time, (18)history minimum temperature,
 *                  (19)history maximum temperature occurred time, (20)history maximum temperature,
 *                  (21)/
 *                  (22)sensor humidity, (23) is humidity warning
 *                  (24)warning enable setting_lower limit, (25)warning enable setting_upper limit, (26)lower limit warning humidity, (27)upper limit warning humidity,
 *                  (28)last alert time, (29)last alert humidity, (30)history record enable setting, (31)history record start time, (32)history record end time, (33)history average humidity,
 *                  (34)history minimum humidity occurred time, (35)history minimum humidity,
 *                  (36)history maximum humidity occurred time, (37)history maximum humidity
 *  Group format: (1)group name, (2)group type, (3)group zone type, (4)device name in the group, ...
 *  <Ex>: 2 bulb devices(zigbee1/zigbee2), 1 door sensor(zigbee3), 1 temperature sensor(zigbee4), and 1 group(abc), "zigbee1/zigbee2" is in group "abc"
 *          4 1
 *          zigbee1 1 258 65535 0
 *          zigbee2 1 258 65535 0
 *          zigbee3 1 1026 21 0 FALSE 1 NA 2 FALSE 2014-12-09-14-53-05 0 2014-12-09-14-53-52 1 2014-12-09-14-53-56 0 2014-12-09-14-54-36 1 2014-12-09-14-54-40 0
 *          zigbee4 1 1026 13 25 FALSE FALSE -9999 9999 NA 9999 FALSE 2014-01-01-8-00-46 2014-01-01-8-06-04 25 2014-01-01-7-00-46 23 2014-01-01-8-00-46 27
 *          abc 258 65535 zigbee1 zigbee2
*/
ECode CZigbeeService::GetAllDeviceInfo(
    /* [out, callee] */ ArrayOf<IParcelStringList*>** infoList)
{
    VALIDATE_NOT_NULL(infoList)
    AutoPtr<List<AutoPtr<IParcelStringList> > > result = new List<AutoPtr<IParcelStringList> >();
    AutoPtr<List<String> > tmpList;

    UpdateZigbeeDevices();
    /*
    if (mDevices.GetSize() == 0) {
        Slogger::I(TAG, "GetAllDeviceInfo(): no device available");
        return NULL;
    }
    */
    Int32 deviceCount = mDevices.GetSize();
    Int32 groupCount = mGroups.GetSize();

    tmpList = new List<String>();
    tmpList->PushBack(StringUtils::Int32ToString(deviceCount));
    tmpList->PushBack(StringUtils::Int32ToString(groupCount));
    AutoPtr<IParcelStringList> psl;
    AutoPtr<ArrayOf<String> > strArray = StringListToStringArray(tmpList);
    CParcelStringList::New(strArray, (IParcelStringList**)&psl);
    result->PushBack(psl);

    HashMap<String, AutoPtr<ZigbeeDevice> >::Iterator iter = mDevices.Begin();
    for (; iter != mDevices.End(); ++iter) {
        if (DBG) Slogger::D(TAG, "GetAllDeviceInfo(): get one device info");
        AutoPtr<ZigbeeDevice> zigbeeDevice = iter->mSecond;

        tmpList = new List<String>();
        tmpList->PushBack(zigbeeDevice->GetName());
        Int32 res;
        GetDeviceOnlineStatus(zigbeeDevice->GetName(), &res);
        tmpList->PushBack(StringUtils::Int32ToString(res));
        tmpList->PushBack(StringUtils::Int32ToString(zigbeeDevice->GetType()));
        tmpList->PushBack(StringUtils::Int32ToString(zigbeeDevice->GetZoneType()));

        if (zigbeeDevice->GetType() == IZigbeeManager::ZIGBEE_DEV_TYPE_BULB) {
            GetBulbOnOffState(zigbeeDevice->GetName(), &res);
            tmpList->PushBack(StringUtils::Int32ToString(res));
        }
        else if (zigbeeDevice->GetType() == IZigbeeManager::ZIGBEE_DEV_TYPE_SWITCH) {
            GetSwitchOnOffState(zigbeeDevice->GetName(), &res);
            tmpList->PushBack(StringUtils::Int32ToString(res));
        }
        else if (zigbeeDevice->GetType() == IZigbeeManager::ZIGBEE_DEV_TYPE_NHR_SENSOR) {
            AutoPtr<ZigbeeTHSensor> thSensor = (ZigbeeTHSensor*)zigbeeDevice.Get();

            // --------- Temperature info ------------
            Int32 temperature;
            GetSensorTemperature(zigbeeDevice->GetName(), &temperature);
            tmpList->PushBack(StringUtils::Int32ToString(temperature));

            AutoPtr<ZigbeeTemperatureSensor> tempSensor = thSensor->GetTemperatureSensor();
            // warning setting
            tmpList->PushBack(StringUtils::BooleanToString(tempSensor->IsWarning(temperature)));
            tmpList->PushBack(StringUtils::BooleanToString(tempSensor->GetLowerLimitWarningSetting()));
            tmpList->PushBack(StringUtils::BooleanToString(tempSensor->GetUpperLimitWarningSetting()));
            tmpList->PushBack(StringUtils::Int32ToString(tempSensor->GetWarningTemperatureLowerLimit()));
            tmpList->PushBack(StringUtils::Int32ToString(tempSensor->GetWarningTemperatureUpperLimit()));
            tmpList->PushBack(tempSensor->GetLastAlertTime());
            tmpList->PushBack(StringUtils::Int32ToString(tempSensor->GetLastAlertTemperature()));

            // history setting
            tmpList->PushBack(StringUtils::BooleanToString(tempSensor->GetHistorySetting()));
            tmpList->PushBack(tempSensor->GetHistoryFileName());

            // short-term solution for history record display
            AutoPtr<List<String> > tempHistoryTime = tempSensor->GetHistoryTime();
            //List<Integer> tempHistoryState = tempSensor->GetHistoryTemperature();
            if (tempHistoryTime->GetSize() > 0) {
                tmpList->PushBack(tempHistoryTime->GetFront());
                tmpList->PushBack(tempHistoryTime->GetBack());
                tmpList->PushBack(StringUtils::Int32ToString(tempSensor->GetHistoryAvgTemperature()));
                tmpList->PushBack(tempSensor->GetHistoryMinTemperatureTime());
                tmpList->PushBack(StringUtils::Int32ToString(tempSensor->GetHistoryMinTemperature()));
                tmpList->PushBack(tempSensor->GetHistoryMaxTemperatureTime());
                tmpList->PushBack(StringUtils::Int32ToString(tempSensor->GetHistoryMaxTemperature()));
            }
            else {
                tmpList->PushBack(NOT_AVAILABLE);
                tmpList->PushBack(NOT_AVAILABLE);
                tmpList->PushBack(NOT_AVAILABLE);
                tmpList->PushBack(NOT_AVAILABLE);
                tmpList->PushBack(NOT_AVAILABLE);
                tmpList->PushBack(NOT_AVAILABLE);
                tmpList->PushBack(NOT_AVAILABLE);
            }

            // add spliter
            tmpList->PushBack(mInfoSpliter);

            // ------------ Humidity info ----------
            Int32 humidity;
            GetSensorHumidity(zigbeeDevice->GetName(), &humidity);
            tmpList->PushBack(StringUtils::Int32ToString(humidity));

            AutoPtr<ZigbeeHumiditySensor> humiditySensor = thSensor->GetHumiditySensor();
            // warning setting
            tmpList->PushBack(StringUtils::BooleanToString(humiditySensor->IsWarning(humidity)));
            tmpList->PushBack(StringUtils::BooleanToString(humiditySensor->GetLowerLimitWarningSetting()));
            tmpList->PushBack(StringUtils::BooleanToString(humiditySensor->GetUpperLimitWarningSetting()));
            tmpList->PushBack(StringUtils::Int32ToString(humiditySensor->GetWarningHumidityLowerLimit()));
            tmpList->PushBack(StringUtils::Int32ToString(humiditySensor->GetWarningHumidityUpperLimit()));
            tmpList->PushBack(humiditySensor->GetLastAlertTime());
            tmpList->PushBack(StringUtils::Int32ToString(humiditySensor->GetLastAlertHumidity()));

            // history setting
            tmpList->PushBack(StringUtils::BooleanToString(humiditySensor->GetHistorySetting()));
            tmpList->PushBack(humiditySensor->GetHistoryFileName());

            // short-term solution for history record display
            AutoPtr<List<String> > humidityHistoryTime = humiditySensor->GetHistoryTime();
            if (humidityHistoryTime->GetSize() > 0) {
                tmpList->PushBack(humidityHistoryTime->GetFront());
                tmpList->PushBack(humidityHistoryTime->GetBack());
                tmpList->PushBack(StringUtils::Int32ToString(humiditySensor->GetHistoryAvgHumidity()));
                tmpList->PushBack(humiditySensor->GetHistoryMinHumidityTime());
                tmpList->PushBack(StringUtils::Int32ToString(humiditySensor->GetHistoryMinHumidity()));
                tmpList->PushBack(humiditySensor->GetHistoryMaxHumidityTime());
                tmpList->PushBack(StringUtils::Int32ToString(humiditySensor->GetHistoryMaxHumidity()));
            }
            else {
                tmpList->PushBack(NOT_AVAILABLE);
                tmpList->PushBack(NOT_AVAILABLE);
                tmpList->PushBack(NOT_AVAILABLE);
                tmpList->PushBack(NOT_AVAILABLE);
                tmpList->PushBack(NOT_AVAILABLE);
                tmpList->PushBack(NOT_AVAILABLE);
                tmpList->PushBack(NOT_AVAILABLE);
            }

        }
        else if (zigbeeDevice->GetType() == IZigbeeManager::ZIGBEE_DEV_TYPE_SENSOR) {
            if (zigbeeDevice->GetZoneType() == IZigbeeManager::ZIGBEE_ZONE_TYPE_TEMPERATURE &&
               zigbeeDevice->GetClass() == "ZigbeeTemperatureSensor") {   // Temperature sensor

                Int32 temperature;
                GetSensorTemperature(zigbeeDevice->GetName(), &temperature);
                tmpList->PushBack(StringUtils::Int32ToString(temperature));

                AutoPtr<ZigbeeTemperatureSensor> tempSensor = (ZigbeeTemperatureSensor*)zigbeeDevice.Get();
                // warning setting
                tmpList->PushBack(StringUtils::BooleanToString(tempSensor->IsWarning(temperature)));
                tmpList->PushBack(StringUtils::BooleanToString(tempSensor->GetLowerLimitWarningSetting()));
                tmpList->PushBack(StringUtils::BooleanToString(tempSensor->GetUpperLimitWarningSetting()));
                tmpList->PushBack(StringUtils::Int32ToString(tempSensor->GetWarningTemperatureLowerLimit()));
                tmpList->PushBack(StringUtils::Int32ToString(tempSensor->GetWarningTemperatureUpperLimit()));
                tmpList->PushBack(tempSensor->GetLastAlertTime());
                tmpList->PushBack(StringUtils::Int32ToString(tempSensor->GetLastAlertTemperature()));

                // history setting
                tmpList->PushBack(StringUtils::BooleanToString(tempSensor->GetHistorySetting()));
                tmpList->PushBack(tempSensor->GetHistoryFileName());

                // short-term solution for history record display
                AutoPtr<List<String> > tempHistoryTime = tempSensor->GetHistoryTime();
                //List<Integer> tempHistoryState = tempSensor->GetHistoryTemperature();
                if (tempHistoryTime->GetSize() > 0) {
                    tmpList->PushBack(tempHistoryTime->GetFront());
                    tmpList->PushBack(tempHistoryTime->GetBack());
                    tmpList->PushBack(StringUtils::Int32ToString(tempSensor->GetHistoryAvgTemperature()));
                    tmpList->PushBack(tempSensor->GetHistoryMinTemperatureTime());
                    tmpList->PushBack(StringUtils::Int32ToString(tempSensor->GetHistoryMinTemperature()));
                    tmpList->PushBack(tempSensor->GetHistoryMaxTemperatureTime());
                    tmpList->PushBack(StringUtils::Int32ToString(tempSensor->GetHistoryMaxTemperature()));
                }
                else {
                    tmpList->PushBack(NOT_AVAILABLE);
                    tmpList->PushBack(NOT_AVAILABLE);
                    tmpList->PushBack(NOT_AVAILABLE);
                    tmpList->PushBack(NOT_AVAILABLE);
                    tmpList->PushBack(NOT_AVAILABLE);
                    tmpList->PushBack(NOT_AVAILABLE);
                    tmpList->PushBack(NOT_AVAILABLE);
                }


            }
            else if (zigbeeDevice->GetZoneType() == IZigbeeManager::ZIGBEE_ZONE_TYPE_DOOR &&
                        zigbeeDevice->GetClass() == "ZigbeeDoorSensor") { // Door sensor
                Int32 doorState;
                GetDoorOnOffState(zigbeeDevice->GetName(), &doorState);
                tmpList->PushBack(StringUtils::Int32ToString(doorState));

                AutoPtr<ZigbeeDoorSensor> doorSensor = (ZigbeeDoorSensor*)zigbeeDevice.Get();
                tmpList->PushBack(StringUtils::Int32ToString(doorSensor->IsDoorWarning(doorState)));
                // warning setting
                tmpList->PushBack(StringUtils::Int32ToString(doorSensor->GetWarningSetting()));
                tmpList->PushBack(StringUtils::Int32ToString(doorSensor->GetWarningState()));
                tmpList->PushBack(doorSensor->GetLastAlertTime());
                tmpList->PushBack(StringUtils::Int32ToString(doorSensor->GetLastAlertState()));

                // history setting
                tmpList->PushBack(StringUtils::BooleanToString(doorSensor->GetHistorySetting()));
                tmpList->PushBack(doorSensor->GetHistoryFileName());

                // short-term solution for history record display
                AutoPtr<List<String> > doorHistoryTime = doorSensor->GetHistoryTime();
                AutoPtr<List<Int32> > doorHistoryState = doorSensor->GetHistoryDoorState();
                List<String>::Iterator it1 = doorHistoryTime->Begin();
                List<Int32>::Iterator it2 = doorHistoryState->Begin();
                for (; it1 != doorHistoryTime->End(); ++it1, ++it2) {
                    tmpList->PushBack(*it1);
                    tmpList->PushBack(StringUtils::Int32ToString(*it2));
                }
            }
        }
        else {
            tmpList->PushBack(NOT_AVAILABLE);
        }
        psl = NULL;
        strArray = StringListToStringArray(tmpList);
        CParcelStringList::New(strArray, (IParcelStringList**)&psl);
        result->PushBack(psl);
    }

    HashMap<String, AutoPtr<ZigbeeGroup> >::Iterator groupIt = mGroups.Begin();
    for (; groupIt != mGroups.End(); ++groupIt) {
        if (DBG) Slogger::D(TAG, "GetAllDeviceInfo(): get one group info");
        AutoPtr<ZigbeeGroup> zigbeeGroup = groupIt->mSecond;

        tmpList = new List<String>();
        tmpList->PushBack(zigbeeGroup->GetName());
        tmpList->PushBack(StringUtils::Int32ToString(zigbeeGroup->GetType()));
        tmpList->PushBack(StringUtils::Int32ToString(zigbeeGroup->GetZoneType()));

        AutoPtr<List<String> > groupDeviceList = zigbeeGroup->GetAllDevices();
        tmpList->Insert(tmpList->End(), groupDeviceList->Begin(), groupDeviceList->End());
        psl = NULL;
        strArray = StringListToStringArray(tmpList);
        CParcelStringList::New(strArray, (IParcelStringList**)&psl);
        result->PushBack(psl);
    }

    if (DBG) {
        Slogger::D(TAG, "------------ GetAllDeviceInfo(): ----------------");
        List<AutoPtr<IParcelStringList> >::Iterator iter = result->Begin();
        for (; iter != result->End(); ++iter) {
            AutoPtr<ArrayOf<String> > parcelStringList;
            (*iter)->GetStringList((ArrayOf<String>**)&parcelStringList);
            String line("");
            for (Int32 j = 0; j < parcelStringList->GetLength(); j++) {
                if (j == 0) {
                    line += (*parcelStringList)[j];
                }
                else {
                    line += String(",") + (*parcelStringList)[j];
                }

            }
            Slogger::D(TAG, line);
            Slogger::D(TAG, "+++++++++++++++++++");
        }
    }

    *infoList = ArrayOf<IParcelStringList*>::Alloc(result->GetSize());
    (*infoList)->AddRef();
    List<AutoPtr<IParcelStringList> >::Iterator pslIter = result->Begin();
    for (Int32 i = 0; pslIter != result->End(); ++pslIter, ++i) {
        (*infoList)->Set(i, *pslIter);
    }
    return NOERROR;
}


/*
 *  [short-term solution] Remote client can only access "String"
 *  (For Remote Client) Get all device name
 *  Parameters: void
 *  Returns: device name - success, NULL- fail
*/
ECode CZigbeeService::GetDeviceNameByID(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    String deviceNameList("");

    if (mDevices.GetSize() == 0) {
        *name = String(NULL);
        return NOERROR;
    }

    HashMap<String, AutoPtr<ZigbeeDevice> >::Iterator iter = mDevices.Begin();
    for (; iter != mDevices.End(); ++iter) {
        deviceNameList = deviceNameList + iter->mFirst + "?";
    }
    if (DBG) Slogger::D(TAG, "GetDeviceNameByID(): deviceNameList: %s", deviceNameList.string());

    *name = deviceNameList;

    return NOERROR;
    /*
    if (DBG) Slogger::D(TAG, "GetDeviceNameByID(" + id + ")");
    if (id==1) {
        mDeviceNameList = new List<String>(mDevices.keySet());
    }
    else {
        if (mDeviceNameList == NULL) {
            Slogger::E(TAG, "GetDeviceNameByID(" + id + "): mDeviceNameList is NULL");
            return NULL;
        }
    }

    if (id > mDeviceNameList->size() || id <= 0) {
        Slogger::E(TAG, "GetDeviceNameByID(" + id + "): Error Index");
        return NULL;
    }
    if (DBG) Slogger::D(TAG, "GetDeviceNameByID(" + id + "): return " + mDeviceNameList->Get(id-1));
    return mDeviceNameList->Get(id-1);
    */
}

/*
 *  Get device name list
 *  Parameters: void
 *  Returns: device name list - success, NULL- fail
*/
ECode CZigbeeService::GetDevices(
    /* [out, callee] */ ArrayOf<String>** list)
{
    VALIDATE_NOT_NULL(list)
    *list = ArrayOf<String>::Alloc(mDevices.GetSize());
    (*list)->AddRef();
    HashMap<String, AutoPtr<ZigbeeDevice> >::Iterator iter = mDevices.Begin();
    for (Int32 i = 0; iter != mDevices.End(); ++iter, ++i) {
        (**list)[i] = iter->mFirst;
    }
    return NOERROR;
}


/*
 *  Get device online status
 *  OnlineStatus defined in IZigbeeManager::java (Ex.IZigbeeManager::STATUS_ONLINE/IZigbeeManager::STATUS_OFFLINE)
 *  Parameters: device name
 *  Returns: online status- success, -1- fail
*/
ECode CZigbeeService::GetDeviceOnlineStatus(
    /* [in] */ const String& devName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(devName)) {
        Slogger::E(TAG, "CZigbeeService.java: getOnlineStatus(): devName: %s , No such device!!! ", devName.string());
        *res = -1;
        return NOERROR;
    }
    // get updated online status from HAL
    AutoPtr<ZigbeeDevice> zigbeeDevice = mDevices[devName];
    Int32 deviceShortAddr = zigbeeDevice->GetShortAddr();
    Int32 onlineStatus = Native_GetDeviceOnlineStatus(deviceShortAddr);

    //update device online status
    if (zigbeeDevice->SetOnlineStatus(onlineStatus) == 1) {
        Slogger::E(TAG, "GetOnlineStatus(0x%04x), deviceName: %s, setOnlineStatus() failed: trying to set a illegal status: %d",
            deviceShortAddr, devName.string(), onlineStatus);
        *res = -1;
        return NOERROR;
    }

    if (DBG) Slogger::D(TAG, "CZigbeeService.java: getOnlineStatus(): devName: %s, status: %d", devName.string(), onlineStatus);
    *res = onlineStatus;

    return NOERROR;
}


/*
 *  Get device short address
 *  Parameters: device name
 *  Returns: short address- success, NULL- fail
*/
ECode CZigbeeService::GetDeviceShortAddr(
    /* [in] */ const String& devName,
    /* [out] */ String* shortAddr)
{
    VALIDATE_NOT_NULL(shortAddr)

    if (!IsDeviceExist(devName)) {
        Slogger::E(TAG, "CZigbeeService.java: getShortAddr(): devName: %s , No such device!!! ", devName.string());
        *shortAddr = String(NULL);
        return NOERROR;
    }
    Int32 byteShortAddr = mDevices[devName]->GetShortAddr();
    *shortAddr = String("");
    shortAddr->AppendFormat("0x%04x", byteShortAddr);
    if (DBG) Slogger::D(TAG, "CZigbeeService.java: getShortAddr(): devName: %s return short addr: %s",
                devName.string(), shortAddr->string());
    return NOERROR;
}


/*
 *  Get device MAC address
 *  Parameters: device name
 *  Returns: MAC address- success, NULL- fail
*/
ECode CZigbeeService::GetDeviceMacAddr(
    /* [in] */ const String& devName,
    /* [out] */ String* macAddr)
{
    VALIDATE_NOT_NULL(macAddr)

    if (!IsDeviceExist(devName)) {
        Slogger::E(TAG, "CZigbeeService.java: getMacAddr(): devName: %s , No such device!!! ", devName.string());
        *macAddr = String(NULL);
        return NOERROR;
    }
    Int64 byteMacAddr = mDevices[devName]->GetMacAddr();
    *macAddr = String("");
    macAddr->AppendFormat("%16x", byteMacAddr);
    if (DBG) Slogger::D(TAG, "CZigbeeService.java: getMacAddr():  devName: %s return MAC addr: ",
                devName.string(), macAddr->string());
    return NOERROR;

}


/*
 *  Get device type
 *  Type defined in IZigbeeManager::java (Ex.IZigbeeManager::ZIGBEE_DEV_TYPE_BULB)
 *  Parameters: device name
 *  Returns: type - success, -1 - fail
*/
ECode CZigbeeService::GetDeviceType(
    /* [in] */ const String& devName,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)

    if (!IsDeviceExist(devName)) {
        Slogger::E(TAG, "CZigbeeService.java: getType(): devName: %s , No such device!!! ", devName.string());
        *type = -1;
        return NOERROR;
    }
    *type = mDevices[devName]->GetType();
    if (DBG) Slogger::D(TAG, "CZigbeeService.java: getType(): devName: %s return type: 0x%04x",
                devName.string(), *type);
    return NOERROR;
}

/*
 *  Get device zone type
 *  Type defined in IZigbeeManager::java (Ex.IZigbeeManager::ZIGBEE_ZONE_TYPE_TEMPERATURE)
 *  Parameters: device name
 *  Returns: zone type - success, -1 - fail
*/
ECode CZigbeeService::GetDeviceZoneType(
    /* [in] */ const String& devName,
    /* [out] */ Int32* zoneType)
{
    VALIDATE_NOT_NULL(zoneType)

    if (!IsDeviceExist(devName)) {
        Slogger::E(TAG, "CZigbeeService.java: getDeviceZoneType(): devName: %s , No such device!!! ", devName.string());
        *zoneType = -1;
    }
    *zoneType = mDevices[devName]->GetZoneType();
    if (DBG) Slogger::D(TAG, "CZigbeeService.java: getDeviceZoneType(): devName: %s return type: 0x%04x",
                devName.string(), *zoneType);
    return NOERROR;
}


/*
 *  Change device name
 *  Parameters: original device name, new device name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetDeviceName(
    /* [in] */ const String& targetDevName,
    /* [in] */ const String& newDevName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = 1;
    //check device exist
    if (!IsDeviceExist(targetDevName)) {
        Slogger::E(TAG, "CZigbeeService.java: setDeviceName(): devName: %s , No such device.", targetDevName.string());
        return NOERROR;
    }

    //make sure no the same device name
    if (IsDeviceExist(newDevName)) {
        Slogger::E(TAG, "CZigbeeService.java: setDeviceName(): new deivce name: %s  is already used.", newDevName.string());
        return NOERROR;
    }

    // update device name/device hashmap
    AutoPtr<ZigbeeDevice> zbgDev = mDevices[targetDevName];
    if (zbgDev->SetName(newDevName) == 1) {
        Slogger::E(TAG, "CZigbeeService.java: setDeviceName(): illegal deivce name: %s", newDevName.string());
        return NOERROR;
    }
    mDevices[newDevName] = zbgDev; //update device object hashmap
    mDevices.Erase(targetDevName);
    List<String>::Iterator find = Find(mDeviceNameList->Begin(), mDeviceNameList->End(), targetDevName);
    assert(find != mDeviceNameList->End());
    *find = newDevName;//[Short-term solution]update device name list

    // update device name in group if needed
    HashMap<String, AutoPtr<ZigbeeGroup> >::Iterator groupIt = mGroups.Begin();
    for (; groupIt != mGroups.End(); ++groupIt) {
        AutoPtr<ZigbeeGroup> zigbeeGroup = groupIt->mSecond;
        if (zigbeeGroup->IsContainDevice(targetDevName)) {
            zigbeeGroup->SetDeviceName(targetDevName, newDevName);
        }
    }

    //update haspmap key of scenario mode thread if scenario mode thread is running
    HashMap<String, AutoPtr<BulbScenarioModeThread> >::Iterator bsmtsFind = mBulbSMThreads.Find(targetDevName);
    if (bsmtsFind != mBulbSMThreads.End()) {
        mBulbSMThreads[newDevName] = bsmtsFind->mSecond;
        mBulbSMThreads.Erase(bsmtsFind);
    }

    //[short-term solution for sync state between remote client and local client]
    if (mCallbackMgr != NULL) {
        mCallbackMgr->NotifyDeviceStateChanged();
    }

    if (DBG) {
        Slogger::D(TAG, "******** after setDeviceName() ******");
        AutoPtr<ArrayOf<String> > nameList1;
        GetDevices((ArrayOf<String>**)&nameList1);
        for (Int32 i = 0; i < nameList1->GetLength(); i++) {
            if (DBG) Slogger::D(TAG, "SetDeviceName():  device %d name: %s", i, (*nameList1)[i].string());
            // Int32 status = GetDeviceOnlineStatus((*nameList1)[i]);
            // String shortAddr = getDeviceShortAddr((*nameList1)[i]);
            // String macAddr = getDeviceMacAddr((*nameList1)[i]);
        }
    }

    *res = 0;
    return NOERROR;
}

/*
 *  get total device count
 *  Parameters: void
 *  Returns: device count-success, -1-fail
*/
ECode CZigbeeService::GetDeviceCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    if (DBG) Slogger::D(TAG, "GetDeviceCount() %d", mDevices.GetSize());
    *count = mDevices.GetSize();
    return NOERROR;
}


/************************************* Group Management API ****************************************/
/*
 *  Create a group
 *  Parameters: group name, group type, group zone type
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::CreateGroup(
    /* [in] */ const String& groupName,
    /* [in] */ Int32 groupType,
    /* [in] */ Int32 groupZoneType,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (IsGroupExist(groupName)) {
        if (DBG) Slogger::D(TAG, "CZigbeeService.java: createGroup fail: group already exist");
        *res = 1;
        return NOERROR;
    }

    AutoPtr<ZigbeeGroup> zbgGrp = new ZigbeeGroup(groupName, groupType, groupZoneType);
    mGroups[groupName] = zbgGrp;

    //[short-term solution for sync state between remote client and local client]
    if (mCallbackMgr != NULL) {
        mCallbackMgr->NotifyDeviceStateChanged();
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Remove a group
 *  Parameters: group name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::RemoveGroup(
    /* [in] */ const String& groupName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsGroupExist(groupName)) {
        if (DBG) Slogger::D(TAG, "CZigbeeService.java: removeGroup fail: group does not exist");
        *res = 1;
        return NOERROR;
    }

    //remove group from group hasp map
    mGroups.Erase(groupName);

    //[short-term solution for sync state between remote client and local client]
    if (mCallbackMgr != NULL) {
        mCallbackMgr->NotifyDeviceStateChanged();
    }

    *res = 0;
    return NOERROR;
}

/*
 *  [short-term solution] Remote client can only access "String"
 *  (For Remote Client) Get group name
 *  Parameters: void
 *  Returns: group name - success, NULL- fail
*/
ECode CZigbeeService::GetGroupsByID(
    /* [out] */ String* groupName)
{
    VALIDATE_NOT_NULL(groupName)

    String groupNameList("");

    if (mGroups.GetSize() == 0) {
        *groupName = String(NULL);
        return NOERROR;
    }

    HashMap<String, AutoPtr<ZigbeeGroup> >::Iterator groupIt = mGroups.Begin();
    for (; groupIt != mGroups.End(); ++groupIt) {
        groupNameList = groupNameList + groupIt->mFirst + "?";

    }
    if (DBG) Slogger::D(TAG, "GetGroupsByID(): groupNameList: %s", groupNameList.string());

    *groupName = groupNameList;
    return NOERROR;

    /*
    if (id==1) {
        mGroupNameList = new List<String>(mGroups.keySet());
    }
    else {
        if (mGroupNameList == NULL) {
            Slogger::E(TAG, "GetGroupsByID(" + id + "): mGroupNameList is NULL");
            return NULL;
        }
    }

    if (id > mGroupNameList.size() || id <= 0) {
        Slogger::E(TAG, "GetGroupsByID(" + id + "): Error Index");
        return NULL;
    }
    if (DBG) Slogger::D(TAG, "GetGroupsByID(" + id + "): return " + mGroupNameList->Get(id-1));
    return mGroupNameList->Get(id-1);
    */
}


/*
 *  [short-term solution] Remote client can only access "String"
 *  (For Remote Client) Get device name in a group
 *  Parameters: group name
 *  Returns: device name in a group - success, NULL- fail
*/
ECode CZigbeeService::GetGroupDevicesByID(
    /* [in] */ const String& groupName,
    /* [out] */ String* deviceName)
{
    VALIDATE_NOT_NULL(deviceName)

    String groupDevicesList("");

    //check group name valid or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "GetGroupDevicesByID(%s): group not exist", groupName.string());
        *deviceName = String(NULL);
        return NOERROR;
    }
    AutoPtr<ZigbeeGroup> zigbeeGroup = mGroups[groupName];
    AutoPtr<List<String> > nameList = zigbeeGroup->GetAllDevices();
    List<String>::Iterator iter = nameList->Begin();
    for (; iter != nameList->End(); ++iter) {
        groupDevicesList = groupDevicesList + *iter + "?";
    }
    if (DBG) Slogger::D(TAG, "GetGroupDevicesByID(%s): devices in : %s", groupName.string(), groupDevicesList.string());

    *deviceName = groupDevicesList;
    return NOERROR;
}

/*
 *  Get all group names
 *  Parameters: void
 *  Returns: group name list - success, NULL- fail
*/
ECode CZigbeeService::GetGroups(
    /* [out, callee] */ ArrayOf<String>** list)
{
    VALIDATE_NOT_NULL(list)

    *list = ArrayOf<String>::Alloc(mGroups.GetSize());
    (*list)->AddRef();
    HashMap<String, AutoPtr<ZigbeeGroup> >::Iterator iter = mGroups.Begin();
    for (Int32 i = 0; iter != mGroups.End(); ++iter, ++i) {
        (**list)[i] = iter->mFirst;
    }

    return NOERROR;
}

/*
 *  Add multiple devices to group
 *  Parameters: group name, device name list
 *  Return value: 0-success, 1-fail
*/
ECode CZigbeeService::AddDevicesToGroup(
    /* [in] */ const String& groupName,
    /* [in] */ const ArrayOf<String>& deviceNameList,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = 1;
    //check group name valid or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "AddDevicesToGroup(%s): group not exist", groupName.string());
        return NOERROR;
    }

    //check all devices' name valid or not
    if (!IsDevicesExist(deviceNameList)) {
        Slogger::E(TAG, "AddDevicesToGroup(%s): one or more devices' name are not exist", groupName.string());
        return NOERROR;
    }

    //check if devices'type and group type is the same
    if (!CompareTypes(groupName, deviceNameList)) {
        Slogger::E(TAG, "AddDevicesToGroup(%s): one or more devices' type are not the same with group type", groupName.string());
        return NOERROR;
    }

    //add device to group
    AutoPtr<ZigbeeGroup> zigbeeGroup = mGroups[groupName];
    for (Int32 i = 0; i < deviceNameList.GetLength(); i++) {
        if (zigbeeGroup->IsContainDevice(deviceNameList[i])) { //check if the devices are already in the gourp
            if (DBG) Slogger::D(TAG, "AddDevicesToGroup(): device \"%s\" is already in the group \"%s\"",
                        deviceNameList[i].string(), groupName.string());
        }
        else {
            if (DBG) Slogger::D(TAG, "AddDevicesToGroup(): add device \"%s\" to the group \"%s\"",
                        deviceNameList[i].string(), groupName.string());
            zigbeeGroup->AddDevice(deviceNameList[i]);
        }
    }

    //[short-term solution for sync state between remote client and local client]
    if (mCallbackMgr != NULL) {
        mCallbackMgr->NotifyDeviceStateChanged();
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Add a device to a group
 *  Parameters: group name, device name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::AddDeviceToGroup(
    /* [in] */ const String& groupName,
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    //check group name and device name valid or not
    if (!IsGroupExist(groupName) || !IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "addDeviceToGroup(): group name or device name not exist");
        *res = 1;
        return NOERROR;
    }

    //check if this device is already in the gourp
    AutoPtr<ZigbeeGroup> zigbeeGroup = mGroups[groupName];
    if (zigbeeGroup->IsContainDevice(deviceName)) {
        if (DBG) Slogger::D(TAG, "addDeviceToGroup(): device \"%s\" is already in the group \"%s\"",
                    deviceName.string(), groupName.string());
        *res = 0;
        return NOERROR;
    }

    //check if device type is the same with group type
    if (!CompareType(groupName, deviceName)) {
        Slogger::E(TAG, "addDeviceToGroup(): Can't add device \"%s\" to the group \"%s\": type/zone type not the same",
                    deviceName.string(), groupName.string());
        *res = 1;
        return NOERROR;
    }

    if (DBG) Slogger::D(TAG, "addDeviceToGroup(): add device \"%s\" to the group \"%s\"",
                deviceName.string(), groupName.string());
    zigbeeGroup->AddDevice(deviceName);

    //[short-term solution for sync state between remote client and local client]
    if (mCallbackMgr != NULL) {
        mCallbackMgr->NotifyDeviceStateChanged();
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Remove all devices from a group
 *  Parameters: group name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::RemoveAllDevicesFromGroup(
    /* [in] */ const String& groupName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    //check group name valid or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "RemoveAllDevicesFromGroup(): group \"%s\" not exist", groupName.string());
        *res = 1;
        return NOERROR;
    }

    if (DBG) Slogger::D(TAG, "RemoveAllDevicesFromGroup(): remove all devices in group  \"%s\"", groupName.string());
    AutoPtr<ZigbeeGroup> zigbeeGroup = mGroups[groupName];
    zigbeeGroup->RemoveAllDevices(); //remove deivces from group hash map

    //[short-term solution for sync state between remote client and local client]
    if (mCallbackMgr != NULL) {
        mCallbackMgr->NotifyDeviceStateChanged();
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Remove a device from a group
 *  Parameters: group name, device name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::RemoveDeviceFromGroup(
    /* [in] */ const String& groupName,
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    //check group name and device name valid or not
    if (!IsGroupExist(groupName) || !IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "RemoveDeviceFromGroup(): group name \"%s\" or device name \"%s\" not exist",
            groupName.string(), deviceName.string());
        *res = 1;
        return NOERROR;
    }

    //check if this device is already not in the gourp
    AutoPtr<ZigbeeGroup> zigbeeGroup = mGroups[groupName];
    if (zigbeeGroup->IsContainDevice(deviceName)) {
        if (DBG) Slogger::D(TAG, "RemoveDeviceFromGroup(): remove device \"%s\" from group \"%s\"",
                    deviceName.string(), groupName.string());
        zigbeeGroup->RemoveDevice(deviceName);
        *res = 0;
        return NOERROR;
    }
    if (DBG) Slogger::D(TAG, "RemoveDeviceFromGroup(): device \"%s\" is already not in the group \"%s\"",
                deviceName.string(), groupName.string());

    //[short-term solution for sync state between remote client and local client]
    if (mCallbackMgr != NULL) {
        mCallbackMgr->NotifyDeviceStateChanged();
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Change group name
 *  Parameters: original group name, new group name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetGroupName(
    /* [in] */ const String& groupName,
    /* [in] */ const String& newGroupName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    //check group name exist or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "SetGroupName(%s): group not exist", groupName.string());
        *res = 1;
        return NOERROR;
    }

    //check new group name valid or not
    if (IsGroupExist(newGroupName)) {
        Slogger::E(TAG, "SetGroupName(%s): new group name is used", newGroupName.string());
        *res = 1;
        return NOERROR;
    }

    AutoPtr<ZigbeeGroup> zigbeeGroup = mGroups[groupName];
    if (zigbeeGroup->SetName(newGroupName) == 1) {
        Slogger::E(TAG, "SetGroupName(%s): illegal new group name", newGroupName.string());
        *res = 1;
        return NOERROR;
    }

    mGroups[newGroupName] = zigbeeGroup;
    mGroups.Erase(groupName);

    //[short-term solution for sync state between remote client and local client]
    if (mCallbackMgr != NULL) {
        mCallbackMgr->NotifyDeviceStateChanged();
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Get group type
 *  Type defined in IZigbeeManager::java (Ex.IZigbeeManager::ZIGBEE_DEV_TYPE_BULB)
 *  Parameters: group name
 *  Returns: group type - success, (-1)-fail
*/
ECode CZigbeeService::GetGroupType(
    /* [in] */ const String& groupName,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = -1;

    //check group name valid or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "GetGroupType(%s): group not exist", groupName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeGroup> zigbeeGroup = mGroups[groupName];
    *type = zigbeeGroup->GetType();
    if (DBG) Slogger::D(TAG, "GetGroupType(%s): group type 0x%04x", groupName.string(), *type);

    return NOERROR;
}

/*
 *  Get group zone type
 *  Type defined in IZigbeeManager::java (Ex.IZigbeeManager::ZIGBEE_ZONE_TYPE_TEMPERATURE)
 *  Parameters: group name
 *  Returns: group zone type - success, (-1)-fail
*/
ECode CZigbeeService::GetGroupZoneType(
    /* [in] */ const String& groupName,
    /* [out] */ Int32* zoneType)
{
    VALIDATE_NOT_NULL(zoneType)
    *zoneType = -1;

    //check group name valid or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "GetGroupZoneType(%s): group not exist", groupName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeGroup> zigbeeGroup = mGroups[groupName];
    *zoneType = zigbeeGroup->GetZoneType();
    if (DBG) Slogger::D(TAG, "GetGroupZoneType(%s): group zone type 0x%04x", groupName.string(), *zoneType);

    return NOERROR;
}

/*
 *  Get device count of a group
 *  Parameters: group name
 *  Returns: device count-success, -1- fail
*/
ECode CZigbeeService::GetDeviceCountOfGroup(
    /* [in] */ const String& groupName,
    /* [out] */ Int32* deviceNumber)
{
    VALIDATE_NOT_NULL(deviceNumber)
    *deviceNumber = -1;

    //check group name valid or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "GetDeviceCountOfGroup(%s): group not exist", groupName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeGroup> zigbeeGroup = mGroups[groupName];
    *deviceNumber = zigbeeGroup->GetDeviceNumber();
    if (DBG) Slogger::D(TAG, "GetDeviceCountOfGroup(%s): device count in group: %d", groupName.string(), *deviceNumber);

    return NOERROR;
}

/*
 *  Get group count
 *  Parameters: void
 *  Returns: group count
*/
ECode CZigbeeService::GetGroupCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    if (DBG) Slogger::D(TAG, "GetGroupCount() %d", mGroups.GetSize());
    *count = mGroups.GetSize();
    return NOERROR;
}

/*
 *  Get all devices of a group
 *  Parameters: group name
 *  Returns: List<String> all devices - success, NULL -fail
*/
ECode CZigbeeService::GetGroupDevices(
    /* [in] */ const String& groupName,
    /* [out, callee] */ ArrayOf<String>** list)
{
    VALIDATE_NOT_NULL(list)

    //check group name valid or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "GetGroupDevices(%s): group not exist", groupName.string());
        *list = NULL;
        return NOERROR;
    }

    if (DBG) Slogger::D(TAG, "GetGroupDevices(): group name: ", groupName.string());
    AutoPtr<ArrayOf<String> > temp = StringListToStringArray(mGroups[groupName]->GetAllDevices());
    *list = temp;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

/**************************************** General Contorl API  *******************************************/
/*
 *  Update all devices' information in the Zigbee network
 *  Parameters: void
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::ScanZigbeeDevices(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    UpdateZigbeeDevices();
    *res = 0;
    return NOERROR;
}


/*
 *  Reset a device, will remove the device from current Zigbee network
 *  Parameters: device name
 *  Returns: void
*/
ECode CZigbeeService::ResetDevice(
    /* [in] */ const String& deviceName)
{
   //putBulbCmdintoQueue(BuldFuncID.resetDevice, "resetDevice", deviceName);

    if (!IsDeviceExist(deviceName)) {
        return NOERROR;
    }

    //call HAL native API
    Int32 shortAddr = GetShortAddrByName(deviceName);
    if (shortAddr != -1) {
        if (DBG) Slogger::D(TAG, "resetDevice():   device short address: 0x%04x", shortAddr);
        Native_ResetDevice(shortAddr);
        RemoveDevice(deviceName);
    }
    else {
        Slogger::E(TAG, "resetDevice(%s):  Fail: NULL device short address", deviceName.string());
    }
    return NOERROR;
}

ECode CZigbeeService::ResetCoordinator()
{
    Native_ResetCoordinator();
    return NOERROR;
}

ECode CZigbeeService::PermitJoinNet(
    /* [in] */ Int32 cmd)
{
    Native_PermitJoinNet(cmd);
    return NOERROR;
}

/*
 *  Get online device count
 *  Parameters: void
 *  Returns: online device count
*/
ECode CZigbeeService::GetOnlineDeviceCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = Native_OnlineNumber();
    return NOERROR;
}

// ********************************************* Switch Control API *************************************************/
/*
 *  Turn on/off a switch device
 *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
 *  Parameters: on/off state, device name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetSwitchOnOffState(
    /* [in] */ Int32 command,
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetSwitchOnOffState(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    Int32 shortAddr = GetShortAddrByName(deviceName);
    *res = Native_SwitchSetState(command, shortAddr);
    if (DBG) Slogger::D(TAG, "SetSwitchOnOffState(%d, %s)", command, deviceName.string());

    //[short-term solution for sync state between remote client and local client]
    if (mCallbackMgr != NULL) {
        mCallbackMgr->NotifyDeviceStateChanged();
    }

    return NOERROR;
}

/*
 *  Turn on/off all devices in a switch group
 *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
 *  Parameters: on/off state, group device list
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetSwitchGroupOnOffState(
    /* [in] */ Int32 command,
    /* [in] */ const String& groupName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    //check group name valid or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "SetSwitchGroupOnOffState(%s): group not exist", groupName.string());
        *res = 1;
        return NOERROR;
    }

    // set group on/off state
    AutoPtr<List<String> > deviceList = mGroups[groupName]->GetAllDevices();
    List<String>::Iterator iter = deviceList->Begin();
    for (; iter != deviceList->End(); ++iter) {
        if (DBG) Slogger::D(TAG, "SetSwitchGroupOnOffState(), switch device \"%s\"", iter->string());
        Int32 shortAddr = GetShortAddrByName(*iter);
        if (shortAddr != -1) {
            if (DBG) Slogger::D(TAG, "SetSwitchGroupOnOffState(), call Native_SwitchSetState()");
            Native_SwitchSetState(command, shortAddr);
        }
    }

    //[short-term solution for sync state between remote client and local client]
    if (mCallbackMgr != NULL) {
        mCallbackMgr->NotifyDeviceStateChanged();
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Get on/off state of a switch device
 *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
 *  Parameters: device name
 *  Returns: on/off state-success, -1-fail
*/
ECode CZigbeeService::GetSwitchOnOffState(
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetSwitchOnOffState(): device \"%s\" does not exist", deviceName.string());
        *state = -1;
        return NOERROR;
    }

    Int32 shortAddr = GetShortAddrByName(deviceName);
    *state = Native_SwitchGetState(shortAddr);
    return NOERROR;
}



// ********************************************* Bulb Control API *************************************************/
/*
 *  Turn on/off a bulb device
 *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
 *  Parameters: on/off state, device name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetBulbOnOffState(
    /* [in] */ Int32 command,
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetBulbOnOffState(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    // kill SM thread if exist
    StopBulbScenarioModeThread(deviceName);

    Int32 shortAddr = GetShortAddrByName(deviceName);
    *res = Native_SwitchSetState(command, shortAddr);
    if (DBG) Slogger::D(TAG, "SetBulbOnOffState(%d, %s ) ", command, deviceName.string());

    //[short-term solution for sync state between remote client and local client]
    if (mCallbackMgr != NULL) {
        mCallbackMgr->NotifyDeviceStateChanged();
    }

    return NOERROR;
}

/*
 *  Turn on/off all devices in a group
 *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
 *  Parameters: on/off state, group device list
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetBulbGroupOnOffState(
    /* [in] */ Int32 command,
    /* [in] */ const String& groupName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    //check group name valid or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "SetBulbGroupOnOffState(%s): group not exist", groupName.string());
        *res = 1;
        return NOERROR;
    }

    // kill devices' Scenario Mode thread if exist
    AutoPtr<List<String> > deviceList = mGroups[groupName]->GetAllDevices();
    List<String>::Iterator iter = deviceList->Begin();
    for (; iter != deviceList->End(); ++iter) {
        StopBulbScenarioModeThread(*iter);
    }

    // set group on/off state
    iter = deviceList->Begin();
    for (; iter != deviceList->End(); ++iter) {
        if (DBG) Slogger::D(TAG, "SetBulbGroupOnOffState(), switch device \"%s\"", iter->string());
        Int32 shortAddr = GetShortAddrByName(*iter);
        if (shortAddr != -1) {
            if (DBG) Slogger::D(TAG, "SetBulbGroupOnOffState(), call Native_SwitchSetState()");
            Native_SwitchSetState(command, shortAddr);
        }
    }

    //[short-term solution for sync state between remote client and local client]
    if (mCallbackMgr != NULL) {
        mCallbackMgr->NotifyDeviceStateChanged();
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Get on/off state of a bulb device
 *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
 *  Parameters: device name
 *  Returns: on/off state-success, -1-fail
*/
ECode CZigbeeService::GetBulbOnOffState(
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* res)
{
    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetBulbOnOffState(): device \"%s\" does not exist", deviceName.string());
        *res = -1;
        return NOERROR;
    }

    Int32 shortAddr = GetShortAddrByName(deviceName);
    *res = Native_SwitchGetState(shortAddr);
    return NOERROR;
}

/*
 *  Start scenario mode of a bulb type group
 *  Parameters:   Par1: group name, Par2: SM mode (1: random color, 2: RGB cycle color)
 *                           Par3(delayTime) and Par4(randomDelay) now is not used
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetBulbGroupScenarioMode(
    /* [in] */ const String& groupName,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 delayTime,
    /* [in] */ Boolean randomDelay,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (DBG) Slogger::D(TAG, "SetBulbGroupScenarioMode(), group: %s", groupName.string());

    //check group name valid or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "SetBulbGroupScenarioMode(%s): group not exist", groupName.string());
        *res = 1;
        return NOERROR;
    }

    // kill devices' Scenario Mode thread if exists
    AutoPtr<List<String> > deviceList = mGroups[groupName]->GetAllDevices();
    List<String>::Iterator iter = deviceList->Begin();
    for (; iter != deviceList->End(); ++iter) {
        StopBulbScenarioModeThread(*iter);
    }

    // set scenario mode of all device in the group
    iter = deviceList->Begin();
    for (; iter != deviceList->End(); ++iter) {
        if (!IsDeviceExist(*iter)) {
            Slogger::E(TAG, "SetBulbGroupScenarioMode(): device \"%s\" does not exist", iter->string());
            *res = 1;
            return NOERROR;
        }
        if (DBG) Slogger::D(TAG, "SetBulbGroupScenarioMode(), create and start device: \"%s\" SM thread", iter->string());
        AutoPtr<BulbScenarioModeThread> bulbThread = new BulbScenarioModeThread(GetShortAddrByName(*iter), mode, this);
        mBulbSMThreads[*iter] = bulbThread;
        bulbThread->Start();
    }

    *res = 0;
    return NOERROR;
}


/*
 *  Start scenario mode of a bulb type device
 *  Parameters:   Par1: group name, Par2: SM mode (1: random color, 2: RGB cycle color)
 *                           Par3(delayTime) and Par4(randomDelay) now is not used
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetBulbScenarioMode(
    /* [in] */ const String& devName,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 delayTime,
    /* [in] */ Boolean randomDelay,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (DBG) Slogger::D(TAG, "SetBulbScenarioMode(), device: %s", devName.string());

    if (!IsDeviceExist(devName)) {
        Slogger::E(TAG, "SetBulbScenarioMode(): device \"%s\" does not exist", devName.string());
        *res = 1;
        return NOERROR;
    }

    // kill SM thread if exist
    StopBulbScenarioModeThread(devName);

    // create new SM thread
    if (DBG) Slogger::D(TAG, "SetBulbScenarioMode(), create and start device: %s SM thread", devName.string());
    AutoPtr<BulbScenarioModeThread> bulbThread = new BulbScenarioModeThread(GetShortAddrByName(devName), mode, this);
    mBulbSMThreads[devName] = bulbThread;
    bulbThread->Start();

    *res = 0;
    return NOERROR;
}

/*
 *  Set luminance of a bulb type group
 *  Parameters: level, group name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetBulbGroupLuminance(
    /* [in] */ Int32 level,
    /* [in] */ const String& groupName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    //check group name valid or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "SetBulbGroupLuminance(%s): group not exist", groupName.string());
        *res = 1;
        return NOERROR;
    }

    AutoPtr<List<String> > deviceList = mGroups[groupName]->GetAllDevices();
    // set group light luminance
    List<String>::Iterator iter = deviceList->Begin();
    for (; iter != deviceList->End(); ++iter) {
        Int32 shortAddr = GetShortAddrByName(*iter);
        if (shortAddr != -1) {
            Native_LightSetDim(level, shortAddr);
            if (DBG) Slogger::D(TAG, "SetBulbGroupLuminance(%d, %s)", level, iter->string());
        }
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Set brightness of a bulb type device
 *  Parameters: level, device name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetBulbLuminance(
    /* [in] */ Int32 level,
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetBulbLuminance(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    Int32 shortAddr = GetShortAddrByName(deviceName);
    Int32 result = Native_LightSetDim(level, shortAddr);

    if (DBG) Slogger::D(TAG, "SetBulbLuminance(%d, %s ) ", level, deviceName.string());

    *res = result;
    return NOERROR;
}

/*
 *  Get brightness of a bulb type device
 *  Parameters: device name
 *  Returns: brightness of the device-success, 1-fail
*/
ECode CZigbeeService::GetBulbLuminance(
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* brightness)
{
    VALIDATE_NOT_NULL(brightness)
    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetBulbLuminance(): device \"%s\" does not exist", deviceName.string());
        *brightness = 1;
        return NOERROR;
    }

    Int32 shortAddr = GetShortAddrByName(deviceName);
    *brightness = Native_LightGetDim(shortAddr);
    return NOERROR;
}

/*
 *  Set color of a bulb type group
 *  Parameters: R, G, B, group name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetBulbGroupColor(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in] */ const String& groupName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    //check group name valid or not
    if (!IsGroupExist(groupName)) {
        Slogger::E(TAG, "SetBulbGroupColor(%s): group not exist", groupName.string());
        *res = 1;
        return NOERROR;
    }

    // kill devices' Scenario Mode thread if exist
    AutoPtr<List<String> > deviceList = mGroups[groupName]->GetAllDevices();
    List<String>::Iterator iter = deviceList->Begin();
    for (; iter != deviceList->End(); ++iter) {
        StopBulbScenarioModeThread(*iter);
    }

    // set group light color
    Int32 rgb = ((red << 16) & 0x00ff0000) + ((green << 8) & 0x0000ff00) + (blue & 0x000000ff);
    iter = deviceList->Begin();
    for (; iter != deviceList->End(); ++iter) {
        Int32 shortAddr = GetShortAddrByName(*iter);
        if (shortAddr != -1) {
            Native_LightSetColor(rgb, shortAddr);
            if (DBG) Slogger::D(TAG, "SetBulbGroupColor(%d, %d, %d (0x%08x), %s)",
                        red, green, blue, rgb, iter->string());
        }
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Set color of a bulb type device
 *  Parameters: R, G, B, device name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetBulbColor(
    /* [in] */ Int32 red,
    /* [in] */ Int32 green,
    /* [in] */ Int32 blue,
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetBulbColor(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    // kill SM thread if exist
    StopBulbScenarioModeThread(deviceName);

    Int32 rgb = ((red << 16) & 0x00ff0000) + ((green << 8) & 0x0000ff00) + (blue & 0x000000ff);
    Int32 shortAddr = GetShortAddrByName(deviceName);
    *res = Native_LightSetColor(rgb, shortAddr);

    if (DBG) Slogger::D(TAG, "SetBulbColor(%d, %d, %d (0x%08x), %s)",
                    red, green, blue, rgb, deviceName.string());

    return NOERROR;
}

/*
 *  Get color of a bulb type device
 *  Parameters: device name
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::GetBulbColor(
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetBulbColor(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    Int32 shortAddr = GetShortAddrByName(deviceName);
    *res = Native_LightGetColor(shortAddr);
    return NOERROR;
}


// ***************************************** Humidity Sensor API ******************************************/
/*
 *  Get sensor humidity
 *  Parameters: device name
 *  Returns: humidity (0~100) - success, -1 - fail
*/
ECode CZigbeeService::GetSensorHumidity(
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* humidity)
{
    AutoPtr<ZigbeeHumiditySensor> humiditySensor;
    // Boolean isLowerWarning = FALSE;
    // Boolean isUpperWarning = FALSE;
    // Boolean isActionSet = FALSE;
    // AutoPtr<IIntent> intent;
    // AutoPtr<IBundle> bundle;

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetSensorHumidity(): device \"%s\" does not exist", deviceName.string());
        *humidity = -1;
        return NOERROR;
    }

    //notify application the warning humidity if warning setting is enabled
    if (mDevices[deviceName]->GetClass() == "ZigbeeHumiditySensor") {
        humiditySensor = (ZigbeeHumiditySensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        humiditySensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetHumiditySensor();
    }
    else {
        Slogger::E(TAG, "GetSensorHumidity(): device \"%s\" is not a ZigbeeHumiditySensor", deviceName.string());
        *humidity = -1;
        return NOERROR;
    }

    // get humidity
    *humidity = Native_GetSensorHumidity(humiditySensor->GetShortAddr());
    if (DBG) Slogger::D(TAG, "GetSensorHumidity(): devName: %s", deviceName.string());
    if (DBG) Slogger::D(TAG, "GetSensorHumidity(): humiditySensor: %p", humiditySensor.Get());

    /*
    //get current system time
    SimpleDateFormat sDateFormat = new SimpleDateFormat("yyyy-MM-dd-H-mm-ss");
    String time = sDateFormat.format(new java.util.Date());

    if (humiditySensor->GetLowerLimitWarningSetting() && !humiditySensor->GetUpperLimitWarningSetting()) {   //check if lower limit warning is enabled
        isLowerWarning = humiditySensor->IsLowerLimitWarning(humidity);
        if (isLowerWarning) {
            // [short-term solution]for remote client warning , due to remote client can't get callback warning message
            humiditySensor->SetLastAlertTime(time);
            humiditySensor->SetLastAlertHumidity(humidity);

            CIntent::New(IZigbeeManager::ACTION_SENSOR_HUMIDITY_LOWER_LIMIT_WARNING, (IIntent**)&intent);
            isActionSet = TRUE;
            if (DBG) Slogger::D(TAG, "GetSensorHumidity(): send ACTION_SENSOR_HUMIDITY_LOWER_LIMIT_WARNING");
        }
    }
    else if (!humiditySensor->GetLowerLimitWarningSetting() && humiditySensor->GetUpperLimitWarningSetting()) {  //check if upper limit warning is enabled
        isUpperWarning = humiditySensor->IsUpperLimitWarning(humidity);
        if (isUpperWarning) {
            //[short-term solution]for remote client warning , due to remote client can't get callback warning message
            humiditySensor->SetLastAlertTime(time);
            humiditySensor->SetLastAlertHumidity(humidity);

            CIntent::New(IZigbeeManager::ACTION_SENSOR_HUMIDITY_UPPER_LIMIT_WARNING, (IIntent**)&intent);
            isActionSet = TRUE;
            if (DBG) Slogger::D(TAG, "GetSensorHumidity(): send ACTION_SENSOR_HUMIDITY_UPPER_LIMIT_WARNING");
        }
    }
    else if (humiditySensor->GetLowerLimitWarningSetting() && humiditySensor->GetUpperLimitWarningSetting()) {
        isLowerWarning = humiditySensor->IsLowerLimitWarning(humidity);
        isUpperWarning = humiditySensor->IsUpperLimitWarning(humidity);
        if (DBG) Slogger::D(TAG, "GetSensorHumidity(): isLowerWarning/isUpperWarning = " + isLowerWarning + " / " + isUpperWarning);
        if (isLowerWarning && isUpperWarning) {
            // [short-term solution]for remote client warning , due to remote client can't get callback warning message
            humiditySensor->SetLastAlertTime(time);
            humiditySensor->SetLastAlertHumidity(humidity);

            CIntent::New(IZigbeeManager::ACTION_SENSOR_HUMIDITY_WARNING, (IIntent**)&intent);
            isActionSet = TRUE;
            if (DBG) Slogger::D(TAG, "GetSensorHumidity(): send ACTION_SENSOR_HUMIDITY_WARNING");
        }

    }

    if (isActionSet) {
        //general field
        bundle = new Bundle();
        bundle->PutString(IZigbeeManager::EXTRA_NAME, deviceName);
        bundle->PutString(IZigbeeManager::EXTRA_TIME, time);
        bundle->PutInt32(IZigbeeManager::EXTRA_HUMIDITY, humidity);

        if (intent != NULL) {
            intent->PutExtras(bundle);

            if (mContext != NULL) {
                if (DBG) Slogger::D(TAG, "GetSensorHumidity(): sendBroadcast()");
                mContext.sendBroadcast(intent);
                //mContext.sendBroadcastAsUser(intent, new UserHandle(UserHandle.USER_CURRENT));
            }
            else {
                Slogger::E(TAG, "GetSensorHumidity(): CZigbeeService's context is NULL");
                return 9999;
            }
        }
        else {
            Slogger::E(TAG, "GetSensorHumidity(): intent is NULL");
            return 9999;
        }
    }
    */

    return NOERROR;
}

/*
 *  Set humidity sensor's history record setting
 *  Parameters: device name, TRUE-enable history record/FALSE-disable history record, history record filename(NULL if disable setting)
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetHumiditySensorHistorySetting(
    /* [in] */ const String& deviceName,
    /* [in] */ Boolean enable,
    /* [in] */ const String& filename,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetHumiditySensorHistorySetting(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    AutoPtr<ZigbeeHumiditySensor> humiditySensor;
    //check if class type is ZigbeeHumiditySensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeHumiditySensor") {
        if (DBG) Slogger::D(TAG, "SetHumiditySensorHistorySetting(): device \"%s\" is ZigbeeHumiditySensor class", deviceName.string());
        humiditySensor = (ZigbeeHumiditySensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        humiditySensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetHumiditySensor();
    }
    else {
        Slogger::E(TAG, "SetHumiditySensorHistorySetting(): device \"%s\" is not ZigbeeHumiditySensor class", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    humiditySensor->SetHistorySetting(enable);
    if (enable) {
        if (humiditySensor->SetHistoryFileName(filename) == 1) {
            Slogger::E(TAG, "SetHumiditySensorHistorySetting(): device \"%s\": illegal history record file name", deviceName.string());
            *res = 1;
            return NOERROR;
        }
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Set humidity sensor's all warning setting
 *  Parameters: device name, lower limit setting, lower limit humidity, upper limit setting, upper limit humidity
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetHumiditySensorAllWarningSetting(
    /* [in] */ const String& deviceName,
    /* [in] */ Boolean lowerLimitSetting,
    /* [in] */ Int32 lowerLimitHumidity,
    /* [in] */ Boolean upperLimitSetting,
    /* [in] */ Int32 upperLimitHumidity,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetHumiditySensorAllWarningSetting(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    AutoPtr<ZigbeeHumiditySensor> humiditySensor;
    if (mDevices[deviceName]->GetClass() == "ZigbeeHumiditySensor") {
        humiditySensor = (ZigbeeHumiditySensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        humiditySensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetHumiditySensor();
    }
    else {
        Slogger::E(TAG, "SetHumiditySensorAllWarningSetting(): device \"%s\" is not ZigbeeHumiditySensor", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    humiditySensor->SetLowerLimitWarningSetting(lowerLimitSetting);
    humiditySensor->SetUpperLimitWarningSetting(upperLimitSetting);
    humiditySensor->SetWarningHumidityLowerLimit(lowerLimitHumidity);
    humiditySensor->SetWarningHumidityUpperLimit(upperLimitHumidity);
    if (DBG) Slogger::D(TAG, "SetHumiditySensorAllWarningSetting(): device \"%s\""
            ": setLower/UpperLimitWarningSetting(%d / %d)", deviceName.string(), lowerLimitSetting, upperLimitSetting);
    if (DBG) Slogger::D(TAG, "SetHumiditySensorAllWarningSetting(): device \"%s\""
            ": setWarningHumidityLower/UpperLimit(%d / %d)", deviceName.string(), lowerLimitHumidity, upperLimitHumidity);

    *res = 0;
    return NOERROR;
}

/*
 *  Set humidity sensor's warning enable setting
 *  Parameters: device name, IZigbeeManager::WARNING_HUMIDITY_UPPER_LIMIT/IZigbeeManager::WARNING_HUMIDITY_LOWER_LIMIT, TRUE: enable/FALSE: disable
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetHumiditySensorWarningSetting(
    /* [in] */ const String& deviceName,
    /* [in] */ Int32 limitSelection,
    /* [in] */ Boolean setting,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetHumiditySensorWarningSetting(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    if (limitSelection != IZigbeeManager::WARNING_HUMIDITY_UPPER_LIMIT
        && limitSelection != IZigbeeManager::WARNING_HUMIDITY_LOWER_LIMIT) {
        Slogger::E(TAG, "SetHumiditySensorWarningSetting(): device \"%s\" - invalid upper/lower limit", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    AutoPtr<ZigbeeHumiditySensor> humiditySensor;
    //check if class type is ZigbeeHumiditySensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeHumiditySensor") {
        humiditySensor = (ZigbeeHumiditySensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        humiditySensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetHumiditySensor();
    }
    else {
        Slogger::E(TAG, "SetHumiditySensorWarningSetting(): device \"%s\" is not ZigbeeHumiditySensor", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    if (limitSelection == IZigbeeManager::WARNING_HUMIDITY_UPPER_LIMIT) {
        humiditySensor->SetUpperLimitWarningSetting(setting);
        if (DBG) Slogger::D(TAG, "SetHumiditySensorWarningSetting(): device \"%s\": setUpperLimitWarningSetting(%d)",
                    deviceName.string(), setting);
    }
    else if (limitSelection == IZigbeeManager::WARNING_HUMIDITY_LOWER_LIMIT) {
        humiditySensor->SetLowerLimitWarningSetting(setting);
        if (DBG) Slogger::D(TAG, "SetHumiditySensorWarningSetting(): device \"%s\": setLowerLimitWarningSetting(%d)",
                    deviceName.string(), setting);
    }

    *res = 0;
    return NOERROR;
}


// ***************************************** Temperature Sensor API ******************************************/
/*
 *  Get temperature sensor's history record start time
 *  Parameters: device name
 *  Returns: start time(format: yyyy-MM-dd-H-mm-ss) - success, NA - fail
*/
ECode CZigbeeService::GetTemperatureHistoryStartTime(
    /* [in] */ const String& deviceName,
    /* [out] */ String* startTime)
{
    VALIDATE_NOT_NULL(startTime)

    *startTime = String("NA");

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetTemperatureHistoryStartTime(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "GetTemperatureHistoryStartTime(): device \"%s\" is not ZigbeeTemperatureSensor", deviceName.string());
        return NOERROR;
    }

    AutoPtr<List<String> > historyTime = tempSensor->GetHistoryTime();
    *startTime = historyTime->GetFront();
    if (DBG) Slogger::D(TAG, "GetTemperatureHistoryStartTime(): device \"%s\": start time: %d", deviceName.string(), startTime);

    return NOERROR;
}

/*
 *  Get temperature sensor's history record end time
 *  Parameters: device name
 *  Returns: end time(format: yyyy-MM-dd-H-mm-ss) - success, NA - fail
*/
ECode CZigbeeService::GetTemperatureHistoryEndTime(
    /* [in] */ const String& deviceName,
    /* [out] */ String* endTime)
{
    VALIDATE_NOT_NULL(endTime)

    *endTime = String("NA");

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetTemperatureHistoryEndTime(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "GetTemperatureHistoryEndTime(): device \"%s\" is not ZigbeeTemperatureSensor", deviceName.string());
        return NOERROR;
    }

    AutoPtr<List<String> > historyTime = tempSensor->GetHistoryTime();
    *endTime = historyTime->GetBack();
    if (DBG) Slogger::D(TAG, "GetTemperatureHistoryEndTime(): device \"%s\": end time: %d", deviceName.string(), endTime);

    return NOERROR;
}

/*
 *  Get temperature sensor's history record average temperature
 *  Parameters: device name
 *  Returns: average temperature - success, -9999 - fail
*/
ECode CZigbeeService::GetHistoryAverageTemperature(
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* avgTemp)
{
    VALIDATE_NOT_NULL(avgTemp)
    *avgTemp = -9999;

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetHistoryAverageTemperature(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "GetHistoryAverageTemperature(): device \"%s\" is not ZigbeeTemperatureSensor", deviceName.string());
        return NOERROR;
    }

    *avgTemp = tempSensor->GetHistoryAvgTemperature();
    if (DBG) Slogger::D(TAG, "GetHistoryAverageTemperature(): device \"%s\": average temperature: %d", deviceName.string(), avgTemp);

    return NOERROR;
}

/*
 *  Get temperature sensor's max temperature in history record
 *  Parameters: device name
 *  Returns: max temperature - success, -9999 - fail
*/
ECode CZigbeeService::GetHistoryMaxTemperature(
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* temp)
{
    VALIDATE_NOT_NULL(temp)
    *temp = -9999;

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetHistoryMaxTemperature(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "GetHistoryMaxTemperature(): device \"%s\" is not ZigbeeTemperatureSensor", deviceName.string());
        return NOERROR;
    }

    *temp = tempSensor->GetHistoryMaxTemperature();
    if (DBG) Slogger::D(TAG, "GetHistoryMaxTemperature(): device \"%s\": max temperature: %d", deviceName.string(), temp);

    return NOERROR;
}

/*
 *  Get temperature sensor's min temperature in history record
 *  Parameters: device name
 *  Returns: min temperature - success, 9999 - fail
*/
ECode CZigbeeService::GetHistoryMinTemperature(
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* temp)
{
    VALIDATE_NOT_NULL(temp)
    *temp = 9999;

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetHistoryMinTemperature(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "GetHistoryMinTemperature(): device \"%s\" is not ZigbeeTemperatureSensor", deviceName.string());
        return NOERROR;
    }

    *temp = tempSensor->GetHistoryMinTemperature();
    if (DBG) Slogger::D(TAG, "GetHistoryMinTemperature(): device \"%s\": min temperature: %d", deviceName.string(), temp);

    return NOERROR;
}

/*
 *  Get temperature sensor's min temperature time in history record
 *  Parameters: device name
 *  Returns: min temperature time(format: yyyy-MM-dd-H-mm-ss) - success, NA - fail
*/
ECode CZigbeeService::GetHistoryMinTemperatureTime(
    /* [in] */ const String& deviceName,
    /* [out] */ String* time)
{
    VALIDATE_NOT_NULL(time)

    *time = String("NA");

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetHistoryMinTemperatureTime(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "GetHistoryMinTemperatureTime(): device \"%s\" is not ZigbeeTemperatureSensor", deviceName.string());
        return NOERROR;
    }

    *time = tempSensor->GetHistoryMinTemperatureTime();
    if (DBG) Slogger::D(TAG, "GetHistoryMinTemperatureTime(): device \"%s\": min temperature time: %s",
                deviceName.string(), time->string());

    return NOERROR;
}

/*
 *  Get temperature sensor's max temperature time in history record
 *  Parameters: device name
 *  Returns: max temperature time(format: yyyy-MM-dd-H-mm-ss) - success, NA - fail
*/
ECode CZigbeeService::GetHistoryMaxTemperatureTime(
    /* [in] */ const String& deviceName,
    /* [out] */ String* time)
{
    VALIDATE_NOT_NULL(time)

    *time = String("NA");

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetHistoryMaxTemperatureTime(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "GetHistoryMaxTemperatureTime(): device \"%s\" is not ZigbeeTemperatureSensor", deviceName.string());
        return NOERROR;
    }

    *time = tempSensor->GetHistoryMaxTemperatureTime();
    if (DBG) Slogger::D(TAG, "GetHistoryMaxTemperatureTime(): device \"%s\": max temperature time: %d",
                deviceName.string(), time->string());

    return NOERROR;
}


/*
 *  Set temperature sensor's all warning setting
 *  Parameters: device name, lower limit setting, lower limit temperature, upper limit setting, upper limit temperature
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetTemperatureSensorAllWarningSetting(
    /* [in] */ const String& deviceName,
    /* [in] */ Boolean lowerLimitSetting,
    /* [in] */ Int32 lowerLimitTemp,
    /* [in] */ Boolean upperLimitSetting,
    /* [in] */ Int32 upperLimitTemp,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetTemperatureSensorAllWarningSetting(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "SetTemperatureSensorAllWarningSetting(): device \"%s\" is not ZigbeeTemperatureSensor", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    tempSensor->SetLowerLimitWarningSetting(lowerLimitSetting);
    tempSensor->SetUpperLimitWarningSetting(upperLimitSetting);
    tempSensor->SetWarningTemperatureLowerLimit(lowerLimitTemp);
    tempSensor->SetWarningTemperatureUpperLimit(upperLimitTemp);
    if (DBG) Slogger::D(TAG, "SetTemperatureSensorAllWarningSetting(): device \"%s\": setLower/UpperLimitWarningSetting(%d / %d)",
                deviceName.string(), lowerLimitSetting, upperLimitSetting);
    if (DBG) Slogger::D(TAG, "SetTemperatureSensorAllWarningSetting(): device \"%s\": setWarningTemperatureLower/UpperLimit(%d / %d)",
                deviceName.string(), lowerLimitTemp, upperLimitTemp);

    *res = 0;
    return NOERROR;
}

/*
 *  Set temperature sensor's warning enable setting
 *  Parameters: device name, IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT/IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetTemperatureSensorWarningSetting(
    /* [in] */ const String& deviceName,
    /* [in] */ Int32 limitSelection,
    /* [in] */ Boolean setting,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetTemperatureSensorWarningSetting(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    if (limitSelection != IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT
        && limitSelection != IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT) {
        Slogger::E(TAG, "SetTemperatureSensorWarningSetting(): device \"%s\" - invalid upper/lower limit", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    //check if class type is ZigbeeTemperatureSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "SetTemperatureSensorWarningSetting(): device \"%s\" is not ZigbeeTemperatureSensor", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    if (limitSelection == IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT) {
        tempSensor->SetUpperLimitWarningSetting(setting);
        if (DBG) Slogger::D(TAG, "SetTemperatureSensorWarningSetting(): device \"%s\": setUpperLimitWarningSetting(%d)",
                    deviceName.string(), setting);
    }
    else if (limitSelection == IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT) {
        tempSensor->SetLowerLimitWarningSetting(setting);
        if (DBG) Slogger::D(TAG, "SetTemperatureSensorWarningSetting(): device \"%s\": setLowerLimitWarningSetting(%d)",
                    deviceName.string(), setting);
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Get temperature sensor's warning enable setting
 *  Parameters: device name, IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT/IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT
 *  Returns: TRUE-warning is enabled/FALSE-warning is disabled/error
*/
ECode CZigbeeService::GetTemperatureSensorWarningSetting(
    /* [in] */ const String& deviceName,
    /* [in] */ Int32 limitSelection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetTemperatureSensorWarningSetting(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    //check if class type is ZigbeeTemperatureSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "GetTemperatureSensorWarningSetting(): device \"%s\" is not ZigbeeTemperatureSensor", deviceName.string());
        return NOERROR;
    }

    if (limitSelection == IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT) {
        *result = tempSensor->GetUpperLimitWarningSetting();
        if (DBG) Slogger::D(TAG, "GetTemperatureSensorWarningSetting(): device \"%s\": getUpperLimitWarningSetting(): %d",
                    deviceName.string(), *result);
    }
    else if (limitSelection == IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT) {
        *result = tempSensor->GetLowerLimitWarningSetting();
        if (DBG) Slogger::D(TAG, "GetTemperatureSensorWarningSetting(): device \"%s\": getLowerLimitWarningSetting(): %d",
                    deviceName.string(), *result);
    }

    return NOERROR;
}

/*
 *  Set temperature sensor's warning temperature
 *  Parameters: device name, IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT/IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT, temperature
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetTemperatureSensorWarningTemperature(
    /* [in] */ const String& deviceName,
    /* [in] */ Int32 limitSelection,
    /* [in] */ Int32 temperature,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetTemperatureSensorWarningTemperature(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    if (limitSelection != IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT
        && limitSelection != IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT) {
        Slogger::E(TAG, "SetTemperatureSensorWarningTemperature(): device \"%s\" - invalid upper/lower limit", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    //check if class type is ZigbeeTemperatureSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "SetTemperatureSensorWarningTemperature(): device \"%s\" is not ZigbeeTemperatureSensor", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    if (limitSelection == IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT) {
        tempSensor->SetWarningTemperatureUpperLimit(temperature);
        if (DBG) Slogger::D(TAG, "SetTemperatureSensorWarningTemperature(): device \"%s\": setWarningTemperatureUpperLimit(%d)",
                    deviceName.string(), temperature);
    }
    else if (limitSelection == IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT) {
        tempSensor->SetWarningTemperatureLowerLimit(temperature);
        if (DBG) Slogger::D(TAG, "SetTemperatureSensorWarningTemperature(): device \"%s\": setWarningTemperatureLowerLimit(%d)",
                    deviceName.string(), temperature);
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Get temperature sensor's warning temperature
 *  Parameters: device name, IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT/IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT
 *  Returns: temperature -success/9999-fail
*/
ECode CZigbeeService::GetTemperatureSensorWarningTemperature(
    /* [in] */ const String& deviceName,
    /* [in] */ Int32 limitSelection,
    /* [out] */ Int32* result)
{
    *result = 9999;
    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetTemperatureSensorWarningTemperature(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    //check if class type is ZigbeeTemperatureSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "GetTemperatureSensorWarningTemperature(): device \"%s\" is not ZigbeeTemperatureSensor", deviceName.string());
        return NOERROR;
    }

    if (limitSelection == IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT) {
        *result = tempSensor->GetWarningTemperatureUpperLimit();
        if (DBG) Slogger::D(TAG, "GetTemperatureSensorWarningTemperature(): device \"%s\": getWarningTemperatureUpperLimit(): %d",
                    deviceName.string(), *result);
    }
    else if (limitSelection == IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT) {
        *result = tempSensor->GetWarningTemperatureLowerLimit();
        if (DBG) Slogger::D(TAG, "GetTemperatureSensorWarningTemperature(): device \"%s\": getWarningTemperatureLowerLimit(): %d",
                    deviceName.string(), *result);
    }

    return NOERROR;
}


/*
 *  Set temperature sensor's history record setting
 *  Parameters: device name, TRUE-enable history record/FALSE-disable history record, history record filename(NULL if disable setting)
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetTemperatureSensorHistorySetting(
    /* [in] */ const String& deviceName,
    /* [in] */ Boolean enable,
    /* [in] */ const String& filename,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetTemperatureSensorHistorySetting(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    //check if class type is ZigbeeTemperatureSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        if (DBG) Slogger::D(TAG, "SetTemperatureSensorHistorySetting(): device \"%s\" is ZigbeeTemperatureSensor class", deviceName.string());
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "SetTemperatureSensorHistorySetting(): device \"%s\" is not ZigbeeTemperatureSensor class", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    tempSensor->SetHistorySetting(enable);
    if (enable) {
        if (tempSensor->SetHistoryFileName(filename) == 1) {
            Slogger::E(TAG, "SetTemperatureSensorHistorySetting(): device \"%s\": illegal history record file name", deviceName.string());
            *res = 1;
            return NOERROR;
        }
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Get temperature sensor's history record setting
 *  Parameters: device name
 *  Returns: TRUE-history record is enabled/FALSE-history record is disabled/error
*/
ECode CZigbeeService::GetTemperatureSensorHistorySetting(
    /* [in] */ const String& deviceName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetTemperatureSensorHistorySetting(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    //check if class type is ZigbeeTemperatureSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        if (DBG) Slogger::D(TAG, "GetTemperatureSensorHistorySetting(): device \"%s\" is ZigbeeTemperatureSensor class", deviceName.string());
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "GetTemperatureSensorHistorySetting(): device \"%s\" is not ZigbeeTemperatureSensor class", deviceName.string());
        return NOERROR;
    }

    *result = tempSensor->GetHistorySetting();

    return NOERROR;
}


/*
 *  Get sensor temperature
 *  Parameters: device name
 *  Returns: temperture-success, 9999-fail
*/
ECode CZigbeeService::GetSensorTemperature(
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* temperature)
{
    AutoPtr<ZigbeeTemperatureSensor> tempSensor;
    // Boolean isLowerWarning = FALSE;
    // Boolean isUpperWarning = FALSE;
    // Boolean isActionSet = FALSE;
    // AutoPtr<IIntent> intent;
    // AutoPtr<Bundle> bundle;

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetSensorTemperature(): device \"%s\" does not exist", deviceName.string());
        *temperature = 9999;
        return NOERROR;
    }

    //notify application the warning temperature if warning setting is enabled
    if (mDevices[deviceName]->GetClass() == "ZigbeeTemperatureSensor") {
        tempSensor = (ZigbeeTemperatureSensor*)mDevices[deviceName].Get();
    }
    else if (mDevices[deviceName]->GetClass() == "ZigbeeTHSensor") {
        tempSensor = ((ZigbeeTHSensor*)mDevices[deviceName].Get())->GetTemperatureSensor();
    }
    else {
        Slogger::E(TAG, "GetSensorTemperature(): device \"%s\" is not a ZigbeeTemperatureSensor", deviceName.string());
        *temperature = 9999;
        return NOERROR;
    }

    // get temperature
    *temperature = Native_GetSensorTemperature(mDevices[deviceName]->GetShortAddr());
    if (DBG) Slogger::D(TAG, "GetSensorTemperature(): devName: %s", deviceName.string());
    if (DBG) Slogger::D(TAG, "GetSensorTemperature(): temperature: %d", *temperature);

    /*
    //get current system time
    SimpleDateFormat sDateFormat = new SimpleDateFormat("yyyy-MM-dd-H-mm-ss");
    String time = sDateFormat.format(new java.util.Date());

    if (tempSensor->GetLowerLimitWarningSetting() && !tempSensor->GetUpperLimitWarningSetting()) {   //check if lower limit warning is enabled
        isLowerWarning = tempSensor->IsLowerLimitWarning(temperature);
        if (isLowerWarning) {
            // [short-term solution]for remote client warning , due to remote client can't get callback warning message
            tempSensor->SetLastAlertTime(time);
            tempSensor->SetLastAlertTemperature(temperature);

            CIntent::New(IZigbeeManager::ACTION_SENSOR_TEMPERATURE_LOWER_LIMIT_WARNING, (IIntent**)&intent);
            isActionSet = TRUE;
            if (DBG) Slogger::D(TAG, "GetSensorTemperature(): send ACTION_SENSOR_TEMPERATURE_LOWER_LIMIT_WARNING");
        }
    }
    else if (!tempSensor->GetLowerLimitWarningSetting() && tempSensor->GetUpperLimitWarningSetting()) {  //check if upper limit warning is enabled
        isUpperWarning = tempSensor->IsUpperLimitWarning(temperature);
        if (isUpperWarning) {
            //[short-term solution]for remote client warning , due to remote client can't get callback warning message
            tempSensor->SetLastAlertTime(time);
            tempSensor->SetLastAlertTemperature(temperature);

            CIntent::New(IZigbeeManager::ACTION_SENSOR_TEMPERATURE_UPPER_LIMIT_WARNING, (IIntent**)&intent);
            isActionSet = TRUE;
            if (DBG) Slogger::D(TAG, "GetSensorTemperature(): send ACTION_SENSOR_TEMPERATURE_UPPER_LIMIT_WARNING");
        }
    }
    else if (tempSensor->GetLowerLimitWarningSetting() && tempSensor->GetUpperLimitWarningSetting()) {
        isLowerWarning = tempSensor->IsLowerLimitWarning(temperature);
        isUpperWarning = tempSensor->IsUpperLimitWarning(temperature);
        if (DBG) Slogger::D(TAG, "GetSensorTemperature(): isLowerWarning/isUpperWarning = " + isLowerWarning + " / " + isUpperWarning);
        if (isLowerWarning && isUpperWarning) {
            // [short-term solution]for remote client warning , due to remote client can't get callback warning message
            tempSensor->SetLastAlertTime(time);
            tempSensor->SetLastAlertTemperature(temperature);

            CIntent::New(IZigbeeManager::ACTION_SENSOR_TEMPERATURE_WARNING, (IIntent**)&intent);
            isActionSet = TRUE;
            if (DBG) Slogger::D(TAG, "GetSensorTemperature(): send ACTION_SENSOR_TEMPERATURE_WARNING");
        }

    }

    if (isActionSet) {
        //general field
        bundle = new Bundle();
        bundle->PutString(IZigbeeManager::EXTRA_NAME, deviceName);
        bundle->PutString(IZigbeeManager::EXTRA_TIME, time);
        bundle->PutInt32(IZigbeeManager::EXTRA_TEMPERATURE, temperature);

        if (intent != NULL) {
            intent->PutExtras(bundle);

            if (mContext != NULL) {
                if (DBG) Slogger::D(TAG, "GetSensorTemperature(): sendBroadcast()");
                mContext.sendBroadcast(intent);
                //mContext.sendBroadcastAsUser(intent, new UserHandle(UserHandle.USER_CURRENT));
            }
            else {
                Slogger::E(TAG, "GetSensorTemperature(): CZigbeeService's context is NULL");
                return 9999;
            }
        }
        else {
            Slogger::E(TAG, "GetSensorTemperature(): intent is NULL");
            return 9999;
        }
    }
    */

    return NOERROR;
}

// ***************************************** General Sensor API ******************************************/
/*
 *  Get sensor voltage
 *  Parameters: device name
 *  Returns: voltage-success, -1-fail
*/
ECode CZigbeeService::GetSensorVoltage(
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* voltage)
{
    VALIDATE_NOT_NULL(voltage)
    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetSensorVoltage(): device \"%s\" does not exist", deviceName.string());
        *voltage = -1;
        return NOERROR;
    }

    *voltage = Native_GetSensorVoltage(mDevices[deviceName]->GetShortAddr());
    if (DBG) Slogger::D(TAG, "GetSensorVoltage(): devName: %s", deviceName.string());
    if (DBG) Slogger::D(TAG, "GetSensorVoltage(): voltage: %d", *voltage);

    return NOERROR;
}


// ***************************************** Door Sensor API ******************************************/
/*
 *  Get door sensor's history time
 *  Parameters: device name
 *  Returns: door history time list(format: yyyy-MM-dd-H-mm-ss) - success, NULL - fail
*/
ECode CZigbeeService::GetDoorHistoryTime(
    /* [in] */ const String& deviceName,
    /* [out, callee] */ ArrayOf<String>** time)
{
    VALIDATE_NOT_NULL(time)

   *time = NULL;

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetDoorHistoryTime(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    if (mDevices[deviceName]->GetClass() == "ZigbeeDoorSensor") {
        AutoPtr<ZigbeeDoorSensor> doorSensor = (ZigbeeDoorSensor*)mDevices[deviceName].Get();
        AutoPtr<ArrayOf<String> > temp = StringListToStringArray(doorSensor->GetHistoryTime());
        *time = temp;
        REFCOUNT_ADD(*time);
        if (DBG) Slogger::D(TAG, "GetDoorHistoryTime(): device \"%s\": door history time: %p", deviceName.string(), *time);
    }
    else {
        Slogger::E(TAG, "GetDoorHistoryTime(): device \"%s\" is not ZigbeeDoorSensor", deviceName.string());
        return NOERROR;
    }

    return NOERROR;
}

/*
 *  Get door sensor's history state
 *  Parameters: device name
 *  Returns: door history state list - success, NULL - fail
*/
ECode CZigbeeService::GetDoorHistoryState(
    /* [in] */ const String& deviceName,
    /* [out, callee] */ ArrayOf<String>** state)
{
    VALIDATE_NOT_NULL(state)

    *state = NULL;

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetDoorHistoryState(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    if (mDevices[deviceName]->GetClass() == "ZigbeeDoorSensor") {
        AutoPtr<ZigbeeDoorSensor> doorSensor = (ZigbeeDoorSensor*)mDevices[deviceName].Get();
        AutoPtr<List<Int32> > intState = doorSensor->GetHistoryDoorState();
        *state = ArrayOf<String>::Alloc(intState->GetSize());
        (*state)->AddRef();
        List<Int32>::Iterator iter = intState->Begin();
        for (Int32 i = 0; iter != intState->End(); ++iter, ++i) {
            (**state)[i] = StringUtils::ToString(i);
        }
        if (DBG) Slogger::D(TAG, "GetDoorHistoryState(): device \"%s\": door history state: %p", deviceName.string(), *state);
    }
    else {
        Slogger::E(TAG, "GetDoorHistoryState(): device \"%s\" is not ZigbeeDoorSensor", deviceName.string());
        return NOERROR;
    }

    return NOERROR;
}

/*
 *  Set door sensor's all warning setting
 *  Parameters: device name, warning enable/disable setting, warning door state(IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetDoorSensorAllWarningSetting(
    /* [in] */ const String& deviceName,
    /* [in] */ Boolean warningSetting,
    /* [in] */ Int32 warningState,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetDoorSensorAllWarningSetting(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    //check warning state valid
    if (warningState != IZigbeeManager::STATE_ON && warningState != IZigbeeManager::STATE_OFF) {
        Slogger::E(TAG, "SetDoorSensorAllWarningSetting(): device \"%s\", invalid door warning state: %d",
            deviceName.string(), warningSetting);
        *res = 1;
        return NOERROR;
    }

    //check if class type is ZigbeeDoorSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeDoorSensor") {
        if (DBG) Slogger::D(TAG, "SetDoorSensorAllWarningSetting(): device \"%s\" is ZigbeeDoorSensor class", deviceName.string());
        AutoPtr<ZigbeeDoorSensor> doorSensor = (ZigbeeDoorSensor*)mDevices[deviceName].Get();
        doorSensor->SetWarningSetting(warningSetting);
        doorSensor->SetWarningState(warningState);
    }
    else {
        Slogger::E(TAG, "SetDoorSensorAllWarningSetting(): device \"%s\" is not ZigbeeDoorSensor class", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    *res = 0;
    return NOERROR;
}


/*
 *  Set door sensor's warning enable setting
 *  Parameters: device name, warning setting
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetDoorSensorWarningSetting(
    /* [in] */ const String& deviceName,
    /* [in] */ Boolean setting,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetDoorSensorWarningSetting(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    //check if class type is ZigbeeDoorSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeDoorSensor") {
        AutoPtr<ZigbeeDoorSensor> doorSensor = (ZigbeeDoorSensor*)mDevices[deviceName].Get();
        doorSensor->SetWarningSetting(setting);
        if (DBG) Slogger::D(TAG, "SetDoorSensorWarningSetting(): device \"%s\": SetWarningSetting(%d)", deviceName.string(), setting);
    }
    else {
        Slogger::E(TAG, "SetDoorSensorWarningSetting(): device \"%s\" is not ZigbeeDoorSensor", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Get door sensor's warning enable setting
 *  Parameters: device name
 *  Returns: TRUE-warning is enabled/FALSE-warning is disabled/error
*/
ECode CZigbeeService::GetDoorSensorWarningSetting(
    /* [in] */ const String& deviceName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetDoorSensorWarningSetting(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    //check if class type is ZigbeeDoorSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeDoorSensor") {
        AutoPtr<ZigbeeDoorSensor> doorSensor = (ZigbeeDoorSensor*)mDevices[deviceName].Get();
        *result = doorSensor->GetWarningSetting();
        if (DBG) Slogger::D(TAG, "GetDoorSensorWarningSetting(): device \"%s\": getWarningSetting(): %d", deviceName.string(), *result);
    }
    else {
        Slogger::E(TAG, "GetDoorSensorWarningSetting(): device \"%s\" is not ZigbeeDoorSensor", deviceName.string());
    }

    return NOERROR;
}

/*
 *  Set door sensor's warning state
 *  Parameters: device name, door warning state(IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetDoorWarningState(
    /* [in] */ const String& deviceName,
    /* [in] */ Int32 warningState,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetDoorWarningState(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    //check warning state valid
    if (warningState != IZigbeeManager::STATE_ON && warningState != IZigbeeManager::STATE_OFF) {
        Slogger::E(TAG, "SetDoorWarningState(): device \"%s\", invalid door warning state: %d", deviceName.string(), warningState);
        *res = 1;
        return NOERROR;
    }

    //check if class type is ZigbeeDoorSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeDoorSensor") {
        AutoPtr<ZigbeeDoorSensor> doorSensor = (ZigbeeDoorSensor*)mDevices[deviceName].Get();
        doorSensor->SetWarningState(warningState);
        if (DBG) Slogger::D(TAG, "SetDoorWarningState(): device \"%s\" : setWarningState(%d)", deviceName.string(), warningState);
    }
    else {
        Slogger::E(TAG, "SetDoorWarningState(): device \"%s\" is not ZigbeeDoorSensor", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Get door sensor's warning state
 *  Parameters: device name
 *  Returns: door warning state(IZigbeeManager::STATE_ON/OFF) -success/IZigbeeManager::STATE_UNKNOWN -fail
*/
ECode CZigbeeService::GetDoorSensorWarningState(
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IZigbeeManager::STATE_UNKNOWN;
    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetDoorSensorWarningState(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    //check if class type is ZigbeeDoorSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeDoorSensor") {
        AutoPtr<ZigbeeDoorSensor> doorSensor = (ZigbeeDoorSensor*)mDevices[deviceName].Get();
        *result = doorSensor->GetWarningState();
        if (DBG) Slogger::D(TAG, "GetDoorSensorWarningState(): device \"%s\": getWarningState(): %d", deviceName.string(), *result);
    }
    else {
        Slogger::E(TAG, "GetDoorSensorWarningState(): device \"%s\" is not ZigbeeDoorSensor", deviceName.string());
    }

    return NOERROR;
}


/*
 *  Set door sensor's history record setting
 *  Parameters: device name, TRUE-enable history record/FALSE-disable history record, history record filename(NULL if disable setting)
 *  Returns: 0-success, 1-fail
*/
ECode CZigbeeService::SetDoorSensorHistorySetting(
    /* [in] */ const String& deviceName,
    /* [in] */ Boolean enable,
    /* [in] */ const String& filename,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "SetDoorSensorHistorySetting(): device \"%s\" does not exist", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    //check if class type is ZigbeeTemperatureSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeDoorSensor") {
        if (DBG) Slogger::D(TAG, "SetDoorSensorHistorySetting(): device \"%s\" is ZigbeeDoorSensor class, history file name: %s",
                    deviceName.string(), filename.string());
        AutoPtr<ZigbeeDoorSensor> doorSensor = (ZigbeeDoorSensor*)mDevices[deviceName].Get();
        doorSensor->SetHistorySetting(enable);
        if (enable) {
            if (doorSensor->SetHistoryFileName(filename) == 1) {
                Slogger::E(TAG, "SetDoorSensorHistorySetting(): device \"%s\": illegal history record file name", deviceName.string());
                *res = 1;
                return NOERROR;
            }
        }
    }
    else {
        Slogger::E(TAG, "SetDoorSensorHistorySetting(): device \"%s\" is not ZigbeeDoorSensor class", deviceName.string());
        *res = 1;
        return NOERROR;
    }

    *res = 0;
    return NOERROR;
}

/*
 *  Get door sensor's history record setting
 *  Parameters: device name
 *  Returns: TRUE-history record is enabled/FALSE-history record is disabled/error
*/
ECode CZigbeeService::GetDoorHistorySetting(
    /* [in] */ const String& deviceName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetDoorHistorySetting(): device \"%s\" does not exist", deviceName.string());
    }

    //check if class type is ZigbeeTemperatureSensor
    if (mDevices[deviceName]->GetClass() == "ZigbeeDoorSensor") {
        if (DBG) Slogger::D(TAG, "GetDoorHistorySetting(): device \"%s\" is ZigbeeDoorSensor class", deviceName.string());
        AutoPtr<ZigbeeDoorSensor> doorSensor = (ZigbeeDoorSensor*)mDevices[deviceName].Get();
        *result = doorSensor->GetHistorySetting();
    }
    else {
        Slogger::E(TAG, "GetDoorHistorySetting(): device \"%s\" is not ZigbeeDoorSensor class", deviceName.string());
    }

    return NOERROR;
}


/*
 *  Get door sensor on/off state
 *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
 *  Parameters: device name
 *  Returns: IZigbeeManager::STATE_ON/OFF -success, IZigbeeManager::STATE_UNKNOWN -fail
*/
ECode CZigbeeService::GetDoorOnOffState(
    /* [in] */ const String& deviceName,
    /* [out] */ Int32* doorState)
{
    VALIDATE_NOT_NULL(doorState)
    *doorState = IZigbeeManager::STATE_UNKNOWN;
    if (!IsDeviceExist(deviceName)) {
        Slogger::E(TAG, "GetDoorSensorOnOffState(): device \"%s\" does not exist", deviceName.string());
        return NOERROR;
    }

    AutoPtr<ZigbeeDoorSensor> doorSensor;
    // Boolean isDoorWarning = FALSE;
    AutoPtr<IIntent> intent;
    AutoPtr<IBundle> bundle;

    //get current system time
    AutoPtr<IDate> date;
    CDate::New((IDate**)&date);
    AutoPtr<ISimpleDateFormat> sDateFormat;
    CSimpleDateFormat::New(String("yyyy-MM-dd-H-mm-ss"), (ISimpleDateFormat**)&sDateFormat);
    String time;
    sDateFormat->FormatDate(date, &time);

    if (mDevices[deviceName]->GetClass() == "ZigbeeDoorSensor") {
        doorSensor = (ZigbeeDoorSensor*)mDevices[deviceName].Get();
    }
    else {
        Slogger::E(TAG, "GetDoorOnOffState(): device \"%s\" is not a ZigbeeDoorSensor", deviceName.string());
        return NOERROR;
    }

    // get updated door state
    Int32 deviceShortAddr = doorSensor->GetShortAddr();
    *doorState = Native_GetDoorOnOffState(deviceShortAddr);
    if (*doorState != IZigbeeManager::STATE_ON && *doorState != IZigbeeManager::STATE_OFF) {
        Slogger::E(TAG, "GetDoorOnOffState(): device \"%s\" get invalid door on/off state: %d", deviceName.string(), *doorState);
        *doorState = IZigbeeManager::STATE_UNKNOWN;
        return NOERROR;
    }

    /*
    // check if door warning
    if (doorSensor->GetWarningSetting()) {
        isDoorWarning = doorSensor->IsDoorWarning(doorState);
        if (isDoorWarning) {
            doorSensor->SetLastAlertTime(time);
            doorSensor->SetLastAlertState(doorState);
            CIntent::New(IZigbeeManager::ACTION_SENSOR_DOOR_WARNING, (IIntent**)&intent);
            if (DBG) Slogger::D(TAG, "GetDoorOnOffState(): setup action: ACTION_SENSOR_DOOR_WARNING");

            //setup intent and data in bundle
            bundle = new Bundle();
            bundle->PutString(IZigbeeManager::EXTRA_NAME, deviceName);
            bundle->PutInt32(IZigbeeManager::EXTRA_STATE, doorState);
            bundle->PutString(IZigbeeManager::EXTRA_TIME, time);
            intent->PutExtras(bundle);

            if (mContext != NULL) {
                if (DBG) Slogger::D(TAG, "GetDoorOnOffState(): sendBroadcast()");
                mContext.sendBroadcast(intent);
                //mContext.sendBroadcastAsUser(intent, new UserHandle(UserHandle.USER_CURRENT));
            }
            else {
                Slogger::E(TAG, "GetDoorOnOffState(): CZigbeeService's context is NULL");
            }

        }
        if (DBG) Slogger::D(TAG, "GetDoorOnOffState(): Warning is enabled, and door warning is NOT triggered.");
    }
    */

    return NOERROR;
}

/******************************************* Debug API ************************************************/
/*
 *  Dump all device and group information
 *  Parameters: void
 *  Returns: void
*/
ECode CZigbeeService::DumpLog()
{
    DumpDeviceInfo();
    DumpGroupInfo();
    return NOERROR;
}

void CZigbeeService::JNI_Callback(
    /* [in] */ Int32 deviceShortAddr,
    /* [in] */ Int32 msgID)
{
    AutoPtr<CallbackThread> callbackThread = new CallbackThread(deviceShortAddr, msgID, this);
    callbackThread->Start();
}

// /************************************************************************************************************
// /* Native methods
// ************************************************************************/

static struct zigbee_coordinator_device_t* zigbee_device = NULL;
static AutoPtr<CZigbeeService> sZigbeeService;

// Callback to framework service, bypass the callback messages
static int Device_state_callback(NotifyData data)
{
    if(CZigbeeService::DBG) Slogger::D(CZigbeeService::TAG, "[ZigbeeJNI] Device_state_callback()");

    if(sZigbeeService == NULL) {
        Slogger::E(CZigbeeService::TAG, "[ZigbeeJNI] Device_state_callback(): obj is NULL ");
        return -1;
    }

    sZigbeeService->JNI_Callback(data.shortAddr, data.msgID);
    if(CZigbeeService::DBG) Slogger::D(CZigbeeService::TAG, "[ZigbeeJNI] Device_state_callback():"
        " env->CallVoidMethod(): deviceShortAddr = %04X, msgID = %04X", data.shortAddr, data.msgID);
    return 0;
}

// Register callback to HAL
static int RegistrationCallback()
{
    if (!zigbee_device) {
        Slogger::E(CZigbeeService::TAG, "[ZigbeeJNI] RegistrationCallback(): device is not open!");
        return -1;
    }

    //register to HAL
    zigbee_device->register_notify_callback((notify_callback)Device_state_callback);
    if(CZigbeeService::DBG) Slogger::D(CZigbeeService::TAG, "[ZigbeeJNI] RegistrationCallback(): register_notify_callback()");
    return 0;
}

static void DumpZigbeeTable()
{
    if (!zigbee_device) {
        Slogger::E(CZigbeeService::TAG, "[ZigbeeJNI] DumpZigbeeTable(): device is not open!");
    }
    Slogger::D(CZigbeeService::TAG, "[ZigbeeJNI] DumpZigbeeTable()");
    int* deviceShortAddrList;
    int deviceCount = zigbee_device->get_address_list(&deviceShortAddrList);

    for(int i = 0; i < deviceCount; i++) {
        Slogger::D(CZigbeeService::TAG, "[ZigbeeJNI] DumpZigbeeTable():==== device %d====\n", i);
        Slogger::D(CZigbeeService::TAG, "[ZigbeeJNI] DumpZigbeeTable():id/type=%d\n", zigbee_device->get_device_id(deviceShortAddrList[i]));
        Slogger::D(CZigbeeService::TAG, "[ZigbeeJNI] DumpZigbeeTable():zone type=%d\n", zigbee_device->get_device_zone_type(deviceShortAddrList[i]));
        Slogger::D(CZigbeeService::TAG, "[ZigbeeJNI] DumpZigbeeTable():online=%d\n", zigbee_device->get_online_status(deviceShortAddrList[i]));
        Slogger::D(CZigbeeService::TAG, "[ZigbeeJNI] DumpZigbeeTable():shortAddr=%X\n", deviceShortAddrList[i]);
        Slogger::D(CZigbeeService::TAG, "[ZigbeeJNI] DumpZigbeeTable():mac=%X\n", zigbee_device->get_mac_address(deviceShortAddrList[i]));
    }
}

static inline int ZigbeeService_open(const hw_module_t* module, struct zigbee_coordinator_device_t** device)
{
    return module->methods->open(module, ZIGBEE_HARDWARE_MODULE_ID, (struct hw_device_t**)device);

}

Boolean CZigbeeService::Init_JNI()
{
    zigbee_module_t* module;

    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] CZigbeeService::Init_JNI()");
    if (hw_get_module(ZIGBEE_HARDWARE_MODULE_ID, (const struct hw_module_t**)&module) != 0) {
        Slogger::E(TAG, "[ZigbeeJNI] failed to get zigbee stub module");
        return FALSE;
    }
    if (ZigbeeService_open(&(module->common), &zigbee_device) != 0) {
        Slogger::E(TAG, "[ZigbeeJNI] failed to open zigbee device");
        return FALSE;
    }

    //register callback to HAL
    RegistrationCallback();

    sZigbeeService = this;

    return TRUE;
}

void CZigbeeService::Native_ResetDevice(
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return;
    }

    zigbee_device->reset_and_remove_device(shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] reset device: %X", shortAddr);
}

void CZigbeeService::Native_ResetCoordinator()
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return;
    }

    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] reset coordinator");
    zigbee_device->reset_coordinator();
}

void CZigbeeService::Native_PermitJoinNet(
    /* [in] */ Int32 cmd)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return;
    }

    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] permit_join_net");
    zigbee_device->permit_join_net(cmd);
}

Int32 CZigbeeService::Native_OnlineNumber()
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->get_online_number();
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] get_online_number: %d", result);
    return result;
}

//including coordinator, ex one bulb: device count = 2(one is coordinator)
Int32 CZigbeeService::Native_GetDeviceNumber()
{
    if(!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->get_device_number();
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] get_device_number: %d", result);
    return result;
}

Int32 CZigbeeService::Native_SwitchSetState(
    /* [in] */ Int32 command,
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->switch_set_state(command, shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] switch_set_state(%X): state %d", shortAddr, command);

    return result;
}

Int32 CZigbeeService::Native_SwitchGetState(
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->switch_get_state(shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] switchGetState(%X), state: %d", shortAddr, result);

    return result;
}

/*** Bulb API ***/
Int32 CZigbeeService::Native_LightSetDim(
    /* [in] */ Int32 level,
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->light_set_dim(level, shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] light_set_dim(%X) %d", shortAddr, level);

    return result;
}

Int32 CZigbeeService::Native_LightGetDim(
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->light_get_dim(shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] light_get_dim(%X): dim: %d", shortAddr, result);

    return result;
}

Int32 CZigbeeService::Native_LightSetColor(
    /* [in] */ Int32 rgb,
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->light_set_color(rgb, shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] light_set_color(%X) RGB:%X", shortAddr, rgb);

    return result;
}

Int32 CZigbeeService::Native_LightGetColor(
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->light_get_color(shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] light_get_color(%X): color: %X", shortAddr, result);

    return result;
}

/***  Sensor API  ***/
Int32 CZigbeeService::Native_GetSensorTemperature(
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return 9999;
    }

    int result = zigbee_device->sensor_get_temperature(shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] getSensorTemperature(%X): temperature: %d", shortAddr, result);

    return result;
}

Int32 CZigbeeService::Native_GetSensorHumidity(
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->sensor_get_humidity(shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] getSensorHumidity(%X): humidity: %d", shortAddr, result);

    return result;
}

Int32 CZigbeeService::Native_GetSensorVoltage(
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->sensor_get_voltage(shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] getSensorVoltage(%X), voltage: %d", shortAddr, result);

    return result;
}

Int32 CZigbeeService::Native_GetDoorOnOffState(
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->sensor_get_zone_status(shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] getDoorOnOffState(%X), door on/off state: %d", shortAddr, result);

    return result;
}

Int32 CZigbeeService::Native_GetDeviceType(
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->get_device_id(shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] getDeviceType(%X), DeviceType: %d", shortAddr, result);

    return result;
}

Int32 CZigbeeService::Native_GetDeviceZoneType(
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int result = zigbee_device->get_device_zone_type(shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] getDeviceZoneType(%X), DeviceZoneType: %d", shortAddr, result);

    return result;
}

Int32 CZigbeeService::Native_GetDeviceOnlineStatus(
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    int status = zigbee_device->get_online_status(shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] getDeviceOnlineStatus(%X), OnlineStatus: %d", shortAddr, status);

    return status;
}

AutoPtr<ArrayOf<Int32> > CZigbeeService::Native_GetAllDeviceShortAddr()
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return NULL;
    }

    int* deviceShortAddrList;
    int size = zigbee_device->get_address_list(&deviceShortAddrList);

    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(deviceShortAddrList, size);
    return result;
}

Int64 CZigbeeService::Native_GetDeviceMacAddr(
    /* [in] */ Int32 shortAddr)
{
    if (!zigbee_device) {
        Slogger::E(TAG, "[ZigbeeJNI] device is not open!");
        return -1;
    }

    long long result = zigbee_device->get_mac_address(shortAddr);
    if(DBG) Slogger::D(TAG, "[ZigbeeJNI] getDeviceMacAddr(%X), DeviceMacAddr: %llx", shortAddr, result);

    return result;
}

}//Server
}//Droid
}//Elastos
