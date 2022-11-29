# Introduction
OLua is a front end of Lua with a serial of new language features. A type system is incorprated including class, interface, enum and explict intrinsic types. Then type validation is more strict as well. The code is compiled into original Lua and runs in Lua virtual machine. It works similar to TypeScript. But one thing to know is that OLua is **not** a super collection of Lua. Although best effort is made to keep the similiar style, their codes are not compatible.

Note that OLua is not a real-time interpreted language. It cannot load real-time string as source code to run. Anyway, it still works when the string is in original Lua. Also compiling takes time during development. OLua trades off flexiblity and agility for code safety. It should be used on the nessesity of purpose.

# Quick Start
## Build from source
1. Compiler recommand: 
    * Windows: MSVC 1925 or above (Visual Studio 2019 version 16.5),  support for option '/Zc:preprocessor' is required
    * macOS: Tested in XCode 14, minimal required version is not comfirmed
    * Linux: Tested in GCC 9.4, minimal required version is not comfirmed
2. Download source or clone this repository
3. Enter the source directory and build directly by cmake with command:
```
cmake .
```
4. Check the build result at bin/Debug or bin/Release

## Commandline
All commands of OLua is in conformity with the format below
```
OLua [Action] [--param1 val1 [val2 ...] [ -param2 val2 [... ] ] ]
```
Actions can be batched in a 'olb' file (any extensions can be used). All actions in a batch file are executed in sequence. The format of this file is very similar to normal commandline. Eg:
```
# Comment1
[Action1] [--param1 val1 [val2 ...] [ -param2 val2 [... ] ] ]

# Comment2
[Action2] [--param1 val1 [val2 ...] [ -param2 val2 [... ] ] ]

# Comment3
[Action3] [--param1 val1 [val2 ...] [ -param2 val2 [... ] ] ]
```
To run the batch action, use the following command
```
OLua BatchAction --batchfile theBatchFile.olb
```

## Config an project
Using a batch file is nessersory to build a multi-file OLua project. The following action can be used to generate a project config and run the OLua build
```
begin
dir ...
file ...
compile ...
```
* **begin** : Clean up the existing project config and prepare to config a new one
* **dir** : Add a directory to the project
  * **--path** : Required. Path of the directory to add. 
  * **--out** : Required. Output path of this directory.
  * **--intermediate** : Optional. Intermediate path for this directory. This directory is used to load and save data needed for increament build. Default value is 'intermediate' sub folder under the output path.
  * **--filter** : Optional. A wildcard filter to specify which files in this folder are included as source files. Default value is '*.olu' .
  * **--out_ext** : Optional. The file extionsion of output files. Default value is ".lua" .
  * **--id_ext** : Optional. When this paramerter is present, a file extension will be added to the file id. **NOTE:** File id is the string to reference a file in the original lua "require" statement.
  * **--id_splitter** : Optional. To Specify a splitter charactor between directories and there sub directories in file id. The default value is '/'.
  * **--id_prefix** : Optional. A custom string added to the beginning of every file id
  * **--id_suffix** : Optional. A custom string added to the end of every file id
* **file** : Add a single file to the project.
  * **--path** : Required. Path of the file to add.
  * **--out** : Required. Output file of this file.
  * **--id** : Required. File id of this file.
  * **--intermediate** : Optional. Intermediate file of this file. Default value is an ".oli" file in the output directory with the same file name of the source.
* **compile** : Begin to compile the configed project
    * **--entry** : Optional. Entry file of current project. Default value is a "__entry.lua" file in the first configed directory, or the current working directory if no directory is configed in this project. **NOTE:** Entry file does the static initialization of classes. It must be the first file to load while running the project in a lua virtual machine.
    * **--api** : Optional. Directory of API definitions. Default value is the "api" sub directory of where "OLua" executable is.
    * **--main** : Optional. Add a function call at the end of entry file.

An example of a project file:
```
begin
dir --path ./scripts --out ./bin/debug/scripts --intermediate ./intermediat/scripts
dir --path ./more_scripts --out ./bin/debug/more_scripts --intermediate ./intermediat/more_scripts
file --path ./thirdparty/some_lib/some_file.olu --out ./bin/debug/scripts/some_lib/some_file.lua --id some_lib:somefile
compile --entry ./bin/debug/scripts/init_lua.lua
```

