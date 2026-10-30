

/* ?????????? */
#include <linux/sched.h>

#include "hw_bfg_ps.h"
#include "plat_debug.h"
#include "plat_uart.h"
#include "bfgx_exception_rst.h"
#include "plat_pm.h"

#ifdef CONFIG_HI1102_PLAT_HW_CHR
#include "chr_user.h"
#endif

/* ???????????? */
typedef void (*platform_msg_handler)(uint8 *buf_ptr);

void platform_unused_msg(uint8 *buf_ptr);
void bfgx_thread_msg_handle(uint8 *buf_ptr);
void bfgx_agree_host_sleep(uint8 *buf_ptr);
void bfgx_not_agree_host_sleep(uint8 *buf_ptr);
void wifi_thread_msg_handle(uint8 *buf_ptr);
void bfgx_read_mem_complete(uint8 *buf_ptr);
void bfgx_read_wifi_mem_complete(uint8 *buf_ptr);
void bfgx_halt_wcpu_complete(uint8 *buf_ptr);
void bfgx_set_uart_loop_complete(uint8 *buf_ptr);
void bfgx_recv_device_chr(uint8 *buf_ptr);
void bfgx_recv_baud_change_cmd(uint8 *buf_ptr);
void gnss_trickle_request_sleep(uint8 *buf_ptr);

platform_msg_handler apf_sys_msg_handle[SYS_INF_BUTT] = {
    platform_unused_msg,         // SYS_INF_PF_INIT = 0x00,                /* ?????????????????? */
    bfgx_thread_msg_handle,      // SYS_INF_BT_INIT = 0x01,                /* BT?????????????? */
    bfgx_thread_msg_handle,      // SYS_INF_GNSS_INIT = 0x02,              /* GNSS?????????????? */
    bfgx_thread_msg_handle,      // SYS_INF_FM_INIT = 0x03,                /* FM?????????????? */
    platform_unused_msg,         // SYS_INF_BT_DISABLE = 0x04,             /* BT???? */
    platform_unused_msg,         // SYS_INF_GNSS_DISABLE = 0x05,           /* GNSS???? */
    platform_unused_msg,         // SYS_INF_FM_DISABLE = 0x06,             /* FM???? */
    bfgx_thread_msg_handle,      // SYS_INF_BT_EXIT = 0x07,                /* BT???? */
    bfgx_thread_msg_handle,      // SYS_INF_GNSS_EXIT = 0x08,              /* GNSS???? */
    bfgx_thread_msg_handle,      // SYS_INF_FM_EXIT = 0x09,                /* FM???? */
    platform_unused_msg,         // SYS_INF_GNSS_WAIT_DOWNLOAD = 0x0A,     /* ????GNSS???????? */
    platform_unused_msg,         // SYS_INF_GNSS_DOWNLOAD_COMPLETE = 0x0B, /* GNSS???????????? */
    platform_unused_msg,         // SYS_INF_BFG_HEART_BEAT = 0x0C,         /* ???????? */
    bfgx_agree_host_sleep,       // SYS_INF_DEV_AGREE_HOST_SLP = 0x0D,     /* device????host???? */
    bfgx_not_agree_host_sleep,   // SYS_INF_DEV_NOAGREE_HOST_SLP = 0x0E,   /* device????host?????? */
    wifi_thread_msg_handle,      // SYS_INF_WIFI_OPEN = 0x0F,              /* WCPU???????? */
    bfgx_thread_msg_handle,      // SYS_INF_IR_INIT = 0x10,                /* IR?????????????? */
    bfgx_thread_msg_handle,      // SYS_INF_IR_EXIT = 0x11,                /* IR???? */
    bfgx_thread_msg_handle,      // SYS_INF_NFC_INIT = 0x12,               /* NFC?????????????? */
    bfgx_thread_msg_handle,      // SYS_INF_NFC_EXIT = 0x13,               /* NFC???? */
    wifi_thread_msg_handle,      // SYS_INF_WIFI_CLOSE = 0x14,             /* WCPU???????? */
    platform_unused_msg,         // SYS_INF_RF_TEMP_NORMAL = 0x15,         /* RF???????? */
    platform_unused_msg,         // SYS_INF_RF_TEMP_OVERHEAT = 0x16,       /* RF???????? */
    bfgx_read_mem_complete,      // SYS_INF_MEM_DUMP_COMPLETE = 0x17,      /* bfgx????????MEM DUMP?????? */
    bfgx_read_wifi_mem_complete, // SYS_INF_WIFI_MEM_DUMP_COMPLETE = 0X18, /* bfgx????????MEM DUMP?????? */
    bfgx_halt_wcpu_complete,     // SYS_INF_UART_HALT_WCPU = 0x19,         /* uart halt wcpu ok */
    bfgx_set_uart_loop_complete, // SYS_INF_UART_LOOP_SET_DONE = 0x1a,     /* device ????uart????ok */
    bfgx_recv_device_chr,        // SYS_INF_CHR_ERRNO_REPORT = 0x1b,       /* device??host????CHR?????? */
    bfgx_recv_baud_change_cmd,   // SYS_INF_BAUD_CHG_2M_REQ = 0x1c,        /* device??host????????2M?????? */
    bfgx_recv_baud_change_cmd,   // SYS_INF_BAUD_CHG_6M_REQ = 0x1D,        /* device??host????????6M?????? */
    bfgx_recv_baud_change_cmd,   // SYS_INF_BAUD_CHG_COMP_ACK = 0x1E,      /* device??host??????????????????????????ACK */
    bfgx_thread_msg_handle,      // SYS_INF_GNSS_LPPE_INIT = 0x1F,         /* 1103 GNSS ???????????????????? */
    bfgx_thread_msg_handle,      // SYS_INF_GNSS_LPPE_EXIT = 0x20,         /* 1103 GNSS ?????????????? */
    gnss_trickle_request_sleep,  // SYS_INF_GNSS_TRICKLE_SlEEP = 0x21,     /* 1103 GNSS TRICKLE Sleep */
};

