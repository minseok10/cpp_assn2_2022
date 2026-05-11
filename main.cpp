#include "Prtstr.h"
#include "Run.h"
#include "Input.h"
#include <iostream>

//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.

using namespace std;

int callrun();

int main()
{
	Prtstr menu(0);
	Prtstr manual(1);

	cout << R"(================================================
|                                              |
|   레지스탕스 쿠 게임에 오신 걸 환영합니다!!  |
|                                              |
)";
	int end = 0;
	while (!end) {
		menu.prt();
		int choice = Input::readInt("");
		switch (choice) {
		case 1:
			manual.prt(); 
			break;
		case 2:
			callrun();
			end = 1;
			break;
		case 3:
			end = 1;
			break;
		default:
			cout << "다시 선택하세요\n" << endl;
			break;
		}
	}
	cout << "게임을 종료합니다.\n";
	
	return 0;
}

int callrun()
{
	Run session;
	session.print(); //초기 세팅 메시지
	session.turns(); //게임 실행

	return 0;
}

