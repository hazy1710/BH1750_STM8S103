#include <iostm8s103f3.h>
#include <intrinsics.h>

#define F_MASTER_MHZ    16UL
#define F_MASTER_HZ     16000000UL

// ������������� ���� �������                                 
void BH1750_init();

// �������������� � ������ ���������� ��������� ������������
void BH1750_read (unsigned char address, char * data);