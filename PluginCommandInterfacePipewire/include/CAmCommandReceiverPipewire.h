#ifndef PIPEWIRECOMMANDRECEIVER_H_
#define PIPEWIRECOMMANDRECEIVER_H_

#include <thread>
#include <pipewire/pipewire.h>

namespace am
{

class CAmCommandReceiverPipewire
{
public:
    CAmCommandReceiverPipewire();

    ~CAmCommandReceiverPipewire();

private:
    std::thread m_pipewireThread;
};

} // namespace am

#endif // PIPEWIRECOMMANDRECEIVER_H_
