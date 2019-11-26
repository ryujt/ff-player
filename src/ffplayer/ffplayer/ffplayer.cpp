#include <string>
#include <iostream>
#include "FFPlayer.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    FFPlayer player;
	player.open("D:/Work/test.mp4");	

	while (true) {
		string line;
		printf("(p)lay, p(a)use, (q)uit: ");
		getline(cin, line);

		if (line == "p") player.play();
		if (line == "a") player.pause();

		if (line == "q") break;
	}

	player.close();

	return 0;
}
