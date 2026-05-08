#pragma once
#include "Card.h"
#include "randoms.h"

//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.

class Player
{
private:
	int coin;
	int type; //0:인간, 1~3:컴퓨터
	int die;
	Card cardl[2];
public:
	static int deadcnt; //총사망자수
	Player(int type_) : coin(2), type(type_), die(0) {}
	void print(); //카드 내용 출력
	void setCard(int, int); //카드 초기세팅
	int coins() const { return coin; }
	void coinplus(int a) { coin += a; }
	void openrand(); //랜덤으로 카드를 공개
	int cardhave(int); //카드를 가지고 있는가?
	void exchgeCard(int o, int news); //카드 교환
	void coup(Player(&plr)[4]); //쿠 실행
	int dead() const { return die; }
	int botherwonjo(); //원조방해 10% 여부
};


