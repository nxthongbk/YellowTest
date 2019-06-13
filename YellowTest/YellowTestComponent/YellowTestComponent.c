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

    //Check main bus i2c
    res = yellow_test_MainBusI2C("50", "08", "71", "34");
    if (res == LE_OK)
    {
        LE_INFO("Check main bus I2C: PASSED\n");
    }
    else
    {
        LE_INFO("Check main bus I2C: FAILED\n");
        return;
    }

    //Check port 1 I2C address
    res = yellow_test_Port1HubI2C("68", "76");
    if (res == LE_OK)
    {
        LE_INFO("Check port 1 hub I2C: PASSED\n");
    }
    else
    {
        LE_INFO("Check port 1 hub I2C: FAILED\n");
        return;
    }

    //Check port 2 I2C address
    res = yellow_test_Port2HubI2C("44", "6B", "55");
    if (res == LE_OK)
    {
        LE_INFO("Check port 2 hub I2C: PASSED\n");
    }
    else
    {
        LE_INFO("Check port 2 hub I2C: FAILED\n");
        return;
    }

    //Check port 3 I2C address
    res = yellow_test_Port3HubI2C("3E", "51", "55");
    if (res == LE_OK)
    {
        LE_INFO("Check port 3 hub I2C: PASSED\n");
    }
    else
    {
        LE_INFO("Check port 3 hub I2C: FAILED\n");
        return;
    }

    //Read Compass
    res = yellow_test_AcceGyroRead(BMI160_REG_CHIP_ID, &data);
    if (res == LE_OK)
    {
        LE_INFO("chip id = %x", data);
        LE_INFO("Read accelerometer and gyroscope: PASSED\n");
    }
    else
    {
        LE_INFO("Read accelerometer and gyroscope: FAILED\n");
        return;
    }    
}
