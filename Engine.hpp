#ifndef CHESS_ENGINE_ENGINE_HPP
#define CHESS_ENGINE_ENGINE_HPP

#include "PrincipalVariation.hpp"
#include "Board.hpp"
#include "TimeInfo.hpp"

#include <string>
#include <optional>
#include <cstddef>

struct HashInfo
{
    std::size_t defaultSize;
    std::size_t minSize;
    std::size_t maxSize;
};

class Engine
{
public:
    virtual ~Engine() = default;

    virtual std::string name() const = 0;

    virtual std::string version() const = 0;

    virtual std::string author() const = 0;

    virtual void newGame() = 0;

    virtual PrincipalVariation pv(
            Board &board,
            const TimeInfo::Optional &timeInfo = std::nullopt
    ) = 0;

    virtual std::optional<HashInfo> hashInfo() const;

    virtual void setHashSize(std::size_t size);
};

class Engine_ : public Engine
{

public:
    Engine_(std::string name, std::string version, std::string author);

    virtual std::string name() const;

    virtual std::string version() const;

    virtual std::string author() const;

    virtual void newGame();

    virtual PrincipalVariation pv(
            Board &board,
            const TimeInfo::Optional &timeInfo = std::nullopt
    );

private:
    std::string name_;
    std::string version_;
    std::string author_;
    static const int inf = 9999999;
    static const int neg_inf = -inf;

    int evaluate(Board &board);

    int negamax(Board &board, int depth, int alpha, int beta, PrincipalVariation& pv);
};

#endif
