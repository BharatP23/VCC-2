#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x522e4321, "proc_create" },
	{ 0x92997ed8, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x5f56c2fd, "single_open" },
	{ 0x7278d328, "all_vm_events" },
	{ 0x299f7cc0, "seq_printf" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0x91c6cf8b, "remove_proc_entry" },
	{ 0x752283b8, "seq_read" },
	{ 0x12a959d4, "seq_lseek" },
	{ 0xaa5cb1ca, "single_release" },
	{ 0x517f61ad, "module_layout" },
};

MODULE_INFO(depends, "");

