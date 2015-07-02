#include <stdint.h>
#include "Misc/Formatter.h"

// 翻转字符串
// @param: 字符串和字符串实际长度
void str_invert(uint8_t* str, uint8_t len) {
  uint8_t i;
  uint8_t tmp;
  for (i=0;i<(len/2);i++) {
    tmp = str[i];
    str[i] = str[len-1-i];
    str[len-1-i] = tmp;
  }
}

uint8_t str_len(uint8_t* str) {
  uint8_t i;
  for (i=0;str[i]!='\0';i++) ;
  return i;
}

void str_cpy(uint8_t* dest, uint8_t* src) {
  uint8_t i;
  for (i=0;src[i]!='\0';i++) {
    dest[i]=src[i];
  }
  dest[i]='\0';
}

void str_cat(uint8_t* str1, uint8_t* str2, uint8_t* dest) { // string concatenate
  uint8_t i;
  uint8_t len = str_len(str1);
  str_cpy(dest, str1);
  for (i=0;str2[i]!='\0';i++) {
    dest[len+i] = str2[i];
  }
  dest[len+i]='\0';
}


// str参数都是传地址的，下同
// 使用方法：在调用之前先声明一个足够大（80合适）的字符数组
//     然后把数组头地址即数组名传给函数
void int_to_string(uint32_t a, uint8_t* str) {
  uint8_t len=0;
  do {
    str[len] = a%10 + 0x30;
    a /= 10;
    len ++;
  } while(a!=0);
  str[len]='\0';
  str_invert(str, len);
}

// 定长，高位补0
void int_to_string_w(uint32_t a, uint8_t w, uint8_t* str) {
  uint8_t len=0;
  uint8_t i; // 循环用
  do {
    str[len] = a%10 + 0x30; // 数字转字符一定要记得加上0x30
    a /= 10;
    len ++;
    if (len == w) break;
  } while(a!=0);
  // 长度为w或者不足
  if (len == w) {
  }
  else if (len < w) {
    for (i=len;i<w;i++) {
      str[i]='0';
      len ++;
    }
  }
  str[w]='\0';
  str_invert(str, w);
}

void float_to_string(float f, uint8_t* str) {
  uint32_t f_val,a_int,a_frac;
  uint8_t str_int[80];
  uint8_t str_frac[80];
  uint8_t tmp[80];
  
  f_val = (uint32_t) (f*1000); //精确到小数点后三位
  a_int = f_val / 1000;
  a_frac = f_val % 1000;
  int_to_string(a_int,str_int);
  int_to_string_w(a_frac,3,str_frac);
  
  uint8_t dot_str[] = ".";
  str_cat(str_int, dot_str, tmp);
  str_cat(tmp, str_frac, str);
}
