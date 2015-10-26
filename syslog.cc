#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "node-syslog.h"

using v8::Isolate;
using v8::HandleScope;
using v8::Function;
using v8::String;
using v8::Integer;
using v8::Exception;
using v8::TryCatch;
using v8::Undefined;

Persistent<FunctionTemplate> Syslog::constructor_tpl;
bool Syslog::connected_ = false;
char Syslog::name_[1024];

struct log_request {
  Persistent<Function>  cb;
  char                 *msg;
  uint32_t              log_level;
};

// ===========================
//      libuuv handlers
// ===========================

static void
afterLogHandler(uv_work_t *req, int code)
{
  Isolate *isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);
  
  struct log_request *log_req = static_cast<struct log_request *>(req->data);

  Local<Function> cb = Local<Function>::New(isolate, log_req->cb);
  if (!cb->IsUndefined() && !cb->IsNull()) {
    TryCatch try_catch;
    Local<Value> argv[0];
    cb->Call(isolate->GetCurrentContext()->Global(), 0, argv);
    if (try_catch.HasCaught())
      node::FatalException(isolate, try_catch);
  }

  log_req->cb.Reset();
  delete log_req->msg;
  delete log_req;
  delete req;
}

static void
logHandler(uv_work_t *req)
{
  struct log_request *log_req = static_cast<log_request *>(req->data);
  char *msg = log_req->msg;
  
  syslog(log_req->log_level, "%s", msg);
  return;
}

// ===========================
//      Syslog class
// ===========================

Syslog::Syslog() {
}

Syslog::~Syslog() {
}

void
Syslog::Initialize(Local<Object> exports)
{
  Isolate *isolate = exports->GetIsolate();

  // Prepare constructor template.
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate);
  tpl->SetClassName(String::NewFromUtf8(isolate, "Syslog"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_METHOD(tpl, "init", Syslog::Init);
  NODE_SET_METHOD(tpl, "log", Syslog::Log);
  NODE_SET_METHOD(tpl, "logSync", Syslog::LogSync);
  NODE_SET_METHOD(tpl, "setMask", Syslog::SetMask);
  NODE_SET_METHOD(tpl, "close", Syslog::Destroy);

  constructor_tpl.Reset(isolate, tpl);
  exports->Set(String::NewFromUtf8(isolate, "Syslog"), tpl->GetFunction());
}

void
Syslog::Init(const FunctionCallbackInfo<Value>& args)
{
	Isolate *isolate = args.GetIsolate();

  if (args.Length() == 0 || !args[0]->IsString()) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, "Must give daemon name string as argument")));
    return;
	}
	
  if (args.Length() < 3 ) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, "Must have at least 3 params as argument")));
    return;
  }
  
  if (connected_)
    Close();
	
  // Open syslog.
  args[0]->ToString()->WriteUtf8(reinterpret_cast<char*>(&name_));
  int options = args[1]->ToInt32()->Value();
  int facility = args[2]->ToInt32()->Value();
  Open(options, facility);

  args.GetReturnValue().Set(Undefined(isolate));
}

void
Syslog::Log(const FunctionCallbackInfo<Value>& args)
{
  Isolate *isolate = Isolate::GetCurrent();
  Local<Function> cb = Local<Function>::Cast(args[2]);

  log_request *log_req = new log_request();

  if(!log_req) {
    //v8::LowMemoryNotification();
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, "Could not allocate enought memory")));
    return;
  }

  if(!connected_) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, "init method has to be called befor syslog")));
    return;
  }

  String::Utf8Value msg(args[1]);
  uint32_t log_level = args[0]->Int32Value();

  log_req->cb.Reset(isolate, cb);
  log_req->msg = strdup(*msg);
  log_req->log_level = log_level;

  uv_work_t *work_req = new uv_work_t();
  work_req->data = log_req;
  int status = uv_queue_work(uv_default_loop(), work_req, logHandler, afterLogHandler);
  assert(status == 0);

  args.GetReturnValue().Set(Undefined(isolate));
}

void
Syslog::LogSync(const FunctionCallbackInfo<Value>& args)
{
  Isolate *isolate = Isolate::GetCurrent();

  if(!connected_) {
    isolate->ThrowException(Exception::Error(
      String::NewFromUtf8(isolate, "init method has to be called befor syslog")));
    return;
  }

  String::Utf8Value msg(args[1]);
  uint32_t log_level = args[0]->Int32Value();

  syslog(log_level, "%s", static_cast<char*>(*msg));

  args.GetReturnValue().Set(Undefined(isolate));
}

void
Syslog::Destroy(const FunctionCallbackInfo<Value>& args)
{
  Isolate *isolate = Isolate::GetCurrent();
  Close();
  args.GetReturnValue().Set(Undefined(isolate));
}

void
Syslog::SetMask(const FunctionCallbackInfo<Value>& args)
{
  Isolate *isolate = args.GetIsolate();
  bool upTo = false;
  int mask, value;

  if (args.Length() < 1) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,
      "You must provide an mask")));
    return;
  }

  if (!args[0]->IsNumber()) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,
      "First parameter (mask) should be numeric")));
    return;
  }

  if (args.Length() == 2 && !args[1]->IsBoolean()) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,
      "Second parameter (upTo) should be boolean")));
    return;
  }

  if (args.Length() == 2 && args[1]->IsBoolean()) {
    upTo = true;
  }

  value = args[0]->Int32Value();
  if (upTo) {
    mask = LOG_UPTO(value);
  } else {
    mask = LOG_MASK(value);
  }

  args.GetReturnValue().Set(Integer::New(isolate, setlogmask(mask)));
}

void
Syslog::Open(int option, int facility)
{
  openlog(name_, option, facility);
  connected_ = true;
}

void
Syslog::Close()
{
  if(connected_) {
    closelog();
    connected_ = false;
  }
}

NODE_MODULE(syslog, Syslog::Initialize);
