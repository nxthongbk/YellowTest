#include "eeprom.h"

#define HEXDUMP( a, ... ) do { printf(__VA_ARGS__);} while(0);
#define identical 0

static void hexdump(const uint8_t *buf, unsigned len)
{
    unsigned i;
    for (i = 0; i < len; i++)
    {
        HEXDUMP( TS_LEVEL_DEBUG, "%02x ", buf[i]);

        if ((i+1)%30 == 0)
            HEXDUMP( TS_LEVEL_DEBUG, "\n");
    }
    HEXDUMP( TS_LEVEL_DEBUG, "\n");
};



le_spi_DeviceHandleRef_t __spiHandle;
bool is_spi_open = false;

// Internal buffer
#define EEPROM_PAGE_SIZE 64
#define EEPROM_BUFF_SIZE 72
uint8_t __write_buff[EEPROM_BUFF_SIZE];
uint8_t __read_buff[EEPROM_BUFF_SIZE];
size_t __read_buff_size;

// 25AA128 EEPROM commands
#define EEPROM_READ     0x03    // Read data from memory array beginning at selected address
#define EEPROM_WRITE    0x02    // Write data to memory array beginning at selected address
#define EEPROM_WRDI     0x04    // Reset the write enable latch (disable write operations)
#define EEPROM_WREN     0x06    // Set the write enable latch (enable write operations)
#define EEPROM_RDSR     0x05    // Read STATUS register
#define EEPROM_WRSR     0x01    // Write STATUS register


enum STATUS_REG
{
  WIP = 0x01<<0, // Writes internal process
  WEL = 0x01<<1, // Writes enable latch
  BP0 = 0x01<<2, // protect bit
  BP1 = 0x01<<3, // protect bit
  XX1 = 0x01<<4, // don't care bit
  XX2 = 0x01<<5, // don't care bit
  XX3 = 0x01<<6, // don't care bit
  WPEN = 0x01<<7 // writes protect enable
};

uint8_t eeprom_get_status(void)
{
    le_result_t res = LE_OK;
    uint8_t eeprom_status = 0;
    if (is_spi_open)
    {
        __write_buff[0] = EEPROM_RDSR;
        __write_buff[1] = 0xFF;
        __read_buff_size = 2;
        LE_DEBUG("EEPROM Status Read");
        res = le_spi_WriteReadFD(__spiHandle, __write_buff, __read_buff_size, __read_buff, &__read_buff_size);
        if ( res != LE_OK )
        {
            LE_ERROR("le_spi_WriteReadFD failed: Result = %s", LE_RESULT_TXT(res));
        }
        else
        {
            eeprom_status = __read_buff[1];
            LE_DEBUG("EEPROM Status Read DONE");
        }
        LE_DEBUG("EEPROM Status: %02X", eeprom_status);
    }
    else
    {
        LE_ERROR("SPI device did not open");
        // res = LE_FAULT;
    }
    return eeprom_status;
}

void eeprom_wait_wip(uint32_t timeout_ms)
{
    while (eeprom_get_status() & WIP && timeout_ms > 0)
    {
        usleep(1000);
    }
}

void eeprom_write_enable()
{
    le_result_t res = LE_OK;
    if (is_spi_open)
    {
        __write_buff[0] = EEPROM_WREN;
        __read_buff_size = 1;
        LE_DEBUG("EEPROM write enable");
        res = le_spi_WriteReadFD(__spiHandle, __write_buff, __read_buff_size, __read_buff, &__read_buff_size);
        if ( res != LE_OK )
        {
            LE_ERROR("le_spi_WriteReadFD failed: Result = %s", LE_RESULT_TXT(res));
        }
        else
        {
            LE_DEBUG("EEPROM write enable DONE");
        }
    }
    else
    {
        LE_ERROR("SPI device did not open");
        res = LE_FAULT;
    }
}
/** 
 * Initial SPI device for EEPROM communication
 * @spi_dev: SPI device name
 * @speed: SPI speed (bps)
 **/
le_result_t eeprom_init(const char * spi_dev, uint32_t speed)
{
    le_result_t res = LE_OK;
	LE_INFO("======= Configure SPI");    
    LE_INFO("Open SPI device %s", spi_dev);
    LE_INFO("Speed %d", speed);
    res = le_spi_Open(spi_dev, &__spiHandle);
    if ( res != LE_OK )
    {
        LE_ERROR("le_spi_Open failed: Result = %s", LE_RESULT_TXT(res) );
    }
    else
    {
        LE_INFO("le_spi_Open sucess");
        is_spi_open = true;
    }
    
    LE_INFO("# Config the SPI");
    le_spi_Configure(__spiHandle, 3, 8, speed, 0);
    // note no check as le_spi_Configure returns void
    
    eeprom_wait_wip(100);
    
    return res;
}

