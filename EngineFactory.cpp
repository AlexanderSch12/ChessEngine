#include "EngineFactory.hpp"

std::unique_ptr<Engine> EngineFactory::createEngine() {
    return std::make_unique<Engine_>("ChessEngine","V12","Alex");
}
