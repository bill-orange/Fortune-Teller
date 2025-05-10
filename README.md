# Fortune-Teller
Fortune Teller for the ESP32-S3 with a Voice

This project demonstrates the use of CHAGPT with Google Translate (used for Text-to-Speech).  Images stored on github supplement the user interface.  

The project works like this.  The user is proped to hit a button mounted under the TFT display. This action generates a call to ChatGPT to get a fortune.  The fortune propt is stored in Prompt.txt in github.  The propt produces a shore fortune.  

Google translate appears to be limited to 80 charater text strings so the fortune has to be brief.An ESP32-S# with PSRAM must be used for this project. The Huge partition scheme must be used.  Audio is produced by the ESP32-S# in I2S format.  This is siliar to I2c.  It is specifically for audio streams.  Three GPIOs are needed.

The amplifier/DAC is a MAX 98357A, I2S to speaker level converter.  It is very loud.

![fortune](https://github.com/user-attachments/assets/e9bc1ecd-8b50-4ef0-8462-ecb36f2eecf6)
