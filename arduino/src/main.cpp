#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <SocketIOClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include "helper.h"

// Wifi credientials
const char* ssid = "TanHaus";
const char* password = "12345678";

// server settings
const char* server_ip = "192.168.137.1";
const int port = 3000;
const char* url = "http://192.168.137.1:3000/";
// const char* server_ip = "hack-and-roll-2021.herokuapp.com";
// const int port = 80;
// const char* url = "http://hack-and-roll-2021.herokuapp.com/";

SocketIOclient io;
MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector

const float alpha = 0.9;
float smooth_data[6];
unsigned long last_time;

float euler[3];
#define INTERRUPT_PIN 18

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length);
void mpu_setup();
void mpu_loop();

void setup() {
  Serial.begin(115200);
  Serial.println();

  setup_wifi(ssid, password);
  
  Serial.print(F("[socket.io] Connecting to "));
  Serial.println(server_ip);
  io.begin(server_ip, port);
  io.onEvent(socketIOEvent);

  Serial.println();

  mpu_setup();
  last_time = millis();
}

void loop() {
  io.loop();
  mpu_loop();
}

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case sIOtype_DISCONNECT:
      Serial.printf("[IOc] Disconnected!\n");
      break;
    case sIOtype_CONNECT:
      Serial.printf("[IOc] Connected to url: %s\n", payload);

      // join default namespace (no auto join in Socket.IO V3)
      io.send(sIOtype_CONNECT, "/");
      io.send(sIOtype_CONNECT, "/");
      break;
    case sIOtype_EVENT:
      Serial.printf("[IOc] get event: %s\n", payload);
      break;
    case sIOtype_ACK:
      Serial.printf("[IOc] get ack: %u\n", length);
      break;
    case sIOtype_ERROR:
      Serial.printf("[IOc] get error: %u\n", length);
      break;
    case sIOtype_BINARY_EVENT:
      Serial.printf("[IOc] get binary: %u\n", length);
      break;
    case sIOtype_BINARY_ACK:
      Serial.printf("[IOc] get binary ack: %u\n", length);
      break;
  }
}

void dmpDataReady() {
    mpuInterrupt = true;
}

void mpu_setup()
{
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties

  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  // verify connection
  Serial.println(F("Testing device connections..."));
  if (mpu.testConnection()) {
    Serial.println(F("MPU6050 connection successful"));
    Serial.println();
  } else {
    Serial.println(F("MPU6050 connection failed"));
    return;
  }

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    Serial.println();
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
    Serial.println();
  }
}

void send_sensor_data() {
  // JSON data
  static const int JSON_SIZE = 150;
  static const char* event_name = "sensor";
  StaticJsonDocument<JSON_SIZE> doc;
  JsonArray array = doc.to<JsonArray>();
  array.add(event_name);
  JsonObject data = array.createNestedObject();
  String payload;

  data["timestamp"] = last_time;
  data["aX"] = smooth_data[0]/16384.;
  data["aY"] = smooth_data[1]/16384.;
  data["aZ"] = smooth_data[2]/16384.;
  data["gX"] = smooth_data[3];
  data["gY"] = smooth_data[4];
  data["gZ"] = smooth_data[5];

  serializeJson(doc, payload);

  // send_data(payload, url);
  io.sendEVENT(payload);
}

void mpu_loop()
{
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  if (!mpuInterrupt && fifoCount < packetSize) return;

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
#ifdef DEBUG
    Serial.println(F("FIFO overflow!"));
#endif
    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x02) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;


    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetEuler(euler, &q);
#ifdef DEBUG
    // Serial.print("euler\t");
    // Serial.print(euler[0] * 180/M_PI);
    // Serial.print("\t");
    // Serial.print(euler[1] * 180/M_PI);
    // Serial.print("\t");
    // Serial.println(euler[2] * 180/M_PI);
#endif

    // display real acceleration, adjusted to remove gravity
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
    // Serial.print("aa\t");
    // Serial.print(aa.x/16384.);
    // Serial.print("\t");
    // Serial.print(aa.y/16384.);
    // Serial.print("\t");
    // Serial.println(aa.z/16384.);
    
    // exponential smoothing
    smooth_data[0] = smooth_data[0]*(1-alpha) + aaWorld.x*alpha;
    smooth_data[1] = smooth_data[1]*(1-alpha) + aaWorld.y*alpha;
    smooth_data[2] = smooth_data[2]*(1-alpha) + aaWorld.z*alpha;
    smooth_data[3] = smooth_data[3]*(1-alpha) + euler[0]*alpha;
    smooth_data[4] = smooth_data[4]*(1-alpha) + euler[1]*alpha;
    smooth_data[5] = smooth_data[5]*(1-alpha) + euler[2]*alpha;
    unsigned long current = millis();
    if (current - last_time > 100) {
      last_time = current;
      send_sensor_data();
    }
  }
}