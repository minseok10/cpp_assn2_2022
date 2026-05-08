#include "Run.h"
//나는 이 프로그래밍 과제를 다른 사람의 부적절한 도움 없이 완수하였습니다.
using namespace std;

void Run::print()
{
	cout << R"(게임에 참여하는 참가자는 플레이어와 컴퓨터 3명으로, 총 4명입니다.
덱을 생성하고 카드를 분배합니다.
참가자에게 카드 분배가 완료되었습니다.

게임을 시작합니다.
================================================
)";
} //초기안내문

void Run::shuffle(int (& shuff)[12]) {
	int a, b;
	int tmp;
	for (int i = 0; i < 100; i++) {
		a = rand() % 12;
		b = rand() % 12;
		tmp = shuff[a];
		shuff[a] = shuff[b];
		shuff[b] = tmp;
	}
} //초기 직업 랜덤섞기

void Run::turns()
{
	srand(time(NULL));
	int shuff[12] = { 0,0,0,1,1,1,2,2,2,3,3,3 };
	shuffle(shuff); //shuffule list randomly
	plr[0].setCard(shuff[0], shuff[1]);
	plr[1].setCard(shuff[2], shuff[3]);
	plr[2].setCard(shuff[4], shuff[5]);
	plr[3].setCard(shuff[6], shuff[7]);
	deck.setCard(shuff[8], shuff[9], shuff[10], shuff[11]);
	dispCard();
	for (int i = 0; Player::deadcnt < 3; i++) { //3명 죽으면 종료
		int j = i % 4;
		if (!plr[j].dead()) {
			if (j) {
				pcturn(j);
				dispCard();
			}
			else {
				usrturn();
				dispCard();
			}
		}
	}
	int win;
	for (win = 0; win < 4; win++) {
		if (!plr[win].dead())
			break;
	}
	cout << "게임이 끝났습니다. 승자는 ";
	if(win)
		cout<<"컴퓨터" << win << " 입니다 축하합니다\n";
	else
		cout << "플레이어 입니다 축하합니다\n";
}

void Run::dispCard()
{
	cout << "플레이어들의 카드는 다음과 같습니다.\n" <<
		"------------------------------------------------\n";
	plr[0].print();
	plr[1].print();
	plr[2].print();
	plr[3].print();
	cout << "------------------------------------------------\n";
}//카드 표시

int Run::p20p(int a) {
	int randi;
	
		if (!(rand() % 5)&& a!=1 && !plr[1].dead()) //20%possibility 도전
			randi = 1;
		else if (!(rand() % 5)&&a!=2 && !plr[2].dead())
			randi = 2;
		else if (!(rand() % 5)&&a!=3 && !plr[3].dead())
			randi = 3;
		else
			randi = a;
	return randi;
} //20% 확률로 도전자 정하기, 본인(a)이 리턴되면, 도전하지않음을 의미

int Run::chall(int out, int b, int in) {
	int honest;
	if (in) {
		cout << "컴퓨터" << in << "가 도전합니다.\n";
		cout << "컴퓨터" << in << "가 ";
	}
	else {
		cout << "플레이어가 도전합니다.\n";
		cout << "플레이어가 ";
	}

	if (plr[out].cardhave(b)) {

		cout << "도전에 실패하였습니다.카드를 한 장 오픈합니다.\n";
		plr[in].openrand();
		//deck shuffle exchange

		if (out)
			cout << "컴퓨터" << out << "은 거짓말을 하지 않았으므로,";
		else
			cout << "플레이어는 거짓말을 하지 않았으므로,";
		cout << "해당 카드를 덱에 포함시킨 후 한장을 새로 뽑습니다\n";
		plr[out].exchgeCard(b, deck.shuffle(b)); //deck과의 데이터 교환.
		honest = 1;
	}
	else {
		cout << "도전에 성공하였습니다. ";
		if (out) 
			cout << "컴퓨터" << out << "의 카드를 한 장 오픈합니다.\n";
		else
			cout << "플레이어의 카드를 한 장 오픈합니다.\n";

		plr[out].openrand();
		honest = 0;
	}
		
	return honest;
}
//challenge, out get challenged, b is target jobtype, in is challenger. return honesty.

