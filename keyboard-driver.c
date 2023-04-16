// You need root to run this.

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>


void ReadKey(int Length, char Buffer[], int *Shift, int *LastKey)
{
	if (Length != 48) { return ; }
	int Key = Buffer[42];

	char Translation[256] = {
		'0' , '0', '1', '2', '3', '4', '5', '6',
		'7' , '8', '9', '0', '-', '=', '0', '0',
		'q' , 'w', 'e', 'r', 't', 'y', 'u', 'i',
		'o' , 'p', '[', ']', '0', '0', 'a', 's',
		'd' , 'f', 'g', 'h', 'j', 'k', 'l', ';',
		'\'', '0', '0', '0', 'z', 'x', 'c', 'v',
		'b' , 'n', 'm', ',', '.', '/', '0', '0',
		'0' , ' '
	};
	int IsKeyDown = Buffer[44];

	if (IsKeyDown == 1) {
		if (Key == 0) { Key = *LastKey; }
		else { *LastKey = Key; }
		if (Key == 42) { *Shift == 0 ? (*Shift)++ : (*Shift)-- ; }
	}

	int Translated = Translation[Key];

	if (*Shift == 1 && (Translated >= 97 && Translated <= 127))
	{
		Translated = Translated - 32;
	}

	if (IsKeyDown == 1)
		printf("KeyPressed: %hhc %hhd %hhd [Shift status: %d]\n", Translated, Key, IsKeyDown, *Shift);
	else {
		printf("KeyReleased: %hhc %hhd %hhd [Shift status: %d]\n", Translated, Key, IsKeyDown, *Shift);
	}
}

int HandleKeyboard()
{
	// Warning: This is my keyboard, it will not work for your keyboard.
	// I'll experiment with various devices to find an all-in-one solution.
	int File = open("/dev/input/by-id/usb-0416_Gaming_Keyboard-event-kbd", O_RDWR);

	char Buffer[48];
	long Length;

	int Shift;
	int LastKey;
	int Key;

	while ((Length=read(File, Buffer, sizeof Buffer)))
		ReadKey(Length, Buffer, &Shift, &LastKey);

	printf("Exiting from the loop\n");
	close(File);
}
void main()
{
	HandleKeyboard();
}
