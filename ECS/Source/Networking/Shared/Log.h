#pragma once

#include <iostream>

template<typename... T>
	inline void Debug(T... args) {
	(std::cout << ... << args) << std::endl;
}

template<typename... T>
inline void Info(T... args) {
	(std::cout << ... << args) << std::endl;
}

template<typename... T>
inline void Warning(T... args) {
	(std::cout << ... << args) << std::endl;
}

template<typename... T>
inline void Error(T... args) {
	(std::cout << ... << args) << std::endl;
}