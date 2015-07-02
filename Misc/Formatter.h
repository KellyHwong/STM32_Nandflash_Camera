#ifndef FORMATTER_H_
#define FORMATTER_H_
#ifdef __cplusplus
 extern "C" {
#endif 
 


#include <stdint.h>

void str_invert(uint8_t* str, uint8_t len);
uint8_t str_len(uint8_t* str);
void str_cpy(uint8_t* dest, uint8_t* src);
void str_cat(uint8_t* str1, uint8_t* str2, uint8_t* dest);

void int_to_string(uint32_t a, uint8_t* str);
void int_to_string_w(uint32_t a, uint8_t w, uint8_t* str);
void float_to_string(float f, uint8_t* str);


#ifdef __cplusplus
}
#endif

#endif
