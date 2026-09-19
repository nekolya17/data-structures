#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <random>
#include <rbt/rbt.hpp>

void test_empty_tree() {
    ds::RBT<int> tree;
    assert(tree.empty());
    assert(tree.size() == 0);

    // Поиск и удаление в пустом дереве
    assert(tree.search(42) == false);
    assert(tree.remove(42) == false);

    // Очистка пустого дерева не должна приводить к падению
    tree.clear();
    assert(tree.empty());
    assert(tree.size() == 0);

    std::cout << "[ OK ] 1. Empty tree operations passed\n";
}

void test_single_element() {
    ds::RBT<int> tree;

    tree.insert(100);
    assert(!tree.empty());
    assert(tree.size() == 1);
    assert(tree.search(100) == true);
    assert(tree.search(99) == false);

    // Удаление единственного узла (он же корень)
    assert(tree.remove(100) == true);
    assert(tree.empty());
    assert(tree.size() == 0);
    assert(tree.search(100) == false);

    // Повторное удаление того же элемента
    assert(tree.remove(100) == false);

    std::cout << "[ OK ] 2. Single element insert/remove passed\n";
}

void test_sequential_insert() {
    ds::RBT<int> tree_asc;
    // Вставка по возрастанию: 1, 2, 3 ... 1000
    for (int i = 1; i <= 1000; ++i) {
        tree_asc.insert(i);
    }
    assert(tree_asc.size() == 1000);
    for (int i = 1; i <= 1000; ++i) {
        assert(tree_asc.search(i) == true);
    }

    ds::RBT<int> tree_desc;
    // Вставка по убыванию: 1000, 999 ... 1
    for (int i = 1000; i >= 1; --i) {
        tree_desc.insert(i);
    }
    assert(tree_desc.size() == 1000);
    for (int i = 1; i <= 1000; ++i) {
        assert(tree_desc.search(i) == true);
    }

    std::cout << "[ OK ] 3. Sequential ascending/descending insert passed\n";
}

void test_deletion_scenarios() {
    ds::RBT<int> tree;
    std::vector<int> values = {50, 25, 75, 10, 30, 60, 90, 5, 15, 28, 35, 80, 100};
    for (int v : values) {
        tree.insert(v);
    }
    assert(tree.size() == values.size());

    // 1. Удаление листа (узел 5)
    assert(tree.remove(5) == true);
    assert(tree.search(5) == false);
    assert(tree.size() == values.size() - 1);

    // 2. Удаление узла с 1 ребенком (узел 90, у которого ребенок 80 и 100? узел 10 с ребенком 15)
    assert(tree.remove(10) == true);
    assert(tree.search(10) == false);
    assert(tree.search(15) == true); // Ребенок остался на месте!

    // 3. Удаление узла с 2 детьми (узел 25, у него дети 30 и 15)
    assert(tree.remove(25) == true);
    assert(tree.search(25) == false);
    assert(tree.search(28) == true);
    assert(tree.search(30) == true);
    assert(tree.search(35) == true);

    // 4. Удаление корня (узел 50)
    assert(tree.remove(50) == true);
    assert(tree.search(50) == false);

    // 5. Попытка удалить несуществующий элемент
    assert(tree.remove(9999) == false);

    // 6. Удаление всех оставшихся элементов до нуля
    std::vector<int> remaining = {75, 60, 90, 15, 28, 30, 35, 80, 100};
    for (int v : remaining) {
        assert(tree.remove(v) == true);
    }
    assert(tree.empty());
    assert(tree.size() == 0);

    std::cout << "[ OK ] 4. All deletion scenarios (leaf, 1 child, 2 children, root) passed\n";
}

void test_rule_of_five() {
    // А. Конструктор копирования (глубокое копирование)
    ds::RBT<int> original;
    for (int i = 1; i <= 50; ++i) {
        original.insert(i);
    }

    ds::RBT<int> copy_constructed = original;
    assert(copy_constructed.size() == 50);
    // Проверяем независимость копии
    copy_constructed.insert(999);
    assert(copy_constructed.size() == 51);
    assert(original.size() == 50); // Оригинал не изменился!
    assert(original.search(999) == false);

    // Б. Копирующее присваивание
    ds::RBT<int> copy_assigned;
    copy_assigned.insert(777); // Старые данные должны очиститься
    copy_assigned = original;
    assert(copy_assigned.size() == 50);
    assert(copy_assigned.search(777) == false);

    // В. Самоприсваивание
    copy_assigned = copy_assigned;
    assert(copy_assigned.size() == 50);

    // Г. Конструктор перемещения
    ds::RBT<int> moved_constructed = std::move(original);
    assert(moved_constructed.size() == 50);
    assert(original.empty()); // Исходный объект опустошен

    // Д. Перемещающее присваивание
    ds::RBT<int> move_assigned;
    move_assigned = std::move(moved_constructed);
    assert(move_assigned.size() == 50);
    assert(moved_constructed.empty());

    // Проверяем, что перемещенный объект можно снова использовать
    original.insert(10);
    assert(original.size() == 1);
    assert(original.search(10) == true);

    std::cout << "[ OK ] 5. Rule of Five (Deep Copy, Move, Self-assignment) passed\n";
}