As script files usually sparse everywhere in a project of large scale, these options may not be sufficent for every project. Realtime generating this file from other build system can work well.

## API Config
Lua provides a number of APIs directly exported from C layer to do works around string, table, etc. OLua, as strong type validation is required, has to make an extra declearation to use them. These declearations locates in the "api" directory of the repository, and would be copied to output executable directory, which is the default search directory when compiling a OLua source. 

Most of these APIs keep their names and calling rules unless they have conflicts with keywords or type rules. These changes can be easily modified by editing these declearation file on demand. When doing that, it is recommanded to put modified API in a new directory and specify them by previously introduced "**--api**" parameter of "**compile**" action.


# Language
## Types
OLua requires every variant to have their types in compile time. There are two kind of type: intrinsic type and user defined type.

Intrinsic type includes: **int**, **float**, **string**, **bool**, **nil**, **any**. 

Some keywords are reserved for future version: **int32**, **uint32**, **int64**, **uint64**, **double**, **byte**, **char**. Currently if these keywords are used, the real type will be map to int or float. But they are still preferable in some occasion despite that no real difference exists in the Lua backend. For example, validating the risk of data overflow while calling to an exported C function with an 32-bit int as a parameter. So they are still planned to be implemented.

User defined type includes **classes**, **interfaces**, **function type** and **enums**. They do the same thing as any ordinary object-oreinted languages.

## Type operations
### Declear a variable of a type
```
[local | global] var_name
[local | global] var_name as type
[local | global] var_name = expression
```
Declearing a global variant implicitly will promote a warning in OLua. All variant must have their declearation by "**local**" or "**global**", or an error would be promoted. The keyword "**as**" is used to specify the type. When missing, the type is default to "**any**", in which way it runs like untyped original Lua, but a lot of warnnings would be promoted when converting it to other typed variant.

Types can be automaticly inferred from initializations as well, if there is. When a variant of "**any**" is required and there is an initialization, use "**as any**" explicitly.

Adding keyword "**const**" before the type identifier makes the variant a constant. Writting to a constant would promote an error.

Examples:
```
local var1;            -- declear var1 as any
local var2 as int;     -- declear var2 as int
local var3 = 3.0;      -- declear var3 as float and initialize it by 3.0
local var4 = 3;        -- declear var4 as int and initialize it by 3
local var5 as const any = 3  -- declear var5 as any constant and initialize it by the int value 3
```

### Declear a function
Declearing a function is similar to Lua except for the type identifiers. Basic format is:
```
function func_name(param1 as type, param2 as type, etc..) as (return_type1, return_type2)
```
If there is no return type, the last "as" part should be removed. For functions with only one return value, the "return_type" is not nessesarily to have the parenthess.

A new keyword "**func**" which is identical to "**function**" can be used to save typing.

Examples:
```
function fun1 () as bool
    -- do something
end

function fun2 (p1 as int)
    -- do something
end

func fun3 (p1 as bool, p2 as float) as string
    -- do something
    return "some string"
end

func fun4 (p1 as bool, p2 as float) as (int, string)
    -- do something
    return AnIntVal, "some string"
end
```

### Variation of function parameter
Variation of function parameter is defined in two forms: optional parameter and variable parameter
* **Optional parameter** : Optional parameter can be defined in a bracket of the function parameter list. Note that they should always stay at the last. When calling the function, these parameters can be given optionally. They should alway be nil checked when accessing them.

Examples:
```
func fun1(a as int, [b as bool, c as string] )
    -- do something
end

func1(0)
func1(0, true)
func1(0, true, "OK")
```
* **Variable parameter** : Variable parameter is a group of parameter of uncertain amount. The symbol '**...**' can be used to define it. A unique type can be defined for the variable parameter, or just leave it to "any". In a function with a variable parameter, symbol '...' can be used to refer the it. For example, wrap it in a '{ }' to access individual value, or pass it to another function with variable parameter.

Examples:
```
```

