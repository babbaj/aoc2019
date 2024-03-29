#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <type_traits>
#include <sstream>
#include <set>
#include <functional>
#include <numeric>
#include <optional>


auto readLines(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file) {
        throw std::runtime_error{"Failed to open input file"};
    }

    std::vector<std::string> vec;
    std::string line;
    while (std::getline(file, line)) {
        vec.push_back(std::move(line));
    }
    file.close();

    return vec;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}



using point_t = std::pair<int, int>;

enum class Dir {
    UP,
    DOWN,
    LEFT,
    RIGHT
};
struct Move {
    Dir dir;
    int tiles;

    static Move parse(const std::string& str) noexcept(false) {
        const char dir = str.at(0);
        const int num = std::stoi(str.substr(1));
        switch (dir) {
            case 'U': return {Dir::UP, num};
            case 'D': return {Dir::DOWN, num};
            case 'L': return {Dir::LEFT, num};
            case 'R': return {Dir::RIGHT, num};
            default: throw std::invalid_argument({dir});
        }
    }
};

std::vector<Move> toMoves(const std::vector<std::string>& moveStr) {
    std::vector<Move> out; out.reserve(moveStr.size());
    std::transform(moveStr.begin(), moveStr.end(), std::back_inserter(out), Move::parse);
    return out;
}

std::vector<point_t> pointsBetweenMove(const point_t& pos, const Move& move) {
    int sign;
    int point_t::* member;

    switch (move.dir) {
        case Dir::UP: { // +y
            sign = 1;
            member = &point_t::second;
        } break;
        case Dir::DOWN: { // -y
            sign = -1;
            member = &point_t::second;
        } break;
        case Dir::LEFT: { // -x
            sign = -1;
            member = &point_t::first;
        } break;
        case Dir::RIGHT: { // +x
            sign = 1;
            member = &point_t::first;
        } break;
    }

    point_t it = pos;
    std::vector<point_t> out;
    for (int i = 0; i < move.tiles; i++) {
        it.*member += sign;
        out.push_back(it);
    }

    return out;
}

std::vector<point_t> toCoords(const std::vector<Move> moves) {
    std::vector<point_t> out;

    point_t point{};
    for (const Move& move : moves) {
        const auto points = pointsBetweenMove(point, move);
        std::copy(points.cbegin(), points.cend(), std::back_inserter(out));
        point = points.back();

    }

    return out;
}

template<typename T>
std::vector<T> collisions(const std::vector<T>& a, const std::vector<T>& b) {
    std::vector<T> out;
    for (const auto& i : a) {
        for (const auto& j : b) {
            if (i == j) {
                out.push_back(i);
            }
        }
    }

    return out;
}

namespace part1 {

    int distance(const point_t& pos) {
        return std::abs(pos.first) + std::abs(pos.second);
    }


    void run() {
        const std::vector<std::string> lines = readLines("../input.txt");

        const std::vector<std::string> lineA = split(lines.at(0), ',');
        const std::vector<std::string> lineB = split(lines.at(1), ',');


        const std::vector<point_t> pointsA = toCoords(toMoves(lineA));
        const std::vector<point_t> pointsB = toCoords(toMoves(lineB));

        const std::vector<point_t> intersects = collisions(pointsA, pointsB);

        const point_t closest = *std::min_element(intersects.cbegin(), intersects.cend(), [](const auto& a, const auto& b) {
           return distance(a) < distance(b);
        });
        std::cout << "{" << closest.first << ", " << closest.second << "}" << '\n';
        std::cout << distance(closest) << '\n';
    }
}

namespace part2 {
    struct AbsoluteMove {
        point_t from;
        Move move;
    };

    std::vector<AbsoluteMove> toAbsoluteMoves(const std::vector<std::string>& moveStrs) {
        std::vector<AbsoluteMove> out; out.reserve(moveStrs.size());

        point_t point{};
        for (const std::string& mov : moveStrs) {
            const Move relativeMove = Move::parse(mov);
            point_t nextPoint;
            switch (relativeMove.dir) {
                case Dir::UP:    nextPoint = {point.first, point.second + relativeMove.tiles};
                    break;
                case Dir::DOWN:  nextPoint = {point.first, point.second - relativeMove.tiles};
                    break;
                case Dir::LEFT:  nextPoint = {point.first - relativeMove.tiles, point.second};
                    break;
                case Dir::RIGHT: nextPoint = {point.first + relativeMove.tiles, point.second};
                    break;
            }
            out.push_back({point, relativeMove});
            point = nextPoint;
        }

        return out;
    }

    std::optional<point_t> intersection(const AbsoluteMove& lhs, const AbsoluteMove& rhs) {
        const auto pointsLhs = pointsBetweenMove(lhs.from, lhs.move);
        const auto pointsRhs = pointsBetweenMove(rhs.from, rhs.move);
        const auto vec = collisions(pointsLhs, pointsRhs);

        return vec.empty() ? std::nullopt : std::optional{vec.at(0)};
    }


    AbsoluteMove shortenMove(const AbsoluteMove& move, const point_t& to) {
        const auto dir = move.move.dir;
        const auto from = move.from;

        switch (dir) {
            // +
            case Dir::UP:    return {from, Move{dir, to.second  - from.second}};
            case Dir::RIGHT: return {from, Move{dir, to.first   - from.first}};

            // -
            case Dir::DOWN: return {from, Move{dir, from.second - to.second}};
            case Dir::LEFT: return {from, Move{dir, from.first  - to.first}};
        }
    }

    auto intersections(const std::vector<AbsoluteMove>& lhs, const std::vector<AbsoluteMove>& rhs) {
        std::vector<std::pair<std::vector<AbsoluteMove>, std::vector<AbsoluteMove>>> out;

        for (auto itA = lhs.cbegin(); itA != lhs.cend(); itA++) {
            for (auto itB = rhs.cbegin(); itB != rhs.cend(); itB++) {
                const std::optional<point_t> intersect = intersection(*itA, *itB);
                if (intersect) {
                    const point_t point = *intersect;

                    std::vector<AbsoluteMove> wireA(lhs.cbegin(), itA); wireA.push_back(shortenMove(*itA, point));
                    std::vector<AbsoluteMove> wireB(rhs.cbegin(), itB); wireB.push_back(shortenMove(*itB, point));

                    out.emplace_back(std::move(wireA), std::move(wireB));
                }
            }
        }

        return out;
    }

    int wireLength(const std::vector<AbsoluteMove>& wire) {
        return std::accumulate(wire.cbegin(), wire.cend(), 0, [](int acc, const AbsoluteMove& mov) {
            return acc + mov.move.tiles;
        });
    }


    void run() {
        const std::vector<std::string> lines = readLines("../input.txt");

        const std::vector<std::string> lineA = split(lines.at(0), ',');
        const std::vector<std::string> lineB = split(lines.at(1), ',');


        const std::vector<AbsoluteMove> wireA = toAbsoluteMoves(lineA);
        const std::vector<AbsoluteMove> wireB = toAbsoluteMoves(lineB);

        const auto intersects = intersections(wireA, wireB);

        std::vector<int> wireLengths;
        std::transform(intersects.begin(), intersects.end(), std::back_inserter(wireLengths), [](const auto& wires) {
           return wireLength(wires.first) + wireLength(wires.second);
        });

        const auto sumIt = std::min_element(wireLengths.cbegin(), wireLengths.cend());

        if (sumIt != wireLengths.cend()) {
            std::cout << "min length: " << *sumIt << '\n';
        } else {
            std::cout << "no answer??\n";
        }
    }
}



int main() {
    part2::run();
    return 0;
}