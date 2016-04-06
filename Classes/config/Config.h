#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "cocos2d\external\win32-specific\icon\include\iconv.h"
#include <string>
#include <vector>

class Config
{
public:
	Config();
	~Config();
	//��������
	enum OPPONENT
	{
		player,
		pc
	};
	//�ƺ�����������Ҳû��ʲô����
	enum CHESS_SIZE
	{
		width = 40,
		height = 40
	};

	enum MAP_SIZE {
		row = 4,
		column = 8,
		w = column * CHESS_SIZE::width,
		h = row * CHESS_SIZE::height
	};

	//��������
	enum CHESSTYPE
	{
		nullChessman = -1,//��Ч�ı�����
		handsome,//˧
		finishing,//��
		phase,//��
		horse,//��
		car,//��
		cannon,//��
		soldiers,//��
	};
	static const char* GBKToUTF8(const char *strChar);
	static const char* GetChessmanName(int value, int opponetType);
private:
	

};
#endif