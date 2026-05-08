#include "Deck.h"
#include <cstdlib>

//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.

void Deck::setCard(int a, int b, int c, int d) {
	cardl[0].setJob(a);
	cardl[1].setJob(b);
	cardl[2].setJob(c);
	cardl[3].setJob(d);
}

int Deck::shuffle(int a) {
	int list[5] = { cardl[0].job,cardl[1].job, cardl[2].job, cardl[3].job,a };
	int rando = rand() % 5;
	if (rando != 4)
		cardl[rando].setJob(a);
	return list[rando]; //새로 받는 카드
} //기존 카드를 덱에 넣고 꺼내기


