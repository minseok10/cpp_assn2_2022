#pragma once
#include "Card.h"
#include "randoms.h"
#include <string>

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
	void reset(int type_); //새 게임 상태로 초기화
	void setCard(int, int); //카드 초기세팅
	int playerType() const { return type; }
	int coins() const { return coin; }
	void coinplus(int a) { coin += a; }
	int takeCoins(int amount); //최대 amount개까지 코인을 빼앗김
	const std::string& cardName(int index) const;
	int cardOpen(int index) const;
	bool openrand(); //랜덤으로 카드를 공개하고 탈락 여부를 반환
	int cardhave(int); //카드를 가지고 있는가?
	bool exchgeCard(int o, int news); //카드 교환
	bool coup(int target, Player(&plr)[4]); //쿠 실행
	int dead() const { return die; }
	int botherwonjo(); //원조방해 10% 여부
};
