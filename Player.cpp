#include "Player.h"
//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.
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
	int j = (cardl[0].job == i) + (cardl[1].job == i);
	return j;
} //플레이어가 카드를 가지고 있는지 확인

void Player::exchgeCard(int o, int news) {
	if (cardl[0].job == o)
		cardl[0].setJob(news);
	else if (cardl[1].job == o)
		cardl[1].setJob(news);
	else
		cout << "\nerror in exchangecard\n";
} //기존의 o 카드를 새 news 카드로 교체

void Player::coup(Player (& plr)[4]) {
	int cou;
	if (!type) { //플레이어 인간
		do {
			cout << "누구에게 쿠를 시도하시겠습니까?\n선택 : ";
			cin >> cou;
			switch (cou) {
			case 1:
			case 2:
			case 3: plr[cou].openrand(); break;
			default: continue; //invalid input
			} break;
		} while (1);
		coinplus(-7);
		cout << "플레이어가 컴퓨터 " << cou
			<< "에게 쿠를 시도합니다. 카드를 한 장 오픈합니다.";
	}
	else { //컴퓨터
		do {
			cou = prob(3); //랜덤 쿠 대상
			if (cou == type)
				cou = 3;
		} while (plr[cou].dead()); //죽은사람 쿠 금지
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


