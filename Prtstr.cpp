#include "Prtstr.h"
#include <fstream>
#include <iostream>
#include <sstream>

//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.

void Prtstr::prt()
{
	std::cout << document;
}

std::string Prtstr::loadText(const std::string& path, const std::string& fallback)
{
	std::ifstream file(path);
	if (!file) {
		return fallback;
	}

	std::ostringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

Prtstr::Prtstr(int a)
{
	switch (a) {
	case 0:
		document = loadText("assets/text/menu.txt", R"(================================================
1. 게임 설명
2. 게임 시작
3. 종료
================================================
선택 : )");
		break;
	case 1:
		document = loadText("assets/text/manual.txt", R"(레지스탕스 쿠는 참가자들이 다양한 역할군의 카드들과 코인을 사용하여, 추리와 거짓말을 활용한 보드게임이다.
각 참가자는 일반 행동과 캐릭터 행동 중 하나를 고를 수 있으며, 어떤 능력을 사용할 지 결정할 수 있다.
참가자는 거짓말로 보유하고 있지 않은 캐릭터의 능력을 사용할 수 있으며, 타 참가자는 언제든지 도전할 수 있다.
거짓말이 들통나면 카드를 한 장 오픈하고, 진실이면 도전한 상대가 카드를 한 장 오픈한다.
카드 2장이 모두 오픈된 참가자는 게임에서 제외되며, 한 명이 남을 때까지 게임을 계속한다.
적절한 블러핑과 상대의 수를 간파하여 이 게임에서 최종 승자가 되어 보세요!

)");
		break;
	default:
		break;
	}
}
