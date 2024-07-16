#ifndef	GLCD_AA_H
#define GLCD_AA_H

#include <openGLCD.h>

class glcd_aa : public glcd {
  private:
  public:
	glcd_aa();

  void drawLine(uint8_t* buffer, int x0, int y0, int x1, int y1);
  void drawHLine(uint8_t* buffer, int x0, int y0, int length, int thickness);
  void drawVlLine(uint8_t* buffer, int x, int yStart, int yEnd, int thickness);
  void drawChar(uint8_t* buffer, int x, int y, char c);
  void drawText(uint8_t* buffer, int x, int y, const char* text);
  void drawArc(uint8_t* buffer, int x, int y, int radius, int startAngle, int endAngle, int thickness);
  void drawBitmap(uint8_t* bitmap, uint8_t x, uint8_t y, uint8_t color);
};

extern glcd_aa GLCD_AA; 


#endif // GLCD_AA_H