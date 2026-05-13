#pragma once
#include <string>

//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.

// 쿠 게임에서 사용되는 영향력 카드 한 장을 표현한다.
// job: 0=공작, 1=암살자, 2=사령관, 3=백작부인, 4=미설정
// open: 0=비공개, 1=공개
class Card
{
private:
	const std::string du;
	const std::string as;
	const std::string ca;
	const std::string co;
public:
	std::string name;
	int job;
	int open;

	Card() : du("공작"), as("암살자"), ca("사령관"), co("백작부인")
	{
		job = 4;
		open = 0;
	}
	void setJob(int);
};