/** 
 * Release SPI device used in EEPROM communication
 **/
le_result_t eeprom_deinit()
{
    le_result_t res = LE_OK;
    if (is_spi_open)
    {
        LE_INFO("Close SPI device");
        le_spi_Close(__spiHandle);
    }
    else
    {
        LE_ERROR("SPI device did not open");
        res = LE_FAULT;
    }
    return res;
}

/** 
 * Write data to EEPROM
 * @address: EEPROM address for writing
 * @data: data for writing
 * @data_len: length of data for writing
 **/
le_result_t eeprom_write(uint16_t address, uint8_t * data, uint32_t data_len)
{
    le_result_t res = LE_OK;
    int eeprom_page_write = data_len / EEPROM_PAGE_SIZE;
    if (data_len % EEPROM_PAGE_SIZE > 0)
    {
        eeprom_page_write += 1;
    }
    LE_DEBUG("Page write: %d. Data len: %d", eeprom_page_write, data_len);
    if (is_spi_open)
    {
        int i = 0;
        int write_len = 0;
        for (i = 0; i < eeprom_page_write; i++)
        {
            eeprom_wait_wip(100);
            eeprom_write_enable();
            
            address += i * EEPROM_PAGE_SIZE;
            __write_buff[0] = EEPROM_WRITE;
            __write_buff[1] = (uint8_t)((address&0xFF00)>>8);
            __write_buff[2] = (uint8_t)((address&0x00FF));
            if (i == eeprom_page_write - 1)
            {
                write_len = data_len - i * EEPROM_PAGE_SIZE;
                memcpy(__write_buff + 3, data + i * EEPROM_PAGE_SIZE, write_len);
            }
            else
            {
                write_len = EEPROM_PAGE_SIZE;
                memcpy(__write_buff + 3, data + i * EEPROM_PAGE_SIZE, write_len);
            }
            __read_buff_size = write_len + 3;
            
            LE_DEBUG("Address: %d. Write len: %d", address, __read_buff_size);
            LE_INFO("Buffer write to Slave");
            hexdump(__write_buff, __read_buff_size);
            
            res = le_spi_WriteReadFD(__spiHandle, __write_buff, __read_buff_size, __read_buff, &__read_buff_size);
            if ( res != LE_OK )
            {
                LE_ERROR("le_spi_WriteReadFD failed: Result = %s", LE_RESULT_TXT(res));
            }
            else
            {
                LE_DEBUG("le_spi_WriteReadFD sucess");
            }
        }
    }
    else
    {
        LE_ERROR("SPI device did not open");
        res = LE_FAULT;
    }
    
    return res;
}


/** 
 * Read data from EEPROM
 * @address: EEPROM address for reading
 * @buffer: buffer to store read data
 * @desire_len: length of data for reading
 **/
le_result_t eeprom_read(uint16_t address, uint8_t * buffer, uint32_t desire_len)
{
    le_result_t res = LE_OK;
    
    if (is_spi_open)
    {
        eeprom_wait_wip(100);
        
        __write_buff[0] = EEPROM_READ;
        __write_buff[1] = (uint8_t)((address&0xFF00)>>8);
        __write_buff[2] = (uint8_t)((address&0x00FF));
        // memcpy(__write_buff + 3, data, data_len);
        __read_buff_size = desire_len + 3;
        
        res = le_spi_WriteReadFD(__spiHandle, __write_buff, __read_buff_size, __read_buff, &__read_buff_size);
        LE_INFO("Buffer write to  Slave");
        hexdump(__write_buff, __read_buff_size);
        LE_INFO("Buffer returned to Master");
        hexdump(__read_buff, __read_buff_size);
        if ( res != LE_OK )
        {
            LE_ERROR("le_spi_WriteReadFD failed: Result = %s", LE_RESULT_TXT(res));
        }
        else
        {
            LE_DEBUG("le_spi_WriteReadFD sucess");
            memcpy(buffer, __read_buff + 3, desire_len);
        }
    }
    else
    {
        LE_ERROR("SPI device did not open");
        res = LE_FAULT;
    }
    
    return res;
}
