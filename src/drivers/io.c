#include <drivers/io.h>

uint8_t inb(uint16_t port){

	uint8_t data;
	asm volatile("inb %1, %0":"=a"(data):"Nd"(port):"memory");

	return data;
}

void outb(uint16_t port, uint8_t data){

	asm volatile("outb %0, %1": :"a"(data),"Nd"(port):"memory");

}

void outb_slow(uint16_t port, uint8_t data){

	outb(port, data);
	asm volatile("jmp 1f\n1: jmp 1f\n1:");

}

uint16_t inw(uint16_t port){

	uint16_t data;
	asm volatile("inw %1, %0":"=a"(data):"Nd"(port):"memory");
	return data;

}

void outw(uint16_t port, uint16_t data){

	asm volatile("outw %0, %1": :"a"(data),"Nd"(port):"memory");

}

uint32_t inl(uint16_t port){

	uint32_t data;
	asm volatile("inl %1, %0":"=a"(data):"Nd"(port):"memory");
	return data;

}

void outl(uint16_t port, uint32_t data){

	asm volatile("outl %0, %1": :"a"(data),"Nd"(port):"memory");

}

void insw(uint16_t port, void* buffer, uint32_t c){

	asm volatile("cld; rep; insw":"+D"(buffer), "+c"(c):"d"(port));

}

void outsw(uint16_t port, void* buffer, uint32_t c){

	asm volatile("cld; rep; outsw":"+S"(buffer), "+c"(c):"d"(port));

}
