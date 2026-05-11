#include "Player.h"
#include <cstdlib>

//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.

int Player::deadcnt = 0; //전체 게임에서 탈락한 참가자 수

void Player::setCard(int a, int b)
{
	cardl[0].setJob(a);
	cardl[1].setJob(b);
} //카드세팅

int Player::takeCoins(int amount)
{
	if (amount <= 0) {
		return 0;
	}

	int taken = coin < amount ? coin : amount;
	coin -= taken;
	return taken;
} //강탈 시 코인이 음수가 되지 않도록 실제 빼앗긴 코인 수를 반환

const std::string& Player::cardName(int index) const
{
	return cardl[index].name;
}

int Player::cardOpen(int index) const
{
	return cardl[index].open;
}

bool Player::openrand()
{
	int rando = rand() % 2;
	if (die)
		return false; //죽은상태라면 리턴

	if (rando) { //2개의 카드중 랜덤으로 오픈
		if (!cardl[0].open)
			cardl[0].open = 1;
		else if (!cardl[1].open)
			cardl[1].open = 1;
	}
	else {
		if (!cardl[1].open)
			cardl[1].open = 1;
		else if (!cardl[0].open)
			cardl[0].open = 1;
	}
	if (cardl[0].open && cardl[1].open) {
		die = 1;
		deadcnt++; //static 변수, 사망자수
		return true;
	}

	return false;
} //카드를 랜덤으로 오픈

int Player::cardhave(int i) {
	int j = (cardl[0].job == i && !cardl[0].open) + (cardl[1].job == i && !cardl[1].open);
	return j;
} //플레이어가 공개되지 않은 활성 카드를 가지고 있는지 확인

bool Player::exchgeCard(int o, int news) {
	if (cardl[0].job == o && !cardl[0].open)
		cardl[0].setJob(news);
	else if (cardl[1].job == o && !cardl[1].open)
		cardl[1].setJob(news);
	else
		return false;

	return true;
} //기존의 o 카드를 새 news 카드로 교체

bool Player::coup(int target, Player (& plr)[4]) {
	bool defeated = plr[target].openrand();
	coinplus(-7);
	return defeated;
}// 쿠 시도

int Player::botherwonjo() {
	int a;
	if (die)
		return 0; //죽은사람은 방해 못함

	if (cardhave(0)) //방해공작 소지
		a = 1;
	else if (!prob(10)) //10% 확률
		a = 1;
	else
		a = 0;
	return a;
} //방해 여부 리턴
