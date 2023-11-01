#ifndef PIPEWIRECOMMANDSENDER_H_
#define PIPEWIRECOMMANDSENDER_H_

#include "CAmCommandReceiverPipewire.h"

#include <map>
#include "IAmCommand.h"


namespace am
{

const char MY_NODE[] = "commandinterface";

/**
 * Dbus Implementation of CommandSendInterface
 */
class CAmCommandSenderPipewire: public IAmCommandSend
{
public:
	CAmCommandSenderPipewire();
    virtual ~CAmCommandSenderPipewire();
    am_Error_e startupInterface(IAmCommandReceive* commandreceiveinterface);
    void setCommandReady(const uint16_t handle);
    void setCommandRundown(const uint16_t handle);
    void cbNewMainConnection(const am_MainConnectionType_s& mainConnection);
    void cbRemovedMainConnection(const am_mainConnectionID_t mainConnection);
    void cbNewSink(const am_SinkType_s& sink);
    void cbRemovedSink(const am_sinkID_t sinkID);
    void cbNewSource(const am_SourceType_s& source);
    void cbRemovedSource(const am_sourceID_t source);
    void cbNumberOfSinkClassesChanged();
    void cbNumberOfSourceClassesChanged();
    void cbMainConnectionStateChanged(const am_mainConnectionID_t connectionID, const am_ConnectionState_e connectionState);
    void cbMainSinkSoundPropertyChanged(const am_sinkID_t sinkID, const am_MainSoundProperty_s& soundProperty);
    void cbMainSourceSoundPropertyChanged(const am_sourceID_t sourceID, const am_MainSoundProperty_s& soundProperty);
    void cbSinkAvailabilityChanged(const am_sinkID_t sinkID, const am_Availability_s& availability);
    void cbSourceAvailabilityChanged(const am_sourceID_t sourceID, const am_Availability_s& availability);
    void cbVolumeChanged(const am_sinkID_t sinkID, const am_mainVolume_t volume);
    void cbSinkMuteStateChanged(const am_sinkID_t sinkID, const am_MuteState_e muteState);
    void cbSystemPropertyChanged(const am_SystemProperty_s& systemProperty);
    void cbTimingInformationChanged(const am_mainConnectionID_t mainConnectionID, const am_timeSync_t time);
    void getInterfaceVersion(std::string& version) const;
    void cbSinkUpdated(const am_sinkID_t sinkID, const am_sinkClass_t sinkClassID, const std::vector<am_MainSoundProperty_s>& listMainSoundProperties);
    void cbSourceUpdated(const am_sourceID_t sourceID, const am_sourceClass_t sourceClassID, const std::vector<am_MainSoundProperty_s>& listMainSoundProperties);
    void cbSinkNotification(const am_sinkID_t sinkID, const am_NotificationPayload_s& notification) ;
    void cbSourceNotification(const am_sourceID_t sourceID, const am_NotificationPayload_s& notification) ;
    void cbMainSinkNotificationConfigurationChanged(const am_sinkID_t sinkID, const am_NotificationConfiguration_s& mainNotificationConfiguration) ;
    void cbMainSourceNotificationConfigurationChanged(const am_sourceID_t sourceID, const am_NotificationConfiguration_s& mainNotificationConfiguration) ;

private:
    CAmCommandReceiverPipewire m_commandReceiverPipewire;
    IAmCommandReceive* m_commandReceive; ///< ! pointer to commandReceive Interface
    bool mReady; ///< ! if false, calls shall be ignored.

    /**
     * list of sinks, needed to send out only deltas
     */
    std::vector<am_SinkType_s> mlistSinks;

    /**
     * list of sources, needed to send out only deltas
     */
    std::vector<am_SourceType_s> mlistSources;

    /**
     * used as comparison function for am_SinkType_s
     */
    struct sortBySinkID
    {
        bool operator()(const am_SinkType_s & a, const am_SinkType_s & b)
        {
            return (a.sinkID < b.sinkID);
        }
    };

    /**
     * used as comparison function for am_SourceType_s
     */
    struct sortBySourceID
    {
        bool operator()(const am_SourceType_s & a, const am_SourceType_s & b)
        {
            return (a.sourceID < b.sourceID);
        }
    };

};

}

#endif /* PIPEWIRECOMMANDSENDER_H_ */
