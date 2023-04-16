/*
    This file changes pixel colors.
    It writes directly to the framebuffer
    You most likely need root privileges to run this.
    If this is not working, switch to one of TTYs,
    using keys Alt+Shift+{0,1,2,...,9}

    Warning: You may stuck in TTY, save all your unsaved files,
    if there's any.
*/

#include <stdlib.h>
#include <linux/kd.h>
#include <signal.h>
#include <linux/vt.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <math.h>
#include <termios.h>

int main()
{
	int fbfd;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	struct vt_mode VT;

	fbfd = open("/dev/fb0", O_RDWR);
	int tty_fd = open(ttyname(STDIN_FILENO), O_RDWR);
	ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);
	ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);

	long int screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	printf("%d\n", screensize);

	char* fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

	const int tmp_save1 = vinfo.bits_per_pixel/8; // * finfo.line_length;
	const int tmp_save2 = finfo.line_length;
	const int tmp_save3 = vinfo.yoffset;
	const int tmp_save4 = vinfo.xoffset;

	const int positionx = 500;
	const int positiony = 300;

	void DrawPixel(int x, int y, int b, int g, int r)
	{
		const int location = (x+tmp_save4) * tmp_save1 + (y+tmp_save3) * tmp_save2;
		*(fbp + location) = r;
		*(fbp + location + 1) = g;
		*(fbp + location + 2) = b;
	}

	void DrawRectangle(int x1, int y1, int x2, int y2, int r, int g, int b)
	{
		for (int x = x1; x <= x2; x++)
		{
			for (int y = y1; y <= y2; y++)
			{
				DrawPixel(x, y, r, g, b);
			}
		}
	}

	int new_vt_num;

	ioctl(tty_fd, VT_OPENQRY, &new_vt_num);
	ioctl(tty_fd, VT_ACTIVATE, new_vt_num);

	ioctl(tty_fd, VT_GETMODE, &VT);

	VT.mode = VT_PROCESS;
	VT.relsig = SIGUSR1;
	VT.acqsig = SIGUSR1;

	/*
	    For some reason it doesn't work,
	    I've tried everything, but it wont let me
	    hide the blinking cursor at the top of the left corner.
	    Please help me if you know how to fix it
	*/
	ioctl(tty_fd, VT_SETMODE, &VT);
	ioctl(tty_fd, KDSETMODE, KD_GRAPHICS);
	ioctl(tty_fd, VT_WAITACTIVE, new_vt_num);

	sleep(0.1);
	
	DrawRectangle(0, 1050, 1919, 1079, 255, 255, 255);
	DrawRectangle(0, 0, 1919, 1050, 255, 0, 0);

	sleep(3);

   	// Switch back to text mode
	ioctl(tty_fd, KDSKBMODE, 1);
	ioctl(tty_fd, KDSETMODE, KD_TEXT);

	//Deallocate the virtual terminal
	ioctl(tty_fd, VT_DISALLOCATE, new_vt_num);
	close(tty_fd);

	munmap(fbp, screensize);
	close(fbfd);
	return 0;
}