void platform_unused_msg(uint8 *buf_ptr)
{
    PS_PRINT_WARNING("unused msg:%d, need to check\n", *buf_ptr);
}

void thread_completon_handle(uint8 flag, uint8 thread)
{
    struct ps_core_s *ps_core_d = NULL;

    ps_get_core_reference_etc(&ps_core_d);
    if (ps_core_d == NULL) {
        PS_PRINT_ERR("ps_core_d is NULL\n");
        return;
    }

    if (thread >= BFGX_BUTT) {
        PS_PRINT_ERR("thread index error:%d\n", thread);
        return;
    }

    if (flag == THREAD_FLAG_INIT) {
        PS_PRINT_INFO("%s thread init success\n", ps_core_d->bfgx_info[thread].name);
        complete(&ps_core_d->bfgx_info[thread].wait_opened);
    } else if (flag == THREAD_FLAG_EXIT) {
        PS_PRINT_INFO("%s thread exit success\n", ps_core_d->bfgx_info[thread].name);
        complete(&ps_core_d->bfgx_info[thread].wait_closed);
    } else {
        PS_PRINT_ERR("bfgx thread handle flag error:%d, thread:%d", flag, thread);
    }
}

void gnss_thread_handle(uint8 flag, uint8 info)
{
    switch (info) {
        case SYS_INF_GNSS_INIT:
            PS_PRINT_INFO("GNSS me thread has opened\n");
            gnss_me_thread_status = DEV_THREAD_INIT;
            break;
        case SYS_INF_GNSS_LPPE_INIT:
            PS_PRINT_INFO("GNSS lppe thread has opened\n");
            gnss_lppe_thread_status = DEV_THREAD_INIT;
            break;
        case SYS_INF_GNSS_EXIT:
            PS_PRINT_INFO("GNSS me thread has exit\n");
            gnss_me_thread_status = DEV_THREAD_EXIT;
            break;
        case SYS_INF_GNSS_LPPE_EXIT:
            PS_PRINT_INFO("GNSS lppe thread has exit\n");
            gnss_lppe_thread_status = DEV_THREAD_EXIT;
            break;
        default:
            PS_PRINT_WARNING("GNSS thread info is error %d\n", info);
            break;
    }

    if (flag == THREAD_FLAG_INIT) {
        if ((gnss_me_thread_status == DEV_THREAD_INIT) && (gnss_lppe_thread_status == DEV_THREAD_INIT)) {
            thread_completon_handle(THREAD_FLAG_INIT, BFGX_GNSS);
        }
    } else if (flag == THREAD_FLAG_EXIT) {
        if (((gnss_me_thread_status == DEV_THREAD_EXIT) && (gnss_lppe_thread_status == DEV_THREAD_EXIT))) {
            thread_completon_handle(THREAD_FLAG_EXIT, BFGX_GNSS);
        }
    }
}

