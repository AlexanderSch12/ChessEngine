#include "Engine.hpp"

#include <utility>
#include <iostream>

Engine_::Engine_(std::string name, std::string version, std::string author):
    name_(std::move(name)),version_(std::move(version)),author_(std::move(author))
{

}

std::string Engine_::name() const
{
    return name_;
}

std::string Engine_::version() const
{
    return version_;
}

std::string Engine_::author() const
{
    return author_;
}
void Engine_::newGame()
{}

std::optional<HashInfo> Engine::hashInfo() const
{
    return std::nullopt;
}

PrincipalVariation Engine_::pv(Board &board, const TimeInfo::Optional &time)
{
    auto pv = PrincipalVariation();
    pv.mate = false;
    negamax(board, 10, neg_inf, inf, pv);
    return pv;
    (void) time;
}

void Engine::setHashSize(std::size_t)
{}

int Engine_::evaluate(Board &board)
{
    int score = 0;
    for(int i = 0 ; i<64 ;i++)
    {
        auto piece = board.board()[i];
        if(!Board::isEmpty(piece) && Board::getColor(piece) == board.getBoardTurn())
        {
            switch (Board::getType(piece))
            {
                case Board::pawn:
                    score += 100;
                    break;
                case Board::knight:
                    score += 300;
                    break;
                case Board::bishop:
                    score += 320;
                    break;
                case Board::rook:
                    score += 500;
                    break;
                case Board::queen:
                    score += 900;
                    break;
                default:
                    score += 0;
            }
        }
    }
    return score;
}

int Engine_::negamax(Board &board, int depth, int alpha, int beta, PrincipalVariation& pv)
{
    if (depth == 0)
    {
        pv.moves().clear();
        return evaluate(board);
    }

    std::vector legalMoves = Board::MoveVec();
    board.pseudoLegalMoves(legalMoves);

    if (legalMoves.empty())
    {
        if(board.isKingCheck(board.getBoardTurn()))
        {
            pv.mate = true;
            return -1000;
        }
        else return 0;
    }

    auto pv_buf = PrincipalVariation();
    for (Move& move: legalMoves)
    {
        auto prev_state = board.makeMoveSaveState(move);
        auto eval = -negamax(board, depth - 1, -beta, -alpha, pv_buf);
        board.reverseMove(move, prev_state);

        if (eval >= beta) return beta;
        if (eval > alpha)
        {
            alpha = eval;
            pv.moves().clear();
            pv.moves().push_back(move);
            for(const auto& move_buf : pv_buf)
            {
                pv.moves().emplace_back(move_buf);
            }
        }
    }
    return alpha;
}