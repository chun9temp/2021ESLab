/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "ble/BLE.h"

#ifndef __BLE_STUID_SERVICE_H__
#define __BLE_STUID_SERVICE_H__

class StuIDService {
public:
    const static uint16_t STUID_SERVICE_UUID        = 0xA004;
    const static uint16_t STUID_CHARACTERISTIC_UUID = 0xA005;

    StuIDService(BLE &_ble, char *studentID):
        ble(_ble), stuID(STUID_CHARACTERISTIC_UUID, studentID)
    {
        GattCharacteristic *charTable[] = {&stuID};
        GattService         stuIDService(STUID_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.gattServer().addService(stuIDService);
    }

    GattAttribute::Handle_t getValueHandle() const
    {
        return stuID.getValueHandle();
    }

private:
    BLE                                     &ble;
    ReadOnlyArrayGattCharacteristic<char,9> stuID;
};

#endif /* #ifndef __BLE_STUID_SERVICE_H__ */
