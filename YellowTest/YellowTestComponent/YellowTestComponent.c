#include "legato.h"
#include "interfaces.h"

#define BMI160_REG_CHIP_ID 0x00

int32_t adc3_value = 0;


COMPONENT_INIT
{
    LE_INFO("Yellow Automation Test App start");
    putenv("PATH=/legato/systems/current/bin:/usr/local/bin:"
             "/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin");

    le_result_t res;
    uint32_t signal_qual = 0;
    uint8_t data = 10;
   
    //Check external SIM card state
    res = yellow_test_CheckSimState();
    if (LE_FAULT == res)
    {
        LE_ERROR("Check external SIM card state FAILED");
        return;
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
        LE_ERROR("Check signal quality: FAILED\n");
        return;
    }

    res = yellow_test_SDCard();
    if(res == LE_OK)
    {
        LE_INFO("SDCard Read/Wrire test PASSED");
    }
    else
    {
        LE_ERROR("SDCard Read/Wrire test FAILED");
        return;
    }  


    //heck main bus i2c
    res = yellow_test_MainBusI2C();
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
    res = yellow_test_Port1HubI2C();
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
    res = yellow_test_Port2HubI2C();
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
    res = yellow_test_Port3HubI2C();
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


    //Test ADC IOT Test Card
    res = yellow_test_Adc3Read(&adc3_value);
    if(res == LE_OK)
    {
        LE_INFO("EXT_ADC3 value is: %d", adc3_value);
    }
    else
    {
        LE_ERROR("Check ADC3 value FAILED");
        return;
    }  
}
