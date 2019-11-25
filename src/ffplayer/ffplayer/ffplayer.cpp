#include <string>
#include <iostream>
#include "FFPlayer.hpp"

using namespace std;

int main()
{
    FFPlayer player;
	player.open("D:/Work/test.mkv");	

	while (true) {
		string line;
		printf("(p)lay, p(a)use, (q)uit: ");
		getline(cin, line);

		if (line == "P") player.play();
		if (line == "a") player.pause();

		if (line == "q") break;
	}

	player.close();
}
