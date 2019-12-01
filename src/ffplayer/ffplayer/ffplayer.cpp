#include <string>
#include <iostream>
#include <ffmpeg/FFPlayer.hpp>

using namespace std;

int main(int argc, char* argv[])
{
    FFPlayer player;
	player.setOnError([](const void *obj, int code, const string msg){
		printf("\nError - %s \n", msg.c_str());
	});
	player.setOnEOF([](const void* obj){
		printf("\n재생이 끝났습니다. \n");
	});

	while (true) {
		string line;
		printf("(o)pen, (p)lay, p(a)use, (c)lose, (m)ove to 10 sec, (q)uit: ");
		getline(cin, line);

		if (line == "o") player.open("D:/Work/test.mp4");
		if (line == "p") player.play();
		if (line == "a") player.pause();
		if (line == "c") player.close();
		if (line == "m") player.move(16 * 1000);

		if (line == "q") break;
	}

	player.close();

	return 0;
}
