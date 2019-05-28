/*
 * StringOp.h
 *
 *  Created on: May 28, 2019
 *      Author: ru1
 */

#ifndef INC_STRINGOP_H_
#define INC_STRINGOP_H_

#include <string>

namespace sops {

std::string separator()
{
#ifdef _WIN32
    return "\\";
#else
    return "/";
#endif
}

std::string lstrip(std::string s, std::string chars = " ") {

	std::size_t index = s.find_first_not_of(chars);
	if (index != std::string::npos) {
		return s.substr(index);
	}
	return s;
}

std::string rstrip(std::string s, std::string chars = " ") {

	std::size_t index = s.find_last_not_of(chars);
	if (index != std::string::npos) {
		return s.substr(index);
	}
	return s;
}

std::string replace(std::string s, std::string olds, std::string news) {

	while(1)
	{
		std::size_t index = s.find(olds);
		if(index == std::string::npos)
			break;

		s.replace(index, olds.size(), news);
	}

	return s;
}

} // namespace sops

#endif /* INC_STRINGOP_H_ */
