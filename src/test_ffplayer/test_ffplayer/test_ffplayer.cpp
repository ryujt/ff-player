#include <string>
#include <iostream>
#include "FFPlayer.hpp"

int main()
{
    FFPlayer player;
	player.open("D:/Work/test.mp4");

	while (true) {
		printf("TASK: ");
		string line;
		getline(cin, line);

		if (line == "c") player.close();
		if (line == "p") player.play();
		if (line == "a") player.pause();
	}
}
