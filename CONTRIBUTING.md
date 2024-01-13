# Contributing is welcome from anyone:

## How to contribute?
There are many different ways to contribute to this project. Below are 
just a few examples of how you can contribute but there are plenty of
other ways to help out to.
- Fix issues on github issue tracker.
- Fix/Patch bugs you find
- Report bugs you find in github issues.
- Request features through github issue.
- Implement features in a fork to merged into main.

**NOTE** There is no gurantee that features will be merged into main repo.
Mostly in cases where the feature requested for merge either goes against
the project goals or causes errors with other features. **However** you 
are always free to maintain your own exclusive forks of this codebase.

## Coding style guide:
While I am aware that everyone has their own coding style for how they
choose to develop. It's important that if you do contribute you try to
follow the style and formatting of the code already present to best of
you ablity simple so the code always follows the same pattern which makes
it easier to read.

Currently on C is used in the project but if other languages are added/used.
language guidance will be written for them aswell.

C Language:
- Function names: Functions are named in snake case. e.g. `int read_from_stdin(args)`
- Function Ptrs: Function pointers are also snake cased but prefixed with pfn. e.g. `int (*pfn_read_from_stdin)(args)`
- Integer types: Standard sized integers are prefered but variable sized should be used where needed/apporiate.
- Type names: type names are simply suffixed with `_t`. e.g. `typedef uint64_t big_number_t`
- Variable Names: variables should be named with snake case 
- Special numbers: Numbers that have a special meaning should be defined with C preprocessor `#define`
- Macros and Defined: defined macros and numbers should be in all capital letters. e.g `#define PI 3.1415`
- Avoid lines above 80 chars in length
- Avoid Global variables unless they are needed and where they are used try to make them static
