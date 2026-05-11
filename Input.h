#pragma once

#include <iostream>
#include <limits>
#include <string>

// 콘솔 입력을 안전하게 처리하기 위한 작은 유틸리티 모음이다.
// 잘못된 입력이 들어오면 스트림 상태를 복구하고 다시 입력받는다.
namespace Input
{
	inline void clearLine()
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	inline int readInt(const std::string& prompt)
	{
		int value;
		while (true) {
			std::cout << prompt;
			if (std::cin >> value) {
				clearLine();
				return value;
			}

			std::cout << "숫자를 입력해주세요.\n";
			clearLine();
		}
	}

	inline char readYesNo(const std::string& prompt)
	{
		char value;
		while (true) {
			std::cout << prompt;
			if (std::cin >> value) {
				clearLine();
				if (value == 'y' || value == 'Y') {
					return 'y';
				}
				if (value == 'n' || value == 'N') {
					return 'n';
				}
			}

			std::cout << "y 또는 n으로 입력해주세요.\n";
			clearLine();
		}
	}

	inline void waitForEnter(const std::string& prompt)
	{
		std::cout << prompt;
		std::string unused;
		std::getline(std::cin, unused);
	}
}
