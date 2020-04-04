// ==============================================================================================================

// 20200690 박진철, 4주차 퀴즈

// ==============================================================================================================

// 방탈 라이브러리 사용을 위한 전처리기
#include <Bangtal.h>
#pragma comment(lib, "Bangtal.lib")

// 비주얼 스튜디오 경고 관련 전처리기
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// sprintf_s 사용을 위한 전처리기
#include <stdio.h>

// Sleep 사용을 위한 전처리기
#include <Windows.h>

// 난수 생성을 위한 전처리기
#include <time.h>
#include <stdlib.h>

// 문자열 처리를 위한 전처리기
#include <string.h>

// 이미지 파일 없음을 NONE으로 정의
#define NONE "NOTHING"

// ==============================================================================================================
// 구조체

// 퍼즐 조각
typedef struct {
	// 오브젝트 이름 및 이미지 파일
	ObjectID id;
	// 이미지 파일 이름
	char* image;
	// 오브젝트의 X, Y좌표 위치
	int x, y;
	// 공백 칸인지 여부
	bool isEmpty;
} Object;

// ==============================================================================================================
// 전역 변수

// 시작 화면, 퍼즐 화면
SceneID startScene, mainScene;

// 시작 버튼, 재시작 버튼
Object startButton, restartButton;

// 퍼즐 조각 (사과)
Object APPLE;
Object apples[9];

// 퍼즐 조각들의 좌표
int appleX[3] = { 400, 570, 740 };
int appleY[3] = { 490, 320, 150 };

// 공백 좌표
int emptyX, emptyY;

// 2차원 배열로 퍼즐 구현
Object puzzle[3][3];

// ==============================================================================================================
// 구조체 사용과 관련한 함수 오버로딩

// locateObject 오버로딩 1
void locateObject(Object& obj, SceneID scene) {
	locateObject(obj.id, scene, obj.x, obj.y);
}

// locateObject 오버로딩 2
void locateObject(Object& obj, SceneID scene, int x, int y) {
	obj.x = x;
	obj.y = y;
	locateObject(obj, scene);
}

// showObject 오버로딩
void showObject(Object& obj) {
	showObject(obj.id);
}

// hideObject 오버로딩
void hideObject(Object& obj) {
	hideObject(obj.id);
}

// scaleObject 오버로딩
void scaleObject(Object& obj, float scale) {
	scaleObject(obj.id, scale);
}

// setObjectImage 오버로딩
void setObjectImage(Object& obj, const char* image) {
	setObjectImage(obj.id, image);
}

// createObject 오버로딩
Object createObject(const char* name, const char* image, int x, int y, float scale = 1.0f, bool empty = false) {
	Object obj = { createObject(name, image), _strdup(image), x, y, empty };
	scaleObject(obj, scale);
	return obj;
}

// ==============================================================================================================
// 그 외 함수

// 퍼즐 조각인 사과 세팅
void puzzleSetting(void) {
	// 반복문을 통해 퍼즐 조각을 세팅함
	for (int i = 0; i < 9; i++) {
		char name[20];
		sprintf_s(name, sizeof(name), "사과%d", i + 1);

		char image[60];
		sprintf_s(image, sizeof(image), ".\\ProjectImages\\partition\\apple_00%d.png", i + 1);

		apples[i] = createObject(name, image, 0, 0);
	}
}

