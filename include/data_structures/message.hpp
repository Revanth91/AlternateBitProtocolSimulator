#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include <assert.h>
#include <iostream>
#include <string>

using namespace std;

/*******************************************/
/**************** message_t ****************/
/*******************************************/

struct message_t {
	float value;
	message_t() {}
  	message_t(float i_value) : value(i_value) {}
    void clear() {
    	message_t(); 
    }
};

istream& operator>>(istream& is, message_t& msg);
ostream& operator<<(ostream& os, const message_t& msg);

#endif // __MESSAGE_HPP__