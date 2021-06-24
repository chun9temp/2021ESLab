#include "mbed.h"
#include "ISM43362Interface.h"
#include "HCSR04.h"
#include "VL53L0X.h"
#include <cstdint>
#include <cstdio>

static DevI2C devI2c(PB_11,PB_10);
static DigitalOut shutdown_pin(PC_6);
static VL53L0X laser(&devI2c, &shutdown_pin, PC_7);

ISM43362Interface wifi(false);
InterruptIn button(BUTTON1);
// DigitalOut socket_led(LED1);
DigitalOut car_led(LED2);
DigitalOut obs_led(LED3);
Timer sonic_timer;
HCSR04 sonic(D2, D4); // (echo, trigger)
EventQueue queue(32*EVENTS_EVENT_SIZE);
Thread button_thread;
TCPSocket socket;
SocketAddress addr("10.0.0.20",8000);
nsapi_error_t response;

bool obsExistence, carExistence;
float obsDistance;
bool pause = 0;

void sensor_init() {
    laser.init_sensor(VL53L0X_DEFAULT_ADDRESS);
    sonic.setRanges(2, 70);
    printf("Obstacle detecting range: %g ~ %g cm\n\n", sonic.getMinRange(), sonic.getMaxRange());
}

void get_car_existence() {
    uint32_t dist1, dist2, difference;
    laser.get_distance(&dist1);
    wait_us(50000);
    laser.get_distance(&dist2);
    if (dist1 > dist2) {
        difference = dist1 - dist2;
    }
    else {
        difference = dist2 - dist1;
    }
    if (difference>=200) {
        carExistence = true;
        car_led = 1;
    }
    else {
        carExistence = false;
        car_led = 0;
    }
}

void get_obs_existence() {
    if (obsDistance>=450) {
        obsExistence = false;
        obs_led = 0;
    }
    else {
        obsExistence = true;
        obs_led = 1;
    }
}

void button_pressed()
{
    pause = !pause;
    // carExistence = !carExistence;
    // car_led = !car_led;
    // obs_led = !obs_led;
    // obsExistence = !obsExistence;
    // printf("\nObstacle: %d\n", obsExistence);
}

void get_obs_distance() {
    sonic_timer.reset();
    sonic_timer.start();
    sonic.startMeasurement();
    while(!sonic.isNewDataReady()) {
        // wait for new data
    }
    obsDistance = sonic.getDistance_mm();
    sonic_timer.stop();
    // wait_us(500000 - sonic_timer.elapsed_time().count());
}

void wifi_connect() {
    printf("Connecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("Connection error\n");
    }

    printf("Success\n\n");
    printf("MAC: %s\n", wifi.get_mac_address());
    printf("IP: %s\n", wifi.get_ip_address());
    printf("Netmask: %s\n", wifi.get_netmask());
    printf("Gateway: %s\n", wifi.get_gateway());
    printf("RSSI: %d\n\n", wifi.get_rssi());
}

int socket_test(NetworkInterface *net) {
    response = socket.open(net);
    if (0 != response){
        printf("Error opening: %d\n", response);
        return 0;
    }
    response = socket.connect(addr);
    if (0 != response){
        printf("Error connecting: %d\n", response);
        return 0;
    }
    return 1;
}

void send_road_state(NetworkInterface *net) {    
    // socket_led = 1;
    char obs_info[64];

    get_obs_distance();
    get_obs_existence();

    int len = sprintf(obs_info,
        "{\"car\":%d,\"obs\":%d,\"dist\":%3.1f}",
        carExistence, obsExistence, obsDistance);
    response = socket.send(obs_info,len);
    if (response > 0) {
        printf("sent %s\n",obs_info);
    }
    else {
        printf("Error sending: %d\n", response);
    }
    // socket_led = 0;
    // wait_us(2000000);
}

int main()
{
    wifi_connect();
    bool isServerAvailable = socket_test(&wifi);
    if (!isServerAvailable)
        return -1;

    button_thread.start(callback(&queue,&EventQueue::dispatch_forever));
    button.fall(queue.event(&button_pressed));

    printf("\nSensor init...\n");
    sensor_init();
    
    bool lastCarExistence = carExistence;
    send_road_state(&wifi);

    while (1) {
        if (!pause){
            get_car_existence();
            if (lastCarExistence != carExistence) {
                send_road_state(&wifi);
            }
            if (carExistence) {
                ThisThread::sleep_for(10s);
            }
            lastCarExistence = carExistence;
        }
    }
}