// 퍼즐 시작
void puzzleStart(void) {
	// 난수 생성을 위해 사용
	srand((unsigned int)time(0));
	
	// 퍼즐 조각들이 담겨있는 apples 배열에서 9개를 랜덤으로 배치한다.
	// 9개의 조각이 모두 순서대로 배열될 확률은 1/9! = 1/362,880 으로 극히 낮은 확률이라 고려하지 않았다.
	bool already[9] = { false, false, false, false, false, false, false, false, false };
	int random_list[9];
	int index = 0;
	while (true) {
		int temp = rand() % 9;
		if (!already[temp]) {
			random_list[index++] = temp;
			already[temp] = true;
		}

		if (index == 9) {
			index = 0;
			break;
		}
	}
	
	/*
	// 정답일 경우 게임이 어떻게 끝나는지 확인할 수 있는지 사용하는 코드
	// 상단 int random_list ~ while문을 주석 처리하고, 이 부분을 주석 해제하면 된다.
	int random_list[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	int index = 0;
	*/

	// 공백이 될 조각의 인덱스를 랜덤으로 설정한다.
	int empty_index = rand() % 9;

	// 반복문을 이용하여 조각을 배치한다.
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			puzzle[y][x] = apples[random_list[index]];
			locateObject(puzzle[y][x], mainScene, appleX[index % 3], appleY[index / 3]);
			showObject(puzzle[y][x]);

			// 공백이 될 조각을 변경한다.
			if (index == empty_index) {
				emptyX = x;
				emptyY = y;
				puzzle[y][x].isEmpty = true;
				strcpy(puzzle[y][x].image, NONE);
				setObjectImage(puzzle[y][x], NONE);
			}

			index += 1;
		}
	}
}

// 퍼즐 조각의 위치 변경 (이미지 파일만 변경)
void swap(Object& clicked, Object& empty) {
	// 이미지 파일의 이름을 변경하고
	char temp[60];
	strcpy(temp, clicked.image);
	strcpy(clicked.image, empty.image);
	strcpy(empty.image, temp);

	// 변경된 이미지 파일을 적용하고
	setObjectImage(clicked, clicked.image);
	setObjectImage(empty, empty.image);

	// 변경된 공백 여부를 적용한다.
	clicked.isEmpty = true;
	empty.isEmpty = false;
}

// 퍼즐 조각이 공백과 맞닿아 있는지 확인하고, 공백이 있다면 해당 오브젝트를 전달
// 공백이 없다면 자기 자신의 오브젝트를 전달
Object isTouching(int y, int x) {
	if (y - 1 >= 0) {
		if (puzzle[y - 1][x].isEmpty) {
			emptyX = x;
			emptyY = y;
			return puzzle[y - 1][x];
		}
	}
	if (y + 1 <= 2) {
		if (puzzle[y + 1][x].isEmpty) {
			emptyX = x;
			emptyY = y;
			return puzzle[y + 1][x];
		}
	}
	if (x - 1 >= 0) {
		if (puzzle[y][x - 1].isEmpty) {
			emptyX = x;
			emptyY = y;
			return puzzle[y][x - 1];
		}
	}
	if (x + 1 <= 2) {
		if (puzzle[y][x + 1].isEmpty) {
			emptyX = x;
			emptyY = y;
			return puzzle[y][x + 1];
		}
	}

	return puzzle[y][x];
}

// 정답 확인
void checkAnswer(void) {
	int correctCnt = 0, index = 0;

	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			char temp[60];
			sprintf_s(temp, sizeof(temp), ".\\ProjectImages\\partition\\apple_00%d.png", index + 1);
			
			index += 1;
			if (strcmp(puzzle[y][x].image, temp) == 0) {
				correctCnt += 1;
			}
		}
	}

	if (correctCnt == 8) {
		showMessage("정답입니다!");
		Sleep(3000);
		endGame();
	}
}

