
#include "CAmCommandReceiverPipewire.h"
#include "CAmDltWrapper.h"

#include <thread>

#include <iostream>

#include <spa/utils/names.h>
#include <spa/monitor/device.h>

#include <pipewire/pipewire.h>

#include <stdexcept>


DLT_IMPORT_CONTEXT(commandPipewire)

namespace am
{

/* [roundtrip] */
struct roundtrip_data {
    int pending;
};

struct alsa_data {
	// struct sm_media_session *session;
	// struct spa_hook session_listener;

	// struct pw_properties *conf;
	struct pw_properties *props;

    struct pw_context *context;
    struct pw_core *core;

	struct spa_handle *handle;

	struct spa_device *monitor;
	struct spa_hook listener;

	struct spa_list device_list;

    struct spa_device *device;

	// struct spa_source *jack_timeout;
	// struct pw_proxy *jack_device;
	// unsigned int reserve:1;
};

void on_core_done(void *data, uint32_t id, int seq)
{
    struct roundtrip_data *d = reinterpret_cast<roundtrip_data *>(data);

    log(&commandPipewire, DLT_LOG_INFO, "Core done with processing");
}

void registry_event_global(void *data, uint32_t id,
    uint32_t permissions, const char *type, uint32_t version,
    const struct spa_dict *props)
{
    log(&commandPipewire, DLT_LOG_INFO, "object: id:", id, "type:", type, "/", version);
}

static void create_alsa_device(alsa_data * data)
{
	struct sm_device *device;
	struct pw_proxy *handle;

	log(&commandPipewire, DLT_LOG_INFO, "Creating new alsa device");

	handle = pw_core_export(data->core, SPA_TYPE_INTERFACE_Device,
			&data->props->dict, data->device, 0);

	// device = (struct sm_device *) create_object(impl, NULL, handle, props, true);

	// monitor_sync(impl);
}

static void alsa_udev_object_info(void *data, uint32_t id, const struct spa_device_object_info *info)
{
    log(&commandPipewire, DLT_LOG_INFO, "Udev object info");

    int res;
    void *iface;
    struct spa_handle *handle;

    alsa_data * alsa_d = reinterpret_cast<alsa_data *>(data);

    handle = pw_context_load_spa_handle(alsa_d->context, SPA_NAME_API_ALSA_ACP_DEVICE, info->props);
	if (handle == nullptr) 
    {
		log(&commandPipewire, DLT_LOG_INFO, "can't make factory instance: ", -errno);
	}

    if ((res = spa_handle_get_interface(handle, SPA_TYPE_INTERFACE_Device, &iface)) < 0) {
		log(&commandPipewire, DLT_LOG_INFO, "can't get interface: ", res);
	}

    alsa_d->device = reinterpret_cast<spa_device *>(iface);
    alsa_d->props = pw_properties_new_dict(info->props);

    create_alsa_device(alsa_d);
}

static const struct spa_device_events alsa_udev_events =
{
	SPA_VERSION_DEVICE_EVENTS,
	.object_info = alsa_udev_object_info,
};

static const struct pw_core_events core_events = {
        PW_VERSION_CORE_EVENTS,
        .done = on_core_done,
};

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
    if (pw_conf_load_conf(
        "/home/dondzip/dev/dev-fedora/AudioManagerPlugins/PluginCommandInterfacePipewire/conf",
        "am-pipewire.conf", 
        pw_config_properties) < 0)
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

    alsa_data alsa_d;
    alsa_d.context = context;
    alsa_d.core = core;

    alsa_d.handle = pw_context_load_spa_handle(context, SPA_NAME_API_ALSA_ENUM_UDEV, NULL);
    if (alsa_d.handle == nullptr) {
		log(&commandPipewire, DLT_LOG_WARN, "can't load alsa-handle");
        throw std::runtime_error("can't load alsa-handle");
	}

    int res;
    void *iface;
    if ((res = spa_handle_get_interface(alsa_d.handle, SPA_TYPE_INTERFACE_Device, &iface)) < 0) 
    {
		log(&commandPipewire, DLT_LOG_WARN, "can't get udev Device interface: ", res);
		throw std::runtime_error("can't get udev Device interface");
	}

    // setup alsa monitoring
    alsa_d.monitor = reinterpret_cast<spa_device *>(iface);
    spa_list_init(&alsa_d.device_list);
	spa_device_add_listener(alsa_d.monitor, &alsa_d.listener, &alsa_udev_events, &alsa_d);

    // setup roundtrip
    struct roundtrip_data d;
    spa_hook core_listener;
    pw_core_add_listener(core, &core_listener, &core_events, &d);
    d.pending = pw_core_sync(core, PW_ID_CORE, 0);

    log(&commandPipewire, DLT_LOG_INFO, "entering loop");
    pw_main_loop_run(*pwMainLoop);
    log(&commandPipewire, DLT_LOG_INFO, "exit loop");

    pw_proxy_destroy((struct pw_proxy*)registry);
    pw_core_disconnect(core);
    pw_context_destroy(context);
    pw_main_loop_destroy(*pwMainLoop);
    pw_unload_spa_handle(alsa_d.handle);
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
