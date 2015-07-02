#include <stdint.h>
#include "Misc/Delay.h"

void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}
