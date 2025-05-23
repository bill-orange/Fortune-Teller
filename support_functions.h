#include <math.h>

#include "pngle.h"
// was 16 WEW 04/04/2025
#define LINE_BUF_SIZE 128  // pixel = 524, 16 = 406, 32 = 386, 64 = 375, 128 = 368, 240 = 367, no draw = 324 (51ms v 200ms)
int16_t px = 0, sx = 0; 
int16_t py = 0, sy = 0;
uint8_t pc = 0;
uint16_t lbuf[LINE_BUF_SIZE];

int16_t png_dx = 0, png_dy = 0;

// Define corner position
void setPngPosition(int16_t x, int16_t y) {
  png_dx = x;
  png_dy = y;
}

// Draw pixel - called by pngle
void pngle_on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]) {
  uint16_t color = (rgba[0] << 8 & 0xf800) | (rgba[1] << 3 & 0x07e0) | (rgba[2] >> 3 & 0x001f);

#if !defined(USE_ADAFRUIT_GFX) && defined(USE_LINE_BUFFER)
  color = (color << 8) | (color >> 8);
#endif

  if (rgba[3] > 127) {  // Transparency threshold (no blending yet...)

#ifdef USE_LINE_BUFFER  // This must handle skipped pixels in transparent PNGs
    if (pc >= LINE_BUF_SIZE) {
#ifdef USE_ADAFRUIT_GFX
      tft.drawRGBBitmap(png_dx + sx, png_dy + sy, lbuf, LINE_BUF_SIZE, 1);
#else
      tft.pushImage(png_dx + sx, png_dy + sy, LINE_BUF_SIZE, 1, lbuf);
#endif
      px = x;
      sx = x;
      sy = y;
      pc = 0;
    }

    if ((x == px) && (sy == y) && (pc < LINE_BUF_SIZE)) {
      px++;
      lbuf[pc++] = color;
    } else {
#ifdef USE_ADAFRUIT_GFX
      tft.drawRGBBitmap(png_dx + sx, png_dy + sy, lbuf, pc, 1);
#else
      tft.pushImage(png_dx + sx, png_dy + sy, pc, 1, lbuf);
#endif
      px = x;
      sx = x;
      sy = y;
      pc = 0;
      px++;
      lbuf[pc++] = color;
    }
#else
    tft.drawPixel(x, y, color);
#endif
  }
}


void load_png(const char *url) {
  HTTPClient http;

  http.begin(url);
 
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("HTTP ERROR: %d\n", httpCode);
    http.end();
    return;
  }
  int total = http.getSize();

  WiFiClient *stream = http.getStreamPtr();
  stream->setTimeout(0);   //   this shall improve readByte() time.

  pngle_t *pngle = pngle_new();
  pngle_set_draw_callback(pngle, pngle_on_draw);

  uint8_t buf[1024];
  int remain = 0;
  int len;
  uint32_t timeout = 0;

#if !defined(USE_ADAFRUIT_GFX) && !defined(USE_LINE_BUFFER)
  tft.startWrite();  // Crashes Adafruit_GFX
#endif
  while (http.connected() && (total > 0 || total == -1)) {
    size_t size = stream->available();

    if (timeout > 40000) break;
    if (!size) {
      delay(2);
      timeout++;
      continue;
    }
    if (size > sizeof(buf) - remain) size = sizeof(buf) - remain;

    if ((len = stream->readBytes(buf + remain, size)) > 0) {
      int fed = pngle_feed(pngle, buf, remain + len);
      if (fed < 0) {
        Serial.printf("ERROR: %s\n", pngle_error(pngle));
        break;
      }
      remain = remain + len - fed;
      if (remain > 0) memmove(buf, buf + fed, remain);
      total -= len;
    }
  }
#ifdef USE_LINE_BUFFER
  // Draw any remaining pixels
  if (pc) {
#ifdef USE_ADAFRUIT_GFX
    tft.drawRGBBitmap(png_dx + sx, png_dy + sy, lbuf, pc, 1);
#else
    tft.pushImage(png_dx + sx, png_dy + sy, pc, 1, lbuf);
#endif
    pc = 0;
  }
#endif
#if !defined(USE_ADAFRUIT_GFX) && !defined(USE_LINE_BUFFER)
  tft.endWrite();
#endif
  pngle_destroy(pngle);
  http.end();
}
