#ifndef __ELASTOS_DROID_SERVER_CZIGBEESERVICE_H__
#define __ELASTOS_DROID_SERVER_CZIGBEESERVICE_H__

#include "_Elastos_Droid_Server_CZigbeeService.h"
#include "zigbee/ZigbeeTHSensor.h"
#include "zigbee/ZigbeeGroup.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::IClassLoader;
using Elastos::Core::IRunnable;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IParcelStringList;
using Elastos::Droid::Server::Zigbee::ZigbeeDevice;
using Elastos::Droid::Server::Zigbee::ZigbeeGroup;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CZigbeeService)
{
private:
    /***** History record manager *********/
    class HistoryRecordThread : public Thread
    {
    public:
        HistoryRecordThread(
            /* [in] */ const String& deviceName,
            /* [in] */ Int32 shortAddr,
            /* [in] */ IDate* date,
            /* [in] */ const String& record,
            /* [in] */ const String& filename,
            /* [in] */ CZigbeeService* host);

        CARAPI Run();

    private:
        String mDeviceName;
        Int32 mShortAddr;
        AutoPtr<IDate> mDate;
        String mRecord;
        String mFilename;
        CZigbeeService* mHost;
    };

    /*
     *  Bulb scenario mode thread
     *  RepeatCount: repeat light color change time
     *  DelayTime: delay time between each change
     *  RandomDelay: set TRUE to make random delay time (0~delayTime seconds)
    */
     class BulbScenarioModeThread : public Thread
    {
    public:
        BulbScenarioModeThread(
            /* [in] */ Int32 shortAddr,
            /* [in] */ Int32 mode,
            /* [in] */ CZigbeeService* host);

        CARAPI Destroy();

        CARAPI Run();

    private:
        //mode define
        static const Int32 SM_RANDOM_COLOR;
        static const Int32 SM_RGB_COLOR;

        //mode parameters
        static const Int32 levelStep;
        static const Int32 SMdelayTime;

    private:
        Boolean mDestory;
        Int32 mMode;
        String mDeviceName;
        Int32 mShortAddr;
        CZigbeeService* mHost;
    };

    class CallbackManager : public ElRefBase
    {
    public:
        CallbackManager(
            /* [in] */ CZigbeeService* host)
            : mHost(host)
        {}

        //[short-term solution for sync state between remote client and local client]
        CARAPI_(void) NotifyDeviceStateChanged();

        CARAPI_(void) NotifyDeviceCountChanged();

        CARAPI_(void) NotifyDeviceOnlineStatusChanged(
            /* [in] */ const String& deviceName);

        CARAPI_(void) NotifySensorHumidityReport(
            /* [in] */ const String& deviceName);

        CARAPI_(void) NotifySensorTemperatureReport(
            /* [in] */ const String& deviceName);;

        CARAPI_(void) NotifyDoorOnOffStateChanged(
            /* [in] */ const String& deviceName);

        CARAPI_(void) NotifySensorVoltageChanged(
            /* [in] */ const String& deviceName);

    private:
        CZigbeeService* mHost;
    };

    class CallbackThread : public Thread
    {
    public:
        CallbackThread(
            /* [in] */ Int32 deviceShortAddr,
            /* [in] */ Int32 msgID,
            /* [in] */ CZigbeeService* host);

        CARAPI Run();

    private:
        Int32 mDeviceShortAddr;
        Int32 mMsgID;
        CZigbeeService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CZigbeeService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /*
     *  Save device/group configuration
     *  Parameters:
     *  Returns: 0 - success, 1 - fail
    */
    CARAPI SaveDeviceConfig(
        /* [out] */ Int32* res);

    /*
     *  Read device/group configuration
     *  Parameters:
     *  Returns: 0 - success, 1 - fail
    */
    CARAPI ReadDeviceConfig(
        /* [out] */ Int32* res);

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
    CARAPI GetAllDeviceInfo(
        /* [out, callee] */ ArrayOf<IParcelStringList*>** infoList);

    /*
     *  [short-term solution] Remote client can only access "String"
     *  (For Remote Client) Get all device name
     *  Parameters: void
     *  Returns: device name - success, NULL- fail
    */
    CARAPI GetDeviceNameByID(
        /* [out] */ String* name);

    /*
     *  Get device name list
     *  Parameters: void
     *  Returns: device name list - success, NULL- fail
    */
    CARAPI GetDevices(
        /* [out, callee] */ ArrayOf<String>** list);


    /*
     *  Get device online status
     *  OnlineStatus defined in IZigbeeManager::java (Ex.IZigbeeManager::STATUS_ONLINE/IZigbeeManager::STATUS_OFFLINE)
     *  Parameters: device name
     *  Returns: online status- success, -1- fail
    */
    CARAPI GetDeviceOnlineStatus(
        /* [in] */ const String& devName,
        /* [out] */ Int32* res);

    /*
     *  Get device short address
     *  Parameters: device name
     *  Returns: short address- success, NULL- fail
    */
    CARAPI GetDeviceShortAddr(
        /* [in] */ const String& devName,
        /* [out] */ String* shortAddr);

    /*
     *  Get device MAC address
     *  Parameters: device name
     *  Returns: MAC address- success, NULL- fail
    */
    CARAPI GetDeviceMacAddr(
        /* [in] */ const String& devName,
        /* [out] */ String* macAddr);

    /*
     *  Get device type
     *  Type defined in IZigbeeManager::java (Ex.IZigbeeManager::ZIGBEE_DEV_TYPE_BULB)
     *  Parameters: device name
     *  Returns: type - success, -1 - fail
    */
    CARAPI GetDeviceType(
        /* [in] */ const String& devName,
        /* [out] */ Int32* type);

    /*
     *  Get device zone type
     *  Type defined in IZigbeeManager::java (Ex.IZigbeeManager::ZIGBEE_ZONE_TYPE_TEMPERATURE)
     *  Parameters: device name
     *  Returns: zone type - success, -1 - fail
    */
    CARAPI GetDeviceZoneType(
        /* [in] */ const String& devName,
        /* [out] */ Int32* zoneType);


    /*
     *  Change device name
     *  Parameters: original device name, new device name
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetDeviceName(
        /* [in] */ const String& targetDevName,
        /* [in] */ const String& newDevName,
        /* [out] */ Int32* res);

    /*
     *  get total device count
     *  Parameters: void
     *  Returns: device count-success, -1-fail
    */
    CARAPI GetDeviceCount(
        /* [out] */ Int32* count);


    /************************************* Group Management API ****************************************/
    /*
     *  Create a group
     *  Parameters: group name, group type, group zone type
     *  Returns: 0-success, 1-fail
    */
    CARAPI CreateGroup(
        /* [in] */ const String& groupName,
        /* [in] */ Int32 groupType,
        /* [in] */ Int32 groupZoneType,
        /* [out] */ Int32* res);

    /*
     *  Remove a group
     *  Parameters: group name
     *  Returns: 0-success, 1-fail
    */
    CARAPI RemoveGroup(
        /* [in] */ const String& groupName,
        /* [out] */ Int32* res);

    /*
     *  [short-term solution] Remote client can only access "String"
     *  (For Remote Client) Get group name
     *  Parameters: void
     *  Returns: group name - success, NULL- fail
    */
    CARAPI GetGroupsByID(
        /* [out] */ String* groupName);


    /*
     *  [short-term solution] Remote client can only access "String"
     *  (For Remote Client) Get device name in a group
     *  Parameters: group name
     *  Returns: device name in a group - success, NULL- fail
    */
    CARAPI GetGroupDevicesByID(
        /* [in] */ const String& groupName,
        /* [out] */ String* deviceName);

    /*
     *  Get all group names
     *  Parameters: void
     *  Returns: group name list - success, NULL- fail
    */
    CARAPI GetGroups(
        /* [out, callee] */ ArrayOf<String>** list);

    /*
     *  Add multiple devices to group
     *  Parameters: group name, device name list
     *  Return value: 0-success, 1-fail
    */
    CARAPI AddDevicesToGroup(
        /* [in] */ const String& groupName,
        /* [in] */ const ArrayOf<String>& deviceNameList,
        /* [out] */ Int32* res);

    /*
     *  Add a device to a group
     *  Parameters: group name, device name
     *  Returns: 0-success, 1-fail
    */
    CARAPI AddDeviceToGroup(
        /* [in] */ const String& groupName,
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* res);

    /*
     *  Remove all devices from a group
     *  Parameters: group name
     *  Returns: 0-success, 1-fail
    */
    CARAPI RemoveAllDevicesFromGroup(
        /* [in] */ const String& groupName,
        /* [out] */ Int32* res);

    /*
     *  Remove a device from a group
     *  Parameters: group name, device name
     *  Returns: 0-success, 1-fail
    */
    CARAPI RemoveDeviceFromGroup(
        /* [in] */ const String& groupName,
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* res);

    /*
     *  Change group name
     *  Parameters: original group name, new group name
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetGroupName(
        /* [in] */ const String& groupName,
        /* [in] */ const String& newGroupName,
        /* [out] */ Int32* res);

    /*
     *  Get group type
     *  Type defined in IZigbeeManager::java (Ex.IZigbeeManager::ZIGBEE_DEV_TYPE_BULB)
     *  Parameters: group name
     *  Returns: group type - success, (-1)-fail
    */
    CARAPI GetGroupType(
        /* [in] */ const String& groupName,
        /* [out] */ Int32* type);

    /*
     *  Get group zone type
     *  Type defined in IZigbeeManager::java (Ex.IZigbeeManager::ZIGBEE_ZONE_TYPE_TEMPERATURE)
     *  Parameters: group name
     *  Returns: group zone type - success, (-1)-fail
    */
    CARAPI GetGroupZoneType(
        /* [in] */ const String& groupName,
        /* [out] */ Int32* zoneType);

    /*
     *  Get device count of a group
     *  Parameters: group name
     *  Returns: device count-success, -1- fail
    */
    CARAPI GetDeviceCountOfGroup(
        /* [in] */ const String& groupName,
        /* [out] */ Int32* deviceNumber);

    /*
     *  Get group count
     *  Parameters: void
     *  Returns: group count
    */
    CARAPI GetGroupCount(
        /* [out] */ Int32* count);

    /*
     *  Get all devices of a group
     *  Parameters: group name
     *  Returns: List<String> all devices - success, NULL -fail
    */
    CARAPI GetGroupDevices(
        /* [in] */ const String& groupName,
        /* [out, callee] */ ArrayOf<String>** list);

    /**************************************** General Contorl API  *******************************************/
    /*
     *  Update all devices' information in the Zigbee network
     *  Parameters: void
     *  Returns: 0-success, 1-fail
    */
    CARAPI ScanZigbeeDevices(
        /* [out] */ Int32* res);

    /*
     *  Reset a device, will remove the device from current Zigbee network
     *  Parameters: device name
     *  Returns: void
    */
    CARAPI ResetDevice(
        /* [in] */ const String& deviceName);

    CARAPI ResetCoordinator();

    CARAPI PermitJoinNet(
        /* [in] */ Int32 cmd);

    /*
     *  Get online device count
     *  Parameters: void
     *  Returns: online device count
    */
    CARAPI GetOnlineDeviceCount(
        /* [out] */ Int32* count);

    // ********************************************* Switch Control API *************************************************/
    /*
     *  Turn on/off a switch device
     *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
     *  Parameters: on/off state, device name
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetSwitchOnOffState(
        /* [in] */ Int32 command,
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* res);

    /*
     *  Turn on/off all devices in a switch group
     *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
     *  Parameters: on/off state, group device list
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetSwitchGroupOnOffState(
        /* [in] */ Int32 command,
        /* [in] */ const String& groupName,
        /* [out] */ Int32* res);

    /*
     *  Get on/off state of a switch device
     *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
     *  Parameters: device name
     *  Returns: on/off state-success, -1-fail
    */
    CARAPI GetSwitchOnOffState(
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* state);



    // ********************************************* Bulb Control API *************************************************/
    /*
     *  Turn on/off a bulb device
     *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
     *  Parameters: on/off state, device name
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetBulbOnOffState(
        /* [in] */ Int32 command,
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* res);

    /*
     *  Turn on/off all devices in a group
     *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
     *  Parameters: on/off state, group device list
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetBulbGroupOnOffState(
        /* [in] */ Int32 command,
        /* [in] */ const String& groupName,
        /* [out] */ Int32* res);

    /*
     *  Get on/off state of a bulb device
     *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
     *  Parameters: device name
     *  Returns: on/off state-success, -1-fail
    */
    CARAPI GetBulbOnOffState(
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* state);

    /*
     *  Start scenario mode of a bulb type group
     *  Parameters:   Par1: group name, Par2: SM mode (1: random color, 2: RGB cycle color)
     *                           Par3(delayTime) and Par4(randomDelay) now is not used
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetBulbGroupScenarioMode(
        /* [in] */ const String& groupName,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 delayTime,
        /* [in] */ Boolean randomDelay,
        /* [out] */ Int32* res);


    /*
     *  Start scenario mode of a bulb type device
     *  Parameters:   Par1: group name, Par2: SM mode (1: random color, 2: RGB cycle color)
     *                           Par3(delayTime) and Par4(randomDelay) now is not used
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetBulbScenarioMode(
        /* [in] */ const String& devName,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 delayTime,
        /* [in] */ Boolean randomDelay,
        /* [out] */ Int32* res);

    /*
     *  Set luminance of a bulb type group
     *  Parameters: level, group name
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetBulbGroupLuminance(
        /* [in] */ Int32 level,
        /* [in] */ const String& groupName,
        /* [out] */ Int32* res);

    /*
     *  Set brightness of a bulb type device
     *  Parameters: level, device name
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetBulbLuminance(
        /* [in] */ Int32 level,
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* res);

    /*
     *  Get brightness of a bulb type device
     *  Parameters: device name
     *  Returns: brightness of the device-success, 1-fail
    */
    CARAPI GetBulbLuminance(
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* brightness);

    /*
     *  Set color of a bulb type group
     *  Parameters: R, G, B, group name
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetBulbGroupColor(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [in] */ const String& groupName,
        /* [out] */ Int32* res);

    /*
     *  Set color of a bulb type device
     *  Parameters: R, G, B, device name
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetBulbColor(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* res);

    /*
     *  Get color of a bulb type device
     *  Parameters: device name
     *  Returns: 0-success, 1-fail
    */
    CARAPI GetBulbColor(
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* res);


    // ***************************************** Humidity Sensor API ******************************************/
    /*
     *  Get sensor humidity
     *  Parameters: device name
     *  Returns: humidity (0~100) - success, -1 - fail
    */
    CARAPI GetSensorHumidity(
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* humidity);

    /*
     *  Set humidity sensor's history record setting
     *  Parameters: device name, TRUE-enable history record/FALSE-disable history record, history record filename(NULL if disable setting)
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetHumiditySensorHistorySetting(
        /* [in] */ const String& deviceName,
        /* [in] */ Boolean enable,
        /* [in] */ const String& filename,
        /* [out] */ Int32* res);

    /*
     *  Set humidity sensor's all warning setting
     *  Parameters: device name, lower limit setting, lower limit humidity, upper limit setting, upper limit humidity
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetHumiditySensorAllWarningSetting(
        /* [in] */ const String& deviceName,
        /* [in] */ Boolean lowerLimitSetting,
        /* [in] */ Int32 lowerLimitHumidity,
        /* [in] */ Boolean upperLimitSetting,
        /* [in] */ Int32 upperLimitHumidity,
        /* [out] */ Int32* res);

    /*
     *  Set humidity sensor's warning enable setting
     *  Parameters: device name, IZigbeeManager::WARNING_HUMIDITY_UPPER_LIMIT/IZigbeeManager::WARNING_HUMIDITY_LOWER_LIMIT, TRUE: enable/FALSE: disable
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetHumiditySensorWarningSetting(
        /* [in] */ const String& deviceName,
        /* [in] */ Int32 limitSelection,
        /* [in] */ Boolean setting,
        /* [out] */ Int32* res);


    // ***************************************** Temperature Sensor API ******************************************/
    /*
     *  Get temperature sensor's history record start time
     *  Parameters: device name
     *  Returns: start time(format: yyyy-MM-dd-H-mm-ss) - success, NA - fail
    */
    CARAPI GetTemperatureHistoryStartTime(
        /* [in] */ const String& deviceName,
        /* [out] */ String* startTime);

    /*
     *  Get temperature sensor's history record end time
     *  Parameters: device name
     *  Returns: end time(format: yyyy-MM-dd-H-mm-ss) - success, NA - fail
    */
    CARAPI GetTemperatureHistoryEndTime(
        /* [in] */ const String& deviceName,
        /* [out] */ String* endTime);

    /*
     *  Get temperature sensor's history record average temperature
     *  Parameters: device name
     *  Returns: average temperature - success, -9999 - fail
    */
    CARAPI GetHistoryAverageTemperature(
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* avgTemp);

    /*
     *  Get temperature sensor's max temperature in history record
     *  Parameters: device name
     *  Returns: max temperature - success, -9999 - fail
    */
    CARAPI GetHistoryMaxTemperature(
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* temp);

    /*
     *  Get temperature sensor's min temperature in history record
     *  Parameters: device name
     *  Returns: min temperature - success, 9999 - fail
    */
    CARAPI GetHistoryMinTemperature(
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* temperature);

    /*
     *  Get temperature sensor's min temperature time in history record
     *  Parameters: device name
     *  Returns: min temperature time(format: yyyy-MM-dd-H-mm-ss) - success, NA - fail
    */
    CARAPI GetHistoryMinTemperatureTime(
        /* [in] */ const String& deviceName,
        /* [out] */ String* time);

    /*
     *  Get temperature sensor's max temperature time in history record
     *  Parameters: device name
     *  Returns: max temperature time(format: yyyy-MM-dd-H-mm-ss) - success, NA - fail
    */
    CARAPI GetHistoryMaxTemperatureTime(
        /* [in] */ const String& deviceName,
        /* [out] */ String* time);


    /*
     *  Set temperature sensor's all warning setting
     *  Parameters: device name, lower limit setting, lower limit temperature, upper limit setting, upper limit temperature
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetTemperatureSensorAllWarningSetting(
        /* [in] */ const String& deviceName,
        /* [in] */ Boolean lowerLimitSetting,
        /* [in] */ Int32 lowerLimitTemp,
        /* [in] */ Boolean upperLimitSetting,
        /* [in] */ Int32 upperLimitTemp,
        /* [out] */ Int32* res);

    /*
     *  Set temperature sensor's warning enable setting
     *  Parameters: device name, IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT/IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetTemperatureSensorWarningSetting(
        /* [in] */ const String& deviceName,
        /* [in] */ Int32 limitSelection,
        /* [in] */ Boolean setting,
        /* [out] */ Int32* res);

    /*
     *  Get temperature sensor's warning enable setting
     *  Parameters: device name, IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT/IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT
     *  Returns: TRUE-warning is enabled/FALSE-warning is disabled/error
    */
    CARAPI GetTemperatureSensorWarningSetting(
        /* [in] */ const String& deviceName,
        /* [in] */ Int32 limitSelection,
        /* [out] */ Boolean* result);

    /*
     *  Set temperature sensor's warning temperature
     *  Parameters: device name, IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT/IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT, temperature
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetTemperatureSensorWarningTemperature(
        /* [in] */ const String& deviceName,
        /* [in] */ Int32 limitSelection,
        /* [in] */ Int32 temperature,
        /* [out] */ Int32* res);

    /*
     *  Get temperature sensor's warning temperature
     *  Parameters: device name, IZigbeeManager::WARNING_TEMPERATURE_UPPER_LIMIT/IZigbeeManager::WARNING_TEMPERATURE_LOWER_LIMIT
     *  Returns: temperature -success/9999-fail
    */
    CARAPI GetTemperatureSensorWarningTemperature(
        /* [in] */ const String& deviceName,
        /* [in] */ Int32 limitSelection,
        /* [out] */ Int32* result);


    /*
     *  Set temperature sensor's history record setting
     *  Parameters: device name, TRUE-enable history record/FALSE-disable history record, history record filename(NULL if disable setting)
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetTemperatureSensorHistorySetting(
        /* [in] */ const String& deviceName,
        /* [in] */ Boolean enable,
        /* [in] */ const String& filename,
        /* [out] */ Int32* res);

    /*
     *  Get temperature sensor's history record setting
     *  Parameters: device name
     *  Returns: TRUE-history record is enabled/FALSE-history record is disabled/error
    */
    CARAPI GetTemperatureSensorHistorySetting(
        /* [in] */ const String& deviceName,
        /* [out] */ Boolean* result);


    /*
     *  Get sensor temperature
     *  Parameters: device name
     *  Returns: temperture-success, 9999-fail
    */
    CARAPI GetSensorTemperature(
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* temperature);

    // ***************************************** General Sensor API ******************************************/
    /*
     *  Get sensor voltage
     *  Parameters: device name
     *  Returns: voltage-success, -1-fail
    */
    CARAPI GetSensorVoltage(
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* voltage);


    // ***************************************** Door Sensor API ******************************************/
    /*
     *  Get door sensor's history time
     *  Parameters: device name
     *  Returns: door history time list(format: yyyy-MM-dd-H-mm-ss) - success, NULL - fail
    */
    CARAPI GetDoorHistoryTime(
        /* [in] */ const String& deviceName,
        /* [out, callee] */ ArrayOf<String>** time);

    /*
     *  Get door sensor's history state
     *  Parameters: device name
     *  Returns: door history state list - success, NULL - fail
    */
    CARAPI GetDoorHistoryState(
        /* [in] */ const String& deviceName,
        /* [out, callee] */ ArrayOf<String>** state);

    /*
     *  Set door sensor's all warning setting
     *  Parameters: device name, warning enable/disable setting, warning door state(IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetDoorSensorAllWarningSetting(
        /* [in] */ const String& deviceName,
        /* [in] */ Boolean warningSetting,
        /* [in] */ Int32 warningState,
        /* [out] */ Int32* res);


    /*
     *  Set door sensor's warning enable setting
     *  Parameters: device name, warning setting
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetDoorSensorWarningSetting(
        /* [in] */ const String& deviceName,
        /* [in] */ Boolean setting,
        /* [out] */ Int32* res);

    /*
     *  Get door sensor's warning enable setting
     *  Parameters: device name
     *  Returns: TRUE-warning is enabled/FALSE-warning is disabled/error
    */
    CARAPI GetDoorSensorWarningSetting(
        /* [in] */ const String& deviceName,
        /* [out] */ Boolean* result);

    /*
     *  Set door sensor's warning state
     *  Parameters: device name, door warning state(IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetDoorWarningState(
        /* [in] */ const String& deviceName,
        /* [in] */ Int32 warningState,
        /* [out] */ Int32* res);

    /*
     *  Get door sensor's warning state
     *  Parameters: device name
     *  Returns: door warning state(IZigbeeManager::STATE_ON/OFF) -success/IZigbeeManager::STATE_UNKNOWN -fail
    */
    CARAPI GetDoorSensorWarningState(
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* result);


    /*
     *  Set door sensor's history record setting
     *  Parameters: device name, TRUE-enable history record/FALSE-disable history record, history record filename(NULL if disable setting)
     *  Returns: 0-success, 1-fail
    */
    CARAPI SetDoorSensorHistorySetting(
        /* [in] */ const String& deviceName,
        /* [in] */ Boolean enable,
        /* [in] */ const String& filename,
        /* [out] */ Int32* res);

    /*
     *  Get door sensor's history record setting
     *  Parameters: device name
     *  Returns: TRUE-history record is enabled/FALSE-history record is disabled/error
    */
    CARAPI GetDoorHistorySetting(
        /* [in] */ const String& deviceName,
        /* [out] */ Boolean* result);


    /*
     *  Get door sensor on/off state
     *  Switch state defined in IZigbeeManager::java (Ex.IZigbeeManager::STATE_ON/IZigbeeManager::STATE_OFF)
     *  Parameters: device name
     *  Returns: IZigbeeManager::STATE_ON/OFF -success, IZigbeeManager::STATE_UNKNOWN -fail
    */
    CARAPI GetDoorOnOffState(
        /* [in] */ const String& deviceName,
        /* [out] */ Int32* doorState);

    /******************************************* Debug API ************************************************/
    /*
     *  Dump all device and group information
     *  Parameters: void
     *  Returns: void
    */
    CARAPI DumpLog();

    CARAPI_(void) JNI_Callback(
        /* [in] */ Int32 deviceShortAddr,
        /* [in] */ Int32 msgID);

