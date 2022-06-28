/****************************************
GameEngine.cpp
Author: Root
Date: 2019/12/09
Description:
定义了程序游戏模块，用GameEngine命名空间防止同名污染，是你主要需要修改的文件
*****************************************/
#include "GameEngine.h"
#include<time.h>
namespace GameEngine
{
	// 引入两个命名空间
	using namespace Graphic;
	using namespace Controller;
	
	size_t mapWidth = 0, mapHeight = 0;				 // 地图宽度和高度
	UCHAR* mapCanvas = nullptr;						 // 地图数组指针，若做基础项请不要直接尝试操作这个指针
	COORD pos = { 0 }, posOld = { 0 };				 // 鼠标的位置
	COORD posChoice = { 0 }, posChoiceOld = { 0 };	 // 选中地图坐标的位置
	char key = '\0';								 // 键盘的按键
	bool gameFlag = false;							 // 游戏运行状态
	size_t frame = 0;								 // 已渲染帧数
	clock_t tic = clock();							 // 游戏开始时刻
	int operation = 0;								 // 执行的操作
	bool isFirst = true;							 // 是否为第一步
	UCHAR mapArray[10][10];							 // 10*10的地图数组
	// 如果你有新加的变量，建议加在下方和预置变量做区别
	
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
	绘制当前地图
	*****************************************/
	void renderMap() 
	{
		// 从行开始遍历
		for (size_t i = 0; i < mapHeight + 1; i++) {
			// 移动坐标至行首 并绘制行首的字符
			MovePos(0, (SHORT)i * 2 + 3);
			if (i == 0) {
				PutString("╔");
			}
			else if (i == mapHeight) {
				PutString("╚");
			}
			else {
				PutString("╠");
			}

			// 遍历列，绘制边界
			for (size_t j = 0; j < mapWidth; j++) {
				// 移动至确切的坐标，绘制方格的边界
				MovePos(2 + (SHORT)j * 8, (SHORT)i * 2 + 3);
				if (i == 0) {
					if (j < mapWidth - 1) {
						PutString("═══╦");
					}
					else {
						PutString("═══╗");
					}
				}
				else if (i == mapHeight) {
					if (j < mapWidth - 1) {
						PutString("═══╩");
					}
					else {
						PutString("═══╝");
					}
				}
				else {
					if (j < mapWidth - 1) {
						PutString("═══╬");
					}
					else {
						PutString("═══╣");
					}
				}
			}

			// 绘制地雷地图
			if (i > 0 && i < mapHeight + 1) {
				// 移动至行首，绘制边界字符
				MovePos(0, (SHORT)i * 2 + 2);
				PutString("║");
				// 遍历列 绘制地雷
				for (size_t j = 0; j < mapWidth; j++) {
					MovePos(2 + (SHORT)j * 5, (SHORT)i * 2 + 2);    // 移动至确切坐标
					const size_t mapIndex = (i - 1) * mapWidth + j; // 确定地图数组的下标
					char numMap[8] = "   ";							// 确定数字字符串
					numMap[1] = '0' + mapCanvas[mapIndex];			// 当mapCanvas[mapIndex]为1到8时，将其转换成字符串
					switch (mapCanvas[mapIndex]) {
					case 0:
						// 0的时候放置空白
						PutString("   ");
						break;
					case 1:
						// 从1开始绘制数字 带颜色
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
						// 9为地雷
						PutString(" ▇ ");
						break;
					case 10:
						// 特殊记号，自行决定用途
						PutStringWithColor(" ☆", 255, 0, 255, 0, 0, 0);
						break;
					case 11:
						// 特殊记号，自行决定用途
						PutStringWithColor(" ⊙", 255, 215, 0, 0, 0, 0);
						break;
					}

					MovePos(5 + (SHORT)j * 5, (SHORT)i * 2 + 2);
					PutString("║");
				}
			}
		}
		Update(); // 将地图更新到屏幕
	}

	/****************************************
	Function:  updateMap()
	Parameter: None(void)
	Return:    None(void)
	Description:
	将二维数组的数据复制到一维数组中
	*****************************************/
	void updateMap() {
		memcpy_s(mapCanvas, mapWidth * mapHeight, mapArray, mapWidth * mapHeight);
	}

