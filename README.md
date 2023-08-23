[![wakatime](https://wakatime.com/badge/user/47966c5b-ebb8-433e-b9e3-f366282ab406/project/99d28663-ce7a-4ab5-ad28-6e3d49883354.svg?style=for-the-badge)](https://wakatime.com/badge/user/47966c5b-ebb8-433e-b9e3-f366282ab406/project/99d28663-ce7a-4ab5-ad28-6e3d49883354)

## Syntax

```teclang
type User {
    age: i32
    favoriteColor: Color
}

func<User> construct(color: Color) User {
    this.color = color
}

enum Color {
    Red
    Green
    Blue
}

/*
 * Main app logic
 */
func main() i32 {
    // Create a new user
    user = new User()
    user.age = 42
    user.favoriteColor = Color::Red
    
    if user.age < 21 {
        return -1
    } else {
        printf("Age check passed.")
    }
    
    userB = new User(Color::Green)
    userB.age = 42
    
    when userB.age {
        42 -> printf("Age is 42.")
        21 -> printf("Age is 21.")
        else -> printf("Age is something else.")
    }
    
    userC = new User {
        age = 42
        favoriteColor = Color::Blue
    }
    
    return when user.favoriteColor {
        Color.Red -> 0
        Color.Green -> 1
        Color.Blue -> 2
    }
}
```