void bfgx_thread_msg_handle(uint8 *buf_ptr)
{
    uint8 msg = *buf_ptr;

    switch (msg) {
        case SYS_INF_BT_INIT:
            thread_completon_handle(THREAD_FLAG_INIT, BFGX_BT);
            break;
        case SYS_INF_FM_INIT:
            thread_completon_handle(THREAD_FLAG_INIT, BFGX_FM);
            break;
        case SYS_INF_IR_INIT:
            thread_completon_handle(THREAD_FLAG_INIT, BFGX_IR);
            break;
        case SYS_INF_NFC_INIT:
            thread_completon_handle(THREAD_FLAG_INIT, BFGX_NFC);
            break;
        case SYS_INF_GNSS_INIT:
        case SYS_INF_GNSS_LPPE_INIT:
            gnss_thread_handle(THREAD_FLAG_INIT, msg);
            break;
        case SYS_INF_BT_EXIT:
            thread_completon_handle(THREAD_FLAG_EXIT, BFGX_BT);
            break;
        case SYS_INF_FM_EXIT:
            thread_completon_handle(THREAD_FLAG_EXIT, BFGX_FM);
            break;
        case SYS_INF_IR_EXIT:
            thread_completon_handle(THREAD_FLAG_EXIT, BFGX_IR);
            break;
        case SYS_INF_NFC_EXIT:
            thread_completon_handle(THREAD_FLAG_EXIT, BFGX_NFC);
            break;
        case SYS_INF_GNSS_EXIT:
        case SYS_INF_GNSS_LPPE_EXIT:
            gnss_thread_handle(THREAD_FLAG_EXIT, msg);
            break;
        default:
            PS_PRINT_ERR("thread msg error:%d\n", msg);
            break;
    };
}

void wifi_thread_msg_handle(uint8 *buf_ptr)
{
    uint8 msg;
    struct ps_core_s *ps_core_d = NULL;

    ps_get_core_reference_etc(&ps_core_d);
    if (ps_core_d == NULL) {
        PS_PRINT_ERR("ps_core_d is NULL\n");
        return;
    }

    msg = *buf_ptr;
    switch (msg) {
        case SYS_INF_WIFI_OPEN:
            PS_PRINT_INFO("WIFI has opened by BFGX\n");
            complete(&ps_core_d->wait_wifi_opened);
            break;
        case SYS_INF_WIFI_CLOSE:
            PS_PRINT_INFO("WIFI has closed by BFGX\n");
            complete(&ps_core_d->wait_wifi_closed);
            break;
        default:
            PS_PRINT_ERR("wifi thread msg error:%d\n", msg);
            break;
    }
}

void bfgx_not_agree_host_sleep(uint8 *buf_ptr)
{
    struct pm_drv_data *pm_data = NULL;
    struct ps_core_s *ps_core_d = NULL;

    ps_get_core_reference_etc(&ps_core_d);
    if (ps_core_d == NULL) {
        PS_PRINT_ERR("ps_core_d is NULL\n");
        return;
    }

    pm_data = pm_get_drvdata_etc();
    if (pm_data == NULL) {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return;
    }

    PS_PRINT_INFO("bfgx dev not agree host sleep\n");

    del_timer_sync(&pm_data->dev_ack_timer);
    ps_core_d->ps_pm->bfgx_dev_state_set(BFGX_ACTIVE);
    ps_core_d->ps_pm->bfgx_uart_state_set(UART_READY);

    if ((!bfgx_other_subsys_all_shutdown_etc(BFGX_GNSS)) ||
        (atomic_read(&pm_data->gnss_sleep_flag) == GNSS_AGREE_SLEEP)) {
        mod_timer(&pm_data->bfg_timer, jiffies + (BT_SLEEP_TIME * HZ / 1000));
        pm_data->bfg_timer_mod_cnt++;
    }

    complete(&pm_data->dev_ack_comp);
}

