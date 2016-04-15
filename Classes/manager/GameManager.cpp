#include "GameManager.h"
#include <new>
#include "EventManager.h"
#include "view\ChessmanNode.h"
#include "ai\PcAi.h"

static GameManager* _manager = nullptr;

GameManager::GameManager()
	:_director(Director::getInstance()),
	_currentMoveChessman(Config::nullChessman),
	_currentSelectChessman(Config::nullChessman)
{
}

GameManager::~GameManager()
{
}

GameManager* GameManager::GetIns()
{
	if (nullptr == _manager)
	{
		_manager = new (std::nothrow) GameManager();
	}
	return _manager;
}

void GameManager::starGame(int opponentType)
{
	setCurrentOpponent(opponentType);
	PcAi::initChessIndex();
}

void GameManager::endGame(int opponentType)
{
	unsigned int pcSize = _pcChessmans.size();
	unsigned int playerSize = _playerChessmans.size();
	if (pcSize <= 0)
	{
		CCLOG("player win game over");
	}
	else if (playerSize <= 0)
	{
		CCLOG("pc win game over");
	}
	else
	{
		//修改下一个下棋的人
		setCurrentOpponent(opponentType == Config::pc ? Config::player : Config::pc);
		CCLOG("Keep it up game pcSize = %d  playerSize = %d", pcSize, playerSize);
	}
}

void GameManager::setCurrentOpponent(int value)
{
	_currentOpponent = value;
	_currentMoveChessman = Config::nullChessman;
	_currentSelectChessman = Config::nullChessman;
	//给玩家一个倒计时
	EventManager::getIns()->dispatchEvent(EventManager::EVENT_NEXT_COUNTDOWN, &_currentOpponent);
	if (_currentOpponent == Config::pc)
	{
		auto index = this->removeOrFindChess(PcAi::startAi(_currentOpponent));
		if (PcAi::getDirection() == Config::nullChessman) {
			dynamic_cast<ChessmanNode*>(_pcChessmans.at(index))->MoveOrSelect(true);
			CCLOG("[setCurrentOpponent 1] <index = %d %d>", dynamic_cast<ChessmanNode*>(_pcChessmans.at(index))->getIndex(), index);
		}
		else
		{
			auto chess1 = dynamic_cast<ChessmanNode*>(_pcChessmans.at(index));
			CCLOG("[setCurrentOpponent 2] <index1 = %d index = %d getChessmanType = %d>", chess1->getIndex(), index, chess1->getChessmanType());
			chess1->MoveOrSelect(true);
			int chessIndex = PcAi::getNextDirectionIndex(chess1->getIndex(), PcAi::getDirection(), 1);
			unsigned int length = _allChessmans.size();
			ChessmanNode* chess2;
			for (size_t i = 0; i < length; i++)
			{
				auto chess = dynamic_cast<ChessmanNode*>(_allChessmans.at(i));
				if (chess->getIndex() == chessIndex)
				{
					chess2 = chess;
					break;
				}
			}
			CCLOG("[setCurrentOpponent 3] <index1 = %d chessmanType1 = %d index2 = %d>", chess1->getIndex(), chess1->getChessmanType(), chess2->getIndex());
			chess2->runAction(Sequence::create(DelayTime::create(0.8), CallFunc::create(
				[=]() {
				chess2->MoveOrSelect(true);
			}
			), nullptr));

		}
	}
}

void GameManager::setCurrentSelectChessman(int chessmanType)
{
	_currentSelectChessman = chessmanType;
}

void GameManager::setCurrentMoveChessman(int chessmanType)
{
	_currentMoveChessman = chessmanType;
}

