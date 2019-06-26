/*
 * AUTO-GENERATED _componentMain.c for the YellowTestServiceComponent component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _YellowTestServiceComponent_le_sim_ServiceInstanceName;
const char** le_sim_ServiceInstanceNamePtr = &_YellowTestServiceComponent_le_sim_ServiceInstanceName;
void le_sim_ConnectService(void);
extern const char* _YellowTestServiceComponent_le_mrc_ServiceInstanceName;
const char** le_mrc_ServiceInstanceNamePtr = &_YellowTestServiceComponent_le_mrc_ServiceInstanceName;
void le_mrc_ConnectService(void);
extern const char* _YellowTestServiceComponent_yellow_test_ServiceInstanceName;
const char** yellow_test_ServiceInstanceNamePtr = &_YellowTestServiceComponent_yellow_test_ServiceInstanceName;
void yellow_test_AdvertiseService(void);
// Component log session variables.
le_log_SessionRef_t YellowTestServiceComponent_LogSession;
le_log_Level_t* YellowTestServiceComponent_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _YellowTestServiceComponent_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _YellowTestServiceComponent_Init(void)
{
    LE_DEBUG("Initializing YellowTestServiceComponent component library.");

    // Advertise server-side IPC interfaces.
    yellow_test_AdvertiseService();

    // Connect client-side IPC interfaces.
    le_sim_ConnectService();
    le_mrc_ConnectService();

    // Register the component with the Log Daemon.
    YellowTestServiceComponent_LogSession = log_RegComponent("YellowTestServiceComponent", &YellowTestServiceComponent_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_YellowTestServiceComponent_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
