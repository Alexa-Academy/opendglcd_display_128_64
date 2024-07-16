#include "openGLCD_AA.h"

glcd_aa::glcd_aa() {
  glcd();
}

// Funzione per disegnare una linea con l'algoritmo di Bresenham
void glcd_aa::drawLine(uint8_t* buffer, int x0, int y0, int x1, int y1) {
  buffer[0] = Width;
  buffer[1] = Height;

  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;

  while (true) {
    if (x0 >= 0 && x0 < Width && y0 >= 0 && y0 < Height) {
      int byteIndex = x0 + (y0 / 8) * Width + 2; // +2 per lasciare spazio a larghezza e altezza
      int bitIndex = y0 % 8;
      buffer[byteIndex] |= (1 << bitIndex);
    }

    if (x0 == x1 && y0 == y1) break;
    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
}

// Funzione per disegnare un singolo carattere su un array di bit
void glcd_aa::drawChar(uint8_t* buffer, int x, int y, char c) {
  buffer[0] = Width;
  buffer[1] = Height;

  int start = 6 + (c - ' ') * 5;

  uint8_t fontChar;

  for (int col = 0; col < 5; col++) { // Ogni carattere è largo 5 pixel
    fontChar = pgm_read_byte(System5x7 + start + col);
    for (int row = 0; row < 8; row++) { // Ogni carattere è alto 8 pixel
      if (fontChar & (1 << row)) {
        int xPos = x + col;
        int yPos = y + row;
        if (xPos >= 0 && xPos < Width && yPos >= 0 && yPos < Height) {
          int byteIndex = xPos + (yPos / 8) * Width + 2; // +2 per lasciare spazio a larghezza e altezza
          int bitIndex = yPos % 8;
          buffer[byteIndex] |= (1 << bitIndex);
        }
      }
    }
  }
}

// Funzione per disegnare una stringa su un array di bit
void glcd_aa::drawText(uint8_t* buffer, int x, int y, const char* text) {
  while (*text) {
    drawChar(buffer, x, y, *text);
    x += 6; // Spazio tra i caratteri (5 pixel + 1 pixel di spazio)
    text++;
  }
}

// Funzione per disegnare una linea orizzontale
void glcd_aa::drawHLine(uint8_t* buffer, int x0, int y0, int length, int thickness) {
  for (int t = 0; t < thickness; t++) {
    int y = y0 + t;
    if (y >= 0 && y < Height) {
      for (int x = x0; x < x0 + length; x++) {
        if (x >= 0 && x < Width) {
          int byteIndex = x + (y / 8) * Width + 2; // +2 per lasciare spazio a larghezza e altezza
          int bitIndex = y % 8;
          buffer[byteIndex] |= (1 << bitIndex);
        }
      }
    }
  }
}

// Funzione per disegnare una linea verticale su un array di bit
void glcd_aa::drawVlLine(uint8_t* buffer, int x, int yStart, int yEnd, int thickness) {
  buffer[0] = Width;
  buffer[1] = Height;
  for (int t = 0; t < thickness; t++) {
    int currentX = x + t;
    if (currentX >= 0 && currentX < Width) {
      for (int y = yStart; y <= yEnd; y++) {
        if (y >= 0 && y < Height) {
          int byteIndex = currentX + (y / 8) * Width;
          int bitIndex = y % 8;
          buffer[byteIndex+2] |= (1 << bitIndex);
        }
      }
    }
  }
}

void glcd_aa::drawArc(uint8_t* buffer, int x, int y, int radius, int startAngle, int endAngle, int thickness) {
  for (int t = 0; t < thickness; t++) {
    int currentRadius = radius + t;
    for (int angle = startAngle; angle <= endAngle; angle++) {
      float radian = angle * (PI / 180);
      int xPos = x + currentRadius * cos(radian);
      int yPos = y + currentRadius * sin(radian);
      if (xPos >= 0 && xPos < Width && yPos >= 0 && yPos < Height) {
        int byteIndex = xPos + (yPos / 8) * Width + 2; // +2 per lasciare spazio a larghezza e altezza
        int bitIndex = yPos % 8;
        buffer[byteIndex] |= (1 << bitIndex);
      }
    }
  }
}

void glcd_aa::drawBitmap(uint8_t* bitmap, uint8_t x, uint8_t y, uint8_t color) {
  uint8_t width, height;
  uint8_t i, j;

  width = *(bitmap++); 
  height = *(bitmap++);

  for(j = 0; j < height / 8; j++) {
    GotoXY(x, y + (j*8) );
    for(i = 0; i < width; i++) {
      uint8_t displayData = *(bitmap++);
      if(color == BLACK)
        WriteData(displayData);
      else
        WriteData(~displayData);
    }
  }
}

// Make one instance for the user
glcd_aa GLCD_AA = glcd_aa();