#ifndef PIPEWIRECOMMANDRECEIVER_H_
#define PIPEWIRECOMMANDRECEIVER_H_

#include <pipewire/pipewire.h>
#include <thread>

namespace am
{

class CAmCommandReceiverPipewire
{
public:
    CAmCommandReceiverPipewire();
    
    void closeConnection();

private:
    pw_main_loop * m_pwMainLoop;
    std::thread m_pipewireThread;
};

} // namespace am

#endif // PIPEWIRECOMMANDRECEIVER_H_
