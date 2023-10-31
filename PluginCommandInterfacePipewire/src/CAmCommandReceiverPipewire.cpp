
#include "CAmCommandReceiverPipewire.h"
#include "CAmDltWrapper.h"

#include <thread>

DLT_IMPORT_CONTEXT(commandPipewire)

namespace am
{

// The function we want to execute on the new thread.
void pwMainThread()
{
    log(&commandPipewire, DLT_LOG_INFO, "Inside pwMainThread");
}


CAmCommandReceiverPipewire::CAmCommandReceiverPipewire()
{
    pw_init(NULL, NULL);
    log(&commandPipewire, DLT_LOG_INFO, "CommandReceiverPipewire constructed");

    m_pipewireThread = std::thread(pwMainThread);
}

CAmCommandReceiverPipewire::~CAmCommandReceiverPipewire()
{
    m_pipewireThread.join();
}


} // namespace am
