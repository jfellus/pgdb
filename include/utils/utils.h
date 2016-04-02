/*
 * utils.h
 *
 *  Created on: 22 mai 2015
 *      Author: jfellus
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <vector>
#include <algorithm>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <string>

template <typename T> class array {
public:
	std::vector<T> v;
public:
	array() {}

	inline const T& add(const T& e) {v.push_back(e); return e;}

	inline bool remove(const T& e) {
		typename std::vector<T>::iterator position = std::find(v.begin(), v.end(), e);
		if (position != v.end()) {v.erase(position); return true;}
		return false;
	}

	inline size_t size() {return v.size();}

	inline T& operator[](int i) {return v[i];}
};


#endif /* UTILS_H_ */
