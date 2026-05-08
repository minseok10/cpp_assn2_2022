#include "randoms.h"
#include <cstdlib>

//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.

int prob(int a)
{
	if (a <= 0) {
		return 0;
	}

	int temp = rand() % a;

	return temp;
}

