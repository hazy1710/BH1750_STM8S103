#include <iostm8s103f3.h>
#include <intrinsics.h>

#define F_MASTER_MHZ    16UL
#define F_MASTER_HZ     16000000UL

// Инициализация шины датчика                                 
void BH1750_init();

// Преобразование и чтение результата измерения освещенности
void BH1750_read (unsigned char address, char * data);