#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_blank()
{
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	return;
}

void terminal_blank_line(uint16_t *line_ptr)
{
	for(size_t x = 0; x < VGA_WIDTH; x++){
		line_ptr[x] = 0xFF00u;
	}
	return;
}

size_t terminal_line_copy(uint16_t* from, uint16_t* dest, size_t len)
{
	size_t i = 0;
	while(i < len){
		dest[i] = from[i];
		i++;
	}
	return i;
}
 
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll_newline()
{
	for(int i=0; i<VGA_HEIGHT-1; i++){
		terminal_blank_line(&terminal_buffer[i*VGA_WIDTH]);		
		terminal_line_copy(&terminal_buffer[(i+1)*VGA_WIDTH], &terminal_buffer[i*VGA_WIDTH], VGA_WIDTH);
	}
}

void terminal_putchar(char c) 
{
	if(c == '\n'){
		if (++terminal_row == VGA_HEIGHT){
			terminal_row = VGA_HEIGHT-1;
			terminal_scroll_newline();
		}
		terminal_column = 0;
		return;
	}

	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if(++terminal_row == VGA_HEIGHT){
			terminal_row = VGA_HEIGHT-1;
			terminal_scroll_newline();
		}

	}
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void time_delay(size_t s)
{
	for(unsigned long i=0; i<s*100000; i++){
		;
	}
	return;
}
 
void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
	/* Newline support is left as an exercise. */
	for(int i=0; i<VGA_HEIGHT; i++){
		terminal_writestring("MYOS KERNEL VER 0.0\nBUILD DATE 0416221428\n\nHello from kernel world!\n");
		time_delay(1000);
	}
	for(int i=0; i<VGA_HEIGHT/2; i++){
		terminal_writestring("Wrapping the screen\n");
		time_delay(1000);
	}
}