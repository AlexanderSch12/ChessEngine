#include "Move.hpp"

#include <ostream>

Move::Move(const Square &from, const Square &to,
           const std::optional<PieceType> &promotion) :
        from_(from), to_(to), score_(0), promotion_(promotion)
{

}

Move::Optional Move::fromUci(const std::string &uci)
{

    if (uci.length() > 5 || uci.length() < 4) return std::nullopt;
    Square::Optional from = Square::fromName(uci.substr(0, 2));
    Square::Optional to = Square::fromName(uci.substr(2, 2));
    if(uci.length() == 5)
    {
        Piece::Optional promotion = Piece::fromSymbol(uci[4]);
        if (!promotion.has_value()) return std::nullopt;
        else return Move(from.value(), to.value(), promotion.value().type());
    } else if (from.has_value() && to.has_value())
    {
        return Move(from.value(), to.value());
    } else
        return std::nullopt;
}

Square Move::from() const
{
    return from_;
}

Square Move::to() const
{
    return to_;
}

int Move::score() const
{
    return score_;
}

void Move::setScore(int score)
{
    score_ = score;
}


std::optional<PieceType> Move::promotion() const
{
    return promotion_;
}

std::ostream &operator<<(std::ostream &os, const Move &move)
{
    auto promo = move.promotion();
    if (promo.has_value())
    {
        return os << move.from() << move.to() << promo.value();
    }
    return os << move.from() << move.to();
}


bool operator<(const Move &lhs, const Move &rhs)
{
    return lhs.from() < rhs.from() && lhs.to() < rhs.to();
}

bool operator==(const Move &lhs, const Move &rhs)
{
    return lhs.from() == rhs.from() && lhs.to() == rhs.to() && lhs.promotion() == rhs.promotion();
}
