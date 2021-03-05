# ModAPI
Generic API to integrate internal modifcations for Win32 processes with a mod loader system.
This library was written with the intention of integrating with HJFod's Geometry Dash [ModLDR](https://github.com/HJfod/ModLdr) project
but the API is flexible enough to be used with mods for other programs.

## How to use this library

The library has two components. 
The first component of ModAPI exposes three functions intended to be used by mod authors
* mod_register_dll
* mod_add_enable
* mod_add_disable

There are more functions to the public API  but these 3 functions are the most important.
You are meant to call them inside your `DLLMain` function like so
```cpp
 BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call){
    case DLL_PROCESS_ATTACH: {
        ModHandle h = mod_register_dll(hModule);
        if (h) {
            mod_add_enable(h, enableCallback);
            mod_add_disable(h, disableCallback);
            mod_set_title(h, "Awesome Mod");
        }
    }
}
    return TRUE;
}

```

Optionally you can also call an additional `mod_set_title` to specify that you want your DLL to display a custom name
```cpp
  mod_set_title(h, "Awesome Mod");
```

### mod_register_dll
`mod_register_dll` is the most important function. It makes your mod visible to ModAPI and without it you wont be able to use the API.
It's important that you check its return value because errors can occur at runtime. Normally the return value will be the base address of your DLL,
which is used to uniquely identify you mod inside ModAPI. In case of an error the return value will be zero.

### mod_add_disable/mod_add_enable
These two functions allow you to specify the code you want to run when ModAPI attempts to enable or disable your mod.
It uses a callback mechanism to allow for flexibility. The callback must have the following signature
```cpp
void __stdcall func(void);
```
### mod_set_title
 As mentioned previously you can use this function to specify a custom name, the default name of your mod will always be the name of your mod as seen by
 the operating system(DLL) unless you call this function

For proper documentation check the [API Reference]() page

The second component of ModAPI is intended to be used by mod loader authors and will be explained at later point in time.