private:
    //check device exist or not
    // return value: TRUE - device exist, FALSE - device not exist
    CARAPI_(Boolean) IsDeviceExist(
        /* [in] */ const String& deviceName);

    //check multiple devices exist or not
    // return value: TRUE - all devices exist, FALSE - at least one device not exist
    CARAPI_(Boolean) IsDevicesExist(
        /* [in] */ const ArrayOf<String>& deviceNameList);

    //check group exist or not
    // return value: TRUE - group exist, FALSE - group not exist
    CARAPI_(Boolean) IsGroupExist(
        /* [in] */ const String& groupName);

    /*
       * Remove a device and the device in all groups(if device belongs to some groups)
       * Parameters: device name
       * Returns: void
    */
    CARAPI_(void) RemoveDevice(
        /* [in] */ const String& deviceName);

    /*
       * Rename a device and rename the  device name in all groups(if device belongs to some groups)
       * Parameters: device name, new device name
       * Returns: void
    */
    CARAPI_(void) RenameDevice(
        /* [in] */ const String& deviceName,
        /* [in] */ const String& newDeviceName);

    /*
       * Check if device type and group type is the same
       * Argument: (device name, group name)
       * Return value: TRUE - device type equals to group type, FALSE - device type does not equal to group type
    */
    CARAPI_(Boolean) CompareType(
        /* [in] */ const String& groupName,
        /* [in] */ const String& deviceName);

    /*
     *  Check if devices' type and group type are the same
     *  Argument: (device name list, group name)
     *  Returns: TRUE - devices' type all equal to group type, FALSE - one or more devices' type does not equal to group type
    */
    CARAPI_(Boolean) CompareTypes(
        /* [in] */ const String& groupName,
        /* [in] */ const ArrayOf<String>& deviceNameList);

    /*
     *  Stop if bulb scenario mode thread is running
     *  Parameters: device name
     *  Returns: void
    */
    CARAPI_(void) StopBulbScenarioModeThread(
        /* [in] */ const String& deviceName);

    CARAPI_(void) DumpDeviceInfo();

    CARAPI_(void) DumpGroupInfo();

    CARAPI_(Int32) InitZigbeeDevices();

    /* update Zigbee device from HAL */
    CARAPI_(Int32) UpdateZigbeeDevices();

    //return value: success - short address of the device
    //return value: fail - NULL
    CARAPI_(Int32) GetShortAddrByName(
        /* [in] */ const String& name);

    //Get device name by short address
    //Return value: device name- success, NULL - no match short address
    CARAPI_(String) GetNameByShortAddr(
        /* [in] */ Int32 shortAddr);

    /**********************************************************************
     * Native methods
     **********************************************************************/
    CARAPI_(Boolean) Init_JNI();

    CARAPI_(void) Native_ResetDevice(
        /* [in] */ Int32 shortAddr);

    CARAPI_(void) Native_ResetCoordinator();

    CARAPI_(void) Native_PermitJoinNet(
        /* [in] */ Int32 cmd);

    CARAPI_(Int32) Native_OnlineNumber();

    CARAPI_(Int32) Native_GetDeviceNumber();    //including coordinator, ex one bulb: device count = 2(one is coordinator)

    CARAPI_(Int32) Native_SwitchSetState(
        /* [in] */ Int32 command,
        /* [in] */ Int32 shortAddr);

    CARAPI_(Int32) Native_SwitchGetState(
        /* [in] */ Int32 shortAddr);

    /*** Bulb API ***/
    CARAPI_(Int32) Native_LightSetDim(
        /* [in] */ Int32 level,
        /* [in] */ Int32 shortAddr);

    CARAPI_(Int32) Native_LightGetDim(
        /* [in] */ Int32 shortAddr);

    CARAPI_(Int32) Native_LightSetColor(
        /* [in] */ Int32 rgb,
        /* [in] */ Int32 shortAddr);

    CARAPI_(Int32) Native_LightGetColor(
        /* [in] */ Int32 shortAddr);

    /***  Sensor API  ***/
    CARAPI_(Int32) Native_GetSensorTemperature(
        /* [in] */ Int32 shortAddr);

    CARAPI_(Int32) Native_GetSensorHumidity(
        /* [in] */ Int32 shortAddr);

    CARAPI_(Int32) Native_GetSensorVoltage(
        /* [in] */ Int32 shortAddr);

    CARAPI_(Int32) Native_GetDoorOnOffState(
        /* [in] */ Int32 shortAddr);

    CARAPI_(Int32) Native_GetDeviceType(
        /* [in] */ Int32 shortAddr);

    CARAPI_(Int32) Native_GetDeviceZoneType(
        /* [in] */ Int32 shortAddr);

    CARAPI_(Int32) Native_GetDeviceOnlineStatus(
        /* [in] */ Int32 shortAddr);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) Native_GetAllDeviceShortAddr();

    CARAPI_(Int64) Native_GetDeviceMacAddr(
        /* [in] */ Int32 shortAddr);

