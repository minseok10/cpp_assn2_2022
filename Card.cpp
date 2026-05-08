#include "Card.h"
//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.
void Card::setJob(int j)
{
	job = j;
	switch (j) {
	case 0:
		name = du;
		break;
	case 1:
		name = as;
		break;
	case 2:
		name = ca;
		break;
	case 3:
		name = co;
		break;
	default:
		cout << "\nproblem, job out of range\n"; //에러
		break;
	}
}

