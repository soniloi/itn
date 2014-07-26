#ifndef USERCOMPARATORS_H
#define USERCOMPARATORS_H

#include "User.h"

/* Generic count compatator, which unfortunately does not actually work *
class UserCountComparator{
public:
	bool operator(string key)(User* left, User* right){
		return left->getCount(key) < right->getCount(key);
	}
};
*/

/* Comparator for comparing two User objects, ensuring that users with more lines end up at the top of the queue */
class UserLineComparator{
	public:
		bool operator()(User* left, User* right){
			return left->getCount(KEY_ATTR_LINE) < right->getCount(KEY_ATTR_LINE);
		}
};

/* Comparator for comparing two User objects, ensuring that users with more words end up at the top of the queue */
class UserWordComparator{
	public:
		bool operator()(User* left, User* right){
			return left->getCount(KEY_ATTR_WORD) < right->getCount(KEY_ATTR_WORD);
		}
};

/* Comparator for comparing two User objects, ensuring that users that have been referenced more end up at the top of the queue */
class UserReferencedComparator{
	public:
		bool operator()(User* left, User* right){
			return left->getCount(KEY_ATTR_REFERENCED) < right->getCount(KEY_ATTR_REFERENCED);
		}
};

/* Comparator for comparing two User objects, placing users with more kicks received at the top of the queue */
class UserKRComparator{
	public:
		bool operator()(User* left, User* right){
			return left->getCount(KEY_ATTR_KICKSRECEIVED) < right->getCount(KEY_ATTR_KICKSRECEIVED);
		}
};

/* Comparator for comparing two User objects, placing users with more kicks performed at the top of the queue */
class UserKPComparator{
	public:
		bool operator()(User* left, User* right){
			return left->getCount(KEY_ATTR_KICKSPERFORMED) < right->getCount(KEY_ATTR_KICKSPERFORMED);
		}
};

/* Comparator for comparing two User objects, placing users granting more voices at the top of the queue */
class UserVPComparator{

	public:
		bool operator()(User* left, User* right){
			return left->getModePlus(KEY_MODE_VOICE) < right->getModePlus(KEY_MODE_VOICE);
		}

};

/* Comparator for comparing two User objects, placing users whose (voices granted - voices removed) count is highest at the top of the queue (B for balanced) */
class UserVBComparator{
	public:
		bool operator()(User* left, User* right){
			return ((left->getModePlus(KEY_MODE_VOICE) - left->getModeMinus(KEY_MODE_VOICE)) < (right->getModePlus(KEY_MODE_VOICE) - right->getModeMinus(KEY_MODE_VOICE)));
		}
};

/* Comparator for comparing two User objects, placing users who are most botlike at the top of the queue */
class UserBotComparator{
	public:
		bool operator()(User* left, User* right){
			return left->getRateByLineCount(KEY_ATTR_BOT) < right->getRateByLineCount(KEY_ATTR_BOT);
		}
};

/* Comparator for comparing two User objects, placing users who are most dictator-likie at the top of the queue */
class UserDictatorComparator{
	public:
		bool operator()(User* left, User* right){
			return left->getRateByTotalCount(KEY_ATTR_DICTATOR) < right->getRateByTotalCount(KEY_ATTR_DICTATOR);
		}
};
#endif
