#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <type_traits>


template<typename F>
auto readLines(const std::string& fileName, F parser) {
    std::ifstream file(fileName);
    if (!file) {
        throw std::runtime_error{"Failed to open input file"};
    }

    std::vector<std::invoke_result_t<F, std::string>> vec;
    std::string line;
    while (std::getline(file, line)) {
        vec.push_back(parser(std::move(line)));
    }
    file.close();

    return vec;
}

template<typename T, typename F>
auto map(const std::vector<T> vec, F func) {
    using U = std::invoke_result_t<F, T>;
    std::vector<U> out;
    out.reserve(vec.size());
    std::transform(vec.begin(), vec.end(), std::back_inserter(out), std::move(func));

    return out;
}

int fuel(int mass) {
    return (mass / 3) - 2;
}

namespace part1 {
    void run() {
        const auto lines = readLines("../input1.txt", [](const std::string& str) { return std::stoi(str); });
        const auto computed = map(lines, fuel);

        const auto sum = std::accumulate(lines.begin(), lines.end(), 0ll);

        std::cout << sum << '\n';
    }
}

namespace part2 {
    int recursiveFuel(int mass) {
        if (mass <= 8) return 0;

        const auto f = fuel(mass);
        return f + recursiveFuel(f);
    }


    void run() {
        const auto masses = readLines("../input2.txt", [](const std::string& str) { return std::stoi(str); });
        const std::vector<int> computed = map(masses, recursiveFuel);

        const auto sum = std::accumulate(computed.begin(), computed.end(), 0ll);

        std::cout << sum << '\n';
    }
}


int main() {
    part2::run();
}


