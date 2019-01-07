#ifndef FONT8x8W_H
#define FONT8x8W_H

// 8x8 Font ASCII 32 - 127 Implemented
// Users can modify this to support more characters (glyphs)

OLEDFONT(font8x8W) PROGMEM =
{
  0x08,                                     // width
  0x08,                                     // height

  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // <space>
  0x00,0x00,0x5F,0x00,0x00,0x00,0x00,0x00,  // !
  0x00,0x00,0x07,0x00,0x07,0x00,0x00,0x00,  // "
  0x00,0x14,0x7F,0x14,0x7F,0x14,0x00,0x00,  // #
  0x00,0x24,0x2A,0x7F,0x2A,0x12,0x00,0x00,  // $
  0x00,0x23,0x13,0x08,0x64,0x62,0x00,0x00,  // %
  0x00,0x36,0x49,0x55,0x22,0x50,0x00,0x00,  // &
  0x00,0x00,0x05,0x03,0x00,0x00,0x00,0x00,  // '
  0x00,0x1C,0x22,0x41,0x00,0x00,0x00,0x00,  // (
  0x00,0x41,0x22,0x1C,0x00,0x00,0x00,0x00,  // )
  0x00,0x08,0x2A,0x1C,0x2A,0x08,0x00,0x00,  // *
  0x00,0x08,0x08,0x3E,0x08,0x08,0x00,0x00,  // +
  0x00,0xA0,0x60,0x00,0x00,0x00,0x00,0x00,  // ,
  0x00,0x08,0x08,0x08,0x08,0x08,0x00,0x00,  // -
  0x00,0x60,0x60,0x00,0x00,0x00,0x00,0x00,  // .
  0x00,0x20,0x10,0x08,0x04,0x02,0x00,0x00,  // /
  
  0x00,0x3E,0x51,0x49,0x45,0x3E,0x00,0x00,  // 00
  0x00,0x00,0x42,0x7F,0x40,0x00,0x00,0x00,  // 11
  0x00,0x62,0x51,0x49,0x49,0x46,0x00,0x00,  // 22
  0x00,0x22,0x41,0x49,0x49,0x36,0x00,0x00,  // 33
  0x00,0x18,0x14,0x12,0x7F,0x10,0x00,0x00,  // 44
  0x00,0x27,0x45,0x45,0x45,0x39,0x00,0x00,  // 55
  0x00,0x3C,0x4A,0x49,0x49,0x30,0x00,0x00,  // 66
  0x00,0x01,0x71,0x09,0x05,0x03,0x00,0x00,  // 77
  0x00,0x36,0x49,0x49,0x49,0x36,0x00,0x00,  // 88
  0x00,0x06,0x49,0x49,0x29,0x1E,0x00,0x00,  // 99
  0x00,0x00,0x36,0x36,0x00,0x00,0x00,0x00,  // ::
  0x00,0x00,0xAC,0x6C,0x00,0x00,0x00,0x00,  // ;;
  0x00,0x08,0x14,0x22,0x41,0x00,0x00,0x00,  // <<
  0x00,0x14,0x14,0x14,0x14,0x14,0x00,0x00,  // ==
  0x00,0x41,0x22,0x14,0x08,0x00,0x00,0x00,  // >>
  0x00,0x02,0x01,0x51,0x09,0x06,0x00,0x00,  // ??
  0x00,0x32,0x49,0x79,0x41,0x3E,0x00,0x00,  // ?
  
  0x00,0x7E,0x09,0x09,0x09,0x7E,0x00,0x00,  // @
  0x00,0x7F,0x49,0x49,0x49,0x36,0x00,0x00,  // A
  0x00,0x3E,0x41,0x41,0x41,0x22,0x00,0x00,  // B
  0x00,0x7F,0x41,0x41,0x22,0x1C,0x00,0x00,  // C
  0x00,0x7F,0x49,0x49,0x49,0x41,0x00,0x00,  // D
  0x00,0x7F,0x09,0x09,0x09,0x01,0x00,0x00,  // E
  0x00,0x3E,0x41,0x41,0x51,0x72,0x00,0x00,  // F
  0x00,0x7F,0x08,0x08,0x08,0x7F,0x00,0x00,  // G
  0x00,0x41,0x7F,0x41,0x00,0x00,0x00,0x00,  // H
  0x00,0x20,0x40,0x41,0x3F,0x01,0x00,0x00,  // I
  0x00,0x7F,0x08,0x14,0x22,0x41,0x00,0x00,  // J
  0x00,0x7F,0x40,0x40,0x40,0x40,0x00,0x00,  // K
  0x00,0x7F,0x02,0x0C,0x02,0x7F,0x00,0x00,  // L
  0x00,0x7F,0x04,0x08,0x10,0x7F,0x00,0x00,  // M
  0x00,0x3E,0x41,0x41,0x41,0x3E,0x00,0x00,  // N
  0x00,0x7F,0x09,0x09,0x09,0x06,0x00,0x00,  // O
  
  0x00,0x3E,0x41,0x51,0x21,0x5E,0x00,0x00,  // P
  0x00,0x7F,0x09,0x19,0x29,0x46,0x00,0x00,  // Q
  0x00,0x26,0x49,0x49,0x49,0x32,0x00,0x00,  // R
  0x00,0x01,0x01,0x7F,0x01,0x01,0x00,0x00,  // S
  0x00,0x3F,0x40,0x40,0x40,0x3F,0x00,0x00,  // T
  0x00,0x1F,0x20,0x40,0x20,0x1F,0x00,0x00,  // U
  0x00,0x3F,0x40,0x38,0x40,0x3F,0x00,0x00,  // V
  0x00,0x63,0x14,0x08,0x14,0x63,0x00,0x00,  // W
  0x00,0x03,0x04,0x78,0x04,0x03,0x00,0x00,  // X
  0x00,0x61,0x51,0x49,0x45,0x43,0x00,0x00,  // Y
  0x00,0x7F,0x41,0x41,0x00,0x00,0x00,0x00,  // Z
  0x00,0x02,0x04,0x08,0x10,0x20,0x00,0x00,  // [
  0x00,0x41,0x41,0x7F,0x00,0x00,0x00,0x00,  // <
  0x00,0x04,0x02,0x01,0x02,0x04,0x00,0x00,  // ]ackslash>// [
  0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,  // ^
  0x00,0x01,0x02,0x04,0x00,0x00,0x00,0x00,  // _^
  
  0x00,0x20,0x54,0x54,0x54,0x78,0x00,0x00,  // `
  0x00,0x7F,0x48,0x44,0x44,0x38,0x00,0x00,  // a
  0x00,0x38,0x44,0x44,0x28,0x00,0x00,0x00,  // b
  0x00,0x38,0x44,0x44,0x48,0x7F,0x00,0x00,  // c
  0x00,0x38,0x54,0x54,0x54,0x18,0x00,0x00,  // d
  0x00,0x08,0x7E,0x09,0x02,0x00,0x00,0x00,  // e
  0x00,0x18,0xA4,0xA4,0xA4,0x7C,0x00,0x00,  // f
  0x00,0x7F,0x08,0x04,0x04,0x78,0x00,0x00,  // g
  0x00,0x00,0x7D,0x00,0x00,0x00,0x00,0x00,  // h
  0x00,0x80,0x84,0x7D,0x00,0x00,0x00,0x00,  // i
  0x00,0x7F,0x10,0x28,0x44,0x00,0x00,0x00,  // j
  0x00,0x41,0x7F,0x40,0x00,0x00,0x00,0x00,  // k
  0x00,0x7C,0x04,0x18,0x04,0x78,0x00,0x00,  // l
  0x00,0x7C,0x08,0x04,0x7C,0x00,0x00,0x00,  // m
  0x00,0x38,0x44,0x44,0x38,0x00,0x00,0x00,  // n
  0x00,0xFC,0x24,0x24,0x18,0x00,0x00,0x00,  // o
  0x00,0x18,0x24,0x24,0xFC,0x00,0x00,0x00,  // p
  0x00,0x00,0x7C,0x08,0x04,0x00,0x00,0x00,  // q
  0x00,0x48,0x54,0x54,0x24,0x00,0x00,0x00,  // r
  0x00,0x04,0x7F,0x44,0x00,0x00,0x00,0x00,  // s
  0x00,0x3C,0x40,0x40,0x7C,0x00,0x00,0x00,  // t
  0x00,0x1C,0x20,0x40,0x20,0x1C,0x00,0x00,  // u
  0x00,0x3C,0x40,0x30,0x40,0x3C,0x00,0x00,  // v
  0x00,0x44,0x28,0x10,0x28,0x44,0x00,0x00,  // w
  0x00,0x1C,0xA0,0xA0,0x7C,0x00,0x00,0x00,  // x
  0x00,0x44,0x64,0x54,0x4C,0x44,0x00,0x00,  // y
  0x00,0x08,0x36,0x41,0x00,0x00,0x00,0x00,  // z
  0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,  // {
  0x00,0x41,0x36,0x08,0x00,0x00,0x00,0x00,  // |
  0x00,0x02,0x01,0x01,0x02,0x01,0x00,0x00,  // }
  0x00,0x02,0x05,0x05,0x02,0x00,0x00,0x00, /// ~
 };

#endif
