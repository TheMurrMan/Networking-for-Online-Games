#pragma once

#include <string>

using namespace std;
class Message
{
public:
	Message(string rName, string sName, bool isPublic, string message);

	string mRName;
	string mSName;
	bool mIsPublic;
	string mMessage;
};