public:
    static const String TAG;
    //debug
    static const Boolean DBG;

private:
    AutoPtr<IContext> mContext;
    Boolean mIsZigbeeServiceReady;
    //Int32 mDeviceNum = 0; //device number in the Zigbee network
    HashMap<String, AutoPtr<ZigbeeDevice> > mDevices;   //device list
    HashMap<String, AutoPtr<ZigbeeGroup> > mGroups; //group list
    AutoPtr<CallbackManager> mCallbackMgr;   // manager for callback to application
    HashMap<String, AutoPtr<BulbScenarioModeThread> > mBulbSMThreads;   // bulb scenario mode thread list
    const String mDefaultDeviceName;
    const String NOT_AVAILABLE;
    const String mDataSpliter;
    const String mInfoSpliter;
    const String mSavePath;
    const String mHistoryRecord_SataSavePath;
    const String mHistoryRecord_InteralSavePath;
    const String mFilenameExtension;
    const String mConfigFileName;
    const String mConfigFileExtName;
    const String mConfigGroupSpliter ;
    //const String[] mIllegalFilenameChar = {"\\", "/", ":", "*", "?", "\"", "<", ">", "|"};    //To be deleted

    /* short-term solution used*/
    AutoPtr<List<String> > mDeviceNameList;   // [short-term solution] Remote client can only access "String"
    AutoPtr<List<String> > mGroupNameList;   // [short-term solution] Remote client can only access "String"
};

}//Server
}//Droid
}//Elastos

#endif //__ELASTOS_DROID_SERVER_CZIGBEESERVICE_H__
