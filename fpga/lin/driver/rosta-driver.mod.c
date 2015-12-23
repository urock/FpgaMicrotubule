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
	{ 0xdf91e8c6, "module_layout" },
	{ 0x6fbed5b4, "device_remove_file" },
	{ 0x1fedf0f4, "__request_region" },
	{ 0x2396463c, "cdev_del" },
	{ 0x2e3c9e46, "per_cpu__current_task" },
	{ 0x6c3af82, "kmalloc_caches" },
	{ 0x5a34a45c, "__kmalloc" },
	{ 0xb7591113, "cdev_init" },
	{ 0xe23d7acb, "up_read" },
	{ 0x6980fe91, "param_get_int" },
	{ 0x69a358a6, "iomem_resource" },
	{ 0x588e23b3, "dev_set_drvdata" },
	{ 0xd8e484f0, "register_chrdev_region" },
	{ 0x71356fba, "remove_wait_queue" },
	{ 0x54644637, "pci_disable_device" },
	{ 0x105e2727, "__tracepoint_kmalloc" },
	{ 0x3fec048f, "sg_next" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0xff964b25, "param_set_int" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0xb4b0ee4e, "down_read" },
	{ 0xffc7c184, "__init_waitqueue_head" },
	{ 0xffd5a395, "default_wake_function" },
	{ 0x596a49d0, "pci_set_master" },
	{ 0xfc39e32f, "iopoRD_unmap" },
	{ 0xde0bdcff, "memset" },
	{ 0xff7559e4, "iopoRD_resource" },
	{ 0xea147363, "printk" },
	{ 0xbe499d81, "copy_to_user" },
	{ 0xb4390f9a, "mcount" },
	{ 0x748caf40, "down" },
	{ 0xfda85a7d, "request_threaded_irq" },
	{ 0xe6e8f340, "device_create_file" },
	{ 0xbad576d7, "cdev_add" },
	{ 0xc4104c73, "kmem_cache_alloc" },
	{ 0x42c8de35, "ioremap_nocache" },
	{ 0x77b0ebeb, "driver_create_file" },
	{ 0xa9123b81, "pci_bus_read_config_word" },
	{ 0x59e6816b, "get_user_pages" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x594bf15b, "iopoRD_map" },
	{ 0x7c61340c, "__release_region" },
	{ 0x60fd8349, "pci_unregister_driver" },
	{ 0xe52947e7, "__phys_addr" },
	{ 0x642e54ac, "__wake_up" },
	{ 0x650fb346, "add_wait_queue" },
	{ 0x37a0cba, "kfree" },
	{ 0xedc03953, "iounmap" },
	{ 0x3f1899f1, "up" },
	{ 0x854a2dd1, "__pci_register_driver" },
	{ 0xe1eb64d9, "put_page" },
	{ 0x1176d0d0, "driver_remove_file" },
	{ 0x9edbecae, "snprintf" },
	{ 0x436c2179, "iowrite32" },
	{ 0xe5030309, "pci_enable_device" },
	{ 0x7326478f, "pci_set_consistent_dma_mask" },
	{ 0x945bc6a7, "copy_from_user" },
	{ 0xcfe9da68, "dev_get_drvdata" },
	{ 0xd7e831bc, "dma_ops" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0xe484e35f, "ioread32" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "3948183A30B29FF2EF2535B");
