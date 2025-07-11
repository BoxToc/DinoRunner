# HeroRunner

A Chrome Dino style endless runner game made for a 16x2 LCD using Arduino.  
Jump over blocks and dodge obstacles with a press of a button!

# Features
- Custom LCD sprite animations
- Jump mechanic with physics logic
- Terrain generation
- Score tracking
- Button-controlled start & jump

#How It Works
The game uses a 1602 LCD and custom characters to animate a dino running and jumping. Terrain scrolls from right to left, and you must jump using a button wired to pin 4.

#Wiring
| Component | Arduino Pin |
|----------|--------------|
| LCD RS   | 7            |
| LCD EN   | 8            |
| LCD D4   | 9            |
| LCD D5   | 10           |
| LCD D6   | 11           |
| LCD D7   | 12           |
| Button   | 4 (to GND)   |

Use `INPUT_PULLUP` for the button.

# Getting Started
1. Upload `DinoLCDRunner.ino` to your Arduino.
2. Make sure your LCD and button are wired correctly.
3. Press the button to start playing!

#Demo
(https://www.dropbox.com/scl/fi/et78x4ly4uhu1w3q8qsnz/20250612_164933.mp4?rlkey=0arus6q5y5a212dpqjwwmozvn&st=6pb1cv0b&dl=0)