void bfgx_agree_host_sleep(uint8 *buf_ptr)
{
    uint64 flags;
    struct pm_drv_data *pm_data = NULL;
    struct ps_core_s *ps_core_d = NULL;

    ps_get_core_reference_etc(&ps_core_d);
    if (ps_core_d == NULL) {
        PS_PRINT_ERR("ps_core_d is NULL\n");
        return;
    }

    pm_data = pm_get_drvdata_etc();
    if (pm_data == NULL) {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return;
    }

    PS_PRINT_INFO("bfgx dev agree host sleep\n");

    if (timer_pending(&pm_data->dev_ack_timer)) {
        PS_PRINT_DBG("dev ack to agree to sleep in normal case\n");
        del_timer_sync(&pm_data->dev_ack_timer);
        complete(&pm_data->dev_ack_comp);

        if (unlikely(pm_data->rcvdata_bef_devack_flag == 1)) {
            PS_PRINT_INFO("device send data to host before dev rcv allow slp msg\n");
            pm_data->rcvdata_bef_devack_flag = 0;
        } else if (atomic_read(&ps_core_d->node_visit_flag) > 0) {
            PS_PRINT_INFO("someone visit dev node during waiting for dev_ack\n");
        } else {
            spin_lock_irqsave(&pm_data->wakelock_protect_spinlock, flags);
            if (pm_data->bfgx_dev_state == BFGX_ACTIVE) {
                PS_PRINT_INFO("dev wkup isr occur during waiting for dev_ack\n");
            } else {
                pm_data->ps_pm_interface->operate_beat_timer(BEAT_TIMER_DELETE);
                bfg_wake_unlock_etc();
            }
            spin_unlock_irqrestore(&pm_data->wakelock_protect_spinlock, flags);
        }
    } else {
        PS_PRINT_INFO("dev ack to agree to sleep after devack timer expired, gpio state:%d\n",
                      board_get_bwkup_gpio_val_etc());
        spin_lock_irqsave(&pm_data->uart_state_spinlock, flags);
        ps_core_d->ps_pm->bfgx_uart_state_set(UART_NOT_READY);
        ps_core_d->ps_pm->bfgx_dev_state_set(BFGX_SLEEP);
        spin_unlock_irqrestore(&pm_data->uart_state_spinlock, flags);
    }
}

void bfgx_read_mem_complete(uint8 *buf_ptr)
{
    struct st_exception_info *pst_exception_data = NULL;

#ifdef HI110X_HAL_MEMDUMP_ENABLE
    bfgx_memdump_finish_etc();
#endif

    get_exception_info_reference_etc(&pst_exception_data);
    if (pst_exception_data != NULL) {
        PS_PRINT_INFO("plat bfgx mem dump complete\n");

        atomic_set(&pst_exception_data->is_memdump_runing, 0);
        complete_all(&pst_exception_data->wait_read_bfgx_stack);

#ifndef HI110X_HAL_MEMDUMP_ENABLE
        if (pst_exception_data->exception_reset_enable != PLAT_EXCEPTION_ENABLE) {
            bfgx_store_stack_mem_to_file_etc();
        }
#endif
    }
}

void bfgx_read_wifi_mem_complete(uint8 *buf_ptr)
{
    PS_PRINT_INFO("uart wifi mem dump complete\n");
    store_wifi_mem_to_file_etc();
}

void bfgx_halt_wcpu_complete(uint8 *buf_ptr)
{
    struct st_exception_info *pst_exception_data = NULL;

    get_exception_info_reference_etc(&pst_exception_data);
    if (pst_exception_data != NULL) {
        PS_PRINT_INFO("uart halt wcpu ok\n");
        complete(&pst_exception_data->wait_uart_halt_wcpu);
    }
}

void bfgx_set_uart_loop_complete(uint8 *buf_ptr)
{
    if (uart_loop_test_info_etc != NULL) {
        PS_PRINT_INFO("bfgx uart loop test set ok\n");
        complete(&uart_loop_test_info_etc->set_done);
    }
}

void bfgx_recv_device_chr(uint8 *buf_ptr)
{
#ifdef CONFIG_HI1102_PLAT_HW_CHR
    CHR_ERRNO_WITH_ARG_STRU *pst_chr = (CHR_ERRNO_WITH_ARG_STRU *)(buf_ptr + 1);
#endif

    PS_PRINT_INFO("rcv devcie chr report\n");

#ifdef CONFIG_HI1102_PLAT_HW_CHR
    if (pst_chr->errlen > CHR_ERR_DATA_MAX_LEN) {
        PS_PRINT_ERR("bfgx recv chr wrong, errno:0x%x, errlen:%d\n", pst_chr->errno, pst_chr->errlen);
        return;
    }

    chr_dev_exception_callback_etc((uint8 *)pst_chr, (pst_chr->errlen + OAL_SIZEOF(CHR_ERRNO_WITH_ARG_STRU)));
#else
    PS_PRINT_INFO("host chr not support, ignore errno:0x%x\n", *(uint32 *)buf_ptr);
#endif
}

void gnss_trickle_request_sleep(uint8 *buf_ptr)
{
    struct pm_drv_data *pm_data;

    pm_data = pm_get_drvdata_etc();
    if (pm_data == NULL) {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return;
    }

    PS_PRINT_INFO("gnss trickle sleep!\n");

    if (!timer_pending(&pm_data->bfg_timer)) {
        PS_PRINT_SUC("gnss low power request sleep!\n");

        if (queue_work(pm_data->wkup_dev_workqueue, &pm_data->send_allow_sleep_work) != true) {
            PS_PRINT_INFO("queue_work send_allow_sleep_work not return true\n");
        }
    }

    /* set the flag to 1 means gnss request sleep */
    atomic_set(&pm_data->gnss_sleep_flag, GNSS_AGREE_SLEEP);
}

