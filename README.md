# MSE450_SU21_G01
Repository for the MSE450/750 course work

# Simulator Game Project
## Corona
Based on the game Space Impact. Project files can be found in the Space Impact directory.

### Gameplay
The game I/O consists of 2 buttons and a joystick for x and y movement

-- Note: x movement unsupported in simulation

Use SW2 (PF0) for start/pause and SW1 (PF4) to fire lasers
use Ain1 (PE2) for y movement and Ain0 (PE3) for x movement

The goal of the game is to destroy the most enemies and last as long as possible.
The score is based on the number of enemies destroyed.
There are 3 lives at the beginning of the game, every time there is a collision with an enemy the ship will lose a life
when all lives are gone the game is over.

### Demo
![demo](https://media.github.sfu.ca/user/157/files/c699d900-d904-11eb-9771-77e67407c180)

### Development Success
* Successfully achieved all the goals of 1.0 release. Some bugs were caught, but many have been resolved
* After difficulties with the Labs, planning the git project allowed for much more seamless integration of each member's contributions. Only limited work was required to run the game after each member sections were complete

### Development Difficulties
* Some difficulties pertaining to the simulator. A lot of register access issues, and a lot of features that do not work in the simulator
* ADC is questionable when implementing in the simulator. Texas simulator allows you to select any ADC and it will work with the only adc channel implemented. Texas Simulator does not support more than 1 ADC from what I can tell
* Difficulties with project files and conflicts, Keil creates too many files when it builds so it's difficult to get rid of everything that isn't required. Due to the nature of Keil project files, it can make an unmanageable mess of the repository which are prone to conflicts and requiring numerous manual project file merges and make code reviews difficult
* Many minor bugs found when adding code together, though nothing catastrophic due to following previously planned structure

### Features
* SSI Control for Nokia 5110 Display - Ver 1.0 - Completed
* Game timing control using SysTick and counter - Ver 1.0 - Completed
* Screen update timer to control display updates - Ver 1.0 - Completed
* ADC for x and y control of joystick - Ver 1.0 - Completed
* Pushbutton control for start/pause and fire - Ver 1.0 - Completed
* Game engine for detecting state changes - Ver 1.0 - Completed
* Collision detection for sprites, and scorekeeping - Ver 1.0 - Completed
* UART to simulate/debug - Ver 1.0 - Unnecessary
* LED backlight control - Ver 2.0
* Buzzer implementation using PWM - Ver 2.0
