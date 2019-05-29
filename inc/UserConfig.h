/*
 * Config.h
 *
 */

#ifndef INC_USERCONFIG_H_
#define INC_USERCONFIG_H_

#include <map>
#include <string>

#include "Defines.h"

class UserConfig
{
public:
    static UserConfig& get()
    {
		static UserConfig instance;
		return instance;
	}

    // getter
    const char* title() { return title_.c_str(); }

    const char* message(){ return message_.c_str(); }

    const char* icon(){ return icon_.c_str(); }

    const char* path(){ return path_.c_str(); }

    unsigned long breakDuration() { return break_duration_s; }

    unsigned long remindAfter() { return remind_after_s; }

    unsigned long minBreakDuration() { return min_break_duration_s; }


    // setter
    void title(std::string t) { title_ = t; }

    void message(std::string m) { message_ = m; }

    void icon(std::string i) { icon_ = i; }

    void path(std::string p) { path_ = p; }

    void breakDuration(unsigned long b) { break_duration_s = b; }

    void remindAfter(unsigned long r) { remind_after_s = r; }

    void minBreakDuration(unsigned long m) { min_break_duration_s = m; }


private:
	UserConfig() {
	}


	UserConfig(const UserConfig&);

	UserConfig& operator=(const UserConfig&);

	std::string path_ = DEFAULT_PATH;

	std::string icon_ = DEFAULT_ICON;

	std::string title_ = DEFAULT_TITLE;

	std::string message_ = DEFAULT_MESSAGE;

	unsigned long break_duration_s = DEFAULT_BREAK_DUR_S;

	unsigned long remind_after_s = DEFAULT_REMIND_AFTER_S;

	unsigned long min_break_duration_s = DEFAULT_MIN_BREAK_DUR_S;

};



#endif /* INC_USERCONFIG_H_ */