void bfgx_recv_baud_change_cmd(uint8 *buf_ptr)
{
    uint8 syschar = *buf_ptr;

    switch (syschar) {
        case SYS_INF_BAUD_CHG_2M_REQ:
        case SYS_INF_BAUD_CHG_6M_REQ:
            bfgx_uart_rcv_baud_change_req(syschar);
            break;
        case SYS_INF_BAUD_CHG_COMP_ACK:
            bfgx_uart_rcv_baud_change_complete_ack();
            break;
        default:
            PS_PRINT_ERR("bfgx uart change cmd error:%d\n", syschar);
            break;
    }
}

void bfgx_recv_heartbeat(uint8 *buf_ptr)
{
    struct st_exception_info *pst_exception_data = NULL;
    struct ps_core_s *ps_core_d = NULL;
    uint8 syschar;
    struct pm_drv_data *pm_data = NULL;

    pm_data = pm_get_drvdata_etc();
    if (pm_data == NULL) {
        PS_PRINT_ERR("pm_data is NULL!\n");
        return;
    }

    ps_get_core_reference_etc(&ps_core_d);
    if (ps_core_d == NULL) {
        PS_PRINT_ERR("ps_core_d is NULL\n");
        return;
    }

    syschar = *buf_ptr;

    PS_PRINT_INFO("%ds,tty_tx=%d,tty_rx=%d,uart_tx=%d,uart_rx=%d\n",
                  syschar - SYS_INF_HEARTBEAT_CMD_BASE - 1, ps_uart_state_cur_etc(STATE_TTY_TX),
                  ps_uart_state_cur_etc(STATE_TTY_RX), ps_uart_state_cur_etc(STATE_UART_TX),
                  ps_uart_state_cur_etc(STATE_UART_RX));
    PS_PRINT_INFO("pkt_num_rx:BT=%lu,GNSS=%lu,FM=%lu,IR=%lu,SYS_MSG=%lu,OML=%lu,timer=%d,mod_cnt=%d,gnss_sleep_flag=%d\n",
                  ps_core_d->rx_pkt_num[BFGX_BT], ps_core_d->rx_pkt_num[BFGX_GNSS],
                  ps_core_d->rx_pkt_num[BFGX_FM], ps_core_d->rx_pkt_num[BFGX_IR],
                  ps_core_d->rx_pkt_sys, ps_core_d->rx_pkt_oml, timer_pending(&pm_data->bfg_timer),
                  pm_data->bfg_timer_mod_cnt, atomic_read(&pm_data->gnss_sleep_flag));
    PS_PRINT_INFO("pkt_num_tx:BT=%lu,GNSS=%lu,FM=%lu,IR=%lu\n",
                  ps_core_d->tx_pkt_num[BFGX_BT], ps_core_d->tx_pkt_num[BFGX_GNSS],
                  ps_core_d->tx_pkt_num[BFGX_FM], ps_core_d->tx_pkt_num[BFGX_IR]);

    get_exception_info_reference_etc(&pst_exception_data);
    if (pst_exception_data != NULL) {
        if (pst_exception_data->debug_beat_flag == 1) {
            atomic_set(&pst_exception_data->bfgx_beat_flag, BFGX_RECV_BEAT_INFO);
        }
    }
}

/*
 * Prototype    : ps_exe_sys_func_etc
 * Description  : called by core when recive sys data event from device
 *              : decode rx sys packet function
 */
int32 ps_exe_sys_func_etc(struct ps_core_s *ps_core_d, uint8 *buf_ptr)
{
    uint8 syschar;

    if (buf_ptr == NULL) {
        PS_PRINT_ERR("buf_ptr is NULL\n");
        return -EINVAL;
    }

    syschar = *buf_ptr;

    if (syschar < SYS_INF_BUTT) {
        if (apf_sys_msg_handle[syschar] != NULL) {
            apf_sys_msg_handle[syschar](buf_ptr);
        }
    } else if (syschar >= SYS_INF_HEARTBEAT_CMD_BASE) {
        bfgx_recv_heartbeat(buf_ptr);
    } else {
        PS_PRINT_WARNING("bfgx recv unsupport msg:0x%x\n", syschar);
    }

    return 0;
}
