#include <iostream>              // Підключення бібліотеки для введення/виведення
#include <stdexcept>            // Підключення для обробки винятків
#include <cctype>               // Підключення для роботи з символами (toupper, tolower тощо)
#include <vector>               // Підключення для використання вектора

using namespace std;            // Використання стандартного простору імен

// Абстрактний клас Transformer — шаблон для перетворення типу T
template <typename T>
class Transformer {
public:
    virtual T operator()(const T& ch) const = 0; // Абстрактний метод трансформації
    virtual ~Transformer() = default;            // Віртуальний деструктор
};

// Клас, що переводить символ у верхній регістр
class ToUpper : public Transformer<char> {
public:
    char operator()(const char& ch) const override {
        return toupper(static_cast<unsigned char>(ch)); // Перетворення в верхній регістр
    }
};

// Клас, що переводить символ у нижній регістр
class ToLower : public Transformer<char> {
public:
    char operator()(const char& ch) const override {
        return tolower(static_cast<unsigned char>(ch)); // Перетворення в нижній регістр
    }
};

//  Клас, що замінює символ на '*'
class ToStar : public Transformer<char> {
public:
    char operator()(const char& ch) const override {
        return '*'; // Завжди повертає '*'
    }
};

// Структура, що зсуває символ на 1
struct Shift {
    char operator()(char ch) const { return ch + 1; }
};

// Структура, що замінює символ на 'x'
struct ToX {
    char operator()(char ch) const { return 'x'; }
};

// Клас шаблонного рядка
template <typename T>
class String {
    size_t len;        // Довжина рядка
    T* data;           // Вказівник на масив символів

public:
    String() : len(0), data(new T[1]{T()}) {} // Конструктор за замовчуванням

    // Конструктор копіювання
    String(const String& other) : len(other.len), data(new T[len + 1]) {
        for (size_t i = 0; i < len; ++i) data[i] = other.data[i]; // Копіювання елементів
        data[len] = T(); // Завершення рядка
    }

    // Конструктор з символом та кількістю
    String(T ch, size_t n) : len(n), data(new T[n + 1]) {
        for (size_t i = 0; i < len; ++i) data[i] = ch;
        data[len] = T(); // Завершення рядка
    }

    // Конструктор з C-рядка
    String(const T* s) {
        len = 0;
        while (s[len] != T()) len++; // Обчислення довжини
        data = new T[len + 1];
        for (size_t i = 0; i < len; ++i) data[i] = s[i];
        data[len] = T(); // Завершення
    }

    ~String() { delete[] data; } // Деструктор

    // Оператор присвоєння
    String& operator=(const String& other) {
        if (this != &other) {
            delete[] data;
            len = other.len;
            data = new T[len + 1];
            for (size_t i = 0; i < len; ++i) data[i] = other.data[i];
            data[len] = T();
        }
        return *this;
    }

    // Доступ за індексом з перевіркою
    T& operator[](size_t pos) {
        if (pos >= len) throw out_of_range("Індекс поза межами");
        return data[pos];
    }

    // Додавання символа до рядка
    String& operator+=(T ch) {
        T* newData = new T[len + 2];
        for (size_t i = 0; i < len; ++i) newData[i] = data[i];
        newData[len] = ch;
        newData[len + 1] = T();
        delete[] data;
        data = newData;
        len++;
        return *this;
    }

    // Додавання рядка
    String& operator+=(const String& other) {
        T* newData = new T[len + other.len + 1];
        for (size_t i = 0; i < len; ++i) newData[i] = data[i];
        for (size_t i = 0; i < other.len; ++i) newData[len + i] = other.data[i];
        newData[len + other.len] = T();
        delete[] data;
        data = newData;
        len += other.len;
        return *this;
    }

    // Оператор + для створення нового рядка
    String operator+(const String& other) const {
        String result;
        result.len = len + other.len;
        result.data = new T[result.len + 1];
        for (size_t i = 0; i < len; ++i) result.data[i] = data[i];
        for (size_t i = 0; i < other.len; ++i) result.data[len + i] = other.data[i];
        result.data[result.len] = T();
        return result;
    }

    // Порівняння на рівність
    bool operator==(const String& other) const {
        if (len != other.len) return false;
        for (size_t i = 0; i < len; ++i) 
            if (data[i] != other.data[i]) return false;
        return true;
    }

