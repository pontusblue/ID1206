#include <unistd.h>

#define COLUMNS 80
#define ROWS 25

char *name = "Pontus cool bluescreen OS";

typedef struct vga_char {
	char character;
	char colors;
} vga_char;

void main(void) {

	// The VGA terminal mapped memory
	vga_char *vga = (vga_char*)0xb8000;

	for(int i = 0; i < COLUMNS*ROWS; i++) {
		vga[i].character = ' ';
		vga[i].colors = 0x1f;
	}

	for(int i = 0; name[i] != '\0'; i++) {
		vga[600+i].character = name[i];
		vga[600+i].colors = 0x1f;
	}
	int n = 0;
	int c = 0;
	while(1) {
		for(int i = 0; i < COLUMNS*ROWS; i++) {
			vga[i].character = ' ' + n;
			n = (n + 1) % (0x7e - ' ');
			vga[i].colors = c;
			c = (c + 1) % (0xff);
			sleep(1);
		}
	}
	return;
}
