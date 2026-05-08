#pragma once
#include<iostream>
//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.
using namespace std;
class Card
{
private:
	const string du;
	const string as;
	const string ca;
	const string co;
public:
	string name;
	int job;
	int open;
	Card() : du("   Duke "), as("Assassin"), ca(" Captain"), co("Contessa")
	{
		job = 4;
		open = 0;
	}
	void setJob(int);
};

