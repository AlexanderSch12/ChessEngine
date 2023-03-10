#include "PrincipalVariation.hpp"

#include <ostream>


bool PrincipalVariation::isMate() const {
    return mate;
}

int PrincipalVariation::score() const {
    return 0;
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

std::vector<Move> PrincipalVariation::moves() const {
    return moves_;
}

std::ostream& operator<<(std::ostream& os, const PrincipalVariation& pv) {
    os << "[ ";
    for (auto move : pv.moves()) os << move << " , ";
    os << " ]";
    return os;
}
