#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xa8c16cf3, "module_layout" },
	{ 0x15692c87, "param_ops_int" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x3ba086f3, "pci_unregister_driver" },
	{ 0x6904684b, "driver_remove_file" },
	{ 0xf04c3d01, "driver_create_file" },
	{ 0x7efeadd, "__pci_register_driver" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0xd8e484f0, "register_chrdev_region" },
	{ 0x15128ba5, "kill_fasync" },
	{ 0xe52947e7, "__phys_addr" },
	{ 0x4f8b5ddb, "_copy_to_user" },
	{ 0xe660bd22, "pci_bus_read_config_word" },
	{ 0x1000e51, "schedule" },
	{ 0x4f6b400b, "_copy_from_user" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xfb578fc5, "memset" },
	{ 0xf5893abf, "up_read" },
	{ 0xa31807da, "get_user_pages" },
	{ 0x57a6ccd0, "down_read" },
	{ 0xd2b09ce5, "__kmalloc" },
	{ 0xec7e91a4, "put_page" },
	{ 0xb5dcab5b, "remove_wait_queue" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0x5860aad4, "add_wait_queue" },
	{ 0xffd5a395, "default_wake_function" },
	{ 0x571ab46f, "current_task" },
	{ 0xfd5f002d, "dma_ops" },
	{ 0x3fec048f, "sg_next" },
	{ 0x39f09a18, "device_create_file" },
	{ 0xf432dd3d, "__init_waitqueue_head" },
	{ 0x42c8de35, "ioremap_nocache" },
	{ 0x1fedf0f4, "__request_region" },
	{ 0xa5a7ba32, "dma_supported" },
	{ 0xc4556ebd, "cdev_add" },
	{ 0x1606bd6d, "cdev_init" },
	{ 0xc5ac94d0, "dev_set_drvdata" },
	{ 0x91715312, "sprintf" },
	{ 0x783c7933, "kmem_cache_alloc_trace" },
	{ 0x352091e6, "kmalloc_caches" },
	{ 0xe28af57f, "pci_set_master" },
	{ 0xe3e5d565, "pci_enable_device" },
	{ 0x37a0cba, "kfree" },
	{ 0x71e3cecb, "up" },
	{ 0x68aca4ad, "down" },
	{ 0x1d26dbf2, "cdev_del" },
	{ 0x1f3ca42, "pci_disable_device" },
	{ 0xd9ecfa2a, "device_remove_file" },
	{ 0x69a358a6, "iomem_resource" },
	{ 0x7c61340c, "__release_region" },
	{ 0xff7559e4, "ioport_resource" },
	{ 0xedc03953, "iounmap" },
	{ 0x436c2179, "iowrite32" },
	{ 0xcf21d241, "__wake_up" },
	{ 0xe484e35f, "ioread32" },
	{ 0x2072ee9b, "request_threaded_irq" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x27e1a049, "printk" },
	{ 0xa3a2bd36, "fasync_helper" },
	{ 0x70c55916, "dev_get_drvdata" },
	{ 0x28318305, "snprintf" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "44D857487AC2CA7F546D845");
