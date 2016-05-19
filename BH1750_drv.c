#include "BH1750_drv.h"
#include <intrinsics.h>
#include <iostm8s103f3.h>

//Таймаут ожидания события I2C
static unsigned long int i2c_timeout;

//Задать таймаут в микросекундах
#define set_tmo_us(time)  i2c_timeout = (unsigned long int)(F_MASTER_MHZ * time)

//Задать таймаут в миллисекундах
#define set_tmo_ms(time)  i2c_timeout = (unsigned long int)(F_MASTER_MHZ * time * 1000)

#define tmo               i2c_timeout--

//Ожидание наступления события event
//в течении времени timeout в мс
#define wait_event(event, timeout) set_tmo_ms(timeout);\
                                   while(event && --i2c_timeout);\
                                   if(!i2c_timeout) return;
                                   
                                   
//******************************************************************************
// Инициализация                            
//******************************************************************************                                   
void BH1750_init(){
  unsigned long int ccr;
  
  //Настройка GPIO
  PB_DDR_bit.DDR4 = 0;
  PB_DDR_bit.DDR5 = 0;
  PB_ODR_bit.ODR5 = 1;  //SDA
  PB_ODR_bit.ODR4 = 1;  //SCL
  
  PB_CR1_bit.C14 = 0;
  PB_CR1_bit.C15 = 0;
  
  PB_CR2_bit.C24 = 0;
  PB_CR2_bit.C25 = 0;
  
  //Частота тактирования периферии MHz
  I2C_FREQR_FREQ = 16;
  //Отключаем I2C
  I2C_CR1_PE = 0;
  //Выбираем стандартный режим 
  I2C_CCRH_F_S = 0;
  //Для I2C=100000Гц
  //CCR = (1/100000гц)/(2*(1/16000000гц))=80
  ccr = 80;
  I2C_TRISER_TRISE = 16+1;
  I2C_CCRL = ccr & 0xFF;
  I2C_CCRH_CCR = (ccr >> 8) & 0x0F;
  //Включаем I2C
  I2C_CR1_PE = 1;
  //Разрешаем подтверждение в конце посылки
  I2C_CR2_ACK = 1;
}

//******************************************************************************
// Преобразование и чтение результата измерения освещенности
//******************************************************************************                                   
void BH1750_read(unsigned char address, char * data){                                  
  //Преобразование
  
  //Ждем освобождения шины I2C
  wait_event(I2C_SR3_BUSY, 10);
    
  //Генерация СТАРТ-посылки
  I2C_CR2_START = 1;
  //Ждем установки бита SB
  wait_event(!I2C_SR1_SB, 1);
  
  //Записываем в регистр данных адрес датчика 0x23 7bit
  I2C_DR = (address << 1) & 0xFE;
  //Ждем подтверждения передачи адреса
  wait_event(!I2C_SR1_ADDR, 1);
  //Очистка бита ADDR чтением регистра SR3
  I2C_SR3;
  
  //Отправка данных
  //Ждем освобождения регистра данных
  wait_event(!I2C_SR1_TXE, 1);
  //Отправляем адрес регистра
  I2C_DR = 0x10;
  
  //Ловим момент, когда DR освободился и данные попали в сдвиговый регистр
  wait_event(!(I2C_SR1_TXE && I2C_SR1_BTF), 1);
  
  //Посылаем СТОП-посылку
  I2C_CR2_STOP = 1;
  //Ждем выполнения условия СТОП
  wait_event(I2C_CR2_STOP, 1);
  
  //Выждим 200ms
  delay_ms(200);
  
  ///Читаем
  
  //Ждем освобождения шины I2C
  wait_event(I2C_SR3_BUSY, 10);
    
  //Разрешаем подтверждение в конце посылки
  I2C_CR2_ACK = 1;
  
  //Генерация СТАРТ-посылки
  I2C_CR2_START = 1;
  //Ждем установки бита SB
  wait_event(!I2C_SR1_SB, 1);
  
  //Записываем в регистр данных адрес ведомого устройства 0x23 7bit
  I2C_DR = (address << 1) | 0x01;
  //Ждем подтверждения передачи адреса
  wait_event(!I2C_SR1_ADDR, 1);
  //Очистка бита ADDR чтением регистра SR3
  I2C_SR3;
  
  //Ждем освобождения регистра данных RD
  wait_event(!I2C_SR1_TXE, 1);
  
  
    //Бит который разрешает NACK на следующем принятом байте
    I2C_CR2_POS = 1;
    //Ждем подтверждения передачи адреса
    wait_event(!I2C_SR1_ADDR, 1);
    //Заплатка из Errata
    __disable_interrupt();
    //Очистка бита ADDR чтением регистра SR3
    I2C_SR3;
    //Запрещаем подтверждение в конце посылки
    I2C_CR2_ACK = 0;
    //Заплатка из Errata
    __enable_interrupt();
    //Ждем момента, когда первый байт окажется в DR,
    //а второй в сдвиговом регистре
    wait_event(!I2C_SR1_BTF, 1);
    
    //Заплатка из Errata
    __disable_interrupt();
    //Устанавлием бит STOP
    I2C_CR2_STOP = 1;
    //Читаем принятые байты
    *data++ = I2C_DR;
    //Заплатка из Errata
    __enable_interrupt();
    *data = I2C_DR;
  
  
  //Ждем отправки СТОП посылки
  wait_event(I2C_CR2_STOP, 1);
  //Сбрасывает бит POS, если вдруг он был установлен
  I2C_CR2_POS = 0;

}
