## Poker Blind Timer using MAX72XX LED Panels (16 8x8 units) with ESP32 and Active Buzzer

A very simple blind timer for poker tournaments.  Includes a 16 unit string of MAX72XX LED matrices, ESP32 microcontroller, Active Buzzer to annound blind changes and a button to start, pause/unpause, advance blinds and reset.

Button operation...

* 1 click = pause / unpuase
* Double click = advance blind level and reset timer
* Triple click = reset device (starts blind level back at 100/200

Blind Levels...

Hardcoded right now as...  (small, big, minutes)

truct blind blinds[17] = {
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
