#include "Message.h"

Message::Message(string rName, string sName, bool isPublic, string message)
{
	mRName = rName;
	mSName = sName;
	mIsPublic = isPublic;
	mMessage = message;
}