/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

#include "mbed.h"
#include "USBMSD.h"
#include "SpecifiedAddressBlockDevice.h"
#include "ObservingBlockDevice.h"
#include "mbed_drv_cfg.h"

static Timer * p_timer = NULL;

static void storage_change(BlockDevice * p_bd) {
    if (p_timer != NULL) {
        p_timer->reset();
        p_timer->start();
    }
}

int main() {
    DigitalIn button(USER_BUTTON0);

    if (button == 0) {
        DigitalOut led(LED1);

        led = 1;
        p_timer = new Timer;
        if (p_timer != NULL) {
            p_timer->reset();

            SpecifiedAddressBlockDevice flashiap_bd(POST_APPLICATION_ADDR, FLASH_SIZE - (POST_APPLICATION_ADDR - FLASH_BASE));
            ObservingBlockDevice observing_bd(&flashiap_bd);
            observing_bd.attach(&storage_change);

            USBMSD usb(&observing_bd);

            while (true) {
                usb.process();
                if (flashiap_bd.is_flash_program()) {
                    if (p_timer->read_ms() > 1000) {
                        break;
                    }
                }
            }
        }
        led = 0;
        while (1); // wait reset
    }

    mbed_start_application(POST_APPLICATION_ADDR);
}