### Function type
If a variant indicates a function, the type identifier is written in this format
```
function(param1 as type, param2 as type, etc..) as (return_type1, return_type2)
```
Examples:
```
local var1 as func(p1 as int)
local var2 as func() as string

func func1(callback as func(p1 as int) as string)
    -- do something
    local str = callback(0)
end

func get_callback() as (int, func(p1 as bool) as int )
    -- do something
    return 0, the_func
end
```

### Type conversion
Passing values between different types triggers implicit type conversion, which promote as much warnings and errors if any risk is detected. Using keyword "**as**" is to do explicit type conversion, in which only errors are promoted. Because it implies that the developer has already confirmed the safety.

Examples for explicit type conversion
```
var1 = var2 as float
callback1 = any_var as func(p1 as int) as string
result = 12.5 + (input as float)
```

## Class and interface
### Definition
Definition of classes and interfaces can be demonstrated by the following example:
```
interface readable
    public func read_a_line() as string
end

class disk_file extends readable
    public local file_name as string = "unknown"
    public override func read_a_file() as string
        -- do something
        return val;
    end
end
```

Keywords "**class**" and "**interface**" are used to start the definition of a class and an interface. Then a list of member is followed, and ended with a keyword "**end**". The class can use keyword "**extends**" to inherit a list of base types, seperated by ',', within which only one class and multiple interfaces are allowed.

In interfaces, only function members without body are allowed, while variants and functions can both be defined in classes. Keyword "**local**" is the only keyword to define a date member.

Every definition of members starts with a bunch of modifiers, including: "**public**", "**private**", "**protected**", "**static**", "**virtual**", "**override**", "**abstract**", "**const**". Most of them have the same meaning with other object-oreinted languages. 

Keyword "**self**" is used to refer the class instance itsself in a function member as well as original Lua. A new keyword "**super**" is avaliable to accese self as a base type.

**NOTE** : 
* "**const**" in the modifier means the function does not modify any date member of the class. Be ware of difference of functions returnning const values.
* Interface does not need to mark any member to be abstract, as they are default to be and can only be abstract.
* Interface is not allowed to have static members.

### Accessing class members
Accessing class members is very straight forward. In a function member (method), just use the name of other members. Note that only static member name is avaliable if it is a static function, while all members can be directly accessed in an instance method. The same rule also goes with the base class member.

To access members on another object outside its class scope, use '.' or ':' following the object expression as well as original Lua. '.' for date member and static function, and ':' for instance method. Class name can be directly used as the object expression when accessing static members.

Example:
```
class my_type
    public local id = 0;
    public func dump()
        print("my id is: " .. id);                -- Use instance member name
        print("next id is: " .. next_id);         -- Use static member name
    end

    static local next_id = 0;
    static func generate_next_id() as int
        local ret = next_id;                     -- Use static member name in static function
        next_id = next_id + 1
        return ret
    end
end

func fun1(obj as my_type)
    local id = obj.id                           -- Use '.' to access instance member
    obj:dump()                                  -- Use ':' to call an instance method
    local new_id = my_type.generate_next_id()   -- Use type name and '.' to access a static member
end
```


### Constructor
Using keyword "**constructor**" or "**ctor**" for short instead of "**function**" or "**func**" makes a function member a constructor. Unlike some other OO languages, constructor need a name because function overwrite is not avaliable. So the explicit constructor name could be a great help.

An example of constructor:
```
class my_class
    public local id as int
    public local name as string
    
    public ctor create_with_id(in_id as int)
        id = in_id;
        name = tostring(id)
    end

    public ctor create_with_name(in_name as string)
        id = 0;
        name = in_name;
    end
end
```
A default constructor named "new" is always present for every class.
Creating an instance of a class just need to call the constructor as if it is a static function member. For example:
```
local inst = my_class.create_with_id(100)
local inst2 = my_class.new()
```

**NOTE**:
* Constructor definition does not return any values. Calling to a constructor returns the class type.
* Constructor cannot be named "new" because this name is reserved for the default constructor
* For data members defined with an initializing expression, the expressions are executed before constructor begins.
* Remember to use "**super**" to call any one of constructors of base class.