    size_t length() const { return len; }  // Отримати довжину
    bool empty() const { return len == 0; } // Перевірка на порожність

    void clear() {
        delete[] data;
        len = 0;
        data = new T[1]{T()}; // Очищення даних
    }

    // Отримання підрядка
    String substr(size_t pos, size_t n) const {
        if (pos > len) throw out_of_range("Позиція поза межами");
        if (n > len - pos) n = len - pos;
        String result;
        result.len = n;
        delete[] result.data;
        result.data = new T[n + 1];
        for (size_t i = 0; i < n; ++i) 
            result.data[i] = data[pos + i];
        result.data[n] = T();
        return result;
    }

    // Застосування трансформера
    void apply(const Transformer<T>& t) {
        for (size_t i = 0; i < len; ++i) 
            data[i] = t(data[i]);
    }

    // Модифікація рядка з використанням функціонального об'єкта
    template <typename F>
    void modify(const F& f) {
        for (size_t i = 0; i < len; ++i) 
            data[i] = f(data[i]);
    }

    // Перевантаження оператора виводу
    friend ostream& operator<<(ostream& os, const String& str) {
        for (size_t i = 0; i < str.len; ++i) os << str.data[i];
        return os;
    }

    // Перевантаження оператора вводу
    friend istream& operator>>(istream& is, String& str) {
        vector<T> buffer;
        T ch;
        while (is.get(ch) && !isspace(ch)) 
            buffer.push_back(ch);
        str.clear();
        str.len = buffer.size();
        delete[] str.data;
        str.data = new T[str.len + 1];
        for (size_t i = 0; i < str.len; ++i) 
            str.data[i] = buffer[i];
        str.data[str.len] = T();
        return is;
    }
};

// Додавання символа зліва до рядка
template <typename T>
String<T> operator+(T ch, const String<T>& str) {
    String<T> result;
    result += ch;
    result += str;
    return result;
}

// Меню операцій
void displayMenu() {
    cout << "\n\n=== МЕНЮ ОПЕРАЦІЙ ===";
    cout << "\n1. Додати рядок";
    cout << "\n2. Додати символ";
    cout << "\n3. Перевести у верхній регістр";
    cout << "\n4. Перевести у нижній регістр";
    cout << "\n5. Замінити символи на '*'";
    cout << "\n6. Вивести довжину";
    cout << "\n7. Очистити рядок";
    cout << "\n8. Вивести підрядок";
    cout << "\n9. Вийти";
    cout << "\nОберіть операцію: ";
}

// Головна функція
int main() {
    String<char> s;             // Створення об'єкта рядка
    int choice;                 // Вибір користувача
    char symbol;                // Символ для додавання

    cout << "Введіть початковий рядок: ";
    cin >> s;                   // Зчитування рядка

    do {
        displayMenu();          // Виведення меню
        cin >> choice;          // Зчитування вибору
        cin.ignore();           // Ігнорування символу нового рядка

        switch (choice) {
            case 1: {
                String<char> add;
                cout << "Введіть рядок для додавання: ";
                cin >> add;
                s += add;
                cout << "Результат: " << s;
                break;
            }
            case 2: {
                cout << "Введіть символ: ";
                cin >> symbol;
                s += symbol;
                cout << "Результат: " << s;
                break;
            }
            case 3: {
                s.apply(ToUpper()); // Перетворення у верхній регістр
                cout << "Результат: " << s;
                break;
            }
            case 4: {
                s.apply(ToLower()); // Перетворення у нижній регістр
                cout << "Результат: " << s;
                break;
            }
            case 5: {
                s.apply(ToStar());  // Замінити всі символи на '*'
                cout << "Результат: " << s;
                break;
            }
            case 6: {
                cout << "Довжина: " << s.length();
                break;
            }
            case 7: {
                s.clear();         // Очистити рядок
                cout << "Рядок очищено";
                break;
            }
            case 8: {
                size_t start, length;
                cout << "Початкова позиція: ";
                cin >> start;
                cout << "Довжина: ";
                cin >> length;
                try {
                    String<char> sub = s.substr(start, length);
                    cout << "Підрядок: " << sub;
                } catch (const exception& e) {
                    cerr << "Помилка: " << e.what();
                }
                break;
            }
            case 9: 
                cout << "До побачення!";
                break;
            default: 
                cout << "Невірний вибір!";
        }
    } while (choice != 9); // Цикл до вибору "вийти"

    return 0; // Завершення програми
}
