![wakatime](https://wakatime.com/badge/user/47966c5b-ebb8-433e-b9e3-f366282ab406/project/99d28663-ce7a-4ab5-ad28-6e3d49883354.svg?style=for-the-badge)

## Syntax

```teclang
// Type declaration.
// By default, everything is private.
// The fields inherit the access modifier of the type.
pub type User {
    priv age: i32 // Not accessible outside the User class.
    favoriteColor: Color // Publicly accessible.
}

// Define a constructor fucntion for User.
func<User> construct(color: Color) User {
    this.color = color
}

// Return type can be inferred.
// For now, all member functions are public.
// This might change in the future to restrict access to the current package scope unless marked as public.
func<User> getAge() {
    return this.age
}

// Enum values start at 0 by default.
// If a value is specified, the next value will be incremented by 1.
enum Color {
    Red   // 0
    Green = 2
    Blue  // 3
}

// Enums can also have functions like types.
func<Color> toString() {
    return when this {
        Red -> "red"
        Green -> "green"
        Blue -> "blue"
    }
}

/*
 * Main app logic
 */
func main() i32 {
    // Iterate over a range of numbers.
    // Step/As can be in either order and are both optional.
    // If step is excluded, it defaults to 1.
    // If as is excluded, it default to 'it'.
    for 0..10 as num step 1 {
        // The iterator is named 'it' by default.
        printf("i = %d", num)
    
        if (num % 2 == 0) {
            pritf("it is even")
        } else {
            pritf("it is odd")
        }
    }
    
    // Create a new user.
    user = new User()
    // user.age = 42 // Error: `age` is marked as private.
    user.favoriteColor = Color::Red
    
    // Parentheses & braces are optional.
    if user.getAge() < 21
        return -1
    else
        printf("Age check passed.")
    
    // Call custom constructor.
    userB = new User(Color::Green)
    
    // Call assignment constructor.
    userC = new User {
        age = 42 // Valid assignment because it's inside the constructor call.
        favoriteColor = Color::Blue
    }
    
    // "switch" like functionality. Simpler than a chain of if/else statements.
    return when user.favoriteColor {
        Color.Red -> 0
        Color.Green -> 1
        Color.Blue -> 2
    }
}

// An enum where the value is a constructor.
enum IpAddress {
    v4(i32, i32, i32, i32)
    v6(string)

    // The arguments can also be named.
    // v4(a: i13, b: i32, c: i32, d: i32)
    // In this case, the values can be access with `var.a` as well as with `var[0]`
}

func main2() {
    loopback = IpAddress.v4(127, 0, 0, 1)
    loopback[0] // 127
    loopback[1] // 0
    loopback[2] // 0
    loopback[3] // 1
    
    google = Ipaddress.v6("::")
    google[0] // "::"
    
    when loopback {
        IpAddress.v4 -> {
            it[0] // 127
            it[3] // 1
        }
        
        IpAddress.v6 -> {
            it[0] // "::"
            it[1] // Error
        }
    }
}
```
