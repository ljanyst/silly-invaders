//------------------------------------------------------------------------------
// Copyright (c) 2016 by Lukasz Janyst <lukasz@jany.st>
//------------------------------------------------------------------------------
// This file is part of silly-invaders.
//
// silly-invaders is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// silly-invaders is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with silly-invaders.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------

#pragma once

#include <stdint.h>

//------------------------------------------------------------------------------
// Register definitions
//------------------------------------------------------------------------------
#define STCTRL_REG           (*(volatile unsigned long *)0xe000e010)
#define STRELOAD_REG         (*(volatile unsigned long *)0xe000e014)
#define STCURRENT_REG        (*(volatile unsigned long *)0xe000e018)
#define INTCTRL_REG          (*(volatile unsigned long *)0xe000ed04)
#define NVIC_EN0_REG         (*(volatile unsigned long *)0xe000e100)
#define NVIC_EN1_REG         (*(volatile unsigned long *)0xe000e104)
#define NVIC_EN2_REG         (*(volatile unsigned long *)0xe000e108)
#define NVIC_EN3_REG         (*(volatile unsigned long *)0xe000e10c)
#define NVIC_EN4_REG         (*(volatile unsigned long *)0xe000e110)
#define NVIC_EN_REG(NUM)     (*(volatile unsigned long *)(0xe000e100+4*NUM))
#define NVIC_PRI_REG(NUM)    (*(volatile unsigned long *)(0xe000e400+4*NUM))
#define SYSPRI3_REG          (*(volatile unsigned long *)0xe000ed20)
#define CPAC_REG             (*(volatile unsigned long *)0xe000ed88)
#define MPUCTRL_REG          (*(volatile unsigned long *)0xe000ed94)
#define MPUBASE_REG          (*(volatile unsigned long *)0xe000ed9c)
#define MPUATTR_REG          (*(volatile unsigned long *)0xe000eda0)

#define RIS_REG              (*(volatile unsigned long *)0x400fe050)
#define RCC_REG              (*(volatile unsigned long *)0x400fe060)
#define GPIOHBCTL_REG        (*(volatile unsigned long *)0x400fe06c)
#define RCC2_REG             (*(volatile unsigned long *)0x400fe070)
#define RCGCTIMER_REG        (*(volatile unsigned long *)0x400fe604)
#define RCGCGPIO_REG         (*(volatile unsigned long *)0x400fe608)
#define RCGCDMA_REG          (*(volatile unsigned long *)0x400fe60c)
#define RCGCUART_REG         (*(volatile unsigned long *)0x400fe618)
#define RCGCSSI_REG          (*(volatile unsigned long *)0x400fe61c)
#define RCGCADC_REG          (*(volatile unsigned long *)0x400fe638)
#define RCGCWTIMER_REG       (*(volatile unsigned long *)0x400fe65c)

//------------------------------------------------------------------------------
// DMA
//------------------------------------------------------------------------------
#define DMACFG_REG           (*(volatile unsigned long *)0x400ff004)
#define DMACTLBASE_REG       (*(volatile unsigned long *)0x400ff008)
#define DMAUSEBURSTSET_REG   (*(volatile unsigned long *)0x400ff018)
#define DMAUSEBURSTCLR_REG   (*(volatile unsigned long *)0x400ff01c)
#define DMAREQMASKSET_REG    (*(volatile unsigned long *)0x400ff020)
#define DMAREQMASKCLR_REG    (*(volatile unsigned long *)0x400ff024)
#define DMAENASET_REG        (*(volatile unsigned long *)0x400ff028)
#define DMAALTSET_REG        (*(volatile unsigned long *)0x400ff030)
#define DMAALTCLR_REG        (*(volatile unsigned long *)0x400ff034)
#define DMAPRIOSET_REG       (*(volatile unsigned long *)0x400ff038)
#define DMAPRIOCLR_REG       (*(volatile unsigned long *)0x400ff03c)
#define DMACHIS_REG          (*(volatile unsigned long *)0x400ff504)
#define DMA_MAP_REG(NUM)     (*(volatile unsigned long *)(0x400ff510+4*NUM))

//------------------------------------------------------------------------------
// GPIO
//------------------------------------------------------------------------------
#define GPIO_PORTA_NUM 0
#define GPIO_PORTB_NUM 1
#define GPIO_PORTC_NUM 2
#define GPIO_PORTD_NUM 3
#define GPIO_PORTE_NUM 4
#define GPIO_PORTF_NUM 5

#define GPIO_PIN0_NUM 0
#define GPIO_PIN1_NUM 1
#define GPIO_PIN2_NUM 2
#define GPIO_PIN3_NUM 3
#define GPIO_PIN4_NUM 4
#define GPIO_PIN5_NUM 5
#define GPIO_PIN6_NUM 6
#define GPIO_PIN7_NUM 7

#define GPIO_PIN7_BIT_OFFSET 0x0200
#define GPIO_PIN6_BIT_OFFSET 0x0100
#define GPIO_PIN5_BIT_OFFSET 0x0080
#define GPIO_PIN4_BIT_OFFSET 0x0040
#define GPIO_PIN3_BIT_OFFSET 0x0020
#define GPIO_PIN2_BIT_OFFSET 0x0010
#define GPIO_PIN1_BIT_OFFSET 0x0008
#define GPIO_PIN0_BIT_OFFSET 0x0004

