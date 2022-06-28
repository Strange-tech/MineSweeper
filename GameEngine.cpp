/****************************************
GameEngine.cpp
Author: Root
Date: 2019/12/09
Description:
�����˳�����Ϸģ�飬��GameEngine�����ռ��ֹͬ����Ⱦ��������Ҫ��Ҫ�޸ĵ��ļ�
*****************************************/
#include "GameEngine.h"
#include<time.h>
namespace GameEngine
{
	// �������������ռ�
	using namespace Graphic;
	using namespace Controller;
	
	size_t mapWidth = 0, mapHeight = 0;				 // ��ͼ��Ⱥ͸߶�
	UCHAR* mapCanvas = nullptr;						 // ��ͼ����ָ�룬�����������벻Ҫֱ�ӳ��Բ������ָ��
	COORD pos = { 0 }, posOld = { 0 };				 // ����λ��
	COORD posChoice = { 0 }, posChoiceOld = { 0 };	 // ѡ�е�ͼ�����λ��
	char key = '\0';								 // ���̵İ���
	bool gameFlag = false;							 // ��Ϸ����״̬
	size_t frame = 0;								 // ����Ⱦ֡��
	clock_t tic = clock();							 // ��Ϸ��ʼʱ��
	int operation = 0;								 // ִ�еĲ���
	bool isFirst = true;							 // �Ƿ�Ϊ��һ��
	UCHAR mapArray[10][10];							 // 10*10�ĵ�ͼ����
	// ��������¼ӵı�������������·���Ԥ�ñ���������
	
	int mapBack[10][10];
	int stepNum;
	int col, row,i,j;
	int realMineNum , showMineNum,randomNum[10]; ;
	bool judgeMine=true;
	clock_t start;
	/****************************************
	Function:  renderMap()
	Parameter: None(void)
	Return:    None(void)
	Description:
	���Ƶ�ǰ��ͼ
	*****************************************/
	void renderMap() 
	{
		// ���п�ʼ����
		for (size_t i = 0; i < mapHeight + 1; i++) {
			// �ƶ����������� ���������׵��ַ�
			MovePos(0, (SHORT)i * 2 + 3);
			if (i == 0) {
				PutString("�X");
			}
			else if (i == mapHeight) {
				PutString("�^");
			}
			else {
				PutString("�d");
			}

			// �����У����Ʊ߽�
			for (size_t j = 0; j < mapWidth; j++) {
				// �ƶ���ȷ�е����꣬���Ʒ���ı߽�
				MovePos(2 + (SHORT)j * 8, (SHORT)i * 2 + 3);
				if (i == 0) {
					if (j < mapWidth - 1) {
						PutString("�T�T�T�j");
					}
					else {
						PutString("�T�T�T�[");
					}
				}
				else if (i == mapHeight) {
					if (j < mapWidth - 1) {
						PutString("�T�T�T�m");
					}
					else {
						PutString("�T�T�T�a");
					}
				}
				else {
					if (j < mapWidth - 1) {
						PutString("�T�T�T�p");
					}
					else {
						PutString("�T�T�T�g");
					}
				}
			}

			// ���Ƶ��׵�ͼ
			if (i > 0 && i < mapHeight + 1) {
				// �ƶ������ף����Ʊ߽��ַ�
				MovePos(0, (SHORT)i * 2 + 2);
				PutString("�U");
				// ������ ���Ƶ���
				for (size_t j = 0; j < mapWidth; j++) {
					MovePos(2 + (SHORT)j * 5, (SHORT)i * 2 + 2);    // �ƶ���ȷ������
					const size_t mapIndex = (i - 1) * mapWidth + j; // ȷ����ͼ������±�
					char numMap[8] = "   ";							// ȷ�������ַ���
					numMap[1] = '0' + mapCanvas[mapIndex];			// ��mapCanvas[mapIndex]Ϊ1��8ʱ������ת�����ַ���
					switch (mapCanvas[mapIndex]) {
					case 0:
						// 0��ʱ����ÿհ�
						PutString("   ");
						break;
					case 1:
						// ��1��ʼ�������� ����ɫ
						PutStringWithColor(numMap, 0, 255, 0, 0, 0, 0);
						break;
					case 2:
						PutStringWithColor(numMap, 255, 0, 0, 0, 0, 0);
						break;
					case 3:
						PutStringWithColor(numMap, 0, 0, 255, 0, 0, 0);
						break;
					case 4:
						PutStringWithColor(numMap, 255, 255, 200, 0, 0, 0);
						break;
					case 5:
						PutStringWithColor(numMap, 0, 255, 255, 0, 0, 0);
						break;
					case 6:
						PutStringWithColor(numMap, 255, 0, 255, 0, 0, 0);
						break;
					case 7:
						PutStringWithColor(numMap, 170, 0, 255, 0, 0, 0);
						break;
					case 8:
						PutStringWithColor(numMap, 255, 255, 0, 0, 0, 0);
						break;
					case 9:
						// 9Ϊ����
						PutString(" �~ ");
						break;
					case 10:
						// ����Ǻţ����о�����;
						PutStringWithColor(" ��", 255, 0, 255, 0, 0, 0);
						break;
					case 11:
						// ����Ǻţ����о�����;
						PutStringWithColor(" ��", 255, 215, 0, 0, 0, 0);
						break;
					}

					MovePos(5 + (SHORT)j * 5, (SHORT)i * 2 + 2);
					PutString("�U");
				}
			}
		}
		Update(); // ����ͼ���µ���Ļ
	}

