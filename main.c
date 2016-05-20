

#include <iostm8s103f3.h>
#include <stm8s_type.h>
#include <intrinsics.h>
#include "BH1750_drv.h"
#include "delay_us.h"

char lightdata[2];



void main(void)
{ 
  
  BH1750_init();
  BH1750_read (0x23, lightdata); //lightdata=два байта освещенности
  
}

