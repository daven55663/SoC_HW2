# 📘 SoC_HW2：ZYNQ LED 中斷控制設計

本專案實作了一個基於 ZYNQ 的 SoC 系統，利用 AXI GPIO 控制 LED 與接收開關輸入，並透過中斷機制觸發特定動作。使用者可於 ZedBoard 上測試按鍵觸發中斷，並觀察 LED 行為變化。

---

## 📐 系統架構

本設計透過 ZYNQ Processing System (PS) 搭配 AXI GPIO 實現：

- `axi_gpio_0`：連接 8-bit LED 輸出 (`leds_8bits`)
- `axi_gpio_1`：連接 8-bit 開關輸入 (`sws_8bits`) 並具中斷輸出 (`ip2intc_irpt`)
- `ps7_0_axi_periph`：AXI Interconnect 管理 PS 與外設間通訊
- `rst_ps7_0_50M`：系統重置信號模組
- PS 端使用 C 程式 (`helloworld.c`) 控制中斷與 GPIO 資料處理

---

## 📂 專案內容

```plaintext
├── design_1.vhd         # RTL 匯出之頂層硬體描述 (Block Design 生成)
├── helloworld.c         # ARM Cortex-A9 上運行的 GPIO 中斷與 LED 控制程式
├── SOC_HW2.PNG          # Block Design 結構圖
├── README.md            # 本文件
```

---

## ⚙️ 功能描述

- ✅ 每 0.5 秒 LED 自動遞增 (模擬流水燈)
- 🔘 使用 `sws_8bits` 中任一按鈕觸發中斷
- 🔁 中斷發生時：
  - 終止 LED 計數、清除顯示
  - 延遲 2 秒
  - 繼續正常計數顯示
- 📟 `UART` 顯示目前 LED 數值與中斷次數

---

## 🧠 核心程式說明 (`helloworld.c`)

- 使用 `XGpio` 控制 GPIO 方向與寫入值
- 使用 `XScuGic` 建立中斷處理機制
- 定義中斷處理函數 `Intr_Handler()`：
  - 執行 debounce 延遲
  - 關閉/清除/重新開啟中斷
  - 將 LED 歸零，並更新中斷次數
- 主迴圈以 `LED_num++` 控制 LED 流水燈效果

---

## 🛠️ 使用方式

### 1️⃣ 硬體設定

1. 在 Vivado 匯出 Block Design 並產生 HDL Wrapper
2. 使用 `design_1.vhd` 實作 FPGA 邏輯
3. 使用 ZYNQ Boot 方式載入程式

### 2️⃣ 軟體編譯

於 Vitis 開發環境中：

- 新增 Platform Project 並匯入硬體描述
- 建立 Application Project，選擇 `helloworld.c` 作為應用程式
- 建立與燒錄程式 (`Run As → Launch on Hardware`)

---

## 💡 補充說明

- GPIO 中斷透過 `ip2intc_irpt` 連接至 PS 的 IRQ，並由 GIC 處理
- `XGpio_SetDataDirection` 控制通道方向（1 為輸入，0 為輸出）
- 使用 `XGpio_DiscreteWrite()` 寫入 LED 狀態

---

## 🧾 參考定義（來自 `xparameters.h`）

| 名稱             | 說明                         |
|------------------|------------------------------|
| `XPAR_AXI_GPIO_0_DEVICE_ID` | LED 輸出 GPIO |
| `XPAR_AXI_GPIO_1_DEVICE_ID` | 開關輸入 GPIO |
| `XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR` | 中斷來源 |

---

## 📸 系統架構圖

![SOC Block Design](SOC_HW2.PNG)

---

如需進一步擴充中斷處理功能（例如按鈕計數、不同 LED pattern），可延伸 `Intr_Handler()` 內容。此專案適合作為 ZYNQ GPIO 與中斷學習範例。
## 實作影片
https://www.youtube.com/shorts/2QIuelzG3dc
