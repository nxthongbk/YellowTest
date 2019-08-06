# YellowTest
1. Copy ```YellowTest``` and ```YellowTestService``` to ```apps``` directory
2. Copy ```iot_test_card``` to ```linux_kernel_modules``` directory
3. In ```linux_kernel_modules\mangoh\mangoh_yellow_dev.mdef``` update ```allow_eeprom_write = "1"``` to make eeprom writable
4. Build system file by: ```make yellow_wp76xx```
5. Copy update file to your test program directory then run.
