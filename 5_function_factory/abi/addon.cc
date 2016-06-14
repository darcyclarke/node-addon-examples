#include <node_jsvmapi.h>

void MyFunction(node::js::env env, node::js::FunctionCallbackInfo info) { 
  node::js::SetReturnValue(env, info, node::js::CreateString(env, "hello world"));
}

void CreateFunction(node::js::env env, node::js::FunctionCallbackInfo info) { 
  node::js::value fn = node::js::CreateFunction(env, MyFunction);

  // omit this to make it anonymous
  node::js::SetFunctionName(env, fn, node::js::CreateString(env, "theFunction"));

  node::js::SetReturnValue(env, info, fn);
}

void new_init(node::js::env env, node::js::value exports, node::js::value module) {
  node::js::SetProperty(env, module,
                        node::js::PropertyName(env, "exports"),
                        node::js::CreateFunction(env, CreateFunction));
}

// NODE_MODULE's init callback's signature uses v8 type for its parameter
// // which complicates our effort to create a VM neutral and ABI stable API.
// // For not I am working around the issue by assuming v8 and thunking into
// // an init callback with a VM neutral signature.
void Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
  node::js::legacy::WorkaroundNewModuleInit(exports, module, new_init);
}

NODE_MODULE(addon, Init)
