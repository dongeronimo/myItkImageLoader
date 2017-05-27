#pragma once
#include <string>
using namespace std;
class ILoaded
{
public:
	virtual bool IsThisExam(string idExame, string idSerie) = 0;
	virtual string GetIdExame() = 0;
	virtual string GetIdSerie() = 0;
};