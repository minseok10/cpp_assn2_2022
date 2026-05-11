#pragma once
#include <string>

//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.

// 메뉴와 설명서처럼 고정된 긴 문자열을 보관하고 출력한다.
class Prtstr
{
private:
	std::string document;
	static std::string loadText(const std::string& path, const std::string& fallback);
public:
	void prt();
	Prtstr(int a);
};
