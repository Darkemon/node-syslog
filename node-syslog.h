#ifndef NODE_SYSLOG_H
#define NODE_SYSLOG_H

#include <node.h>
#include <node_object_wrap.h>
#include <v8.h>

using v8::FunctionTemplate;
using v8::FunctionCallbackInfo;
using v8::Local;
using v8::Object;
using v8::Persistent;
using v8::Value;

class Syslog : public node::ObjectWrap {
  public:
    static void Initialize(Local<Object> exports);
	    
  protected:
    static Persistent<FunctionTemplate> constructor_tpl;

    static void Init(const FunctionCallbackInfo<Value>& args);
    static void Log(const FunctionCallbackInfo<Value>& args);
    static void LogSync(const FunctionCallbackInfo<Value>& args);
    static void SetMask(const FunctionCallbackInfo<Value>& args);
    static void Destroy(const FunctionCallbackInfo<Value>& args);
	
    Syslog();
    ~Syslog();

  private:
    static bool connected_;
    static char name_[1024];

    static void Open(int option, int facility);
    static void Close();
};

#endif /* NODE_SYSLOG_H */