#define GPIO_REG_BASE    0x40058000
#define GPIO_PORTA       0x0000
#define GPIO_PORTB       0x1000
#define GPIO_PORTC       0x2000
#define GPIO_PORTD       0x3000
#define GPIO_PORTE       0x4000
#define GPIO_PORTF       0x5000

#define GPIO_DATA        0x03fc
#define GPIO_DIR         0x0400
#define GPIO_IBE         0x0408
#define GPIO_IM          0x0410
#define GPIO_MIS         0x0418
#define GPIO_ICR         0x041c
#define GPIO_AFSEL       0x0420
#define GPIO_PUR         0x0510
#define GPIO_DEN         0x051c
#define GPIO_LOCK        0x0520
#define GPIO_PCTL        0x052c
#define GPIO_CR          0x0524
#define GPIO_AMSEL       0x0528
#define GPIO_PCTL        0x052c

#define GPIO_PORT_OFFSET 0x1000

#define GPIO_REG(PORT, REG) (*(volatile unsigned long*)(GPIO_REG_BASE + PORT + REG))
#define GPIO_DATA_REG(PORT, PIN) (*(volatile unsigned long*)(GPIO_REG_BASE + PORT + (1 << (PIN+2))))

//------------------------------------------------------------------------------
// UART
//------------------------------------------------------------------------------
#define UART_REG_BASE      0x4000c000
#define UART_MODULE0       0x0000
#define UART_MODULE1       0x1000
#define UART_MODULE2       0x2000
#define UART_MODULE3       0x3000
#define UART_MODULE4       0x4000
#define UART_MODULE5       0x5000
#define UART_MODULE6       0x6000
#define UART_MODULE7       0x7000

#define UART_DR            0x0000
#define UART_FR            0x0018
#define UART_IBRD          0x0024
#define UART_FBRD          0x0028
#define UART_LCRH          0x002c
#define UART_CTL           0x0030
#define UART_IFLS          0x0034
#define UART_IM            0x0038
#define UART_RIS           0x003c
#define UART_MIS           0x0040
#define UART_ICR           0x0044
#define UART_DMACTL        0x0048

#define UART_MODULE_OFFSET 0x1000

#define UART_REG(MODULE, REG) (*(volatile unsigned long*)(UART_REG_BASE + MODULE + REG))

//------------------------------------------------------------------------------
// SSI
//------------------------------------------------------------------------------
#define SSI_REG_BASE      0x40008000
#define SSI_CR0           0x0000
#define SSI_CR1           0x0004
#define SSI_DR            0x0008
#define SSI_SR            0x000c
#define SSI_CPSR          0x0010
#define SSI_IM            0x0014
#define SSI_MIS           0x001c
#define SSI_DMACTL        0x0024
#define SSI_CC            0x0fc8

#define SSI_MODULE_OFFSET 0x1000

#define SSI_REG(MODULE, REG) (*(volatile unsigned long*)(SSI_REG_BASE + MODULE + REG))

//------------------------------------------------------------------------------
// Timers
//------------------------------------------------------------------------------
#define GPTM_CFG           0x0000
#define GPTM_TAMR          0x0004
#define GPTM_CTL           0x000c
#define GPTM_IMR           0x0018
#define GPTM_ICR           0x0024
#define GPTM_TAILR         0x0028
#define GPTM_TBILR         0x002c

#define GPTM_MODULE_OFFSET 0x1000

#define GPTM_REG_BASE(MODULE) (MODULE < 0x8000 ? 0x40030000 : 0x40044000)
#define GPTM_REG(MODULE, REG) (*(volatile unsigned long*)(GPTM_REG_BASE(MODULE) + MODULE + REG))

//------------------------------------------------------------------------------
// ADC
//------------------------------------------------------------------------------
#define ADC_REG_BASE      0x40038000
#define ADC_ACTSS         0x0000
#define ADC_RIS           0x0004
#define ADC_IM            0x0008
#define ADC_ISC           0x000c
#define ADC_PSSI          0x0028
#define ADC_SSMUX0        0x0040
#define ADC_SSCTL0        0x0044
#define ADC_SSFIFO0       0x0048
#define ADC_SSMUX1        0x0060
#define ADC_SSCTL1        0x0064
#define ADC_SSFIFO1       0x0068
#define ADC_SSMUX2        0x0080
#define ADC_SSCTL2        0x0084
#define ADC_SSFIFO2       0x0088
#define ADC_SSMUX3        0x00a0
#define ADC_SSCTL3        0x00a4
#define ADC_SSFIFO3       0x00a8
#define ADC_MODULE_OFFSET 0x1000

#define ADC_REG(MODULE, REG) (*(volatile unsigned long*)(ADC_REG_BASE + MODULE + REG))

//------------------------------------------------------------------------------
//! Initialize the board
//------------------------------------------------------------------------------
int32_t TM4C_init(uint32_t stack_size);

//------------------------------------------------------------------------------
//! Enable an interrupt
//------------------------------------------------------------------------------
void TM4C_enable_interrupt(uint8_t number, uint8_t priority);
