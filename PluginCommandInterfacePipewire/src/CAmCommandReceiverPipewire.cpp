
#include "CAmCommandReceiverPipewire.h"
#include "CAmDltWrapper.h"

#include <thread>

#include <iostream>

DLT_IMPORT_CONTEXT(commandPipewire)

namespace am
{

void registry_event_global(void *data, uint32_t id,
                uint32_t permissions, const char *type, uint32_t version,
                const struct spa_dict *props)
{
    log(&commandPipewire, DLT_LOG_INFO, "object: id:", id, "type:", type, "/", version);
}

const struct pw_registry_events registry_events = {
        PW_VERSION_REGISTRY_EVENTS,
        .global = registry_event_global
};
 


// The function we want to execute on the new thread.
void pwMainThread(pw_main_loop ** pwMainLoop)
{
    log(&commandPipewire, DLT_LOG_INFO, "Inside pwMainThread");

    *pwMainLoop = pw_main_loop_new(NULL);

    pw_context *context = pw_context_new(pw_main_loop_get_loop(*pwMainLoop), nullptr, 0);

    pw_core * core = pw_context_connect(context, nullptr, 0);

    if (core == nullptr)
    {
        log(&commandPipewire, DLT_LOG_ERROR, "Error connecting pipewire context!");
        return;
    }

    spa_hook registry_listener;

    pw_registry * registry = pw_core_get_registry(core, PW_VERSION_REGISTRY, 0);

    spa_zero(registry_listener);

    pw_registry_add_listener(registry, &registry_listener, &registry_events, nullptr);

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
