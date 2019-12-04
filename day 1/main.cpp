#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <numeric>
#include <type_traits>

using namespace std::string_literals;

template<typename F>
auto readLines(const std::string& fileName, F parser) {
    std::ifstream file(fileName);

    std::vector<std::invoke_result_t<F, std::string>> vec;
    std::string line;
    while (std::getline(file, line)) {
        vec.push_back(parser(std::move(line)));
    }
    file.close();

    return vec;
}

namespace part1 {
    int64_t fuel(int64_t mass) {
        return (mass / 3) - 2;
    }

    void run() {
        const auto lines = readLines("../input1.txt"s, [](const std::string& str) { return fuel(std::stoi(str)); });

        const auto sum = std::accumulate(lines.begin(), lines.end(), 0ll);

        std::cout << sum << '\n';
    }
}

namespace part2 {

}


int main() {

}


