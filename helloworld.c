#include <stdio.h>
#include "platform.h"

#include "xparameters.h"  // �]�t�B�z������}�P�]��ID�w�q
#include "xgpio.h"        // GPIO�禡�w
#include "xscugic.h"      // GIC�禡�w

#define Z700_INTC_ID      XPAR_PS7_SCUGIC_0_DEVICE_ID         // for ZYNQ 700�t�C���_�s��
#define BTN_ID            XPAR_AXI_GPIO_1_DEVICE_ID           // sws_8bits GPIO��J
#define LED_ID            XPAR_AXI_GPIO_0_DEVICE_ID           // leds_8bits GPIO��X
#define INTC_GPIO_ID      XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR  // GPIO���_�s��
#define BTN_INT           XGPIO_IR_CH1_MASK                   // GPIO���_�q�D�B�n
#define GIC_ID            XPAR_SCUGIC_SINGLE_DEVICE_ID        // GIC�q�Τ��_�s��

XGpio LED, BTN;
XScuGic INTCInst;
int LED_num = 0;
int Intr_CTN = 0;

// ================================
void Intr_Handler()  // ISR�禡
{
    delay(500);  // �h�u������

    XGpio_InterruptDisable(&BTN, BTN_INT);  // ���� GPIO ���_
    XGpio_InterruptClear(&BTN, BTN_INT);    // �M�����_�X��

    Intr_CTN++;  // ���_���ƥ[ 1
    printf("Interrupt : %x\n", Intr_CTN);

    delay(2000);  // ��� LED ���
    LED_num = 0b00000000;

    XGpio_InterruptEnable(&BTN, BTN_INT);  // ���s�}�Ҥ��_
}
// ================================
void Intr_Setup(XScuGic *GicInstancePtr, XGpio *GpioInstancePtr)
{
    XScuGic_Config *IntcConfig;
    IntcConfig = XScuGic_LookupConfig(GIC_ID);
    XScuGic_CfgInitialize(GicInstancePtr, IntcConfig, IntcConfig->CpuBaseAddress);

    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, GicInstancePtr);
    Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

    XScuGic_Connect(GicInstancePtr, INTC_GPIO_ID, (Xil_ExceptionHandler)Intr_Handler, (void *)GpioInstancePtr);
    XScuGic_Enable(GicInstancePtr, INTC_GPIO_ID);

    XGpio_InterruptGlobalEnable(GpioInstancePtr);
    XGpio_InterruptEnable(GpioInstancePtr, BTN_INT);
}
void delay(int ms) {
    for (int i = 0; i < ms * 10000; i++) {
        asm("nop");
    }
}

// ================================
int main()
{
    init_platform();  // ��l�ƨt��

    XGpio_Initialize(&LED, LED_ID);
    XGpio_SetDataDirection(&LED, 1, 0);  // CH1 ����X

    XGpio_Initialize(&BTN, BTN_ID);
    XGpio_SetDataDirection(&BTN, 1, 1);  // CH1 ����J

    Intr_Setup(&INTCInst, &BTN);  // ��l�Ƥ��_�t��
    print("Init successful\n");

    while (1) {
        XGpio_DiscreteWrite(&LED, 1, LED_num);  // ����LED��X
        printf("LED_num=%x\n", LED_num);
        LED_num = LED_num + 1;
        delay(500);  // 0.5 ��
    }
}
