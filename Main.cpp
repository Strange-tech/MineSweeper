/****************************************
Main.cpp
Author: Root
Date: 2019/12/09
Description:
�����������������㣬����Menuͷ�ļ�����ʼ���˵���������ȴ�ѡ���ѭ��
*****************************************/
#include "GameMenu.h"
#include "GameEngine.h"

int main()
{
	GameMenu::InitMenu();
	GameMenu::WaitChoice();
	GameEngine::Play();
}