void GameManager::startTime()
{
	//_director->getScheduler()->scheduleScriptFunc(schedule_selector(), 10, false);
}
//
bool GameManager::isEatOrMove(int opponentType)
{
	bool isEatOrMove = false;
	if (_currentSelectChessman == Config::nullChessman)
	{
		isEatOrMove = true;
	}
	else
	{
		if (opponentType == _currentOpponent)
		{
			isEatOrMove = false;
		}
		else
		{
			if (_currentMoveChessman <= _currentSelectChessman)
			{
				isEatOrMove = true;
			}
			else if (_currentSelectChessman == Config::CHESSTYPE::handsome && _currentMoveChessman == Config::CHESSTYPE::soldiers)
			{
				isEatOrMove = true;
			}
		}
	}
	return isEatOrMove;
}

void GameManager::pushChessman(Node * node)
{
	auto chessman = dynamic_cast<ChessmanNode*>(node);
	if (chessman->getOpponentType() == Config::pc) {
		_pcChessmans.push_back(chessman);
	}
	else
	{
		_playerChessmans.push_back(chessman);
	}
	_allChessmans.push_back(node);
}



int GameManager::getSelectChessType(int type)
{
	int t = Config::nullChessman;
	if (!_moveChessmens.empty() && _moveChessmens.size() > 0)
	{
		if (type == Config::TYPE_OPPONENT)
		{
			t = dynamic_cast<ChessmanNode*>(_moveChessmens.at(0))->getOpponentType();
		}
		else
		{
			t = dynamic_cast<ChessmanNode*>(_moveChessmens.at(0))->getChessmanType();
		}
	}
	return t;
}

std::vector<Node*> GameManager::getChessmans(int opponentType)
{
	if (opponentType == Config::pc) {
		return _pcChessmans;
	}
	else
	{
		return _playerChessmans;
	}
}

void GameManager::pushMoveChessmens(Node * node)
{
	_moveChessmens.push_back(node);
}

void GameManager::moveChessman(int opponentType)
{
	//在这里写交换两个棋子的位置
	if (!_moveChessmens.empty() && _moveChessmens.size() > 1)
	{
		ChessmanNode* chessman1 = dynamic_cast<ChessmanNode*>(_moveChessmens.at(0));
		ChessmanNode* chessman2 = dynamic_cast<ChessmanNode*>(_moveChessmens.at(1));
		int type1 = chessman1->getOpponentType(), type2 = chessman1->getChessmanType(), index = chessman1->getIndex();
		if (chessman2->getOpponentType() == Config::pc)
		{
			this->removeOrFindChess(chessman2->getIndex(), true);
		}
		else
		{
			this->removeOrFindChess(chessman1->getIndex(),true);
			this->removeOrFindChess(chessman2->getIndex(), true, Config::player);
		}
		chessman1->clearChessman(chessman1->getIndex());
		chessman2->setChessType(type1, type2, chessman2->getIndex());
		this->pushChessman(chessman2);
		CCLOG("<moveChessman> type1 = %d type2 = %d index1 = %d index2 = %d", type1, type2, chessman1->getIndex(), chessman2->getIndex());
		_moveChessmens.erase(_moveChessmens.begin(), _moveChessmens.end());
		this->endGame(opponentType);
	}
}

int GameManager::removeOrFindChess(int index, bool isRemove, int opponentType)
{
	unsigned int length = opponentType == Config::pc ? _pcChessmans.size() : _playerChessmans.size();
	int ind = -1;
	for (unsigned int i = 0; i < length; i++)
	{
		auto node = opponentType == Config::pc ? _pcChessmans.at(i) : _playerChessmans.at(i);
		auto chess = dynamic_cast<ChessmanNode*>(node);
		if (chess->getIndex() == index) {
			if (isRemove)
			{
				if (opponentType == Config::pc)
				{
					_pcChessmans.erase(_pcChessmans.begin() + i);
					PcAi::clearOrFindChessIndex(index);
				}
				else
				{
					_playerChessmans.erase(_playerChessmans.begin() + i);
				}
			}
			else
			{
				PcAi::clearOrFindChessIndex(index);
			}
			ind = i;
			break;
		}
	}
	return ind;
}