	/****************************************
	Function:  updateMap()
	Parameter: None(void)
	Return:    None(void)
	Description:
	����ά��������ݸ��Ƶ�һά������
	*****************************************/
	void updateMap() {
		memcpy_s(mapCanvas, mapWidth * mapHeight, mapArray, mapWidth * mapHeight);
	}

	/****************************************
	Function:  InitGame()
	Parameter: None(void)
	Return:    None(void)
	Description:
	��ʼ����Ϸ
	*****************************************/
	void InitGame() {
		ClearScreen();								 // ����
		mapWidth = 10; mapHeight = 10;				 // ���ÿ��
		mapCanvas = new UCHAR[mapWidth * mapHeight]; // ��ʼ����������
		for (size_t i = 0; i < mapHeight; i++) {
			for (size_t j = 0; j < mapWidth; j++) {
				mapArray[i][j] = 11;// ���µ�ͼ���飬���γ��ָ��������ַ�
			}
		}
		updateMap();	// ���µ�ͼ
		renderMap();	// ���Ƶ�ͼ
	}

	
	/****************************************
	Function:  renderChoiceBackground(COORD choice)
	Parameter: COORD choice
	Return:    None(void)
	Description:
	����ѡ�еĵؿ�ı���
	*****************************************/
	void renderChoiceBackground(COORD choice) {
		const SHORT x = choice.X, y = choice.Y;
		const BYTE alpha = 255 - (BYTE)((frame % 50) * 5);        // ���屳����ɫ��ǰ����ɫֵ��ͨ������Ⱦ��֡���������㣨�˴�Ҳ���Ըĳ�ʹ��ʱ�䣩
		const size_t mapIndex = (size_t)y * mapWidth + (size_t)x; // ȷ����ͼ�±�

		MovePos(2 + x * 4, y * 2 + 3);

		// ��������ͬrenderMap�л��Ƶ��ײ�����ͬ������������
		char numMap[8] = "  ";
		numMap[0] = '0' + mapCanvas[mapIndex];
		switch (mapCanvas[mapIndex]) {
		case 0:
			PutStringWithColor("  ", 255, 255, 255, alpha, alpha, alpha);
			break;
		case 1:
			// ��1��ʼ�������� ����ɫ
			PutStringWithColor(numMap, 0, 255, 0, 0, 0, 0);
			break;
		case 2:
			PutStringWithColor(numMap, 255, 0, 0, 0, 0, 0);
			break;
		case 3:
			PutStringWithColor(numMap, 0, 0, 255, 0, 0, 0);
			break;
		case 4:
			PutStringWithColor(numMap, 255, 255, 200, alpha, alpha, alpha);
			break;
		case 5:
			PutStringWithColor(numMap, 0, 255, 255, alpha, alpha, alpha);
			break;
		case 6:
			PutStringWithColor(numMap, 255, 0, 255, alpha, alpha, alpha);
			break;
		case 7:
			PutStringWithColor(numMap, 170, 0, 255, alpha, alpha, alpha);
			break;
		case 8:
			PutStringWithColor(numMap, 255, 255, 0, alpha, alpha, alpha);
			break;
		case 9:
			PutStringWithColor("�~", 255, 255, 255, alpha, alpha, alpha);
			break;
		case 10:
			PutStringWithColor("��", 255, 0, 255, alpha, alpha, alpha);
			break;
		case 11:
			PutStringWithColor("��", 255, 215, 0, alpha, alpha, alpha);
			break;
		}
	}

