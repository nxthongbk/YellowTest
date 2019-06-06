#include "legato.h"
#include "interfaces.h"


COMPONENT_INIT
{
    LE_INFO("Yellow Test App start");

    le_result_t res;
    uint32_t signal_qual = 0;

    //Check external SIM card state
    res = yellow_test_CheckSimState();
    if (LE_FAULT == res)
    {
            LE_ERROR("Check external SIM card state FAILED");
    }

    //Measure signal strength
    res = yellow_test_MeasureSignalStrength(&signal_qual);
    if (res == LE_OK)
    {
        LE_INFO("Signal strength quality = %d\n", signal_qual);
        LE_INFO("Check signal quality: PASSED\n");
    }
    else
    {
        LE_INFO("Check signal quality: FAILED\n");
    }



}
