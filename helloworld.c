#include <stdio.h>
#include "platform.h"

#include "xparameters.h"  // 包含處理器的位址與設備ID定義
#include "xgpio.h"        // GPIO函式庫
#include "xscugic.h"      // GIC函式庫

#define Z700_INTC_ID      XPAR_PS7_SCUGIC_0_DEVICE_ID         // for ZYNQ 700系列中斷編號
#define BTN_ID            XPAR_AXI_GPIO_1_DEVICE_ID           // sws_8bits GPIO輸入
#define LED_ID            XPAR_AXI_GPIO_0_DEVICE_ID           // leds_8bits GPIO輸出
#define INTC_GPIO_ID      XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR  // GPIO中斷編號
#define BTN_INT           XGPIO_IR_CH1_MASK                   // GPIO中斷通道遮罩
#define GIC_ID            XPAR_SCUGIC_SINGLE_DEVICE_ID        // GIC通用中斷編號

XGpio LED, BTN;
XScuGic INTCInst;
int LED_num = 0;
int Intr_CTN = 0;

// ================================
void Intr_Handler()  // ISR函式
{
    delay(500);  // 去彈跳延遲

    XGpio_InterruptDisable(&BTN, BTN_INT);  // 關閉 GPIO 中斷
    XGpio_InterruptClear(&BTN, BTN_INT);    // 清除中斷旗標

    Intr_CTN++;  // 中斷次數加 1
    printf("Interrupt : %x\n", Intr_CTN);

    delay(2000);  // 顯示 LED 兩秒
    LED_num = 0b00000000;

    XGpio_InterruptEnable(&BTN, BTN_INT);  // 重新開啟中斷
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
    init_platform();  // 初始化系統

    XGpio_Initialize(&LED, LED_ID);
    XGpio_SetDataDirection(&LED, 1, 0);  // CH1 為輸出

    XGpio_Initialize(&BTN, BTN_ID);
    XGpio_SetDataDirection(&BTN, 1, 1);  // CH1 為輸入

    Intr_Setup(&INTCInst, &BTN);  // 初始化中斷系統
    print("Init successful\n");

    while (1) {
        XGpio_DiscreteWrite(&LED, 1, LED_num);  // 控制LED輸出
        printf("LED_num=%x\n", LED_num);
        LED_num = LED_num + 1;
        delay(500);  // 0.5 秒
    }
}
