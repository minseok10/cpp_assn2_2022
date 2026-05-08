#pragma once
#include "Card.h"

//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.

// 남은 카드 더미를 관리한다. 도전 성공/실패 후 공개되지 않은 카드를
// 덱에 되돌리고 새 카드를 뽑는 동작을 담당한다.
class Deck
{
private:
	Card cardl[4];
public:
	Deck() = default;
	void setCard(int, int, int, int);
	int shuffle(int); //기존 카드를 덱에 넣고 꺼내기
};