### Meta method
Like original Lua, OLua supports meta method to change the behavior of an object in some grammar. A couple of method names are reserved for the meta method. Once methods with these names are defined, they are seen as meta method. These names are the same with original Lua. Currently supporting:

* **__add**
* **__sub**
* **__mul**
* **__div**
* **__concat**
* **__eq**
* **__lt**
* **__le**

These methods should be defined in conformity of the requirements of original Lua, except removing the first parameter because it is provided with the object itself.

Examples:
```
class vector2
    public local x, y
    public func __add(op as vector2) as vector2
        local ret = vector2.new()
        ret.x = x + op.x
        ret.y = y + op.y
        return ret
    end
end

func fun1(base as vector2, offset as vector2)
    local pos = base + offset
end

```

## Array and map
### Definition
OLua applies different restriction on the different usage of Lua table. Tables with sequencial integer as keys are seen as array, and the others are treated as map. These are different types in OLua. Type identifier to define them is shown in this example.
```
local the_array as int[]         --- Define an array of int
local the_map as [string, bool]  --- Define a map with string as key, and bool as value
```
Note that in the backend they are both Lua tables. So table API applies to both. '.' is still avaliable to access map element if the key is string. And '[ ]' still works to access elements of them both.

### Initialization
'{ }' is used to initialize array or map in the same way original Lua does. Type of each element will be validated. When defining an array or a map without type identifier, the actual type will be inferred from the elements as well. If different type of elements detected, the resulted type will be "any"

Example:
```
local arr1 = {33, 44, 77}      -- Type of arr1 will be int[]
local arr2 = {1, 2, "DNF" }    -- Type of arr2 will be any[]

local map1 = { ["name"] = "ham", ["title"] = "wdc" }  -- Type of map1 will be [string, string]
local map2 = { ["name"] = "lec", ["No"]    = 16 }     -- Type of map2 will be [string, any]

```

## Enum
Enum can be defined by keyword "**enum**". A integer constant can be assigned to each enum item. If no value is given to assign explicitly, the value will be automaticly given sequencially from 1. '.' is used to access the value of an enum.

Example:
```
enum actor_type
    player,
    npc,
    monster = 100
end

global the_type = actor_type.player
```

Enums are converted to integer when compiled, meaning that no Lua code like table, variant, or meta will be generated.

## Type alias
Alias name can be defined for any types, including intricsic types, in order to shorten long type identities. Alias name is equvalent to original type identity. Use keyword '**alias**' to define.
```

alias callback_type as func(id as int) as string     -- define alias name 'callback_type' for a function type
local callback as callback_type                      -- use the alias name like other type identity

local ret as string = callback(10)                   -- equvalent to original function type

```

# Work with Lua
## External class and function
As the program is running in a Lua virtual machine, there are many symbols defined from other raw Lua source or native C layer. In OLua it is free to refer these undefined symbol if massive warnings are not bothering. A better way is to write an external definition for them so that more validation of code safety could be enabled.

Class and function can be defined as external by adding keyword "**extern**" before the definition. External functions and methods in an external class do not need the function body, neither they do not generate any Lua code. They just work as the declearations of imported symbols. 

Example:
```
extern class native_actor
    public func run()
    public local hp as int
end

extern func is_player(actor as native_actor)

func fun1(the_actor as native_actor)
    if (is_player(the_actor)) then
        the_actor:run()
    end
    local hp = the_actor.hp;
end
```

**Note**:
* OLua generates Lua codes to access external class under following rule
  * Use ':' to make a instance method call. (Index the callable in the object and pass the object itself as the first parameter)
  * Use '.' to access a member as a variable (Index the callable in the object)
  * Use '.' following a class name to access static object (Find an object of the same name with the class in the current context, and index it)
  
  Any Lua object matching these rules can be define as an external class or function. Like tables and userdatas with __index metatable defined for external classes, or C closure, Lua function, object with __call defined as external function.
* External classes cannot be the base class of normal classes, vice versa.

## Lua API
Lua API is defined as a bunch of extern classes and functions in the API directory illustrated in previous part. These files are included by default every time of compiling. Most of these APIs keep their origin form, while type restrictions are added. But a few things should be noted:

