/* Poker Blind Timer using MAX72XX LED Panels (16 8x8 units) with ESP32 and Active Buzzer

(C) Brad Black - 2024

A very simple blind timer for poker tournaments.  Includes a 16 unit string of MAX72XX LED matrices, ESP32 microcontroller, Active Buzzer to announce blind changes and a button to start, pause/unpause, advance blinds and reset.

### Button operation...

* 1 click = pause / unpause
* Double click = advance blind level and reset timer
* Triple click = reset device (starts blind level back at 100/200)

### Remote "Poker Pod" Displays Using EPS-NOW and LilyGo T-Display S3 Modules...

The poker blind timer can send the current blinds over WiFi using ESP-NOW to one or more battery powered "Poker Pod" modules so that players can have additional displays or use the "Poker Pod" as a card protector.  See image below and GitHub repo here (https://github.com/bradrblack/poker-pods) 

*/

#include <time.h>
#include <FastLED.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "fonts.h"
#include <WiFi.h>
#include "esp_wifi.h"
#include <esp_now.h>

#define buttonPin 3 // long press to reset device
#define buzzerPin 4

#include "jsbutton.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 16

#define CLK_PIN 0
#define DATA_PIN 2
#define CS_PIN 1

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

char buf[20];
char buf2[20];

int blind_level = 1;
int seconds_in_blind_level = 0;
bool paused = true;

struct blind
{
  int small;
  int big;
  int duration;
};

struct blind blinds[17] = {
    {0, 0, 0},
    {100, 200, 15},
    {200, 400, 15},
    {300, 600, 15},
    {400, 800, 15},
    {500, 1000, 15},
    {600, 1200, 15},
    {800, 1600, 15},
    {1000, 2000, 15},
    {1500, 3000, 15},
    {2000, 4000, 15},
    {3000, 6000, 15},
    {4000, 8000, 15},
    {5000, 10000, 10},
    {6000, 12000, 10},
    {8000, 16000, 10},
    {10000, 20000, 10}};

typedef struct msg
{
  int small;
  int big;
} msg;

msg blindData;

uint8_t broadcastAddress1[] = {0x68, 0xB6, 0xB3, 0x23, 0x30, 0x64};
uint8_t broadcastAddress2[] = {0x68, 0xB6, 0xB3, 0x21, 0x87, 0x70};

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{

 char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

}

void sendBlinds(){
  blindData.small = blinds[blind_level].small;
  blindData.big = blinds[blind_level].big;   
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(0, (uint8_t *)&blindData, sizeof(blindData));

    if (result == ESP_OK)
    {
      Serial.println("Sent with success");
    }
    else
    {
      Serial.println("Error sending the data");
    }


}
void beep()
{

  pinMode(buzzerPin, OUTPUT);
  for (int i = 1; i < 3; i++)
  {
    digitalWrite(buzzerPin, HIGH);
    delay(75);
    digitalWrite(buzzerPin, LOW);
    delay(50 * i);
  }
  for (int i = 3; i > 1; i--)
  {
    digitalWrite(buzzerPin, HIGH);
    delay(75);
    digitalWrite(buzzerPin, LOW);
    delay(50 * i);
  }
}

void sendMessage(String m, int tlen);
void sendMessage(char *m);

void readbutton()
{ // Read the button and increase the mode

  uint8_t b = checkButton();

  if (b == 1)
  { // Single Button == PAUSE

    Serial.println("Button Down");

    paused = !paused;
    Serial.println(paused);
    delay(400);
    if (paused)
    {

      showPaused();
    }
    else
    {

      P.displayClear();

      showBlinds();
      showTime();
      sendBlinds();
    }
  }

  if (b == 2)
  { // A double-click event to advance blind
    blind_level++;
    seconds_in_blind_level = 0;
    showBlinds();
    showTime();
    sendBlinds();
  }

  if (b == 3)
  { // A hold event to RESTART

    ESP.restart();
  }
}

void showMessage(char *m)
{

  P.displayClear();
  if (MAX_DEVICES == 16)
  {
    P.setZone(3, 0, 15);
  }
  else if (MAX_DEVICES == 8)
  {
    P.setZone(3, 0, 15);
  }
  else if (MAX_DEVICES == 6)
  {
    P.setZone(3, 0, 5);
  }

  P.displayZoneText(3, m, PA_LEFT, 10, 250, PA_SCROLL_LEFT);

  while (!P.getZoneStatus(3))
    P.displayAnimate();
}

void showPaused()
{

  P.setFont(1, nullptr);

  P.displayZoneText(1, "Paused", PA_LEFT, 10, 250, PA_SCROLL_LEFT);

  while (!P.getZoneStatus(1))
    P.displayAnimate();

  P.setFont(1, numeric7Seg);
}

void showTime()
{

  int seconds, minutes;

  if (seconds_in_blind_level != 0)
  {
    seconds = 60 - (seconds_in_blind_level % 60);
    minutes = (blinds[blind_level].duration - 1) - (seconds_in_blind_level / 60);
    if (seconds == 60)
    {
      seconds = 0;
      minutes++;
    }
  }
  else
  {
    seconds = 0;
    minutes = blinds[blind_level].duration;
  }

  sprintf(buf2, "%d:%02d", minutes, seconds);

  Serial.println(buf2);

  P.displayZoneText(1, buf2, PA_RIGHT, 5, 10, PA_PRINT, PA_NO_EFFECT);
  P.displayAnimate();
}

void showBlinds()
{

  sprintf(buf, "%d / %d", blinds[blind_level].small, blinds[blind_level].big);

  P.displayZoneText(0, buf, PA_LEFT, 5, 5, PA_PRINT, PA_NO_EFFECT);

  while (!P.getZoneStatus(0))
    P.displayAnimate();

  P.displayZoneText(0, buf, PA_LEFT, 5, 10, PA_PRINT, PA_NO_EFFECT);
}

void setup()
{
  esp_wifi_stop();
  delay(10);
  pinMode(buttonPin, INPUT_PULLUP); // Set button input pin
  digitalWrite(buttonPin, HIGH);
  pinMode(9, OUTPUT);

  Serial.begin(115200);
  // wait for serial monitor to open
  delay(1000);

  beep();

  // Set device as a Wi-Fi Station & set "Low Data Rate Mode for ESPNOW"
  WiFi.mode(WIFI_STA);
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);

    // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register Callback function to get status of transmitted data

  esp_now_register_send_cb(OnDataSent);


  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  // register first peer  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // register second peer  
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


  delay(1000);
  Serial.println("About to start Parola...");

  P.begin(16);

  P.setZone(0, 8, 15);
  P.setFont(0, numeric7Seg);
  P.setZone(1, 0, 7);
  P.setFont(1, numeric7Seg);

  P.displayClear();
  P.setIntensity(3);

  delay(500);

  Serial.println("Starting...");

  showMessage("Shuffle Up and Deal...");
}

void loop()
{

  readbutton();

  EVERY_N_SECONDS(5) {
    sendBlinds();
  }

  EVERY_N_MILLISECONDS(1000)
  {
    if (!paused)
    {

      if (seconds_in_blind_level >= blinds[blind_level].duration * 60)
      {
        if (blind_level < 16)
          blind_level++;

        beep();

        seconds_in_blind_level = 0;

        showBlinds();
        showTime();
        sendBlinds();
      }

      showTime();
      seconds_in_blind_level++;
    }
  }
}
