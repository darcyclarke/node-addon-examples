---
id: getting-started.objectwrap
title: Object wrap
prev: getting-started.first
next: getting-started.migration
---

This tutorial is an alternative to [A first project](./first.html) that generates a more complex project. Instead of generating a simple function binding, this project demonstrates how to create a JavaScript object from a C++ object. 

Before you start, make sure you've got all the necessary [prerequisites](prerequisites.html) and [tools](tools.html) installed.

As you select where to begin, you should be aware that N-API operates at two levels which we can think of as the "C level" and the "C++ level". 

The "C level" code is built entirely into Node itself and is very well documented on the [Node documentation pages](https://nodejs.org/api/n-api.html). If you need low-level access to the intricacies of Node, this is the tool for you. 

Alternatively, there is the [node-addon-api ](https://github.com/nodejs/node-addon-api) package which adds a C++ wrapper to the N-API code built into Node. This package makes working with N-API much easier as it implements a very nice object model and abstracts away much of the detailed coding that would otherwise be required, while retaining the N-API promise of ABI stability and forward compatibility.

This tutorial uses `node-addon-api`.

> N-API has been in public release and active development starting with Node 8.0.0. Since then, it's undergone a number of refinements. This tutorial has been tested with Node 10.10.0 and is known to fail with older versions of Node. You can determine the version of Node you're running with the command `node -v`.

## Creating a project

The easiest way to create a new N-API project is to use the [`generator-napi-module`](https://www.npmjs.com/package/generator-napi-module) package. As the package documentation describes, `generator-napi-module` relies on [Yeoman](http://yeoman.io) which must also be installed:

```bash
npm install -g yo
npm install -g generator-napi-module
```

On some systems, you may receive the error `Error: EACCES: permission denied, access`. In that case, on Mac and Linux systems you need to run the commands with elevated privileges:

```bash
sudo npm install -g yo
sudo npm install -g generator-napi-module
```

> Using [`nvm`](https://github.com/creationix/nvm) is an _excellent_ way to banish permission issues. 

Then enter these commands to generate a new project:

```bash
mkdir object-wrap-demo
cd object-wrap-demo
yo napi-module
```

Here are the prompts you'll see and some suggested responses:

```
package name: (object-wrap-demo) 
version: (1.0.0) 
description: An object wrapper demo
git repository: 
keywords: 
author: Your name goes here
license: (ISC) 
```

Yeoman will display the generated `package.json` file here.

```
Is this OK? (yes) yes
? Choose a template Object Wrap
? Would you like to generate TypeScript wrappers for your module? No
```

Yeoman will now build your "Hello World" add-on module. 

At this point, you might try running `npm test` to make sure everything is correctly installed:

```bash
npm test
```

## Project structure

At this point you have a completely functional N-API module project. The project files are structured according to N-API best practices. It should look like this:

```
.
├── binding.gyp                         Used by gyp to compile the C code
├── build                               The intermdiary and final build products
│   └── < contents not shown here >
├── lib                                 The N-API code that accesses the C/C++ binary
│   └── binding.js
├── node_modules                        Node modules required by your project
│   └── < contents not shown here >
├── package.json                        npm description of your module
├── package-lock.json                   Used by npm to insure deployment consistency
├── src                                 The C/C++ code
│   ├── object_wrap_demo.cc
│   └── object_wrap_demo.h
└── test                                Test code
    └── test_binding.js
```

Let's take a look at the essential files.

## package.json

[**package.json**](https://github.com/nodejs/node-addon-examples/blob/master/object-wrap-demo/node-addon-api/package.json)

`embed:object-wrap-demo/node-addon-api/package.json`

This is a typical `package.json` file as generated by [Yeoman](http://yeoman.io) from the responses we entered earlier to the `yo napi-module` command. There are a couple of entries of interest here. 

Notice the [`node-addon-api`](https://github.com/nodejs/node-addon-api) dependency. This package, which is not strictly a part of Node, adds a C++ wrapper to the C API implemented in Node. The package makes it very straightforward to create and manipulate JavaScript objects inside C++. The package is useful even if the underlying library you're accessing is in C. 

There is also a `"gypfile": true` entry which informs npm that your package requires a build using the capabilities of the `node-gyp` package which is covered next. 

## binding.gyp

[**binding.gyp**](https://github.com/nodejs/node-addon-examples/blob/master/object-wrap-demo/node-addon-api/binding.gyp)

`embed:object-wrap-demo/node-addon-api/binding.gyp`

One of the challenges of making C/C++ code available to Node is getting the code compiled, linked, and packaged for a variety of operating systems and architectures. Historically, this would require learning the intricacies of a variety of build tools across a number of operating systems. This is the specific issue GYP seeks to address.  

Using [GYP](https://gyp.gsrc.io/index.md) permits having a single configuration file that works across all platforms and architectures GYP supports. (It's GYP, by the way, that requires Python).

[node-gyp](https://github.com/nodejs/node-gyp) is a command line tool built in Node that orchestrates GYP to compile your C/C++ files to the correct destination. When npm sees the `"gypfile": true` entry in your `package.json` file, it automatically invokes its own internal copy of `node-gyp` which looks for this `binding.gyp` file which must be called `binding.gyp` in order for node-gyp to locate it.

The `binding.gyp` file is a GYP file which is thoroughly documented [here](https://gyp.gsrc.io/docs/UserDocumentation.md). There is also specific information about building libraries [here](https://gyp.gsrc.io/docs/UserDocumentation.md#skeleton-of-a-typical-library-target-in-a-gyp-file).

## src/object\_wrap\_demo.h and src/object\_wrap\_demo.cc

[**object\_wrap\_demo.h**](https://github.com/nodejs/node-addon-examples/blob/master/object-wrap-demo/node-addon-api/src/object_wrap_demo.h)

`embed:object-wrap-demo/node-addon-api/src/object_wrap_demo.h`

[**object\_wrap\_demo.cc**](https://github.com/nodejs/node-addon-examples/blob/master/object-wrap-demo/node-addon-api/src/object_wrap_demo.cc)

`embed:object-wrap-demo/node-addon-api/src/object_wrap_demo.cc`

Here is the nub of our project where all the magic occurs. This is a sample C++ file that shows how to use the power of the `node-addon-api` package to access, create, and manipulate JavaScript objects in C++. 

The `napi.h` file included in the header file comes from `node-addon-api`. This is the C++ wrapper that declares a number of C++ classes representing JavaScript primitive data types and objects. 

The `object_wrap_demo.cc` file defines a C++ object called `ObjectWrapDemo` with a constructor that takes a single JavaScript string as the argument. The constructor stores this string in its private data member `_greeterName`.

The code also defines a `ObjectWrapDemo::Greet` method that takes a single JavaScript string as the argument. The method prints two strings to stdout and returns a JavaScript string containing the value originally passed to the constructor. 

The `ObjectWrapDemo::GetClass` static method returns a class definition that N-API uses in order know how to call the methods implemented by the C++ class.

The `Init` function declares the "exports" of the module. These are analogous to the exports declared by traditional JavaScript modules. This module exports the `ObjectWrapDemo` class as declared by the `ObjectWrapDemo::GetClass` static method.

The macro call at the bottom of the C++ file, `NODE_API_MODULE(addon, Init)`, specifies that the `Init` function is to be called when the module is loaded.  

## lib/binding.js

[**binding.js**](https://github.com/nodejs/node-addon-examples/blob/master/object-wrap-demo/node-addon-api/lib/binding.js)

`embed:object-wrap-demo/node-addon-api/lib/binding.js`

This JavaScript file defines a JavaScript class that acts as an intermediary to the C++ binary. 

The file defines a `ObjectWrapDemo` JavaScript class and then exports it. When `new ObjectWrapDemo (value)` is invoked, the JavaScript class creates a `ObjectWrapDemo` object using the N-API binary and stores it internally as `_addonInstance`. The `_addonInstance` value is used by the JavaScript `greet` method to call the same method in the C++ binary. 

## test/test_binding.js

[**test_binding.js**](https://github.com/nodejs/node-addon-examples/blob/master/object-wrap-demo/node-addon-api/test/test_binding.js)

`embed:object-wrap-demo/node-addon-api/test/test_binding.js`

This code demonstrates how to use the `ObjectWrapDemo` JavaScript class defined in `lib/binding.js`. 

Note that as a side-effect of the `printf` code in the C++ module, two text strings are written to stdout each time the `greet` method is called. 

## Conclusion

This project provides a solid foundation on which you can create your own N-API modules. In addition, here are some things you might want to try:

- Run `test_binding.js` in your debugger. See if you can step through the code to get a better understanding of how it works. What sort of visibility are you getting into the JavaScript object created by the C++ code?

- Modify `test_binding.js` to use the C++ binary module directly instead of through `binding.js`. Step through the code in your debugger to see how things are different.

- Modify `object_wrap_demo.cc`, or create a new C++ module, to export functions instead of an object.  

## Resources

- [node-addon-api](https://github.com/nodejs/node-addon-api) Documentation

- The [generator-napi-module](https://www.npmjs.com/package/generator-napi-module) Package

- The [node-gyp](https://www.npmjs.com/package/node-gyp) Package

- [GYP](https://gyp.gsrc.io) and [.gyp file](https://gyp.gsrc.io/docs/UserDocumentation.md) Documentation.

- [Yeoman](http://yeoman.io)