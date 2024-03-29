![poker-blind-timer](IMG_2418.jpg)

## Poker Blind Timer using MAX72XX LED Panels (16 8x8 units) with ESP32 and Active Buzzer

A very simple blind timer for poker tournaments.  Includes a 16 unit string of MAX72XX LED matrices, ESP32 microcontroller, Active Buzzer to announce blind changes and a button to start, pause/unpause, advance blinds and reset.

### Button operation...

* 1 click = pause / unpause
* Double click = advance blind level and reset timer
* Triple click = reset device (starts blind level back at 100/200)

### Remote "Poker Pod" Displays Using EPS-NOW and LilyGo T-Display S3 Modules...

The poker blind timer can send the current blinds over WiFi using ESP-NOW to one or more battery powered "Poker Pod" modules so that players can have additional displays or use the "Poker Pod" as a card protector.  See image below and GitHub repo here (https://github.com/bradrblack/poker-pods)

### Blind Levels...

Hardcoded right now as...  (small, big, minutes)


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

![poker-pod](IMG_2421.jpg)
