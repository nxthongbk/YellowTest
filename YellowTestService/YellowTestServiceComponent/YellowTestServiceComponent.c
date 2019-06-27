#include "legato.h"
#include "interfaces.h"
#include "i2c-utils.h"
#include "eeprom.h"

#define I2C_HUB_MAIN_BUS    0x00
#define I2C_HUB_PORT_3      0x08
#define I2C_HUB_PORT_IOT    0x01
#define I2C_HUB_PORT_2      0x04
#define I2C_HUB_PORT_1      0x02
#define I2C_HUB_PORT_ALL    0x0F
#define BMI160_I2C_ADDR     0x68

#define LEN(x)  (sizeof(x) / sizeof(x[0]))

char i2c_bus[256] = "/dev/i2c-0";



//--------------------------------------------------------------------------------------------------
/**
 * Configure I2C hub to enable port I2C .
 *
 */
//--------------------------------------------------------------------------------------------------
int i2c_hub_select_port(uint8_t hub_address, uint8_t port)
{
    int result = 0;
    int i2c_fd = open(i2c_bus, O_RDWR);
    if (i2c_fd < 0) {
        LE_ERROR("i2cSendByte: failed to open %s", i2c_bus);
    }
    if (ioctl(i2c_fd, I2C_SLAVE_FORCE, hub_address) < 0) {
        LE_ERROR("Could not set address to 0x%02x: %s\n",
             hub_address,
             strerror(errno));
        close(i2c_fd);
        return -1;
    }
    const int writeResult = i2c_smbus_write_byte(i2c_fd, port);
    if (writeResult < 0) {
        LE_ERROR("smbus write failed with error %d\n", writeResult);
        result = -1;
    } else {
        result = 0;
    }
    close(i2c_fd);
    return result;
}


//--------------------------------------------------------------------------------------------------
/**
 * runs the command
 */
//--------------------------------------------------------------------------------------------------
static int RunSystemCommand(
    char *commandStringPtr)
{
    int systemResult;

    if (NULL == commandStringPtr)
    {
        LE_ERROR("ERROR Parameter is NULL.");
        return -1;
    }
    if ('\0' == *commandStringPtr)
    {
        LE_INFO("INFO Nothing to execute.");
        return -1;
    }

    systemResult = system(commandStringPtr);
    // Return value of -1 means that the fork() has failed (see man system).
    if (0 == WEXITSTATUS(systemResult))
    {
        LE_INFO("Success: %s", commandStringPtr);
        return 0;
    }
    else
    {
        LE_ERROR("Error %s Failed: (%d)", commandStringPtr, systemResult);
        return -1;
    }
}

//--------------------------------------------------------------------------------------------------
/**
 * Check: SIM State.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_CheckSimState(void)
{
    le_sim_States_t state;
    le_result_t res = le_sim_SelectCard(LE_SIM_EXTERNAL_SLOT_1);
    LE_ASSERT(res == LE_OK);

    state = le_sim_GetState(LE_SIM_EXTERNAL_SLOT_1);
    if (LE_SIM_READY == state)
    {
        return LE_OK;
    }
    else
    {
    return LE_FAULT;
    }
}

//--------------------------------------------------------------------------------------------------
/**
 * Check: signal quality.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_MeasureSignalStrength
(
    uint32_t* qual
)
{
    le_result_t res;
    res = le_mrc_GetSignalQual(qual);
    return res;
}

//--------------------------------------------------------------------------------------------------
/**
 * Check: SD Card Read/Write.
 *
 */
//--------------------------------------------------------------------------------------------------

