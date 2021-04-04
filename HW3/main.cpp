/* WiFi Example
 * Copyright (c) 2018 ARM Limited
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
#include "TCPSocket.h"
#include "TCPServer.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"
#include <cmath>
#include <cstdint>

#define WIFI_IDW0XX1    2

#if (defined(TARGET_DISCO_L475VG_IOT01A) || defined(TARGET_DISCO_F413ZH))
#include "ISM43362Interface.h"
ISM43362Interface wifi(false);

#else // External WiFi modules

#if MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1
#include "SpwfSAInterface.h"
SpwfSAInterface wifi(MBED_CONF_APP_WIFI_TX, MBED_CONF_APP_WIFI_RX);
#endif // MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1

#endif
DigitalOut led(LED1);
const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

int scan_demo(WiFiInterface *wifi)
{
    WiFiAccessPoint *ap;

    printf("WiFi scan:\n");

    int count = wifi->scan(NULL,0);
    printf("%d networks available.\n", count);

    /* Limit number of network arbitrary to 15 */
    count = count < 15 ? count : 15;

    ap = new WiFiAccessPoint[count];
    count = wifi->scan(ap, count);
    for (int i = 0; i < count; i++)
    {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }

    delete[] ap;
    return count;
}

void http_demo(NetworkInterface *net)
{
    TCPSocket socket;
    nsapi_error_t response;

    // Show the network address
    SocketAddress a;
    net->get_ip_address(&a);
    printf("IP address: %s \n", a.get_ip_address() ? a.get_ip_address() : "None");
    printf("Sending HTTP request to info.cern.ch...\n");
    // Open a socket on the network interface, and create a TCP connection to www.arm.com
    socket.open(net);
    net->gethostbyname("info.cern.ch", &a);
    a.set_port(80);
    response=socket.connect(a);

    if(0 != response) {
        printf("Error connecting: %d\n", response);
        socket.close();
        return;
    }

    // Send a simple http request
    char sbuffer[] = "GET / HTTP/1.1\r\nHost: info.cern.ch\r\n\r\n";
    nsapi_size_t size = strlen(sbuffer);
    response = 0;
    while(size)
    {
        response = socket.send(sbuffer+response, size);
        if (response < 0) {
            printf("Error sending data: %d\n", response);
            socket.close();
            return;
        } else {
            size -= response;
            // Check if entire message was sent or not
            printf("sent %d [%.*s]\n", response, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);
        }
    }

    // Recieve a simple http response and print out the response line
    char rbuffer[64];
    response = socket.recv(rbuffer, sizeof rbuffer);
    if (response < 0) {
        printf("Error receiving data: %d\n", response);
    } else {
        printf("recv %d [%.*s]\n", response, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);
    }

    // Close the socket to return its memory and bring down the network interface
    socket.close();
}

void data_sending(NetworkInterface *net)//collect data and send via socket
{
    TCPSocket socket;
    SocketAddress addr("10.0.0.20",8000);
    nsapi_error_t response;
    float t=0,h=0,p=0;
    int16_t pAccDataXYZ[3]={0};
    float pGyroDataXYZ[3]={0};
    int16_t pMagDataXYZ[3]={0};
    char sens_data[128];
    //int sample_num = 0; //counter
   
    // Open a socket on the network interface, and create a TCP connection to addr
    response = socket.open(net);
    if (0 != response){
        printf("Error opening: %d\n", response);
    }
    response = socket.connect(addr);
    if (0 != response){
        printf("Error connecting: %d\n", response);
    }

    //socket.set_blocking(1);
    while(1){
        led=1;
        //++sample_num;
        BSP_ACCELERO_AccGetXYZ(pAccDataXYZ);
        BSP_GYRO_GetXYZ(pGyroDataXYZ);
        BSP_MAGNETO_GetXYZ(pMagDataXYZ);
        int16_t ax=pAccDataXYZ[0],ay=pAccDataXYZ[1],az=pAccDataXYZ[2];
        float gx=pGyroDataXYZ[0],gy=pGyroDataXYZ[1],gz=pGyroDataXYZ[2];
        int16_t mx=pMagDataXYZ[0],my=pMagDataXYZ[1],mz=pMagDataXYZ[2];
        t=BSP_TSENSOR_ReadTemp();
        h=BSP_HSENSOR_ReadHumidity();
        p=BSP_PSENSOR_ReadPressure();

        int len=sprintf(sens_data,
            "{\"t\":%.2f,\"h\":%.2f,\"p\":%.2f,\"ax\":%d,\"ay\":%d,\"az\":%d,\"gx\":%.0f,\"gy\":%.0f,\"gz\":%.0f,\"mx\":%d,\"my\":%d,\"mz\":%d}",
            t,h,p,ax,ay,az,gx,gy,gz,mx,my,mz);
        response=socket.send(sens_data,len);
        printf("sent %s\n",sens_data);
        if (0>=response){
            printf("Error sending: %d\n",response);
        }
        led=0;
        wait_us(1000000);
    }
    socket.close();
}

int main()
{
    /*
    int count = 0;
    count = scan_demo(&wifi);
    if (count == 0) {
        printf("No WIFI APNs found - can't continue further.\n");
        return -1;
    }
    */
    printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\nConnection error\n");
        return -1;
    }

    printf("Success\n\n");
    printf("MAC: %s\n", wifi.get_mac_address());
    printf("IP: %s\n", wifi.get_ip_address());
    printf("Netmask: %s\n", wifi.get_netmask());
    printf("Gateway: %s\n", wifi.get_gateway());
    printf("RSSI: %d\n\n", wifi.get_rssi());

    //http_demo(&wifi);
    //wifi.disconnect();
    //printf("\nDone\n");

    printf("Sensor init\n");

    BSP_TSENSOR_Init();
    BSP_HSENSOR_Init();
    BSP_PSENSOR_Init();
    BSP_MAGNETO_Init();
    BSP_GYRO_Init();
    BSP_ACCELERO_Init();

    while(1){
        data_sending(&wifi);
    }
}
