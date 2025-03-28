#include "mik32_hal_usart.h"
#include "mik32_hal_scr1_timer.h"

static void SystemClock_Config();
static void USART_Init();
static uint32_t fibonachi(uint32_t n);
static uint32_t fibonachi_fast(uint32_t n);

static USART_HandleTypeDef husart0;

void USART_Print_int(uint64_t val) {
    char str[20]; // 20 символов достаточно для 64-битного int
    int i = 0;

    if (val == 0) {
        HAL_USART_Print(&husart0, "0", USART_TIMEOUT_DEFAULT);
        return;
    }

    while (val != 0) {
        str[i++] = '0' + (val % 10);
        val /= 10;
    }

    for (int j = i - 1; j >= 0; --j) {
        char ch[2] = {str[j], '\0'};
        HAL_USART_Print(&husart0, ch, USART_TIMEOUT_DEFAULT);
    }
}

void USART_Print_fixed_point(uint64_t val) {
    USART_Print_int(val / 1000);
    HAL_USART_Print(&husart0, ".", USART_TIMEOUT_DEFAULT);

    uint32_t frac_part = val % 1000;
    char frac_str[4] = {'0', '0', '0', '\0'};
    frac_str[2] = '0' + (frac_part % 10);
    frac_str[1] = '0' + ((frac_part / 10) % 10);
    frac_str[0] = '0' + ((frac_part / 100) % 10);
    HAL_USART_Print(&husart0, frac_str, USART_TIMEOUT_DEFAULT);
}

int main()
{
    volatile uint32_t fibonachi_result;
    volatile uint64_t start_mtime, end_mtime;
    SystemClock_Config();

    USART_Init();

    while (1)
    {
        HAL_USART_Print(&husart0, "YADRO RISCV\r\n", USART_TIMEOUT_DEFAULT);
        start_mtime = __HAL_SCR1_TIMER_GET_TIME();
        fibonachi_result = fibonachi(30);
        end_mtime = __HAL_SCR1_TIMER_GET_TIME();
        HAL_USART_Print(&husart0, "RECURSIVE:\r\n30-th Fibonacci number is ", USART_TIMEOUT_DEFAULT);
        USART_Print_int(fibonachi_result);
        HAL_USART_Print(&husart0, ", it is calculated in ", USART_TIMEOUT_DEFAULT);
        end_mtime -= start_mtime;
        USART_Print_int(end_mtime);
        HAL_USART_Print(&husart0, " cycles or ", USART_TIMEOUT_DEFAULT);
        uint64_t total_time = end_mtime / 32000;
        USART_Print_fixed_point(total_time);
        HAL_USART_Print(&husart0, " s\r\n", USART_TIMEOUT_DEFAULT);

        start_mtime = __HAL_SCR1_TIMER_GET_TIME();
        fibonachi_result = fibonachi_fast(30);
        end_mtime = __HAL_SCR1_TIMER_GET_TIME();
        HAL_USART_Print(&husart0, "NOT RECURSIVE:\r\n30-th Fibonacci number is ", USART_TIMEOUT_DEFAULT);
        USART_Print_int(fibonachi_result);
        HAL_USART_Print(&husart0, ", it is calculated in ", USART_TIMEOUT_DEFAULT);
        end_mtime -= start_mtime;
        USART_Print_int(end_mtime);
        HAL_USART_Print(&husart0, " cycles or ", USART_TIMEOUT_DEFAULT);
        total_time = end_mtime / 32000;
        USART_Print_fixed_point(total_time);
        HAL_USART_Print(&husart0, " s\r\n\n", USART_TIMEOUT_DEFAULT);

        HAL_DelayMs(5000);
    }
}

void SystemClock_Config(void)
{
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

void USART_Init()
{
    husart0.Instance = UART_0;
    husart0.transmitting = Enable;
    husart0.receiving = Enable;
    husart0.frame = Frame_8bit;
    husart0.parity_bit = Disable;
    husart0.parity_bit_inversion = Disable;
    husart0.bit_direction = LSB_First;
    husart0.data_inversion = Disable;
    husart0.tx_inversion = Disable;
    husart0.rx_inversion = Disable;
    husart0.swap = Disable;
    husart0.lbm = Disable;
    husart0.stop_bit = StopBit_1;
    husart0.mode = Asynchronous_Mode;
    husart0.xck_mode = XCK_Mode3;
    husart0.last_byte_clock = Disable;
    husart0.overwrite = Disable;
    husart0.rts_mode = AlwaysEnable_mode;
    husart0.dma_tx_request = Disable;
    husart0.dma_rx_request = Disable;
    husart0.channel_mode = Duplex_Mode;
    husart0.tx_break_mode = Disable;
    husart0.Interrupt.ctsie = Disable;
    husart0.Interrupt.eie = Disable;
    husart0.Interrupt.idleie = Disable;
    husart0.Interrupt.lbdie = Disable;
    husart0.Interrupt.peie = Disable;
    husart0.Interrupt.rxneie = Disable;
    husart0.Interrupt.tcie = Disable;
    husart0.Interrupt.txeie = Disable;
    husart0.Modem.rts = Disable; //out
    husart0.Modem.cts = Disable; //in
    husart0.Modem.dtr = Disable; //out
    husart0.Modem.dcd = Disable; //in
    husart0.Modem.dsr = Disable; //in
    husart0.Modem.ri = Disable;  //in
    husart0.Modem.ddis = Disable;//out
    husart0.baudrate = 9600;
    HAL_USART_Init(&husart0);
}

uint32_t fibonachi_fast(uint32_t n)
{
    if (n == 0)
        return 0;
    if (n == 1)
        return 1;
    
    uint32_t prev1 = 0, prev2 = 1;
    for (int i = 2; i <= n; ++i) {
        uint32_t temp = prev1 + prev2;
        prev1 = prev2;
        prev2 = temp;
    }
    
    return prev2;
}

uint32_t fibonachi(uint32_t n)
{
    if (n == 0)
    {
        return 0;
    }
    if ((n == 1) || (n == 2))
    {
        return 1;
    }
    return fibonachi(n - 1) + fibonachi(n - 2);
}