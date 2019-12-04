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


int64_t fuel(int64_t mass) {
    return (mass / 3) - 2;
}

int main() {
    const auto lines = readLines("../input.txt"s, [](const std::string& str) { return fuel(std::stoi(str));});

    const auto sum = std::accumulate(lines.begin(), lines.end(), 0ll);

    std::cout << sum << '\n';
}



/*#include <math.h>
#include <stdio.h>

int main() {
    //for (int i = 1; i <= 10; i++) {
    //    printf("%d %d\n", i, i * 2);
    //}

    //double r;
    //printf("Enter radius of circle: ");
    //scanf("%lf", &r);
    //printf("The area of the circle is %.2f", M_PI * r * r);
}*/


