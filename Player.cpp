#include "Player.h"
#include "Input.h"
#include <cstdlib>
#include <iostream>

//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.

using namespace std;

int Player::deadcnt = 0; //전체 게임에서 탈락한 참가자 수

void Player::print()
{
	if (type) {
		cout << "컴퓨터 " << type << " : ";
		cout << "|  " << (cardl[0].open ? cardl[0].name : "  ????  ") << " (" << (cardl[0].open ? "o" : "c") << ") |   ";
		cout << "|  " << (cardl[1].open ? cardl[1].name : "  ????  ") << " (" << (cardl[1].open ? "o" : "c") << ") |" << endl;
		//cout << "|  " << cardl[0].name << " (" << (cardl[0].open ? "o" : "c") << ") |   ";
		//cout << "|  " << cardl[1].name << " (" << (cardl[1].open ? "o" : "c") << ") |" << endl;
	}
	else {
		cout << "플레이어 : ";
		cout << "|  " << cardl[0].name << " (" << (cardl[0].open ? "o" : "c") << ") |   ";
		cout << "|  " << cardl[1].name << " (" << (cardl[1].open ? "o" : "c") << ") |" << endl;
	}
}

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

void Player::openrand()
{
	int rando = rand() % 2;
	if (die)
		return; //죽은상태라면 리턴

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

		cout << "\n------------------------------------------\n";
		if (type)
			cout << "컴퓨터" << type;
		else
			cout << "플레이어";
		cout << "가 패배하여 게임에서 제외됩니다.\n"
			<< "------------------------------------------\n";
	}
} //카드를 랜덤으로 오픈

int Player::cardhave(int i) {
	int j = (cardl[0].job == i && !cardl[0].open) + (cardl[1].job == i && !cardl[1].open);
	return j;
} //플레이어가 공개되지 않은 활성 카드를 가지고 있는지 확인

void Player::exchgeCard(int o, int news) {
	if (cardl[0].job == o && !cardl[0].open)
		cardl[0].setJob(news);
	else if (cardl[1].job == o && !cardl[1].open)
		cardl[1].setJob(news);
	else
		cout << "\nerror in exchangecard\n";
} //기존의 o 카드를 새 news 카드로 교체

void Player::coup(Player (& plr)[4]) {
	int cou;
	if (!type) { //플레이어 인간
		do {
			cou = Input::readInt("누구에게 쿠를 시도하시겠습니까?\n선택 : ");
			if (cou < 1 || cou > 3) {
				cout << "1~3번 컴퓨터 중에서 다시 선택하세요.\n";
				continue;
			}
			if (plr[cou].dead()) {
				cout << "이미 탈락한 대상입니다. 다시 선택하세요.\n";
				continue;
			}
			plr[cou].openrand();
			break;
		} while (1);
		coinplus(-7);
		cout << "플레이어가 컴퓨터 " << cou
			<< "에게 쿠를 시도합니다. 카드를 한 장 오픈합니다.";
	}
	else { //컴퓨터
		do {
			cou = prob(4); //0~3번 참가자 중 자기 자신을 제외하고 선택
		} while (cou == type || plr[cou].dead()); //자기 자신 또는 탈락자는 대상에서 제외
		plr[cou].openrand();
		coinplus(-7);
		if (cou) {
			cout << "컴퓨터가 컴퓨터 " << cou
				<< "에게 쿠를 시도합니다. 카드를 한 장 오픈합니다.";
		}
		else {
			cout << "컴퓨터가 플레이어 "
				<< "에게 쿠를 시도합니다. 카드를 한 장 오픈합니다.";
		}
	}
	return;
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
