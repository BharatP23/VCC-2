#include <stddef.h>
#include <stdint.h>

static char call1[100];
static char call2[100];
int flag=0;
static void outb(uint16_t port, uint8_t value) {
	asm("outb %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}
void HC_print8bit(uint8_t val)
{
	outb(0xE9, val);
}
static void outl(uint16_t port, uint32_t value) {
    asm("outl %0,%1" : /* No output */ : "a" (value), "Nd" (port));
}
void HC_print32bit(uint32_t val)
{
	outl(0xE6,val);
}
static inline uint32_t inb(uint16_t port) {
  uint32_t return_value;
  asm("in %1, %0" : "=a"(return_value) : "Nd"(port) : "memory" );
  return return_value;
}
uint32_t HC_numExits()
{
	unsigned int count=inb(0xE2);
	return count;	
}

void HC_printStr(char *str)
{
	uintptr_t string_address = (uintptr_t)str;
	outl(0xE1,string_address);
}

char *HC_numExitsByType()
{
	uintptr_t string_address1,string_address2;
	if(flag==0)
	{
		string_address1=(uintptr_t)call1;
		flag=1;
		outl(0xF5,string_address1);
		return call1;
	}	
	string_address2=(uintptr_t)call2;
	flag=0;
	outl(0xF5,(string_address2));
	return call2;
}

uint32_t HC_gvaToHva(uint32_t gva)
{
	outl(0xF1,gva);
	unsigned int address_value=inb(0xF2);
	return address_value;
}

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p;
	for (p = "Hello 695!\n"; *p; ++p)
		HC_print8bit(*p);


	/*----------Don't modify this section. We will use grading script---------*/
	/*---Your submission will fail the testcases if you modify this section---*/
	HC_print32bit(2048);
	HC_print32bit(4294967295);

	uint32_t num_exits_a, num_exits_b;
	num_exits_a = HC_numExits();

	char *str = "CS695 Assignment 2\n";
	HC_printStr(str);

	num_exits_b = HC_numExits();

	HC_print32bit(num_exits_a);
	HC_print32bit(num_exits_b);

	char *firststr = HC_numExitsByType();
	uint32_t hva;
	hva = HC_gvaToHva(1024);
	HC_print32bit(hva);
	hva = HC_gvaToHva(4294967295);
	HC_print32bit(hva);
	char *secondstr = HC_numExitsByType();

	HC_printStr(firststr);
	HC_printStr(secondstr);
	/*------------------------------------------------------------------------*/

	*(long *) 0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
