/**
 *  Copyright (c) 2012 BMW
 *
 *  \author Christian Linke, christian.linke@bmw.de BMW 2011,2012
 *
 *  \copyright
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction,
 *  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
 *  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 *  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  For further information see http://www.genivi.org/.
 */

#include "CAmCommandSenderPipewire.h"
#include <algorithm>
#include <string>
#include <vector>
#include <cassert>
#include <set>
#include "CAmDbusMessageHandler.h"
#include "CAmDltWrapper.h"


using namespace am;
DLT_DECLARE_CONTEXT(commandPipewire)


/**
 * factory for plugin loading
 */
extern "C" IAmCommandSend* PluginCommandInterfacePipewireFactory()
{
    CAmDltWrapper::instance()->registerContext(commandPipewire, "PWP", "Pipewire Plugin");
    return (new CAmCommandSenderPipewire());
}

/**
 * destroy instance of commandSendInterface
 */
extern "C" void destroyRoutingPluginInterfaceDbus(IAmCommandSend* commandSendInterface)
{
    delete commandSendInterface;
}

CAmCommandSenderPipewire::CAmCommandSenderPipewire() :
        mCAmDbusMessageHandler(), //
        mIAmCommandReceiverShadow(), //
        mpCAmDbusWrapper(NULL), //
        mpIAmCommandReceive(NULL), //
        mReady(false)
{
    log(&commandPipewire, DLT_LOG_INFO, "PipewireCommandSender constructor called");
}

CAmCommandSenderPipewire::~CAmCommandSenderPipewire()
{
    log(&commandPipewire, DLT_LOG_INFO, "PipewireCommandSender destructed");
    CAmDltWrapper::instance()->unregisterContext(commandPipewire);
}

am_Error_e CAmCommandSenderPipewire::startupInterface(IAmCommandReceive* commandreceiveinterface)
{
    log(&commandPipewire, DLT_LOG_INFO, "startupInterface called");

    mpIAmCommandReceive = commandreceiveinterface;
    return (E_OK);
}

void CAmCommandSenderPipewire::setCommandReady(const uint16_t handle)
{
    //todo:implement handle handling
    log(&commandPipewire, DLT_LOG_INFO, "cbCommunicationReady called");
    mReady = true;
    mpIAmCommandReceive->confirmCommandReady(handle,E_OK);
}

void CAmCommandSenderPipewire::setCommandRundown(const uint16_t handle)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbCommunicationRundown called");
    mReady = false;
    mpIAmCommandReceive->confirmCommandRundown(handle,E_OK);
}

void CAmCommandSenderPipewire::cbNewMainConnection(const am_MainConnectionType_s& mainConnection)
{
    (void)mainConnection;
    log(&commandPipewire, DLT_LOG_INFO, "cbNewMainConnection called");
}

void CAmCommandSenderPipewire::cbRemovedMainConnection(const am_mainConnectionID_t mainConnectionId)
{
    //todo: change xml and interface to differetiate between new connection and removed one
    log(&commandPipewire, DLT_LOG_INFO, "cbRemovedMainConnection called");
}

void CAmCommandSenderPipewire::cbNewSink(const am_SinkType_s& sink)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbNewSink called");
}

void CAmCommandSenderPipewire::cbRemovedSink(const am_sinkID_t sinkID)
{
    //todo: check if this really works!
    log(&commandPipewire, DLT_LOG_INFO, "cbRemovedSink called");
}

void CAmCommandSenderPipewire::cbNewSource(const am_SourceType_s& source)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbNewSource called");
}

void am::CAmCommandSenderPipewire::cbRemovedSource(const am_sourceID_t source)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbRemovedSource called");
}

void CAmCommandSenderPipewire::cbNumberOfSinkClassesChanged()
{
    log(&commandPipewire, DLT_LOG_INFO, "cbNumberOfSinkClassesChanged called");
}

void CAmCommandSenderPipewire::cbNumberOfSourceClassesChanged()
{
    log(&commandPipewire, DLT_LOG_INFO, "cbNumberOfSourceClassesChanged called");
}

