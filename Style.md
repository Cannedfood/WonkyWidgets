# Style guide

Here is a quick overview:

## Indention

Tabs for indention, spaces for alignment. (Which is, btw. the only correct way to indent)

## Naming
- **Classes**/**Enums**/**Structs**: `CamelCase`
- **Variables**: `lowerCamelCase`
- **Constants**: `CamelCase` or `<EnumName>CamelCase`
- **Private member variables**: camel case with m prefix `mCamelCase`
- **Public members**: like variables `lowerCamelCase`
- **Member functions**: like variables `lowerCamelCase`

## Getters and setters
Do **not** prefix the name of setters/getters with set/get.
Just use the name of the property:

```C++
std::string text = button->text(); // Getter
button->text("Click to cuddle Wombat"); // Setter
```
Exception: You can add the set and get if they have
1. Side effects
2. Large-scale effects
3. Setters take no arguments
4. Use sets?
```C++
std::unique_ptr<Widget> w = w0->getOwnership(); // Side effect: Removes ownership from parent
window->setRedrawFlag();
world->setNewWorldOrder(WorldOrder::WombatsRuleHumans);
```

## Pointers & References:

The goal is to:
1. Keep the basic type at the beginning.
	- Most of the time you don't care if it is a const& but you do care that it is e.g. a string.
2. Use an order that makes sense
	- `const int*` is not a const int pointer (meaning: the pointer is not const, the int is) and somewhat misleading while
	- `int const*` is consistent in that all attributes are left-associative

```C++
using namespace std;
string&       myString0;
string const& myString1;
string&&      myString2;
string*       myString3;
string const* myString4;
```

## Default colors

- Foreground color 1
	- `#d99601`
	- `#2e2a21`
- Background color
	- `#303030`
	- `#575757`
