#include <string>
#include <iostream>
#include "FFPlayer.hpp"

int main()
{
    FFPlayer player;

	while (true) {
		printf("TASK: ");
		string line;
		getline(cin, line);

		if (line == "o") player.open("D:/Work/test.mp4");
		if (line == "c") player.close();
		if (line == "p") player.play();
		if (line == "a") player.pause();
	}
}