void CAmCommandSenderPipewire::cbMainConnectionStateChanged(const am_mainConnectionID_t connectionID, const am_ConnectionState_e connectionState)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbMainConnectionStateChanged called, connectionID=", connectionID, "connectionState=", connectionState);
}

void CAmCommandSenderPipewire::cbMainSinkSoundPropertyChanged(const am_sinkID_t sinkID, const am_MainSoundProperty_s & soundProperty)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbMainSinkSoundPropertyChanged called, sinkID", sinkID, "SoundProperty.type", soundProperty.type, "SoundProperty.value", soundProperty.value);
}

void CAmCommandSenderPipewire::cbMainSourceSoundPropertyChanged(const am_sourceID_t sourceID, const am_MainSoundProperty_s & SoundProperty)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbMainSourceSoundPropertyChanged called, sourceID", sourceID, "SoundProperty.type", SoundProperty.type, "SoundProperty.value", SoundProperty.value);
}

void CAmCommandSenderPipewire::cbSinkAvailabilityChanged(const am_sinkID_t sinkID, const am_Availability_s & availability)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbSinkAvailabilityChanged called, sinkID", sinkID, "availability.availability", availability.availability, "SoundProperty.reason", availability.availabilityReason);
}

void CAmCommandSenderPipewire::cbSourceAvailabilityChanged(const am_sourceID_t sourceID, const am_Availability_s & availability)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbSourceAvailabilityChanged called, sourceID", sourceID, "availability.availability", availability.availability, "SoundProperty.reason", availability.availabilityReason);
}

void CAmCommandSenderPipewire::cbVolumeChanged(const am_sinkID_t sinkID, const am_mainVolume_t volume)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbVolumeChanged called, sinkID", sinkID, "volume", volume);
}

void CAmCommandSenderPipewire::cbSinkMuteStateChanged(const am_sinkID_t sinkID, const am_MuteState_e muteState)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbSinkMuteStateChanged called, sinkID", sinkID, "muteState", muteState);
}

void CAmCommandSenderPipewire::cbSystemPropertyChanged(const am_SystemProperty_s & SystemProperty)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbSystemPropertyChanged called, SystemProperty.type", SystemProperty.type, "SystemProperty.value", SystemProperty.value);
}

void am::CAmCommandSenderPipewire::cbTimingInformationChanged(const am_mainConnectionID_t mainConnectionID, const am_timeSync_t time)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbTimingInformationChanged called, mainConnectionID=", mainConnectionID, "time=", time);
}

void CAmCommandSenderPipewire::getInterfaceVersion(std::string & version) const
{
    version = CommandVersion;
}

void am::CAmCommandSenderPipewire::cbSinkUpdated(const am_sinkID_t sinkID, const am_sinkClass_t sinkClassID, const std::vector<am_MainSoundProperty_s>& listMainSoundProperties)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbSinkUpdated called, sinkID", sinkID);
}

void am::CAmCommandSenderPipewire::cbSourceUpdated(const am_sourceID_t sourceID, const am_sourceClass_t sourceClassID, const std::vector<am_MainSoundProperty_s>& listMainSoundProperties)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbSourceUpdated called, sourceID", sourceID);
}

void am::CAmCommandSenderPipewire::cbSinkNotification(const am_sinkID_t sinkID, const am_NotificationPayload_s& notification)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbSinkNotification called, sinkID", sinkID);
}

void am::CAmCommandSenderPipewire::cbSourceNotification(const am_sourceID_t sourceID, const am_NotificationPayload_s& notification)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbSourceNotification called, sourceID", sourceID);
}

void am::CAmCommandSenderPipewire::cbMainSinkNotificationConfigurationChanged(const am_sinkID_t sinkID, const am_NotificationConfiguration_s& mainNotificationConfiguration)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbSinkMainNotificationConfigurationChanged called, sinkID", sinkID);
}

void am::CAmCommandSenderPipewire::cbMainSourceNotificationConfigurationChanged(const am_sourceID_t sourceID, const am_NotificationConfiguration_s& mainNotificationConfiguration)
{
    log(&commandPipewire, DLT_LOG_INFO, "cbSourceMainNotificationConfigurationChanged called, sourceID", sourceID);
}
