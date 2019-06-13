#include "legato.h"
#include "interfaces.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "i2c-utils.h"
#include "legato.h"
#include "interfaces.h"

#define I2C_HUB_PORT_3      0x08
#define I2C_HUB_PORT_IOT    0x01
#define I2C_HUB_PORT_2      0x04
#define I2C_HUB_PORT_1      0x02
#define I2C_HUB_PORT_ALL    0x0F

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
 * Check: Main Bus I2C Address.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_MainBusI2C
(
    const char* LE_NONNULL eeprAddr,
    const char* LE_NONNULL tca9546Addr,
    const char* LE_NONNULL usb3503Addr,
    const char* LE_NONNULL addr
)
{
    FILE *cmd;
    char command[256];
    int buf_size = 8;
    char bus_address[buf_size];
    for (int n = 0; n < 2; n++)
    {
        bus_address[n] = eeprAddr[n];
        bus_address[n+2] = tca9546Addr[n];
        bus_address[n+4] = usb3503Addr[n];
        bus_address[n+6] = addr[n];
    }
    for (int i = 0; i < buf_size; i += 2)
    {
        sprintf(command, "/usr/sbin/i2cdetect -y -r 0 | grep \" %c%c \"",
               bus_address[i], bus_address[i+1]);
        cmd = popen(command, "r");
        if (WEXITSTATUS(pclose(cmd)) == 1)
        {
            return LE_FAULT;
        }
    }
    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * Check: Port 1 I2C Address.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_Port1HubI2C
(
    const char* LE_NONNULL gyroAddr,
    const char* LE_NONNULL bme680Addr
)
{
    FILE *cmd;
    char command[256];
    int buf_size = 4;
    char bus_address[buf_size];
    int res;
    res = i2c_hub_select_port(0x71, I2C_HUB_PORT_1);
    if (res != 0 )
    {
        return LE_FAULT;
    }

    for (int n = 0; n < 2; n++)
    {
        bus_address[n] = gyroAddr[n];
        bus_address[n+2] = bme680Addr[n];
    }
    for (int i = 0; i < buf_size; i += 2)
    {
        sprintf(command, "/usr/sbin/i2cdetect -y -r 0 | grep \" %c%c \"",
               bus_address[i], bus_address[i+1]);
        cmd = popen(command, "r");
        if (WEXITSTATUS(pclose(cmd)) == 1)
        {
            return LE_FAULT;
        }
    }
    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * Check: Port 2 I2C Address.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_Port2HubI2C
(
    const char* LE_NONNULL opt3002,
    const char* LE_NONNULL bq25601Addr,
    const char* LE_NONNULL bq27426Addr
)
{
    FILE *cmd;
    char command[256];
    int buf_size = 6;
    char bus_address[buf_size];
    int res;
    res = i2c_hub_select_port(0x71, I2C_HUB_PORT_2);
    if (res != 0 )
    {
        return LE_FAULT;
    }
    for (int n = 0; n < 2; n++)
    {
        bus_address[n] = opt3002[n];
        bus_address[n+2] = bq25601Addr[n];
        bus_address[n+4] = bq27426Addr[n];
    }    
    for (int i = 0; i < buf_size; i += 2)
    {
        sprintf(command, "/usr/sbin/i2cdetect -y -r 0 | grep \" %c%c \"",
               bus_address[i], bus_address[i+1]);
        cmd = popen(command, "r");
        if (WEXITSTATUS(pclose(cmd)) == 1)
        {
            return LE_FAULT;
        }
    }
    return LE_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * Check: Port 3 I2C Address.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_Port3HubI2C
(
    const char* LE_NONNULL ioExpAddr,
    const char* LE_NONNULL pcf85063Addr,
    const char* LE_NONNULL nt3h2211Addr
)
{
    FILE *cmd;
    char command[256];
    int buf_size = 6;
    int8_t bus_address[buf_size];
    int res;
    res = i2c_hub_select_port(0x71, I2C_HUB_PORT_3);
    if (res != 0 )
    {
        return LE_FAULT;
    }
    for (int n = 0; n < 2; n++)
    {
        bus_address[n] = ioExpAddr[n];
        bus_address[n+2] = pcf85063Addr[n];
        bus_address[n+4] = nt3h2211Addr[n];
    } 
    for (int i = 0; i < buf_size; i += 2)
    {
        sprintf(command, "/usr/sbin/i2cdetect -y -r 0 | grep \" %c%c \"", 
               bus_address[i], bus_address[i+1]);
        cmd = popen(command, "r");
        if (WEXITSTATUS(pclose(cmd)) == 1)
        {
            return LE_FAULT;
        }
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


COMPONENT_INIT
{
    LE_INFO("YellowTest Service Start.");
}