* **string** table is rename to **strlib** due to the conflict with the keyword.
* **string.char()**, **string.byte()**, **utf8.char()** are renamed to **strlib.tochar()**, **strlib.tobyte()** and **utf8.tochar()** due to the conflict with reserved keywords.
* **pairs()** and **ipairs()** are removed because they will be automaticly added to the "for-in" statement while iterating an array or a map
* **math.maxi()**, **math.maxf()**, **math.mini()**, **math.minf()**, **math.absi()**, **math.absf()** are new functions added to the library to work with integer and float respectively. Original **math.max()**, **math.min()**, **math.abs()** are still avaliable, but their return type are all "any". Use the new version to prevent possible type warning.
* **math.randomi()** is added to handle randam integer, while **math.random()** only generates random float now.
* **table** functions works with both array and map

# Nil-Safety
An optional nil-safety system is included in OLua. It helps to write safe code where as many  protential nil errors can be checked up staticly. Some extra syntex is needed to write nil-safety code.

## Compile with nil-safety
Adding paremeter to '**compile**' command controls nil-safety check
* **--nilsafety true** or **--nilsafety yes** or **--nilsafety 1** : Enable nil-safety check in strict mode, in which serious problems will be promoted as errors.
* **--nilsafety warning** : Enable nil-safety check, but all problems will be promoted as warnings, meaning that the compilation will still go successfully. **This is the default mode**.
* **--nilsafety false** or **--nilsafety no** or **--nilsafety 0** : Disable nil-safety check. No messages will be promoted.



## Define nilable and non-nilable variant
In nil-safety mode, variants are divided into nilable and non-nilable. Non-nilable variants are garenteed not to be nil by various restrictions. Normal definations are seen as non-nilable, unless a '?' is added to the type identity.

```
local a as int?        -- nilable
local b as MyClass?    -- nilable
local c as func? ()    -- '?' should be added after keyword 'func' or 'function' for function type

```
Nilable mark can be use at the definition of local variant, global variant, class variant, function paremeter and function return. But type alias and type cast do not accept nilable mark. In OLua, nilable is a state of an expression rather than an implicit type, so it is pointless in these statements. Type cast always keeps the nilable state of the original expression.

All non-nilable variant should be given an non-nilable expression as initialization, otherwise a problem will be promoted.

```
local x as int        -- Non-nilable variant without initialization. Problem promoted
local y as int = 10   -- Correct
```
When defining variants without type but an initializing expression, the nilable state will be derived from the expression as well as the type.

## Convert between nilable and non-nilable
First of all, nilable variants are always compatible to any non-nilable expression. Directly passing the value is all right, including assignment, function parameter, etc.

```
local a as int?
local b as int = 10
a = b       -- OK
a = 10      -- OK
```
Non-nilable variants do not accept any nilable expression or nil. Passing them to non-nilable will promote a problem. There are 2 ways of giving nilables to non-nilables.
* **Nil assertion by '!'** : Using '!' can convert a nilable expression to non-nilable. Nil check is strongly recommanded before doing this
```
local a as int?
local b as int
if(a ~= nil) then
    b = a!  -- Convert nilable expression 'a' to non-nilable
end
```
* **Nil coalescing by '??'** : Binary operator '??' gives a nilable expression and a default non-nilable expression if it is nil, resulting a non-nilable expression.
```
local a as int?
local b as int
b = a ?? 10   -- Equvalent to : if( a ~= nil ) then b = a else b = 10 end
```

## Function call and dereference
Nilable variants of function type are not eligible to call. Converting to non-nilable is nessorary. Same thing is needed while dereferencing a custom class, accessing elements of arrays and maps.
```
local a as [string, int]?
local b as MyClass?
local c as func? ()

x = a["key1"]  -- problem
y = b.member1  -- problem
c()            -- problem

x = a!["key1"]  -- OK
y = b!.member1  -- OK
c!()            -- OK
```

## Notes and future works
* Non-nilable class member can be left uninitialized because in many cases this can only be done in constructors. A reminder will be promoted during compilation. There are some plans to make this look better in the future.
* Mechanism like "method chain" or "optional chainning" is not supported currently, because this cannot map to any lua syntex directly. But it is still in the plan to support.
