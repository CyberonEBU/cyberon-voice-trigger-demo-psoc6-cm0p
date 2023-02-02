/*
PLEASE READ THE CYBERON END USER LICENSE AGREEMENT ("LICENSE(Cyberon)") CAREFULLY BEFORE DOWNLOADING, INSTALLING, COPYING, OR USING THIS SOFTWARE AND ACCOMPANYING DOCUMENTATION.
BY DOWNLOADING, INSTALLING, COPYING OR USING THE SOFTWARE, YOU ARE AGREEING TO BE BOUND BY THE AGREEMENT.
IF YOU DO NOT AGREE TO ALL OF THE TERMS OF THE AGREEMENT, PROMPTLY RETURN AND DO NOT USE THE SOFTWARE.
*/
/*******************************************************************************
* (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "cy_pdl.h"
#include "ipc_def.h"
#include "cycfg.h"
#include "cybsp.h"

#include "cyberon_asr.h"

#define FRAME_SIZE                  (480)

void pdm_pcm_isr_handler(void);

void asr_callback(const char *function, char *message, char *parameter);

volatile bool pdm_pcm_flag = false;
int16_t pdm_pcm_ping[FRAME_SIZE] = {0};
int16_t pdm_pcm_pong[FRAME_SIZE] = {0};
int16_t *pdm_pcm_buffer = &pdm_pcm_ping[0];
const cy_stc_sysint_t pdm_dma_int_cfg = {
    .intrSrc = (IRQn_Type) NvicMux7_IRQn,
    .cm0pSrc = CYBSP_PDM_DMA_IRQ,
    .intrPriority = 2
};

int main(void)
{
    char print_message[128];
    uint64_t uid;
    cy_rslt_t result;

    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    __enable_irq();
    Cy_IPC_Sema_Set(SEMA_NUM, false);
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR);

    do
    {
        __WFE();
    } 
    while(Cy_IPC_Sema_Status(SEMA_NUM) == CY_IPC_SEMA_STATUS_LOCKED);
    
    SystemCoreClockUpdate();

    Cy_DMA_Descriptor_Init(&CYBSP_PDM_DMA_Descriptor_0, &CYBSP_PDM_DMA_Descriptor_0_config);
    Cy_DMA_Descriptor_Init(&CYBSP_PDM_DMA_Descriptor_1, &CYBSP_PDM_DMA_Descriptor_1_config);
    Cy_DMA_Descriptor_SetSrcAddress(&CYBSP_PDM_DMA_Descriptor_0, (const void *) &CYBSP_PDM_PCM_HW->RX_FIFO_RD);
    Cy_DMA_Descriptor_SetSrcAddress(&CYBSP_PDM_DMA_Descriptor_1, (const void *) &CYBSP_PDM_PCM_HW->RX_FIFO_RD);
    Cy_DMA_Descriptor_SetDstAddress(&CYBSP_PDM_DMA_Descriptor_0, (const void *) &pdm_pcm_ping);
    Cy_DMA_Descriptor_SetDstAddress(&CYBSP_PDM_DMA_Descriptor_1, (const void *) &pdm_pcm_pong);
    Cy_DMA_Descriptor_SetXloopDataCount(&CYBSP_PDM_DMA_Descriptor_0, FRAME_SIZE/4);
    Cy_DMA_Descriptor_SetXloopDataCount(&CYBSP_PDM_DMA_Descriptor_1, FRAME_SIZE/4);
    Cy_DMA_Descriptor_SetYloopDstIncrement(&CYBSP_PDM_DMA_Descriptor_0, FRAME_SIZE/4);
    Cy_DMA_Descriptor_SetYloopDstIncrement(&CYBSP_PDM_DMA_Descriptor_1, FRAME_SIZE/4);
    Cy_DMA_Channel_Init(CYBSP_PDM_DMA_HW, CYBSP_PDM_DMA_CHANNEL, &CYBSP_PDM_DMA_channelConfig);
    Cy_DMA_Channel_SetDescriptor(CYBSP_PDM_DMA_HW, CYBSP_PDM_DMA_CHANNEL, &CYBSP_PDM_DMA_Descriptor_0);
    Cy_DMA_Channel_Enable(CYBSP_PDM_DMA_HW, CYBSP_PDM_DMA_CHANNEL);
    Cy_DMA_Enable(CYBSP_PDM_DMA_HW);

    Cy_DMA_Channel_SetInterruptMask(CYBSP_PDM_DMA_HW, CYBSP_PDM_DMA_CHANNEL, CY_DMA_INTR_MASK);
    Cy_SysInt_Init(&pdm_dma_int_cfg, pdm_pcm_isr_handler);
    NVIC_EnableIRQ((IRQn_Type) pdm_dma_int_cfg.intrSrc);

    Cy_PDM_PCM_Init(CYBSP_PDM_PCM_HW, &CYBSP_PDM_PCM_config);
    Cy_PDM_PCM_Enable(CYBSP_PDM_PCM_HW);

    uid = Cy_SysLib_GetUniqueId();
    sprintf(print_message, "uniqueIdHi: 0x%08lX, uniqueIdLo: 0x%08lX\r\n", (uint32_t)(uid >> 32), (uint32_t)(uid << 32 >> 32));
    Cy_SCB_UART_PutString(CYBSP_UART_HW, print_message);

    if(!cyberon_asr_init(asr_callback))
        while(1);

    Cy_SCB_UART_PutString(CYBSP_UART_HW, "\r\nAwaiting voice input trigger command (\"Hello CyberVoice\"):\r\n");

    while(1)
    {
        if(pdm_pcm_flag)
        {
            pdm_pcm_flag = 0;
            cyberon_asr_process(pdm_pcm_buffer, FRAME_SIZE);
        }
    }
}

void pdm_pcm_isr_handler(void)
{
    static bool ping_pong = false;

    if (ping_pong)
    {
        pdm_pcm_buffer = &pdm_pcm_pong[0];
    }
    else
    { 
        pdm_pcm_buffer = &pdm_pcm_ping[0]; 
    }

    ping_pong = !ping_pong;
    pdm_pcm_flag = true;

    Cy_DMA_Channel_ClearInterrupt(CYBSP_PDM_DMA_HW, CYBSP_PDM_DMA_CHANNEL);
}

void asr_callback(const char *function, char *message, char *parameter)
{
    char print_message[128];

    sprintf(print_message, "[%s]%s(%s)\r\n", function, message, parameter);
    Cy_SCB_UART_PutString(CYBSP_UART_HW, print_message);
}