// ============================================================================
// 6. Стресс-тест на 5 000 случайных элементов
// ============================================================================
void test_stress_random() {
    const int NUM_ELEMENTS = 5000;
    std::vector<int> data(NUM_ELEMENTS);
    std::iota(data.begin(), data.end(), 1);

    std::mt19937 rng(1337);
    std::shuffle(data.begin(), data.end(), rng);

    ds::RBT<int> tree;

    // 1. Вставка 5000 элементов
    for (int x : data) {
        tree.insert(x);
    }
    assert(tree.size() == NUM_ELEMENTS);

    // 2. Проверка наличия всех 5000 элементов
    for (int x : data) {
        assert(tree.search(x) == true);
    }

    // 3. Удаление первой половины (2500 элементов)
    for (int i = 0; i < NUM_ELEMENTS / 2; ++i) {
        assert(tree.remove(data[i]) == true);
    }
    assert(tree.size() == NUM_ELEMENTS / 2);

    // 4. Проверка: удаленные отсутствуют, оставшиеся на месте
    for (int i = 0; i < NUM_ELEMENTS / 2; ++i) {
        assert(tree.search(data[i]) == false);
    }
    for (int i = NUM_ELEMENTS / 2; i < NUM_ELEMENTS; ++i) {
        assert(tree.search(data[i]) == true);
    }

    // 5. Удаление второй половины
    for (int i = NUM_ELEMENTS / 2; i < NUM_ELEMENTS; ++i) {
        assert(tree.remove(data[i]) == true);
    }
    assert(tree.empty());
    assert(tree.size() == 0);

    std::cout << "[ OK ] 6. Stress test (5000 random inserts & removals) passed\n";
}

// ============================================================================
// 7. Тест с пользовательскими типами и std::string (проверка концепта)
// ============================================================================
void test_custom_types() {
    // Тест со строками
    ds::RBT<std::string> str_tree;
    str_tree.insert("apple");
    str_tree.insert("banana");
    str_tree.insert("cherry");
    str_tree.insert("date");

    assert(str_tree.size() == 4);
    assert(str_tree.search("banana") == true);
    assert(str_tree.search("mango") == false);

    assert(str_tree.remove("banana") == true);
    assert(str_tree.search("banana") == false);
    assert(str_tree.size() == 3);

    // Тест с пользовательской структурой
    struct Person {
        int id;
        std::string name;

        // Операторы для соответствия концепту comparable
        bool operator<(const Person& other) const noexcept { return id < other.id; }
        bool operator>(const Person& other) const noexcept { return id > other.id; }
        bool operator==(const Person& other) const noexcept { return id == other.id; }
    };

    ds::RBT<Person> person_tree;
    person_tree.insert({1, "Alice"});
    person_tree.insert({2, "Bob"});
    person_tree.insert({3, "Charlie"});

    assert(person_tree.size() == 3);
    assert(person_tree.search({2, "Bob"}) == true);
    assert(person_tree.search({4, "David"}) == false);

    std::cout << "[ OK ] 7. Custom types and std::string support passed\n";
}

// ============================================================================
// Главная функция запуска всех тестов
// ============================================================================
int main() {
    std::cout << "========================================\n";
    std::cout << "   STARTING RED-BLACK TREE TEST SUITE   \n";
    std::cout << "========================================\n\n";

    try {
        test_empty_tree();
        test_single_element();
        test_sequential_insert();
        test_deletion_scenarios();
        test_rule_of_five();
        test_stress_random();
        test_custom_types();
    } catch (const std::exception& ex) {
        std::cerr << "\n[ FAIL ] Unhandled exception: " << ex.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n[ FAIL ] Unknown exception caught!\n";
        return 1;
    }

    std::cout << "\n========================================\n";
    std::cout << "   🎉 ALL 7 TEST SUITES PASSED! 🎉     \n";
    std::cout << "========================================\n";
    return 0;
}