/* Copyright (c) 2012-2018 Chris Swinchatt.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <libk/kchar.h>
#include <libk/kmacro.h>

#include <libk/ktypes.h>

enum {
    ALNUM  = 1 <<  0,
    ALPHA  = 1 <<  1,
    BLANK  = 1 <<  2,
    CNTRL  = 1 <<  3,
    DIGIT  = 1 <<  4,
    GRAPH  = 1 <<  5,
    LOWER  = 1 <<  6,
    PRINT  = 1 <<  7,
    PUNCT  = 1 <<  8,
    SPACE  = 1 <<  9,
    UPPER  = 1 << 10,
    XDIGIT = 1 << 11
};

static const unsigned kchar_table[] = {
    /*   0  NUL   */ CNTRL,
    /*   1  SOH   */ CNTRL,
    /*   2  STX   */ CNTRL,
    /*   3  ETX   */ CNTRL,
    /*   4  EOT   */ CNTRL,
    /*   5  ENQ   */ CNTRL,
    /*   6  ACK   */ CNTRL,
    /*   7  BEL   */ CNTRL,
    /*   8  BS    */ CNTRL,
    /*   9  TAB   */ CNTRL | SPACE,
    /*  10  LF    */ CNTRL | SPACE,
    /*  11  VT    */ CNTRL | SPACE,
    /*  12  FF    */ CNTRL | SPACE,
    /*  13  CR    */ CNTRL | SPACE,
    /*  14  SO    */ CNTRL | SPACE,
    /*  15  SI    */ CNTRL,
    /*  16  DLE   */ CNTRL,
    /*  17  DC1   */ CNTRL,
    /*  18  DC2   */ CNTRL,
    /*  19  DC3   */ CNTRL,
    /*  20  DC4   */ CNTRL,
    /*  21  NAK   */ CNTRL,
    /*  22  SYN   */ CNTRL,
    /*  23  ETB   */ CNTRL,
    /*  24  CAN   */ CNTRL,
    /*  25  EM    */ CNTRL,
    /*  26  SUB   */ CNTRL,
    /*  27  ESC   */ CNTRL,
    /*  28  FS    */ CNTRL,
    /*  29  GS    */ CNTRL,
    /*  30  RS    */ CNTRL,
    /*  31  US    */ CNTRL,
    /*  32  Space */ SPACE | PRINT,
    /*  33  !     */ PUNCT | GRAPH | PRINT,
    /*  34  "     */ PUNCT | GRAPH | PRINT,
    /*  35  #     */ PUNCT | GRAPH | PRINT,
    /*  36  $     */ PUNCT | GRAPH | PRINT,
    /*  37  %     */ PUNCT | GRAPH | PRINT,
    /*  38  &     */ PUNCT | GRAPH | PRINT,
    /*  39  '     */ PUNCT | GRAPH | PRINT,
    /*  40  (     */ PUNCT | GRAPH | PRINT,
    /*  41  )     */ PUNCT | GRAPH | PRINT,
    /*  42  *     */ PUNCT | GRAPH | PRINT,
    /*  43  +     */ PUNCT | GRAPH | PRINT,
    /*  44  ,     */ PUNCT | GRAPH | PRINT,
    /*  45  -     */ PUNCT | GRAPH | PRINT,
    /*  46  .     */ PUNCT | GRAPH | PRINT,
    /*  47  /     */ PUNCT | GRAPH | PRINT,
    /*  48  0     */ DIGIT | XDIGIT | ALNUM | GRAPH | PRINT,
    /*  49  1     */ DIGIT | XDIGIT | ALNUM | GRAPH | PRINT,
    /*  50  2     */ DIGIT | XDIGIT | ALNUM | GRAPH | PRINT,
    /*  51  3     */ DIGIT | XDIGIT | ALNUM | GRAPH | PRINT,
    /*  52  4     */ DIGIT | XDIGIT | ALNUM | GRAPH | PRINT,
    /*  53  5     */ DIGIT | XDIGIT | ALNUM | GRAPH | PRINT,
    /*  54  6     */ DIGIT | XDIGIT | ALNUM | GRAPH | PRINT,
    /*  55  7     */ DIGIT | XDIGIT | ALNUM | GRAPH | PRINT,
    /*  56  8     */ DIGIT | XDIGIT | ALNUM | GRAPH | PRINT,
    /*  57  9     */ DIGIT | XDIGIT | ALNUM | GRAPH | PRINT,
    /*  58  :     */ PUNCT | GRAPH | PRINT,
    /*  59  ;     */ PUNCT | GRAPH | PRINT,
    /*  60  <     */ PUNCT | GRAPH | PRINT,
    /*  61  =     */ PUNCT | GRAPH | PRINT,
    /*  62  >     */ PUNCT | GRAPH | PRINT,
    /*  63  ?     */ PUNCT | GRAPH | PRINT,
    /*  64  @     */ PUNCT | GRAPH | PRINT,
    /*  65  A     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT | XDIGIT,
    /*  66  B     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT | XDIGIT,
    /*  67  C     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT | XDIGIT,
    /*  68  D     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT | XDIGIT,
    /*  69  E     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT | XDIGIT,
    /*  70  F     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT | XDIGIT,
    /*  71  G     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  72  H     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  73  I     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  74  J     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  75  K     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  76  L     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  77  M     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  78  N     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  79  O     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  80  P     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  81  Q     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  82  R     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  83  S     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  84  T     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  85  U     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  86  V     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  87  W     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  88  X     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  89  Y     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  90  Z     */ UPPER | ALPHA | ALNUM | GRAPH | PRINT,
    /*  91  [     */ PUNCT | GRAPH | PRINT,
    /*  92  \     */ PUNCT | GRAPH | PRINT,
    /*  93  ]     */ PUNCT | GRAPH | PRINT,
    /*  94  ^     */ PUNCT | GRAPH | PRINT,
    /*  95  _     */ PUNCT | GRAPH | PRINT,
    /*  96  `     */ PUNCT | GRAPH | PRINT,
    /*  97  a     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT | XDIGIT,
    /*  98  b     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT | XDIGIT,
    /*  99  c     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT | XDIGIT,
    /* 100  d     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT | XDIGIT,
    /* 101  e     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT | XDIGIT,
    /* 102  f     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT | XDIGIT,
    /* 103  g     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 104  h     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 105  i     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 106  j     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 107  k     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 108  l     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 109  m     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 110  n     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 111  o     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 112  p     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 113  q     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 114  r     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 115  s     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 116  t     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 117  u     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 118  v     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 119  w     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 120  x     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 121  y     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 122  z     */ LOWER | ALPHA | ALNUM | GRAPH | PRINT,
    /* 123  {     */ PUNCT | GRAPH | PRINT,
    /* 124  |     */ PUNCT | GRAPH | PRINT,
    /* 125  }     */ PUNCT | GRAPH | PRINT,
    /* 126  ~     */ PUNCT | GRAPH | PRINT,
    /* 127  DEL   */ CNTRL
};

