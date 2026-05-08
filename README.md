# Resistance Coup Console Game

콘솔에서 플레이하는 간단한 `Resistance: Coup` 스타일 카드 게임입니다. 한 명의 사용자 플레이어와 세 명의 컴퓨터 플레이어가 참여하며, 마지막까지 탈락하지 않은 참가자가 승리합니다.

## 주요 기능

- 플레이어 1명, 컴퓨터 3명으로 진행
- 공작, 암살자, 사령관, 백작부인 카드 구현
- 소득, 해외 원조, 쿠 등 기본 행동 지원
- 캐릭터 행동과 도전, 방해 처리
- 컴퓨터 플레이어의 무작위 행동 선택

## 빌드 방법

`g++`가 설치되어 있다면 아래 명령으로 빌드할 수 있습니다.

```bash
make
```

직접 컴파일하려면 다음 명령을 사용할 수 있습니다.

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Card.cpp Deck.cpp Player.cpp Prtstr.cpp Run.cpp randoms.cpp -o coup_game
```

## 실행 방법

```bash
./coup_game
```

메뉴에서 `1`을 입력하면 게임 설명을 볼 수 있고, `2`를 입력하면 게임을 시작합니다.

## 파일 구조

- `main.cpp`: 프로그램 시작점과 메인 메뉴 처리
- `Run.h`, `Run.cpp`: 게임 진행, 턴 처리, 도전/방해 로직
- `Player.h`, `Player.cpp`: 플레이어 상태, 카드 공개, 쿠 처리
- `Deck.h`, `Deck.cpp`: 덱과 카드 교환 처리
- `Card.h`, `Card.cpp`: 카드 직업과 표시 이름 관리
- `Prtstr.h`, `Prtstr.cpp`: 메뉴와 설명서 문자열 출력
- `randoms.h`, `randoms.cpp`: 난수 보조 함수

## 개선 사항

- 모든 소스 파일 인코딩을 UTF-8로 정리했습니다.
- 컴파일 경고를 제거했습니다.
- 잘못된 대상 번호 입력으로 배열 범위를 벗어날 수 있던 문제를 방지했습니다.
- 컴퓨터가 공격 대상을 고를 때 살아 있는 상대만 동일 확률로 선택하도록 수정했습니다.
- GitHub 업로드용 README와 Makefile, `.gitignore`를 추가했습니다.
