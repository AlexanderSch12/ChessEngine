#include "PrincipalVariation.hpp"

#include <ostream>


bool PrincipalVariation::isMate() const {
    return mate;
}

int PrincipalVariation::score() const {
    return score_;
}

void PrincipalVariation::setScore(int score)
{
    score_ = score;
}

std::size_t PrincipalVariation::length() const {
    return moves_.size();
}

PrincipalVariation::MoveIter PrincipalVariation::begin() const {
    return moves_.begin();
}

PrincipalVariation::MoveIter PrincipalVariation::end() const {
    return moves_.end();
}

std::vector<Move>& PrincipalVariation::moves(){
    return moves_;
}

std::ostream& operator<<(std::ostream& os, PrincipalVariation& pv) {
    os << "Score: " << pv.score() << " ";
    os << "[ ";
    for (auto move : pv.moves()) os << move << " ";
    os << "]";
    return os;
}