void Run::usrturn()
{
	int randi;
	int a;
	int b;
	int passed1; //방해,도전 통과
	char yesi = 'n';;
	cout << "플레이어 턴입니다.\n";
	cout << "무슨 행동을 하시겠습니까? 현재 코인 : " << plr[0].coins() << "개\n";
	if (plr[0].coins() > 9) {
		cout << "코인이 10개 이상이므로, 자동으로 쿠를 시도합니다.\n";
		plr[0].coup(plr);
	}
	else {
		cout << R"(================================================
1. 일반행동
2. 캐릭터 행동
================================================
선택 : )";
		cin >> a;
		if (a == 1) {
			do {
				cout << "\n다음 중 시행할 행동을 고르시오. 현재 코인 : " << plr[0].coins() << "개\n";
				cout << R"(================================================
1. 소득
2. 해외 원조
3. 쿠(Coup)
================================================
선택 : )";
				cin >> b;
				switch (b) {
				case 1: //소득
					plr[0].coinplus(1);
					cout << "플레이어가 코인을 1개 얻습니다.\n";
					break;
				case 2: //원조
					cout << "플레이어가 해외 원조를 선택하였습니다.\n";
					if (plr[1].botherwonjo())
						randi = 1;
					else if (plr[2].botherwonjo())
						randi = 2;
					else if (plr[3].botherwonjo()) //10% 방해
						randi = 3;
					else
						randi = 0;

					switch (randi) {
					case 1:
					case 2:
					case 3:
						cout << "컴퓨터" << randi << "가 플레이어의 해외 원조를 막았습니다.\n";
						cout << "도전하시겠습니까?\n선택(y/n) :";
						cin >> yesi;
						if (yesi == 'y') {
							passed1 = !chall(randi, 0, 0); //방해에 도전
						}
						else
							passed1 = 0;
						break;
					default:
						cout << "아무도 방해 하지 않았습니다.";
						passed1 = 1;
						break;
					}
						if (passed1) {
							plr[0].coinplus(2);
							cout << "플레이어가 코인을 2개 얻습니다.\n";
						}
						else {
							cout << "플레이어가 코인을 얻지 못합니다.";
						}
					break; 
				case 3: //쿠
					if (plr[0].coins() < 7) {
						cout << "\n코인부족.코인7개필요\n";
						continue;
					}
					plr[0].coup(plr);
					break;
				default:
					continue;
				} break;
			} while (1);
		} //end ilban일반 behav.
		else if (a == 2) { //캐릭터행동
			int attack; //공격대상
			int randi2; //랜덤 결과 저장
			int passed2; //passed와 같은기능
			do {
				cout << "\n현재 보유 카드는 다음과 같습니다. 무슨 캐릭터의 행동을 시도하겠습니까?(공작: 0, 암살자: 1, 사령관: 2)\n";
				plr[0].print();
				cout << "선택: ";
				cin >> b;
				switch (b) {
				case 0: //duke
					if (!plr[0].cardhave(b))
						cout << "블러핑을 시도합니다.\n";
					randi = p20p(0); //도전자
					switch (randi) { 
					case 1:
					case 2:
					case 3:
						passed2 = chall(0, b,randi); //첫번째 카드도전
						break;
					case 0:
						cout << "아무도 도전하지 않았습니다.\n";
						passed2 = 1;
						break;
					default:
						cout << "error? code231duke";
						passed2 = 1;
					}
					if (passed2) {
						plr[0].coinplus(3);
						cout << "플레이어가 3개의 코인을 얻습니다.\n\n";
					}
					break; //case 0 duke role end
				case 1: //assassin
					if (plr[0].coins() < 3) {
						cout << "\n코인부족.코인3개필요\n";
					}
					cout << "카드를 오픈할 상대방을 선택해주세요.\n선택 : ";
					cin >> attack;
					if (plr[attack].dead()) {
						cout << "already dead\n";
						continue;
					}
					randi = p20p(0); //도전자
					if (!plr[0].cardhave(b))
						cout << "블러핑을 시도합니다.\n";
					switch (randi) {
					case 1:
					case 2:
					case 3:
						passed2 = chall(0, b, randi); //첫번째 카드 도전
						break;
					case 0:
						passed2 = 1;
						cout << "아무도 도전하지 않았습니다.\n";
						break;
					default:
						cout << "error? code271assasin";
						passed2 = 1;
					}
					if (passed2) {
						passed1 = 1;
						if (plr[attack].cardhave(3)) //방해카드소지
							randi2 = 1;
						else
							randi2 = !prob(5); //20% 방해
						if (randi2) {
							cout << "컴퓨터" << attack << "가 플레이어의 암살을 막았습니다.\n";
							cout << "도전하시겠습니까?\n선택(y/n) :";
							cin >> yesi;
							if (yesi == 'y') {
								passed1 = !chall(attack, 3, 0); //두번째 방해도전
							} //end challenge
							else {
								cout << "도전하지 않아 암살을 포기합니다.";
								passed1 = 0;
							}
						}
						if (passed1) {
							cout << "암살자 카드를 통해 컴퓨터" << attack << "의 카드를 하나 오픈합니다.";
							plr[attack].openrand();
							plr[0].coinplus(-3);
						}
						else {
							cout << "플레이어가 컴퓨터" << attack << "암살에 실패하였습니다. ";
						}
					}
					break; //case 1 assasuin end
				case 2: //commander begin
					cout << "상대에게 2코인을 강탈합니다. 강탈할 상대방을 정해주세요.\n선택 : ";
					cin >> attack;
					if (plr[attack].dead()) {
						cout << "already dead\n";
						continue;
					}
					randi = p20p(0);
					if (!plr[0].cardhave(b)) //challenger picker
						cout << "블러핑을 시도합니다.\n";
					switch (randi) {
					case 1:
					case 2:
					case 3:
						passed2 = chall(0, b, randi); //첫번째 카드도전
						break;
					case 0:
						passed2 = 1;
						cout << "아무도 도전하지 않았습니다.\n";
						break;
					default:
						cout << "error? code331commander";
						passed2 = 1;
					}
					if (passed2) {
						passed1 = 1;
						if (plr[attack].cardhave(2)) //방해카드소지
							randi2 = 1;
						else
							randi2 = !prob(5); //20% 방해
						if (randi2) {
							cout << "컴퓨터" << attack << "가 플레이어의 강탈을 막았습니다.\n";
							cout << "도전하시겠습니까?\n선택(y/n) :";
							cin >> yesi;
							if (yesi == 'y') {
								passed1 = !chall(attack, 2,0); //두번째 방해 도전
							}
							else {
								cout << "도전하지 않아 강탈을 포기합니다.";
								passed1 = 0;
							}
						} //end challenge
						
						if (passed1) {
							cout << "사령관 카드를 통해 컴퓨터" << attack << "의 코인 2개를 강탈합니다.";
							plr[0].coinplus(2);
							plr[attack].coinplus(-2);
						}
						else {
							cout << "플레이어가 컴퓨터" << attack << "강탈에 실패하였습니다. ";
						}
					}
					break;//case 2 commandor role end
				default:
					cout << "error error 355, character role";
				}
				break; //while break
			} while (1);
		}
		else
			cout << "invalid input by user fault" << endl;
	} //endelse
	cout <<"현재 코인 수 : "
		<< plr[0].coins() << "개. 턴이 넘어갑니다.\n";
	return;
}