le_result_t yellow_test_SDCard( void )
{
    int res;

    res = RunSystemCommand("/sbin/rmmod msm_sdcc");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/sbin/modprobe msm_sdcc");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/bin/mkdir /tmp/sd");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/bin/mount -ofmask=0111 -odmask=0000 -osmackfsdef=sd /dev/mmcblk0p1 /tmp/sd/");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    //Create file
    res = RunSystemCommand("/bin/touch /tmp/sd/log.txt");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    //write file
    res = RunSystemCommand("/bin/echo foo >> /tmp/sd/log.txt");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    //read file
    res = RunSystemCommand("/bin/cat /tmp/sd/log.txt");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    return LE_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 * Check: Main Bus I2C Address.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_MainBusI2C( void )
{
    int res;

    res = i2c_hub_select_port(0x71, I2C_HUB_MAIN_BUS);
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 50 \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 71 \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }


    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 08 \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 34 \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * Check: Port 1 I2C Address.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_Port1HubI2C( void )
{
    int res;

    res = i2c_hub_select_port(0x71, I2C_HUB_PORT_1);
    if (res != 0 )
    {
        return LE_FAULT;
    }
 
    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 50 \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 71 \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }


    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 08 \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 34 \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    return LE_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 * Check: Port 2 I2C Address.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_Port2HubI2C( void )
{
    int res;

    res = i2c_hub_select_port(0x71, I2C_HUB_PORT_2);
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 68 \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 76 \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    return LE_OK;
}

// //--------------------------------------------------------------------------------------------------
// /**
//  * Check: Port 3 I2C Address.
//  *
//  */
// //--------------------------------------------------------------------------------------------------
le_result_t yellow_test_Port3HubI2C( void )
{
    int res;

    res = i2c_hub_select_port(0x71, I2C_HUB_PORT_3);
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 3e \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 51 \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    res = RunSystemCommand("/usr/sbin/i2cdetect -y -r 0 | grep \" 55 \"");
    if (res != 0 )
    {
        return LE_FAULT;
    }

    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * Check: Read compass heading.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_AcceGyroRead
(
    uint8_t reg,
    uint8_t* data
)
{
    int i2c_fd = open(i2c_bus, O_RDWR);
    if (i2c_fd < 0) {
        LE_ERROR("i2cSendByte: failed to open %s", i2c_bus);
    }
    if (ioctl(i2c_fd, I2C_SLAVE_FORCE, BMI160_I2C_ADDR) < 0) {
        LE_ERROR("Could not set address to 0x%02x: %s\n",
               BMI160_I2C_ADDR,
               strerror(errno));
        return LE_FAULT;
    }
    *data = i2c_smbus_read_byte_data(i2c_fd, reg);
    close(i2c_fd);
    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * Check: Read ADC on IOT Test Card.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_Adc3Read
(
    int32_t* value
)
{
    le_result_t result;
    i2c_hub_select_port(0x71, I2C_HUB_PORT_IOT);

    result = le_adc_ReadValue("EXT_ADC3", value);

    if (result == LE_FAULT)
    {
        LE_INFO("Couldn't get ADC value");
    }
    return result;
}

//--------------------------------------------------------------------------------------------------
/**
 * Check: Read SPI EEPROM on IOT Test Card.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_SPIEeprom
(
    void
)
{
    le_result_t result;
    int res;
    //int8_t* value;

    res = RunSystemCommand("/legato/systems/current/bin/app start spiService");
    if (res != 0 )
    {
        LE_INFO("spiService is started");
    }

    result = eeprom_init("spidev0.0", 1000000);
    if (result == LE_FAULT)
    {
        LE_ERROR("Couldn't Init eeprom");
        return LE_FAULT;
    }


    uint8_t writeData[] = "123456789";
    result = eeprom_write(0x1000, writeData, 1);
    if (result == LE_FAULT)
    {
        LE_ERROR("Couldn't write eeprom");
        return LE_FAULT;
    }


    // result = eeprom_read(0x0000, value, 1)
    // if (result == LE_FAULT)
    // {
    //     LE_ERROR("Couldn't read eeprom");
    //     return LE_FAULT;
    // }

    return result;
}



COMPONENT_INIT
{
    LE_INFO("YellowTest Service Start.");
}