	/****************************************
	Function:  InitGame()
	Parameter: None(void)
	Return:    None(void)
	Description:
	初始化游戏
	*****************************************/
	void InitGame() {
		ClearScreen();								 // 清屏
		mapWidth = 10; mapHeight = 10;				 // 设置宽高
		mapCanvas = new UCHAR[mapWidth * mapHeight]; // 初始化画板数组
		for (size_t i = 0; i < mapHeight; i++) {
			for (size_t j = 0; j < mapWidth; j++) {
				mapArray[i][j] = 11;// 更新地图数组，依次出现各个特殊字符
			}
		}
		updateMap();	// 更新地图
		renderMap();	// 绘制地图
	}

	
	/****************************************
	Function:  renderChoiceBackground(COORD choice)
	Parameter: COORD choice
	Return:    None(void)
	Description:
	绘制选中的地块的背景
	*****************************************/
	void renderChoiceBackground(COORD choice) {
		const SHORT x = choice.X, y = choice.Y;
		const BYTE alpha = 255 - (BYTE)((frame % 50) * 5);        // 定义背景白色当前的颜色值，通过已渲染的帧数进行运算（此处也可以改成使用时间）
		const size_t mapIndex = (size_t)y * mapWidth + (size_t)x; // 确定地图下标

		MovePos(2 + x * 4, y * 2 + 3);

		// 以下内容同renderMap中绘制地雷部分相同，不详做介绍
		char numMap[8] = "  ";
		numMap[0] = '0' + mapCanvas[mapIndex];
		switch (mapCanvas[mapIndex]) {
		case 0:
			PutStringWithColor("  ", 255, 255, 255, alpha, alpha, alpha);
			break;
		case 1:
			// 从1开始绘制数字 带颜色
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
			PutStringWithColor("▇", 255, 255, 255, alpha, alpha, alpha);
			break;
		case 10:
			PutStringWithColor("☆", 255, 0, 255, alpha, alpha, alpha);
			break;
		case 11:
			PutStringWithColor("⊙", 255, 215, 0, alpha, alpha, alpha);
			break;
		}
	}

	/****************************************
	Function:  clearChoiceBackground(COORD choice)
	Parameter: COORD choice
	Return:    None(void)
	Description:
	清除choice中指示位置的背景，功能同上，不详做介绍
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
			// 从1开始绘制数字 带颜色
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
			PutStringWithColor("▇", 255, 255, 255, 0, 0, 0);
			break;
		case 10:
			PutStringWithColor("☆", 255, 0, 255, 0, 0, 0);
			break;
		case 11:
			PutStringWithColor("⊙", 255, 215, 0, 0, 0, 0);
			break;
		}
	}

	/****************************************
	Function:  checkChoice()
	Parameter: None(void)
	Return:    None(void)
	Description:
	检查输入和操作，并设置相关变量
	*****************************************/
	void checkChoice() {
		FlushInput();							// 刷新输入缓冲区
		pos = GetCursorPos();					// 获取鼠标坐标
		COORD hitLeftPos = GetCursorHitPos();	// 获取左键单击坐标
		COORD hitRightPos = GetCursorHitPos(2);	// 获取右键单击坐标
		key = GetKeyHit();						// 获取键盘输入
		operation = 0;							// 当前操作 (0无操作，1为挖开地块，2为标值旗帜，或者自己指定)

		// 检查键盘输入
		switch (key) {
		case VK_ESCAPE:
			// ESC键，将游戏运行状态置否以退出游戏
			gameFlag = false;
			break;

			// 上下左右键移动选中的坐标
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

			// 回车和空格分别对应操作1和2
		case VK_RETURN:
			operation = 1;
			break;
		case VK_SPACE:
			operation = 2;
			break;
		}

		// 将鼠标控制台的坐标放缩至地图坐标
		const size_t mouseY = (size_t)(pos.Y + 1) / 2 - 2;
		const size_t mouseX = (size_t)(pos.X - 1) / 4;
		if (mouseY < mapHeight && mouseX < mapWidth) {
			// 若没有超边界，更新选择的坐标
			posChoice.X = (SHORT)mouseX;
			posChoice.Y = (SHORT)mouseY;
		}

		// 左键和右键分别对应操作1和2
		if (hitLeftPos.X == pos.X && hitLeftPos.Y == pos.Y) {
			operation = 1;
		}
		else if (hitRightPos.X == pos.X && hitRightPos.Y == pos.Y) {
			operation = 2;
		}
	}

