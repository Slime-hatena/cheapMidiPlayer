#include <windows.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "winmm")

#define VK_KEY_O 0x4F
#define VK_KEY_P 0x50
#define VK_KEY_Q 0x51

typedef enum {
	STOP,
	PLAY,
	PAUSE
}State;

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

class MidiPlayer {
	MCI_OPEN_PARMS mop;
	std::string title;
	State state = STOP;

	void play() {
		state = PLAY;
		mciSendCommand(mop.wDeviceID, MCI_PLAY, 0, 0);
		system("cls");
		std::cout << "再生: " << title << std::endl;
		std::cout << "[p:一時停止 o:楽曲変更 q: 終了]" << std::endl;
	}

	void pause() {
		state = PAUSE;
		mciSendCommand(mop.wDeviceID, MCI_PAUSE, 0, 0);
		system("cls");
		std::cout << "停止: " << title << std::endl;
		std::cout << "[p:再生 o:楽曲変更 q: 終了]" << std::endl;
	}

public:
	MidiPlayer() {
		change();
	}

	~MidiPlayer() {
		mciSendCommand(mop.wDeviceID, MCI_CLOSE, 0, 0);
	}

	void change() {
		std::string filePath = "";
		std::wstring stemp;

		while (true) {
			std::cout << "再生したいmidiファイルをフルパスで指定: ";
			std::cin >> filePath;
			stemp = s2ws(filePath);
			LPCWSTR lstr = stemp.c_str();

			if (PathFileExists(lstr)) {
				int path_i = filePath.find_last_of("\\") + 1;
				int ext_i = filePath.find_last_of(".");

				if (filePath.substr(ext_i, filePath.size() - ext_i) == ".mid") {
					title = filePath.substr(path_i, ext_i - path_i);
					break;
				}
				else {
					std::cout << "ファイルの種類が間違っています。読み込めるファイルは*.midです。\n\n";
				}
			}
			else {
				std::cout << "指定されたパスにファイルが見つかりませんでした。\n\n";
			}
		}

		mop.lpstrDeviceType = L"Sequencer";
		mop.lpstrElementName = stemp.c_str();
		mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)&mop);
		mciSendCommand(mop.wDeviceID, MCI_PLAY, 0, 0);
		play();
	}

	void togglePlayAndPause() {
		if (state == PLAY) {
			pause();
		}
		else {
			play();
		}
	}
};

int main(int argc, char *argv[]) {
	 std::unique_ptr<MidiPlayer> midi(new MidiPlayer());

	while (true) {
		if (GetAsyncKeyState(VK_KEY_P)) {
			midi->togglePlayAndPause();
		}

		if (GetAsyncKeyState(VK_KEY_O)) {
				midi->change();
		}

		if (GetAsyncKeyState(VK_KEY_Q)) {
			break;
		}
		Sleep(100);
	}
	
	return 0;
}