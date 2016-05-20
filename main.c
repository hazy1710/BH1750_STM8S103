

#include <iostm8s103f3.h>
#include <stm8s_type.h>
#include <intrinsics.h>
#include "BH1750_drv.h"
#include "ds1w.h" 
#include "delay_us.h"
#include "usart.h"


u8 b_1, b_2, b_crc;
u8 mode;
u8 uart_count=0;
u8 *onewire_dev_addr;
char lightdata[2];
char string[30];



u8 ModeTempRead(u8 *addr);
u8 OneWireDevList();
u8 ModeLightRead();
void ModeRun(void);

//��������������� � ������ ASCII
u8 * ConvertByte(u8 *byte, u8 count);


// ���������� ���������� �� ������ UART
#pragma vector=UART1_R_RXNE_vector
__interrupt void uart_rx_interrupt(void){
    unsigned char data;
    data = UART1_DR;
   // ����� ��������� AGR.COMM.(�������)
    switch (uart_count)
    {
    case 0:
      if (data==0x41) {
        uart_count++;
        break;
      }
      uart_count=0;
      break;
      
    case 1:
      if (data==0x47) {
        uart_count++;
        break;
      }
      uart_count=0;
      break;
      
    case 2:
      if (data==0x52) {
        uart_count++;
        break;
      }
      uart_count=0;
      break;
      
    case 3:
      if (data==0x2E) {
        uart_count++;
        break;
      }
      uart_count=0;
      break;
      
    case 4:
      if (data==0x43) {
        uart_count++;
        break;
      }
      uart_count=0;
      break;
      
    case 5:
      if (data==0x4F) {
        uart_count++;
        break;
      }
      uart_count=0;
      break;
      
    case 6:
      if (data==0x4D) {
        uart_count++;
        break;
      }
      uart_count=0;
      break;
      
    case 7:
      if (data==0x4D) {
        uart_count++;
        break;
      }
      uart_count=0;
      break;
      
    case 8:
      if (data==0x2E) {
        uart_count++;
        break;
      }
      uart_count=0;
      break;
      
    case 9:            ///���� �������
      mode=data;
      uart_count=0;
      break;
      
    }
    
    
    //
    //
    //
    //
    //
    //
    //�������� �������� ���� �������
    //usart_tx_byte(data);
    //mode=data;
}

//��������������� � ������ ASCII
u8 * ConvertByte(u8 *byte,u8 count){

   ///������������ �������
    for(u8 counter=0; counter<count; counter++){
        string[counter*2]= ( *byte         >> 4 ) & 0x0F; 
        string[counter*2+1]= *byte                & 0x0F; 
        byte++;
  }//////�������. �������
    for(u8 counter=0; counter<count*2; counter++){
        string[counter]=string[counter]+0x30;
        if (string[counter]>0x39) {string[counter]=string[counter]+0x07;}
    }
  return &string[0];
}


void ModeRun(void)
{ while (1){
  switch (mode)  //�������� ������
    {
    case 0x10://     0 � ����� 1
        PA_ODR_bit.ODR3=0;
        mode=0;
        break;
        
    case 0x11: //     1 � ����� 1
        PA_ODR_bit.ODR3=1;
        mode=0;
        break;
        
    case 0x20://     0 � ����� 2
        PA_ODR_bit.ODR2=0;
        mode=0;
        break;
        
    case 0x21: //     1 � ����� 2
        PA_ODR_bit.ODR2=1;
        mode=0;
        break;
        
    case 0x30://     0 � ����� 3
        PA_ODR_bit.ODR1=0;
        mode=0;
        break;
        
    case 0x31: //     1 � ����� 3
        PA_ODR_bit.ODR1=1;
        mode=0;
        break;
        
    case 0x40://     0 � ����� 4
        PD_ODR_bit.ODR4=0;
        mode=0;
        break;
        
    case 0x41: //     1 � ����� 4
        PD_ODR_bit.ODR4=1;
        mode=0;
        break;
        
        
    case 0x50://     0 � ����� 5
        PD_ODR_bit.ODR2=0;
        mode=0;
        break;
        
    case 0x51: //     1 � ����� 5
        PD_ODR_bit.ODR2=1;
        mode=0;
        break;
        
        
    case 0x60://     0 � ����� 6
        PD_ODR_bit.ODR3=0;
        mode=0;
        break;
        
    case 0x61: //     1 � ����� 6
        PD_ODR_bit.ODR3=1;
        mode=0;
        break;
        
    case 0x63:             //send command
       // ModeKeyCommand();
        break;
    case 0x99:             //������ �����������
        ModeLightRead();
        break;
    case 0x32:             //������ 1wire 18b20 ��������� � �� �����������
        OneWireDevList();
        break;
    }
}
}

