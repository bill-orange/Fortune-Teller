# Fortune-Teller and More
Fortune Teller for the ESP32-S3 with graphics and a Voice

This project demonstrates the use of:

* the Chat GPT API
* Google Translate (used for Text-to-Speech).  
* Images stored on github to supplement the user interface.
* Audio.h to play MP3 files

The project works like this.  The user is promped to hit a button mounted under the TFT display. This action generates a call to ChatGPT to get a fortune.  The fortune prompt is stored in Prompt.txt in github.  The prompt produces a short fortune.  

Google translate appears to be limited to about 100 charater text strings so the fortune has to be brief. An ESP32-S3 with PSRAM must be used for this project. The Huge partition scheme must be used.  Audio is produced by the ESP32-S3 in I2S format.  This is siliar to I2C.  It is specifically for audio streams.  A separate 3.3v regulator circuit may be needed for the audio board. It draws a lot of current and puts noise on the Vcc.  Three GPIOs are needed.

The amplifier/DAC is a MAX 98357A, I2S to speaker level converter.  It is very loud.  Along with "fortune Teller" a Ham Radio Simulator is also included.  This demonstrates the versatility of the libraries used in these sketches and Libraries.

![fortune](https://github.com/user-attachments/assets/e9bc1ecd-8b50-4ef0-8462-ecb36f2eecf6)
