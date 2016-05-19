#include "BH1750_drv.h"
#include <intrinsics.h>
#include <iostm8s103f3.h>

//������� �������� ������� I2C
static unsigned long int i2c_timeout;

//������ ������� � �������������
#define set_tmo_us(time)  i2c_timeout = (unsigned long int)(F_MASTER_MHZ * time)

//������ ������� � �������������
#define set_tmo_ms(time)  i2c_timeout = (unsigned long int)(F_MASTER_MHZ * time * 1000)

#define tmo               i2c_timeout--

//�������� ����������� ������� event
//� ������� ������� timeout � ��
#define wait_event(event, timeout) set_tmo_ms(timeout);\
                                   while(event && --i2c_timeout);\
                                   if(!i2c_timeout) return;
                                   
                                   
//******************************************************************************
// �������������                            
//******************************************************************************                                   
void BH1750_init(){
  unsigned long int ccr;
  
  //��������� GPIO
  PB_DDR_bit.DDR4 = 0;
  PB_DDR_bit.DDR5 = 0;
  PB_ODR_bit.ODR5 = 1;  //SDA
  PB_ODR_bit.ODR4 = 1;  //SCL
  
  PB_CR1_bit.C14 = 0;
  PB_CR1_bit.C15 = 0;
  
  PB_CR2_bit.C24 = 0;
  PB_CR2_bit.C25 = 0;
  
  //������� ������������ ��������� MHz
  I2C_FREQR_FREQ = 16;
  //��������� I2C
  I2C_CR1_PE = 0;
  //�������� ����������� ����� 
  I2C_CCRH_F_S = 0;
  //��� I2C=100000��
  //CCR = (1/100000��)/(2*(1/16000000��))=80
  ccr = 80;
  I2C_TRISER_TRISE = 16+1;
  I2C_CCRL = ccr & 0xFF;
  I2C_CCRH_CCR = (ccr >> 8) & 0x0F;
  //�������� I2C
  I2C_CR1_PE = 1;
  //��������� ������������� � ����� �������
  I2C_CR2_ACK = 1;
}

//******************************************************************************
// �������������� � ������ ���������� ��������� ������������
//******************************************************************************                                   
void BH1750_read(unsigned char address, char * data){                                  
  //��������������
  
  //���� ������������ ���� I2C
  wait_event(I2C_SR3_BUSY, 10);
    
  //��������� �����-�������
  I2C_CR2_START = 1;
  //���� ��������� ���� SB
  wait_event(!I2C_SR1_SB, 1);
  
  //���������� � ������� ������ ����� ������� 0x23 7bit
  I2C_DR = (address << 1) & 0xFE;
  //���� ������������� �������� ������
  wait_event(!I2C_SR1_ADDR, 1);
  //������� ���� ADDR ������� �������� SR3
  I2C_SR3;
  
  //�������� ������
  //���� ������������ �������� ������
  wait_event(!I2C_SR1_TXE, 1);
  //���������� ����� ��������
  I2C_DR = 0x10;
  
  //����� ������, ����� DR ����������� � ������ ������ � ��������� �������
  wait_event(!(I2C_SR1_TXE && I2C_SR1_BTF), 1);
  
  //�������� ����-�������
  I2C_CR2_STOP = 1;
  //���� ���������� ������� ����
  wait_event(I2C_CR2_STOP, 1);
  
  //������ 200ms
  delay_ms(200);
  
  ///������
  
  //���� ������������ ���� I2C
  wait_event(I2C_SR3_BUSY, 10);
    
  //��������� ������������� � ����� �������
  I2C_CR2_ACK = 1;
  
  //��������� �����-�������
  I2C_CR2_START = 1;
  //���� ��������� ���� SB
  wait_event(!I2C_SR1_SB, 1);
  
  //���������� � ������� ������ ����� �������� ���������� 0x23 7bit
  I2C_DR = (address << 1) | 0x01;
  //���� ������������� �������� ������
  wait_event(!I2C_SR1_ADDR, 1);
  //������� ���� ADDR ������� �������� SR3
  I2C_SR3;
  
  //���� ������������ �������� ������ RD
  wait_event(!I2C_SR1_TXE, 1);
  
  
    //��� ������� ��������� NACK �� ��������� �������� �����
    I2C_CR2_POS = 1;
    //���� ������������� �������� ������
    wait_event(!I2C_SR1_ADDR, 1);
    //�������� �� Errata
    __disable_interrupt();
    //������� ���� ADDR ������� �������� SR3
    I2C_SR3;
    //��������� ������������� � ����� �������
    I2C_CR2_ACK = 0;
    //�������� �� Errata
    __enable_interrupt();
    //���� �������, ����� ������ ���� �������� � DR,
    //� ������ � ��������� ��������
    wait_event(!I2C_SR1_BTF, 1);
    
    //�������� �� Errata
    __disable_interrupt();
    //����������� ��� STOP
    I2C_CR2_STOP = 1;
    //������ �������� �����
    *data++ = I2C_DR;
    //�������� �� Errata
    __enable_interrupt();
    *data = I2C_DR;
  
  
  //���� �������� ���� �������
  wait_event(I2C_CR2_STOP, 1);
  //���������� ��� POS, ���� ����� �� ��� ����������
  I2C_CR2_POS = 0;

}