	//初始化地图：
	void initMap()
	{
		bool flag;
		srand((unsigned)time(NULL));//产生十个不相同的0-99的数
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

		//初始化mapBack
		for (int row = 0; row < 10; row++)
		{
			for (int col = 0; col < 10; col++)
			{
				mapBack[row][col] = 0;
			}
		}

		//将每个生成的两位数的十位和个位作为一个地雷的坐标
		for (int i = 0; i < 10; i++) {
			if(randomNum[i] / 10 <= 9)
				mapBack[(randomNum[i] / 10)][(randomNum[i] % 10)] = 9;
		}

		//根据每个方格周围的地雷数，确定该方格内应该填入的数字
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

	//定义一个作用为翻开周围八个格子的函数
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

	//失败提示函数
	void guideWord1()
	{
		Sleep(2000);
		ClearScreen();
		MovePos(0, 5);
		PutString("你在拯救陈博士的过程中不慎踩到了地雷！！！");
		MovePos(0, 7);
		PutString("你辜负了上善若水先生对你的殷切希望，并让大魔 王得逞了！");
		MovePos(0, 10);
		PutString("但是相信凭你的实力，一定可以卷土重来！");
		MovePos(10, 12);
		PutString("按下任意键重新开盘！");
		Update();
		int input = getchar();
		if (input)
		{
			InitGame();
			isFirst = true;
			Play();
		}
	}

	//胜利提示函数
	void guideWord2()
	{
		Sleep(2000);
		ClearScreen();
		MovePos(0, 5);
		PutString("恭喜你！！！你扫除了所有的地雷并救出了陈博士");
		MovePos(0, 7);
		PutString("你不负上善若水先生对你的殷切希望，并让大魔 王哭晕在厕所了！");
		MovePos(0, 10);
		PutString("如果你还想体验快感，就再此开盘吧；如果你想低调一点，也可以深藏功名！");
		MovePos(10, 13);
		PutString("按下Y键重新开盘");
		MovePos(10, 15);
		PutString("按下N键深藏功名");
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
	这是你需要完成的函数，请通过posChoice变量判断操作，并操作mapArray数组，请不要操作其他程序预置好的变量
	*****************************************/
	void digBlock() //左键函数
	{
		
		col = posChoice.X; row = posChoice.Y;
		if (isFirst)
		{
			initMap();//注意，仅能初始化一次地图哦
			//如果是第一步走，则先初始化地图，注意不要在落点设置一个地雷
			while (mapBack[row][col] == 9)
			{
				initMap();
			}
			isFirst = false; // 将第一步设置为否
			eightSur(row, col);
			updateMap();	// 更新地图
			renderMap();	// 绘制地图
		}
		else
		{
			eightSur(row, col);
			updateMap();	// 更新地图
			renderMap();	// 绘制地图
			//如果踩到一个地雷，自动翻开所有剩余地雷并输出失败提示语
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
							updateMap();	// 更新地图
							renderMap();	// 绘制地图
						}
					}
				}

				//输出对应的提示语句
				guideWord1();
			}
		}
		stepNum++;//步数加一

		//如果只点击数字块且剩余全部为雷时，自动将雷块变成旗帜并输出胜利提示语
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
						updateMap();	// 更新地图
						renderMap();	// 绘制地图
					}
				}
			}
			//输出对应的提示语句
			guideWord2();
		}
	}


	/****************************************
	Function:  flagBlock()
	Parameter: ?
	Return:    ?
	Description:
	这是你需要完成的函数，请通过posChoice变量判断操作，并操作mapArray数组，请不要操作其他程序预置好的变量
	*****************************************/
	void flagBlock() //右键函数
	{
		col = posChoice.X; row = posChoice.Y;
		if (mapArray[row][col] == 11)
		{
			mapArray[row][col] = 10;//插上旗帜
			if (mapBack[row][col] == 9)
			{
				realMineNum--;//如果在正确位置插上旗帜地雷数减一
			}
			if(showMineNum > 0)
			{
				showMineNum--;//显示的雷数减一，如果显示的雷数为零就不再减少
			}
		}

		else if (mapArray[row][col] == 10)
		{
			mapArray[row][col] = 11;//将旗帜取消
			if (mapBack[row][col] == 9)
			{
				realMineNum++;//在正确位置将旗帜拔去地雷数加一
			}
			showMineNum++;//显示的雷数加一
		}

		stepNum++;//步数加一
	}


	/****************************************
	Function:  Play()
	Parameter: None(void)
	Return:    None(void)
	Description:
	开始游戏
	*****************************************/
	void Play()
	{
		realMineNum = 10; showMineNum = 10;
		stepNum = 0;
		gameFlag = true;
		
		while (gameFlag) {
			checkChoice(); // 检查输入

			// 查看当前坐标是否需要更新背景
			if (posChoice.X != posChoiceOld.X || posChoice.Y != posChoiceOld.Y) {
				clearChoiceBackground(posChoiceOld);
				posChoiceOld = posChoice;
			}
			renderChoiceBackground(posChoice);

			// 在0,0处放置当前选择位置的字符串，注意结尾有空格留白，可以思考为什么要加这么多空格
			MovePos(0, 0);
			char str[32] = "";
			sprintf_s(str, "当前选择(%u, %u)            ", posChoice.X, posChoice.Y);
			PutString(str);

			MovePos(1, 0);
			char str1[32] = "";
			sprintf_s(str1, "步数：%u", stepNum);
			PutString(str1);

			char str2[32] = "";
			sprintf_s(str2, "\n剩余雷数：%u            ", showMineNum);
			PutString(str2);

			start = clock();
			char str3[32] = "";
			sprintf_s(str3, "已用时(s)：%lds " ,start/ 1000);
			PutString(str3);
			
			// 执行相应操作
			switch (operation) {
			case 1:
				// 翻开地块
				digBlock();
				break;
			case 2:
				// 标记为地雷
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
			
			// 以下内容不建议修改 处理每帧的事务
			updateMap(); // 更新地图画板
			Update();    // 更新操作到屏幕

			frame++;  // 渲染帧数自增
			clock_t elapsed = 25 - (clock() - tic); // 检查上一帧渲染时间，并计算与25的差值
			Sleep(elapsed > 0 ? elapsed : 0);	    // 若差值大于零，则休眠该差值的毫秒数，以确保每帧渲染不超过50帧
			tic = clock();						    // 更新上一次记录的时间
		}
	}

	/****************************************
	Function:  DestroyGame()
	Parameter: None(void)
	Return:    None(void)
	Description:
	结束游戏，回收动态申请的变量
	*****************************************/
	void DestroyGame() {
		delete[] mapCanvas;
	}
}