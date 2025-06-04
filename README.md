# Fortune-Teller
Fortune Teller for the ESP32-S3 with graphics and a Voice

This project demonstrates the use of:

* the CHAGPT API
* Google Translate (used for Text-to-Speech).  
* Images stored on github to supplement the user interface.
* Audio.h to play MP3 files

The project works like this.  The user is promped to hit a button mounted under the TFT display. This action generates a call to ChatGPT to get a fortune.  The fortune prompt is stored in Prompt.txt in github.  The prompt produces a short fortune.  

Google translate appears to be limited to 80 charater text strings so the fortune has to be brief. An ESP32-S3 with PSRAM must be used for this project. The Huge partition scheme must be used.  Audio is produced by the ESP32-S3 in I2S format.  This is siliar to I2c.  It is specifically for audio streams.  A separate 3.3v regulator circuit may be needed for the audio board. It draws a lot of current and puts noise on the bus.  Three GPIOs are needed.

The amplifier/DAC is a MAX 98357A, I2S to speaker level converter.  It is very loud.

![fortune](https://github.com/user-attachments/assets/e9bc1ecd-8b50-4ef0-8462-ecb36f2eecf6)