void main(void)
{   
  // ��������� ��������� ����������
  CLK_CKDIVR = 0;                    // ������������ ����� ����
  
  ONEWIRE_PIN_DDR = 1; //�����������  ��� �����  �� �����
  ONEWIRE_PIN_CR1 = 0; //����������� ��� � ����� push-pull (��� ����� �� ����� �������� 
  //� ������ � ������� �������), � �� ��-��������� �� ������������� ����� � �������� ������
  //(��� ����� ����� �������� ������ ������ �������, � ������ �������� ������������� �� ����)
  ONEWIRE_PIN_OUT = 1;
  
  PD_DDR_bit.DDR3=1; //PD3 ����� 6
  PD_ODR_bit.ODR3=0;
  PD_CR1_bit.C13=1;
  
  PD_DDR_bit.DDR2=1; //PD2 ����� 5
  PD_ODR_bit.ODR2=0;
  PD_CR1_bit.C12=1;
  
  PD_DDR_bit.DDR4=1; //PD4 ����� 4
  PD_ODR_bit.ODR4=0;
  PD_CR1_bit.C14=1;
  
  PA_DDR_bit.DDR1=1; //PA1 ����� 3
  PA_ODR_bit.ODR1=0;
  PA_CR1_bit.C11=1;
  
  PA_DDR_bit.DDR2=1; //PA2 ����� 2
  PA_ODR_bit.ODR2=0;
  PA_CR1_bit.C12=1;
  
  PA_DDR_bit.DDR3=1; //PA3 ����� 1
  PA_ODR_bit.ODR3=0;
  PA_CR1_bit.C13=1;
  
  BH1750_init();
  BH1750_read (0x23, lightdata);
  
  //EEPROM
  //EEPROM_Unlock();
  
  //USART
  __enable_interrupt(); //��������� ����������
  usart_init();
  
  mode=0x01;
  
  //Flash();
  while (1){
    ModeRun();
  } //����. ����

}


///������� � �������� ������� 1wire ���������
u8 OneWireDevList(){
  mode=0x00;
  
  //������� �������������� ����������� �������� DS18b20
  if(!onewire_reset()){return 0;}
  onewire_send(0xCC,0);
  onewire_send(0x44,0);
  delay_ms(750); 
  
  ///�������������
  onewire_enum_init();
  
  ///������� � �������� �������
  onewire_dev_addr=onewire_enum_next();                       //������� ��������� �� ������ �����
  while (onewire_dev_addr){
    send_massiv(ConvertByte(onewire_dev_addr,8),16);         //�������� � ���� ������ �������
    usart_tx_byte(';');                                      //���������
    ModeTempRead(onewire_dev_addr);
    usart_tx_byte(0x0d);                                     //������� ������
    onewire_dev_addr=onewire_enum_next();                    ////������� ���������� �����
  }
  usart_tx_byte(0x04);                                       //�������� ��������
  return 1;
}

//������ �����������
u8 ModeTempRead(u8 *addr){ 
  u8 tb_0, tb_1, t_digit, t_drob;
  mode=0x00;

  //������� ������� DS18b20
  if(!onewire_reset()){return 0;}
  onewire_send(0x55,0);
  for(u8 counter=0; counter<8; counter++){             //����� ������ ������������� �������
        onewire_send(*addr,0);
        addr++;
  }
  onewire_send(0xBE,0);
  tb_0=onewire_read();
  tb_1=onewire_read();
  
  //�������������� �����������
  t_drob   = (  tb_0                & 0x0F);  
  t_digit  = (  tb_0         >> 4 ) & 0x0F;  
  t_digit |= ( (tb_1 & 0x0F) << 4 ) & 0xF0;  
 
  //�������� ������ � ����
  
  if (tb_1 & 0x80) {          // ������������� ����������� 
    t_digit=~t_digit;
    usart_tx_byte('-');
  }
  else usart_tx_byte('+');  //�������������
   
  usart_tx_byte(';');                     //���������
  send_massiv(ConvertByte(&t_digit,1),2); //����� �������
  usart_tx_byte(';');                     //���������
  send_massiv(ConvertByte(&t_drob,1),2);  //������� �������/0.0625
  mode=0x00;
// usart_tx_byte(b_crc);
return 1;
}

//������ ������������
u8 ModeLightRead(){
   BH1750_read (0x23, lightdata);
   send_massiv(lightdata,2);
   mode=0x00;
   return 1;
}

