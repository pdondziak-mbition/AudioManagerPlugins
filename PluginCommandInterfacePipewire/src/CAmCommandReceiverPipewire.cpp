
#include "CAmCommandReceiverPipewire.h"
#include "CAmDltWrapper.h"

#include <thread>

#include <iostream>

DLT_IMPORT_CONTEXT(commandPipewire)

namespace am
{

// The function we want to execute on the new thread.
void pwMainThread(pw_main_loop ** pwMainLoop)
{
    log(&commandPipewire, DLT_LOG_INFO, "Inside pwMainThread");

    *pwMainLoop = pw_main_loop_new(NULL);

    log(&commandPipewire, DLT_LOG_INFO, "entering loop");
    pw_main_loop_run(*pwMainLoop);
    log(&commandPipewire, DLT_LOG_INFO, "exit loop");
}


CAmCommandReceiverPipewire::CAmCommandReceiverPipewire() :
    m_pwMainLoop(nullptr)
{
    log(&commandPipewire, DLT_LOG_INFO, "CommandReceiverPipewire constructed");
    pw_init(NULL, NULL);

    m_pipewireThread = std::thread(pwMainThread, &m_pwMainLoop);
}

void CAmCommandReceiverPipewire::closeConnection()
{
    log(&commandPipewire, DLT_LOG_INFO, "Inside CAmCommandReceiverPipewire::closeConnection");

    pw_main_loop_quit(m_pwMainLoop);
    m_pipewireThread.join();

    pw_deinit();
}


} // namespace am
