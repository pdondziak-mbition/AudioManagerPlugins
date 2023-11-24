
#include "CAmCommandReceiverPipewire.h"
#include "CAmDltWrapper.h"

#include <thread>

#include <iostream>

#include <spa/utils/names.h>

#include <pipewire/pipewire.h>

#include <stdexcept>


DLT_IMPORT_CONTEXT(commandPipewire)

namespace am
{

/* [roundtrip] */
struct roundtrip_data {
    int pending;
};

void on_core_done(void *data, uint32_t id, int seq)
{
    struct roundtrip_data *d = reinterpret_cast<roundtrip_data *>(data);

    log(&commandPipewire, DLT_LOG_INFO, "Core done with processing");

    // if (id == PW_ID_CORE && seq == d->pending)
    // {
    //     pw_main_loop_quit(d->loop);
    // }
}
 
void setup_on_core_done(struct pw_core *core)
{
    static const struct pw_core_events core_events = {
            PW_VERSION_CORE_EVENTS,
            .done = on_core_done,
    };

    struct roundtrip_data d;
    struct spa_hook core_listener;

    pw_core_add_listener(core, &core_listener, &core_events, &d);

    d.pending = pw_core_sync(core, PW_ID_CORE, 0);
}

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

void pwMainThread(pw_main_loop ** pwMainLoop)
{
    log(&commandPipewire, DLT_LOG_INFO, "Inside pwMainThread");

    pw_properties * pw_config_properties;
    if ((pw_config_properties = pw_properties_new(nullptr, nullptr)) == nullptr)
    {
        throw std::runtime_error("can't crate pw properties");
    }

    log(&commandPipewire, DLT_LOG_WARN, "Loading pipewire configuration");
    if (pw_conf_load_conf("/home/dondzip/dev/dev-fedora", "am-pipewire.conf", pw_config_properties) < 0)
    {
        throw std::runtime_error("can't load pw configuration");
    }

    const char *str;
    if ((str = pw_properties_get(pw_config_properties, "properties")) != NULL)
    {
        pw_properties_update_string(pw_config_properties, str, strlen(str));
    }

    *pwMainLoop = pw_main_loop_new(NULL);

    pw_context *context = pw_context_new(pw_main_loop_get_loop(*pwMainLoop), pw_config_properties, 0);

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

    auto alsa_handle = pw_context_load_spa_handle(context, SPA_NAME_API_ALSA_ENUM_UDEV, NULL);

    if (alsa_handle == nullptr) {
		log(&commandPipewire, DLT_LOG_WARN, "can't load alsa-handle");
        throw std::runtime_error("can't load alsa-handle");
	}

    log(&commandPipewire, DLT_LOG_INFO, "entering loop");
    setup_on_core_done(core);
    pw_main_loop_run(*pwMainLoop);
    log(&commandPipewire, DLT_LOG_INFO, "exit loop");

    pw_proxy_destroy((struct pw_proxy*)registry);
    pw_core_disconnect(core);
    pw_context_destroy(context);
    pw_main_loop_destroy(*pwMainLoop);
    pw_unload_spa_handle(alsa_handle);
}


CAmCommandReceiverPipewire::CAmCommandReceiverPipewire() :
    m_pwMainLoop(nullptr)
{
    log(&commandPipewire, DLT_LOG_INFO, "CommandReceiverPipewire constructed");
    pw_init(nullptr, nullptr);

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
