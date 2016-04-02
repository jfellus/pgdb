/*
 * PlugData.h
 *
 *  Created on: 25 mai 2015
 *      Author: jfellus
 */

#ifndef PLUGDATA_H_
#define PLUGDATA_H_

#include "../utils/utils.h"
#include <pg.h>
#include <image.h>
#include <matrix.h>

class PlugData {
public:
	bool bReadOnly;
public:
	PlugData() {bReadOnly = false;}
	virtual ~PlugData() {}

	virtual void set(const char* value) = 0;
	virtual void* get() = 0;
	virtual size_t get_size()  = 0;
	virtual std::string get_info() {return "";}
	virtual bool is_only_on_request() { return false;}
};


///////////////
// CALLBACKS //
///////////////

class callback_fn {
public:
	callback_fn() {}
	virtual ~callback_fn() {}
	virtual void do_callback() {}
};

template <typename T> class callback_functor : public callback_fn{
public:
	T* _this;
	void (*callback)(T*);
	callback_functor(T* _this, void(*callback)(T*)) : _this(_this),callback(callback) {}
	virtual ~callback_functor() {}
	virtual void do_callback() { callback(_this); }
};
template <typename T> callback_fn create_callback_fn(T* _this, void (*callback)(T*)) { return callback_functor<T>(_this,callback);}


//////////////////
// READ & WRITE //
//////////////////

template <typename T> class PlugDataPtr : public PlugData {
public:
	T* data;
	callback_fn callback;
	PlugDataPtr(T* data, callback_fn callback):data(data),callback(callback){}
	virtual ~PlugDataPtr() {}
	virtual void set(const char* value) { *data = (T) atof(value); callback.do_callback();}
	virtual void* get() {return data;}
	virtual size_t get_size()  {return sizeof(*data);}
};

template <> class PlugDataPtr<std::string> : public PlugData {
public:
	std::string* data;
	callback_fn callback;
	PlugDataPtr(std::string* data, callback_fn callback):data(data),callback(callback){}
	virtual ~PlugDataPtr() {}
	virtual void set(const char* value) { *data = value; callback.do_callback();}
	virtual void* get() {return (void*)data->c_str();}
	virtual size_t get_size()  {return data->length()+1;}
};

template <> class PlugDataPtr<Matrix> : public PlugData {
public:
	Matrix* data;
	callback_fn callback;
	PlugDataPtr(Matrix* data, callback_fn callback):data(data),callback(callback){bReadOnly = false;}
	virtual ~PlugDataPtr(){}
	virtual void set(const char* value) {
		char skey[256]; strcpy(skey, value);
		char* sval = strchr(skey, '=');*sval++ = 0;
		data->data[atoi(skey)] = atof(sval);
		DBG(atoi(skey) << "=" << atof(sval));
		callback.do_callback();
	}
	virtual void* get() {return data->data;}
	virtual size_t get_size()  {return data->w*data->h*sizeof(float);}
	virtual std::string get_info() {return TOSTRING(data->h << "," << data->w);}
};



///////////////
// READ-ONLY //
///////////////

template <typename T> class PlugDataPtrReadOnly : public PlugData {
public:
	T* data;
	PlugDataPtrReadOnly(T* data):data(data){bReadOnly = true;}
	virtual void set(const char* value) {}
	virtual void* get() {return data;}
	virtual size_t get_size()  {return sizeof(T);}
};


template <> class PlugDataPtrReadOnly<ImageFloat> : public PlugData {
public:
	ImageFloat* data;
	PlugDataPtrReadOnly(ImageFloat* data):data(data){bReadOnly = true;}
	virtual ~PlugDataPtrReadOnly(){}
	virtual void set(const char* value) {}
	virtual void* get() {return data->data;}
	virtual size_t get_size()  {return data->w*data->h*sizeof(float);}
	virtual std::string get_info() {return TOSTRING(data->w << "," << data->h);}
	virtual bool is_only_on_request() { return true;}

};

template <> class PlugDataPtrReadOnly<ImageRGB> : public PlugData {
public:
	ImageRGB* data;
	PlugDataPtrReadOnly(ImageRGB* data):data(data){bReadOnly = true;}
	virtual ~PlugDataPtrReadOnly(){}
	virtual void set(const char* value) {}
	virtual void* get() {return data->data;}
	virtual size_t get_size()  {return data->w*data->h*3;}
	virtual std::string get_info() {return TOSTRING(data->w << "," << data->h);}
	virtual bool is_only_on_request() { return true;}
};


template <> class PlugDataPtrReadOnly<Matrix> : public PlugData {
public:
	Matrix* data;
	PlugDataPtrReadOnly(Matrix* data):data(data){bReadOnly = true;}
	virtual ~PlugDataPtrReadOnly(){}
	virtual void set(const char* value) {}
	virtual void* get() {return data->data;}
	virtual size_t get_size()  {return data->w*data->h*sizeof(float);}
	virtual std::string get_info() {return TOSTRING(data->h << "," << data->w);}
};


#endif /* PLUGDATA_H_ */
