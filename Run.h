#pragma once
#include "Player.h"
#include "Deck.h"

//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.

class Run
{
private:
	Player plr[4] = { Player(0), Player(1), Player(2) ,Player(3) };
	Deck deck;
	void usrturn();
	void pcturn(int);
	void dispCard(); //카드목록표시
	void dispPlayerCard(int); //참가자 카드 표시
	void openRandomCard(int); //참가자 카드 공개
	int readCoupTarget(); //사용자 쿠 대상 입력
	void executeCoup(int, int); //쿠 실행 및 표시
	int p20p(int); //20%확률로 돌아가며 대상자 추출
	int chall(int, int, int); //도전하기 challenge
	void shuffle(int(&shuff)[12]); //초기 카드섞기
	int randomOpponent(int self); //살아 있는 상대 중 한 명을 무작위로 선택
public:
	void print(); //초기설명서 출력
	void turns(); //게임 진행
};