// 마우스 입력 처리
void mouseCallback(ObjectID obj_id, const int x, const int y, MouseAction action) {
	
	// 공백 처리
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 3; x++) {
			if (puzzle[y][x].isEmpty && (x != emptyX || y != emptyY)) {
				puzzle[y][x].isEmpty = false;
			}
		}
	}
	
	// 버튼이 마우스 입력을 받았을 경우
	if (obj_id == startButton.id) {
		if (action == MOUSE_CLICK) {
			hideObject(startButton);
			hideObject(APPLE);

			showObject(restartButton);

			puzzleSetting();
			puzzleStart();

			enterScene(mainScene);
		}
	}
	else if (obj_id == restartButton.id) {
		if (action == MOUSE_CLICK) {
			hideObject(restartButton);
			for (int i = 0; i < 9; i++) {
				hideObject(apples[i]);
			}
			
			showObject(startButton);
			showObject(APPLE);

			enterScene(startScene);
		}
	}

	// 퍼즐 조각이 마우스 입력을 받았을 경우
	else if (obj_id == puzzle[0][0].id) {
		if (puzzle[0][0].isEmpty) {
			return;
		}

		Object empty = isTouching(0, 0);
		if (empty.id != puzzle[0][0].id) {
			swap(puzzle[0][0], empty);
		}
	}
	else if (obj_id == puzzle[0][1].id) {
		if (puzzle[0][1].isEmpty) {
			return;
		}

		Object empty = isTouching(0, 1);
		if (empty.id != puzzle[0][1].id) {
			swap(puzzle[0][1], empty);
		}
	}
	else if (obj_id == puzzle[0][2].id) {
		if (puzzle[0][2].isEmpty) {
			return;
		}

		Object empty = isTouching(0, 2);
		if (empty.id != puzzle[0][2].id) {
			swap(puzzle[0][2], empty);
		}
	}
	else if (obj_id == puzzle[1][0].id) {
		if (puzzle[1][0].isEmpty) {
			return;
		}

		Object empty = isTouching(1, 0);
		if (empty.id != puzzle[1][0].id) {
			swap(puzzle[1][0], empty);
		}
	}
	else if (obj_id == puzzle[1][1].id) {
		if (puzzle[1][1].isEmpty) {
			return;
		}

		Object empty = isTouching(1, 1);
		if (empty.id != puzzle[1][1].id) {
			swap(puzzle[1][1], empty);
		}
	}
	else if (obj_id == puzzle[1][2].id) {
		if (puzzle[1][2].isEmpty) {
			return;
		}

		Object empty = isTouching(1, 2);
		if (empty.id != puzzle[1][2].id) {
			swap(puzzle[1][2], empty);
		}
	}
	else if (obj_id == puzzle[2][0].id) {
		if (puzzle[2][0].isEmpty) {
			return;
		}

		Object empty = isTouching(2, 0);
		if (empty.id != puzzle[2][0].id) {
			swap(puzzle[2][0], empty);
		}
	}
	else if (obj_id == puzzle[2][1].id) {
		if (puzzle[2][1].isEmpty) {
			return;
		}

		Object empty = isTouching(2, 1);
		if (empty.id != puzzle[2][1].id) {
			swap(puzzle[2][1], empty);
		}
	}
	else if (obj_id == puzzle[2][2].id) {
		if (puzzle[2][2].isEmpty) {
			return;
		}

		Object empty = isTouching(2, 2);
		if (empty.id != puzzle[2][2].id) {
			swap(puzzle[2][2], empty);
		}
	}

	checkAnswer();
}

// ==============================================================================================================

int main(void) {
	// 1. 시작 버튼을 누르면 장면이 전환되며 퍼즐이 랜덤하게 배치된다.
	// 2. 랜덤하게 배치된 퍼즐 조각들 중 하나가 하얀 이미지(이하 공백)로 변경된다. (이미지 파일 미부여를 통해 하얀 이미지로 변경)
	// 추가. 재시작 버튼을 누르면 시작 장면으로 다시 전환된다.
	// 3. 공백과 상하좌우로 닿아 있는 퍼즐 조각을 클릭할 경우 공백과 해당 퍼즐 조각이 자리가 변경된다.
	// 4. 퍼즐이 완벽하게 맞은 경우 클리어된다.

	// 마우스 처리 함수 등록
	setMouseCallback(mouseCallback);

	// 장면
	startScene = createScene("준비", NONE);
	mainScene = createScene("사과 퍼즐", NONE);

	// 버튼
	startButton = createObject("시작", ".\\ProjectImages\\start.png", 600, 25, 0.5f);
	locateObject(startButton, startScene);
	showObject(startButton);

	restartButton = createObject("재시작", ".\\ProjectImages\\restart.png", 600, 25, 0.5f);
	locateObject(restartButton, mainScene);
	showObject(restartButton);

	// 시작 화면의 사과 1개
	APPLE = createObject("사과", ".\\ProjectImages\\apple.png", 400, 150);
	locateObject(APPLE, startScene);
	showObject(APPLE);


	// 게임 시작
	startGame(startScene);
}