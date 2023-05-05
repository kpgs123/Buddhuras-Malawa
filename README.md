# Buddhuras-Malawa
Create a Buddhuras Malawa using Attiny85 microcontroller, 74HC595N shift registors and 5 mm LEDs, Also used 2N3906 PNP transistors to switch the LEDs at the cathode end(because of current limitation of shift registers).
LEDs are controlled by multiplexing them. Used clock speed is 8 MHz (default oscillator of the microcontroller wthout devide by 8 option). So, the LED flickering effect is lsser compared to 1 MHz default clock speed.
There are 10 rows(two rows for each colour in Buddhist flag) and 18 columns in the LED matrix. So, I have used 4, 74HC595N shift registors to controll them column-wise and row-wise.

https://user-images.githubusercontent.com/56798215/236384405-39eb3200-2fb9-414c-aaf8-8106b70bec16.mp4

<img src="https://github.com/kpgs123/Buddhuras-Malawa/blob/main/Circuit%201.jpg" width="369" height="538" />            <img src="https://github.com/kpgs123/Buddhuras-Malawa/blob/main/Circuit%202.jpg" width="369" height="538" />
