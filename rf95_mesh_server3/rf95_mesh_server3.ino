#include <Wire.h>
#include "Adafruit_TCS34725.h"

// RiverMesh Sink Node
// Project By Shiza Ali, Anthony Byrne, and Hammas Saeed
//
// Some code adapted from RadioHead and Adafruit library examples

// Mesh has much greater memory requirements, and you may need to limit the
// max message length to prevent wierd crashes
#define RH_MESH_MAX_MESSAGE_LEN 50

#include <RHMesh.h>
#include <RH_RF95.h>
#include <SPI.h>

// In this small artifical network of 4 nodes,
#define CLIENT_ADDRESS 1
#define SERVER1_ADDRESS 2
#define SERVER2_ADDRESS 3
#define SERVER3_ADDRESS 4

#define redpin 13
#define greenpin 12
#define bluepin 11

// set to false if using a common cathode LED
#define commonAnode true

// our RGB -> eye-recognized gamma color
byte gammatable[256];

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

/* for feather m0 RFM9x */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// Singleton instance of the radio driver
RH_RF95 driver(RFM95_CS, RFM95_INT);

// Class to manage message delivery and receipt, using the driver declared above
RHMesh manager(driver, SERVER3_ADDRESS);

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

void setup()
{
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);
    Serial.begin(9600);
    delay(1000);

// use these three pins to drive an LED
#if defined(ARDUINO_ARCH_ESP32)
    ledcAttachPin(redpin, 1);
    ledcSetup(1, 12000, 8);
    ledcAttachPin(greenpin, 2);
    ledcSetup(2, 12000, 8);
    ledcAttachPin(bluepin, 3);
    ledcSetup(3, 12000, 8);
#else
    pinMode(redpin, OUTPUT);
    pinMode(greenpin, OUTPUT);
    pinMode(bluepin, OUTPUT);
#endif

    if (tcs.begin()) {
        Serial.println("Found sensor");
    }
    else {
        Serial.println("No TCS34725 found ... check your connections");
        //while (1)
        //  ;  // halt!
    }

    Serial.println("Feather LoRa Mesh Test: Data Sink/Server 3");

    // manual reset
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    while (!driver.init()) {
        Serial.println("LoRa radio driver init failed");
        Serial.println(
            "Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug "
            "info");
        while (1)
            ;
    }
    Serial.println("LoRa radio init OK!");

    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
    if (!driver.setFrequency(RF95_FREQ)) {
        Serial.println("setFrequency failed");
        while (1)
            ;
    }
    Serial.print("Set Freq to: ");
    Serial.println(RF95_FREQ);

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf =
    // 128chips/symbol, CRC on

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter
    // pin, then
    // you can set transmitter powers from 5 to 23 dBm:
    driver.setTxPower(5, false);

    if (!manager.init())
        Serial.println("Mesh manager init failed");

    // thanks PhilB for this gamma table!
    // it helps convert RGB colors to what humans see
    for (int i = 0; i < 256; i++) {
        float x = i;
        x /= 255;
        x = pow(x, 2.5);
        x *= 255;

        if (commonAnode) {
            gammatable[i] = 255 - x;
        }
        else {
            gammatable[i] = x;
        }
        // Serial.println(gammatable[i]);

        pinMode(A3, INPUT_PULLUP);
    }
}

uint8_t data[] = "123 server3";
// Dont put this on the stack:
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

void loop()
{
    int red, green, blue;
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from)) {
        Serial.print("got request from : 0x");
        Serial.print(from, HEX);
        Serial.print(": ");
        Serial.println((char*)buf);

        red = ((char*)buf)[0];
        green = ((char*)buf)[1];
        blue = ((char*)buf)[2];

#if defined(ARDUINO_ARCH_ESP32)
        ledcWrite(1, gammatable[(int)red]);
        ledcWrite(2, gammatable[(int)green]);
        ledcWrite(3, gammatable[(int)blue]);
#else
        analogWrite(redpin, gammatable[(int)red]);
        analogWrite(greenpin, gammatable[(int)green]);
        analogWrite(bluepin, gammatable[(int)blue]);
#endif

        // Send a reply back to the originator client
        if (manager.sendtoWait(buf, sizeof(buf), from) != RH_ROUTER_ERROR_NONE)
            Serial.println("sendtoWait failed");
    }
}