void Run::pcturn(int self)
{
	int randi;
	int a;
	int b;
	int passed1; //방해및도전 통과
	char yesi = 'n';;
	cout << "컴퓨터" << self << "의 턴입니다.\n";
	cout << "행동을 고르고 있습니다...(Enter)\n";
	while (getchar() != '\n'); //키보드버퍼 정리
	cout << "Enter를 눌러주세요";
	getchar();
	if (plr[self].coins() > 9) {
		cout << "코인이 10개 이상이므로, 자동으로 쿠를 시도합니다.\n";
		plr[self].coup(plr);
	}
	else {
		if (prob(2)) //50% 확률
			a = 1;
		else
			a = 2;
		if (a == 1) {
			if (plr[self].coins() > 6)
				b = prob(3) + 1; //3개중 선택
			else
				b = prob(2) + 1; //2개중 선택

			switch (b) {
			case 1: //income
				plr[self].coinplus(1);
				cout << "컴퓨터" << self << "가 코인을 1개 얻습니다.\n";
				break;
			case 2: //원조
				cout << "컴퓨터" << self << "가 해외 원조를 선택하였습니다.\n";
				yesi = 'n';
				if (!plr[0].dead()) {
					cout << "해외 원조를 방해합니까? (y/n):\n"; //사람이 도전 선택
					cin >> yesi;
				}
				if (yesi == 'y')
					randi = 0;
				else {
					if (plr[1].botherwonjo() && self != 1)
						randi = 1;
					else if (plr[2].botherwonjo() && self != 2)
						randi = 2;
					else if (plr[3].botherwonjo() && self != 3) //10% 방해
						randi = 3;
					else
						randi = self;
				}

				if (randi == self) {
					cout << "아무도 방해 하지 않았습니다.";
					passed1 = 1;
				}
				else {
					if (randi == 0)
						cout << "플레이어가 컴퓨터" << self << "의 해외 원조를 막았습니다.\n";
					else
						cout << "컴퓨터" << randi << "가 컴퓨터" << self << "의 해외 원조를 막았습니다.\n";
					if (!prob(5)) { //20%확률로 도전
						cout << "컴퓨터는 도전한다.\n";
						passed1 = !chall(randi, 0, self); //방해 도전
					}
					else {
						cout << "컴퓨터는 도전하지 않는다.\n";
						passed1 = 0;
					}
				}
				if (passed1) {
					plr[self].coinplus(2);
					cout << "컴퓨터" << self << "가 코인을 2개 얻습니다.\n";
				}
				else {
					cout << "컴퓨터" << self << "가 코인을 얻지 못합니다.";
				}
				break;//case2 원조 end
			case 3: //coup
				plr[self].coup(plr);
				break;
			default:
				cout << "error 451";
			}
		} //end ilban일반 behav.
		else if (a == 2) { //캐릭터행동
			int attack; //attack target
			int randi2; //random result save
			int passed2; //passed1과 같은 기능
			do {
				if (plr[self].coins() < 3) { //코인이 적을겅우 공작과 사령관중 택1 (암살불가)
					b = prob(2);
					if (b == 1)
						b = 2;
				}
				else
					b = prob(3); //1/3 확률로 카드 선택
				switch (b) {
				case 0: //duke begin
					cout << "컴퓨터" << self << "가 공작 카드가 있다고 주장합니다.\n";
					yesi = 'n';
					if (!plr[0].dead()) {
						cout << "도전하시겠습니까 ? \n선택(y / n) : "; //challenge by human
						cin >> yesi;
					}
					if (yesi == 'y')
						randi = 0;
					else 
						randi = p20p(self);

					if (randi == self) {
						passed2 = 1;
						cout << "아무도 도전하지 않았습니다.\n";
					}
					else
						passed2 =chall(self,b , randi); //첫번째 카드도전

					if (passed2) {
						plr[self].coinplus(3);
						cout << "컴퓨터" << self << "가 3개의 코인을 얻습니다.\n\n";
					}
					break; //case 0 duke role end
				case 1: //assassin begin
					if (plr[self].coins() < 3) {
						cout << "\n코인부족.코인3개필요(나오면 안됨)\n";
					}
					do {
						attack = prob(3); //33%
						if (attack == self)
							attack = 3;
					} while (plr[attack].dead()); //동일확률로 대상지정 (33%), 대상이 죽었을 경우 다시 고르기
					cout << "컴퓨터" << self << "가 암살자 카드가 있다고 주장합니다.\n";
					yesi = 'n';
					if (!plr[0].dead()) { //죽은플레이어 스킵
						cout << "도전하시겠습니까 ? \n선택(y / n) : ";
						cin >> yesi;
					}
					if (yesi == 'y')
						randi = 0;
					else
						randi = p20p(self); //도전자 선발

					if (randi == self) {
						passed2 = 1;
						cout << "아무도 도전하지 않았습니다.\n";
					}
					else
						passed2 = chall(self, b, randi); //첫번째 카드도전

					if (passed2) {
						passed1 = 1;
						if (attack) {
							if (plr[attack].cardhave(3))
								randi2 = 1; //방해카드소지
							else
								randi2 = prob(5); //20% 확률로 방해
						}
						else {
							cout << "플레이어가 암살을 당하려고 합니다. 방해합니까? (y/n)\n";
							cin >> yesi;
							if (yesi == 'y')
								randi2 = 1;
							else randi2 = 0;
						}
						if (randi2) {
							if (attack)
								cout << "컴퓨터" << attack << "가 암살을 막았습니다.\n";
							else
								cout << "플레이어가 암살을 막았습니다\n";
							if (!prob(5)) { //20% 확률로 도전
								passed1 = !chall(attack, 3,self); //두번째 방해도전
							} //end challenge
							else {
								cout << "도전하지 않아 암살을 포기합니다.";
								passed1 = 0;
							}
						}
						if (passed1) {
							cout << "암살자 카드를 통해 ";
							if (attack)
								cout << "컴퓨터" << attack << "의 ";
							else
								cout << "플레이어의 ";
							cout << "카드를 하나 오픈합니다.";
							plr[self].coinplus(-3);
							plr[attack].openrand();
						}
						else {
							cout << "컴퓨터" << self << "가 ";
							if (attack)
								cout << "컴퓨터" << attack;
							else
								cout << "플레이어";
							cout << "암살에 실패하였습니다. ";
						}
					}
					break; //case 1 assasuin end
				case 2: //commmander begin
					do {
						attack = prob(3); //33%
						if (attack == self)
							attack = 3;
					} while (plr[attack].dead()); //동일확률로 대상지정 (33%), 대상이 죽었을 경우 다시 고르기

					cout << "컴퓨터" << self << "가 사령관 카드가 있다고 주장합니다.\n";
					yesi = 'n';
					if (!plr[0].dead()) { //죽은플레이어 스킵
						cout << "도전하시겠습니까 ? \n선택(y / n) : ";
						cin >> yesi;
					}

					if (yesi == 'y')
						randi = 0;
					else
						randi = p20p(self); //도전자 선발

					if (randi == self) {
						cout << "아무도 도전하지 않았습니다.\n";
						passed2 = 1;
					}
					else
						passed2 = chall(self, b, randi); //카드 첫번째 도전

					if (passed2) {
						passed1 = 1;
						if (attack) {
							if (plr[attack].cardhave(2))
								randi2 = 1; //방해카드 소지
							else
								randi2 = !prob(5); //20% 확률로 방해
						}
						else {
							cout << "플레이어가 강탈을 당하려고 합니다. 방해합니까? (y/n)\n";
							cin >> yesi;
							if (yesi == 'y')
								randi2 = 1;
							else randi2 = 0;
						}
						if (randi2) {
							if (attack)
								cout << "컴퓨터" << attack << "가 강탈을 막았습니다.\n";
							else
								cout << "플레이어가 강탈을 막았습니다.\n";
							if (!prob(5)) { //20% 도전
								passed1 = !chall(attack, 2, self); //두번째 방해도전
							}
							else {
								cout << "도전하지 않아 강탈을 포기합니다.";
								passed1 = 0;
							}
						} //end challenge

						if (passed1) {
							cout << "암살자 카드를 통해 ";
							if (attack)
								cout << "컴퓨터" << attack << "의 ";
							else
								cout << "플레이어의 ";
							cout << "코인 2개를 강탈합니다.";
							plr[self].coinplus(2);
							plr[attack].coinplus(-2);
						}
						else {
							cout << "컴퓨터" << self << "가 ";
							if (attack)
								cout << "컴퓨터" << attack;
							else
								cout << "플레이어";
							cout << "강탈에 실패하였습니다. ";
						}
					}
					break;//case 2 commandor role end
				default:
					cout << "error 637";
				}
				break; //while break
			} while (1);
		}
		else
			cout << "invalid input by user fault" << endl;
	} //endelse
	cout << "현재 코인 수 : "
		<< plr[self].coins() << "개. 턴이 넘어갑니다.\n";
	return;
}

