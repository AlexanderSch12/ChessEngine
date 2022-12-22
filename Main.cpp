#include "Uci.hpp"
#include "EngineFactory.hpp"
#include "Fen.hpp"
#include "Engine.hpp"

#include <fstream>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    auto engine = EngineFactory::createEngine();

    if (engine == nullptr) {
        std::cerr << "Failed to create engine\n";
        return EXIT_FAILURE;
    }

    if (argc > 1) {
        auto fen = argv[1];
        auto board = Fen::createBoard(fen);

        if (!board.has_value()) {
            std::cerr << "Parsing FEN failed\n";
            return EXIT_FAILURE;
        }

        std::cout << board.value();
        auto pv = engine->pv(board.value());
        std::cout << "PV: " << pv << '\n';

        for(auto move: pv)
        {
            board->makeMove(move);
            std::cout << "----- " << move << " -----" << std::endl;
            std::cout << board.value();
        }

    } else {
        auto uciLog = std::ofstream("uci-log.txt");
        auto uci = Uci(std::move(engine), std::cin, std::cout, uciLog);
        uci.run();
    }
}