	/****************************************
	Function:  clearChoiceBackground(COORD choice)
	Parameter: COORD choice
	Return:    None(void)
	Description:
	���choice��ָʾλ�õı���������ͬ�ϣ�����������
	*****************************************/
	void clearChoiceBackground(COORD choice) {
		const SHORT x = choice.X, y = choice.Y;
		const size_t mapIndex = (size_t)y * mapWidth + (size_t)x;

		MovePos(2 + x * 4, y * 2 + 3);

		char numMap[8] = "  ";
		numMap[0] = '0' + mapCanvas[mapIndex];
		switch (mapCanvas[mapIndex]) {
		case 0:
			PutStringWithColor("  ", 255, 255, 255, 0, 0, 0);
			break;
		case 1:
			// ��1��ʼ�������� ����ɫ
			PutStringWithColor(numMap, 0, 255, 0, 0, 0, 0);
			break;
		case 2:
			PutStringWithColor(numMap, 255, 0, 0, 0, 0, 0);
			break;
		case 3:
			PutStringWithColor(numMap, 0, 0, 255, 0, 0, 0);
			break;
		case 4:
			PutStringWithColor(numMap, 255, 255, 200, 0, 0, 0);
			break;
		case 5:
			PutStringWithColor(numMap, 0, 255, 255, 0, 0, 0);
			break;
		case 6:
			PutStringWithColor(numMap, 255, 0, 255, 0, 0, 0);
			break;
		case 7:
			PutStringWithColor(numMap, 170, 0, 255, 0, 0, 0);
			break;
		case 8:
			PutStringWithColor(numMap, 255, 255, 0, 0, 0, 0);
			break;
		case 9:
			PutStringWithColor("�~", 255, 255, 255, 0, 0, 0);
			break;
		case 10:
			PutStringWithColor("��", 255, 0, 255, 0, 0, 0);
			break;
		case 11:
			PutStringWithColor("��", 255, 215, 0, 0, 0, 0);
			break;
		}
	}

	/****************************************
	Function:  checkChoice()
	Parameter: None(void)
	Return:    None(void)
	Description:
	�������Ͳ�������������ر���
	*****************************************/
	void checkChoice() {
		FlushInput();							// ˢ�����뻺����
		pos = GetCursorPos();					// ��ȡ�������
		COORD hitLeftPos = GetCursorHitPos();	// ��ȡ�����������
		COORD hitRightPos = GetCursorHitPos(2);	// ��ȡ�Ҽ���������
		key = GetKeyHit();						// ��ȡ��������
		operation = 0;							// ��ǰ���� (0�޲�����1Ϊ�ڿ��ؿ飬2Ϊ��ֵ���ģ������Լ�ָ��)

		// ����������
		switch (key) {
		case VK_ESCAPE:
			// ESC��������Ϸ����״̬�÷����˳���Ϸ
			gameFlag = false;
			break;

			// �������Ҽ��ƶ�ѡ�е�����
		case VK_UP:
			if (posChoice.Y > 0)posChoice.Y--;
			break;
		case VK_RIGHT:
			if (posChoice.X < (SHORT)mapWidth - 1)posChoice.X++;
			break;
		case VK_DOWN:
			if (posChoice.Y < (SHORT)mapHeight - 1)posChoice.Y++;
			break;
		case VK_LEFT:
			if (posChoice.X > 0)posChoice.X--;
			break;

			// �س��Ϳո�ֱ��Ӧ����1��2
		case VK_RETURN:
			operation = 1;
			break;
		case VK_SPACE:
			operation = 2;
			break;
		}

		// ��������̨�������������ͼ����
		const size_t mouseY = (size_t)(pos.Y + 1) / 2 - 2;
		const size_t mouseX = (size_t)(pos.X - 1) / 4;
		if (mouseY < mapHeight && mouseX < mapWidth) {
			// ��û�г��߽磬����ѡ�������
			posChoice.X = (SHORT)mouseX;
			posChoice.Y = (SHORT)mouseY;
		}

		// ������Ҽ��ֱ��Ӧ����1��2
		if (hitLeftPos.X == pos.X && hitLeftPos.Y == pos.Y) {
			operation = 1;
		}
		else if (hitRightPos.X == pos.X && hitRightPos.Y == pos.Y) {
			operation = 2;
		}
	}

