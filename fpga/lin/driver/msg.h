#ifndef __MSG_H
#define __MSG_H

//#define debug_msg printk
#define msg_err(format, arg...) printk(KERN_ERR    "RSP ERROR::: " format "\n" , ## arg)

#ifdef RSP5XX_DEBUG
#define msg_dbg(format, arg...) printk(KERN_INFO "RSP DBG::: " format "\n" , ## arg)
#else
#define msg_dbg(format, arg...)
#endif


#endif

