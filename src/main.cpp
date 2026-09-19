#include <iostream>
#include <chrono>
#include <vector>
#include <set>
#include <random>
#include <numeric>
#include <iomanip>
#include <rbt/rbt.hpp>

template <typename Func>
double measure_ms(Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}


void benchmark_vs_std_set() {
    const int NUM_ELEMENTS = 500'000;
    std::cout << "Бенчмарк: ds::RBT vs std::set (" << NUM_ELEMENTS << " элементов)\n";

    std::vector<int> data(NUM_ELEMENTS);
    std::iota(data.begin(), data.end(), 1);
    std::mt19937 rng(42);
    std::shuffle(data.begin(), data.end(), rng);

    ds::RBT<int> my_tree;
    std::set<int> std_tree;

    double my_insert_time = measure_ms([&]() {
        for (int x : data) my_tree.insert(x);
    });

    double std_insert_time = measure_ms([&]() {
        for (int x : data) std_tree.insert(x);
    });

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Вставка 500k элементов:\n";
    std::cout << "  -> ds::RBT:  " << my_insert_time << " ms\n";
    std::cout << "  -> std::set: " << std_insert_time << " ms\n\n";

    // 2. Тест скорости поиска
    double my_search_time = measure_ms([&]() {
        for (int x : data) my_tree.search(x);
    });

    double std_search_time = measure_ms([&]() {
        for (int x : data) std_tree.find(x);
    });

    std::cout << "Поиск 500k элементов:\n";
    std::cout << "  -> ds::RBT:  " << my_search_time << " ms\n";
    std::cout << "  -> std::set: " << std_search_time << " ms\n\n";

    // 3. Тест скорости удаления
    double my_delete_time = measure_ms([&]() {
        for (int x : data) my_tree.remove(x);
    });

    double std_delete_time = measure_ms([&]() {
        for (int x : data) std_tree.erase(x);
    });

    std::cout << "Удаление 500k элементов:\n";
    std::cout << "  -> ds::RBT:  " << my_delete_time << " ms\n";
    std::cout << "  -> std::set: " << std_delete_time << " ms\n";
}

int main() {
    benchmark_vs_std_set();

    std::cout << "\nПрограмма завершена успешно!\n";
    return 0;
}