	//��ʼ����ͼ��
	void initMap()
	{
		bool flag;
		srand((unsigned)time(NULL));//����ʮ������ͬ��0-99����
		for (i = 0; i < 10; i++)
		{
			flag = false;
			int t = rand() % 100;
			for (j = 0; j < i; j++)
			{
				if (randomNum[j] == t)
				{
					flag = true;
					break;
				}
			}
			if (flag)
			{
				i--;
				continue;
			}
			randomNum[i] = t;
		}

		//��ʼ��mapBack
		for (int row = 0; row < 10; row++)
		{
			for (int col = 0; col < 10; col++)
			{
				mapBack[row][col] = 0;
			}
		}

		//��ÿ�����ɵ���λ����ʮλ�͸�λ��Ϊһ�����׵�����
		for (int i = 0; i < 10; i++) {
			if(randomNum[i] / 10 <= 9)
				mapBack[(randomNum[i] / 10)][(randomNum[i] % 10)] = 9;
		}

		//����ÿ��������Χ�ĵ�������ȷ���÷�����Ӧ�����������
		for (int row = 0; row < 10; row++)
		{
			for (int col = 0; col < 10; col++)
			{
				if (mapBack[row][col] != 9)
				{
					if (row == 0 && col > 0 && col < 9)
					{
						mapBack[row][col] = mapBack[row][col - 1] / 9 +
							mapBack[row][col + 1] / 9 + mapBack[row + 1][col - 1] / 9 + mapBack[row + 1][col] / 9 + mapBack[row + 1][col + 1] / 9;
					}
					else if (row > 0 && row < 9 && col == 0)
					{
						mapBack[row][col] = mapBack[row - 1][col] / 9 + mapBack[row - 1][col + 1] / 9 +
							mapBack[row][col + 1] / 9 + mapBack[row + 1][col] / 9 + mapBack[row + 1][col + 1] / 9;
					}
					else if (row == 9 && col > 0 && col < 9)
					{
						mapBack[row][col] = mapBack[row - 1][col] / 9 + mapBack[row - 1][col + 1] / 9 +
							mapBack[row - 1][col - 1] / 9 + mapBack[row][col + 1] / 9 + mapBack[row][col - 1] / 9;
					}
					else if (row > 0 && row < 9 && col == 9)
					{
						mapBack[row][col] = mapBack[row - 1][col] / 9 + mapBack[row + 1][col] / 9 +
							mapBack[row][col - 1] / 9 + mapBack[row + 1][col - 1] / 9 + mapBack[row - 1][col - 1] / 9;
					}
					else if (row == 0 && col == 0)
					{
						mapBack[row][col] = mapBack[row + 1][col + 1] / 9 + mapBack[row][col + 1] / 9 + mapBack[row + 1][col] / 9;
					}
					else if (row == 0 && col == 9)
					{
						mapBack[row][col] = mapBack[row + 1][col - 1] / 9 + mapBack[row][col - 1] / 9 + mapBack[row + 1][col] / 9;
					}
					else if (row == 9 && col == 0)
					{
						mapBack[row][col] = mapBack[row - 1][col + 1] / 9 + mapBack[row][col + 1] / 9 + mapBack[row - 1][col] / 9;
					}
					else if (row == 9 && col == 9)
					{
						mapBack[row][col] = mapBack[row - 1][col - 1] / 9 + mapBack[row][col - 1] / 9 + mapBack[row - 1][col] / 9;
					}
					else
					{
						mapBack[row][col] = mapBack[row - 1][col - 1] / 9 + mapBack[row - 1][col] / 9 + mapBack[row - 1][col + 1] / 9 + mapBack[row][col - 1] / 9 +
							mapBack[row][col + 1] / 9 + mapBack[row + 1][col - 1] / 9 + mapBack[row + 1][col] / 9 + mapBack[row + 1][col + 1] / 9;
					}
				}
			}
		}
	}

