void setup_wifi(const char* ssid, const char* password);
void send_data(const char* payload, const char* url);
void mpu_setup();
void mpu_loop();

#define DEBUG