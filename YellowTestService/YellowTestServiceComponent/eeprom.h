/** 
 * Author: tqkieu@tma.com.vn
 * Description: Create for 25AA128 EEPROM
 **/
#include "legato.h"
#include "interfaces.h"

/** 
 * Initial SPI device for EEPROM communication
 * @spi_dev: SPI device name
 * @speed: SPI speed (bps)
 **/
le_result_t eeprom_init(const char * spi_dev, uint32_t speed);

/** 
 * Release SPI device used in EEPROM communication
 **/
le_result_t eeprom_deinit();

/** 
 * Write data to EEPROM
 * @address: EEPROM address for writing
 * @data: data for writing
 * @data_len: length of data for writing
 **/
le_result_t eeprom_write(uint16_t address, uint8_t * data, uint32_t data_len);


/** 
 * Read data from EEPROM
 * @address: EEPROM address for reading
 * @buffer: buffer to store read data
 * @desire_len: length of data for reading
 **/
le_result_t eeprom_read(uint16_t address, uint8_t * buffer, uint32_t desire_len);