	//����һ������Ϊ������Χ�˸����ӵĺ���
	void eightSur(int x, int y)
	{
		mapArray[x][y] = mapBack[x][y];
		int i, j;
		if (mapBack[x][y] == 0)
		{
			for (i = x - 1; i <= x + 1; i++)
			{
				for (j = y - 1; j <= y + 1; j++)
				{
					if (i >= 0 && i <= 9 && j >= 0 && j <= 9)
					{
						if (mapArray[i][j] == 11)
						{
							if (mapBack[i][j] == 0)
							{
								eightSur(i, j);
							}
							else
							{
								mapArray[i][j] = mapBack[i][j];
							}
						}
					}
				}
			}
		}
	}

	//ʧ����ʾ����
	void guideWord1()
	{
		Sleep(2000);
		ClearScreen();
		MovePos(0, 5);
		PutString("�������ȳ²�ʿ�Ĺ����в����ȵ��˵��ף�����");
		MovePos(0, 7);
		PutString("�㹼����������ˮ�������������ϣ�������ô�ħ ���ó��ˣ�");
		MovePos(0, 10);
		PutString("��������ƾ���ʵ����һ�����Ծ���������");
		MovePos(10, 12);
		PutString("������������¿��̣�");
		Update();
		int input = getchar();
		if (input)
		{
			InitGame();
			isFirst = true;
			Play();
		}
	}

	//ʤ����ʾ����
	void guideWord2()
	{
		Sleep(2000);
		ClearScreen();
		MovePos(0, 5);
		PutString("��ϲ�㣡������ɨ�������еĵ��ײ��ȳ��˳²�ʿ");
		MovePos(0, 7);
		PutString("�㲻��������ˮ�������������ϣ�������ô�ħ �������ڲ����ˣ�");
		MovePos(0, 10);
		PutString("����㻹�������У����ٴ˿��̰ɣ��������͵�һ�㣬Ҳ������ع�����");
		MovePos(10, 13);
		PutString("����Y�����¿���");
		MovePos(10, 15);
		PutString("����N����ع���");
		Update();
		int input = getchar();
		if (input=='y')
		{
			InitGame();
			isFirst = true;
			Play();
		}
		else if (input == 'n')
		{
			exit(0);
		}
	}


	/****************************************
	Function:  digBlock()
	Parameter: ?
	Return:    ?
	Description:
	��������Ҫ��ɵĺ�������ͨ��posChoice�����жϲ�����������mapArray���飬�벻Ҫ������������Ԥ�úõı���
	*****************************************/
	void digBlock() //�������
	{
		
		col = posChoice.X; row = posChoice.Y;
		if (isFirst)
		{
			initMap();//ע�⣬���ܳ�ʼ��һ�ε�ͼŶ
			//����ǵ�һ���ߣ����ȳ�ʼ����ͼ��ע�ⲻҪ���������һ������
			while (mapBack[row][col] == 9)
			{
				initMap();
			}
			isFirst = false; // ����һ������Ϊ��
			eightSur(row, col);
			updateMap();	// ���µ�ͼ
			renderMap();	// ���Ƶ�ͼ
		}
		else
		{
			eightSur(row, col);
			updateMap();	// ���µ�ͼ
			renderMap();	// ���Ƶ�ͼ
			//����ȵ�һ�����ף��Զ���������ʣ����ײ����ʧ����ʾ��
			if (mapBack[row][col] == 9)
			{
				mapArray[row][col] = 9;
				for (i = 0; i < 10; i++)
				{
					for (j = 0; j < 10; j++)
					{
						if (mapBack[i][j] == 9)
						{
							mapArray[i][j] = 9;	
							updateMap();	// ���µ�ͼ
							renderMap();	// ���Ƶ�ͼ
						}
					}
				}

				//�����Ӧ����ʾ���
				guideWord1();
			}
		}
		stepNum++;//������һ

		//���ֻ������ֿ���ʣ��ȫ��Ϊ��ʱ���Զ����׿������Ĳ����ʤ����ʾ��
		bool flag = true;
		for (i = 0; i < 10; i++)
		{
			for (j = 0; j < 10; j++)
			{
				if (mapArray[i][j] == 11 && mapBack[i][j] != 9)
				{
					flag =false;
				}
			}
		}
		if (flag)
		{
			for (i = 0; i < 10; i++)
			{
				for (j = 0; j < 10; j++)
				{
					if (mapArray[i][j] == 11)
					{
						mapArray[i][j] = 10;
						updateMap();	// ���µ�ͼ
						renderMap();	// ���Ƶ�ͼ
					}
				}
			}
			//�����Ӧ����ʾ���
			guideWord2();
		}
	}


