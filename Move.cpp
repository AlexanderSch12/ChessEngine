#include "Move.hpp"

#include <ostream>

Move::Move(const Square &from, const Square &to,
           const std::optional<PieceType> &promotion) :
        from_(from.index()), to_(to.index()), score_(0), promotion_(promotion)
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

int Move::getFrom() const
{
    return from_;
}

int Move::getTo() const
{
    return to_;
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
    return lhs.score() < rhs.score();
}

bool operator==(const Move &lhs, const Move &rhs)
{
    return lhs.from().index() == rhs.from().index() && lhs.to().index() == rhs.to().index() && lhs.promotion() == rhs.promotion();
}