bool kchar_is_alnum(int c)
{
    return TEST_FLAG(kchar_table[c], ALNUM);
}

bool kchar_is_alpha(int c)
{
    return TEST_FLAG(kchar_table[c], ALPHA);
}

bool kchar_is_blank(int c)
{
    return TEST_FLAG(kchar_table[c], BLANK);
}

bool kchar_is_control(int c)
{
    return TEST_FLAG(kchar_table[c], CNTRL);
}

bool kchar_is_digit(int c)
{
    return TEST_FLAG(kchar_table[c], DIGIT);
}

bool kchar_is_graphical(int c)
{
    return TEST_FLAG(kchar_table[c], GRAPH);
}

bool kchar_is_lower_case(int c)
{
    return TEST_FLAG(kchar_table[c], LOWER);
}

bool kchar_is_printable(int c)
{
    return TEST_FLAG(kchar_table[c], PRINT);
}

bool kchar_is_punctuation(int c)
{
    return TEST_FLAG(kchar_table[c], PUNCT);
}

bool kchar_is_space(int c)
{
    return TEST_FLAG(kchar_table[c], SPACE);
}

bool kchar_is_upper_case(int c)
{
    return TEST_FLAG(kchar_table[c], UPPER);
}

bool kchar_is_hex_digit(int c)
{
    return TEST_FLAG(kchar_table[c], XDIGIT);
}

int kchar_to_lower_case(int c)
{
    if (!(kchar_is_alpha(c)) || kchar_is_lower_case(c)) {
        return c;
    }
    return ((c - 'A') + 'a');
}

int kchar_to_upper_case(int c)
{
    if (!(kchar_is_alpha(c)) || kchar_is_upper_case(c)) {
        return c;
    }
    return ((c - 'a') + 'A');
}