	/****************************************
	Function:  flagBlock()
	Parameter: ?
	Return:    ?
	Description:
	��������Ҫ��ɵĺ�������ͨ��posChoice�����жϲ�����������mapArray���飬�벻Ҫ������������Ԥ�úõı���
	*****************************************/
	void flagBlock() //�Ҽ�����
	{
		col = posChoice.X; row = posChoice.Y;
		if (mapArray[row][col] == 11)
		{
			mapArray[row][col] = 10;//��������
			if (mapBack[row][col] == 9)
			{
				realMineNum--;//�������ȷλ�ò������ĵ�������һ
			}
			if(showMineNum > 0)
			{
				showMineNum--;//��ʾ��������һ�������ʾ������Ϊ��Ͳ��ټ���
			}
		}

		else if (mapArray[row][col] == 10)
		{
			mapArray[row][col] = 11;//������ȡ��
			if (mapBack[row][col] == 9)
			{
				realMineNum++;//����ȷλ�ý����İ�ȥ��������һ
			}
			showMineNum++;//��ʾ��������һ
		}

		stepNum++;//������һ
	}


	/****************************************
	Function:  Play()
	Parameter: None(void)
	Return:    None(void)
	Description:
	��ʼ��Ϸ
	*****************************************/
	void Play()
	{
		realMineNum = 10; showMineNum = 10;
		stepNum = 0;
		gameFlag = true;
		
		while (gameFlag) {
			checkChoice(); // �������

			// �鿴��ǰ�����Ƿ���Ҫ���±���
			if (posChoice.X != posChoiceOld.X || posChoice.Y != posChoiceOld.Y) {
				clearChoiceBackground(posChoiceOld);
				posChoiceOld = posChoice;
			}
			renderChoiceBackground(posChoice);

			// ��0,0�����õ�ǰѡ��λ�õ��ַ�����ע���β�пո����ף�����˼��ΪʲôҪ����ô��ո�
			MovePos(0, 0);
			char str[32] = "";
			sprintf_s(str, "��ǰѡ��(%u, %u)            ", posChoice.X, posChoice.Y);
			PutString(str);

			MovePos(1, 0);
			char str1[32] = "";
			sprintf_s(str1, "������%u", stepNum);
			PutString(str1);

			char str2[32] = "";
			sprintf_s(str2, "\nʣ��������%u            ", showMineNum);
			PutString(str2);

			start = clock();
			char str3[32] = "";
			sprintf_s(str3, "����ʱ(s)��%lds " ,start/ 1000);
			PutString(str3);
			
			// ִ����Ӧ����
			switch (operation) {
			case 1:
				// �����ؿ�
				digBlock();
				break;
			case 2:
				// ���Ϊ����
				flagBlock();
				break;
			}
			
			for (i = 0; i < 10; i++)
			{
				for (j = 0; j < 10; j++)
				{
					if (mapArray[i][j] == 11)
					{
						judgeMine = false;
					}
				}
			}
			if (realMineNum == 0 && judgeMine)
			{
				guideWord2();
			}
			
			// �������ݲ������޸� ����ÿ֡������
			updateMap(); // ���µ�ͼ����
			Update();    // ���²�������Ļ

			frame++;  // ��Ⱦ֡������
			clock_t elapsed = 25 - (clock() - tic); // �����һ֡��Ⱦʱ�䣬��������25�Ĳ�ֵ
			Sleep(elapsed > 0 ? elapsed : 0);	    // ����ֵ�����㣬�����߸ò�ֵ�ĺ���������ȷ��ÿ֡��Ⱦ������50֡
			tic = clock();						    // ������һ�μ�¼��ʱ��
		}
	}

	/****************************************
	Function:  DestroyGame()
	Parameter: None(void)
	Return:    None(void)
	Description:
	������Ϸ�����ն�̬����ı���
	*****************************************/
	void DestroyGame() {
		delete[] mapCanvas;
	}
}