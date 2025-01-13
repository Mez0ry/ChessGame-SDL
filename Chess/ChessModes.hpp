#ifndef __CHESS_MODES_HPP__
#define __CHESS_MODES_HPP__
#include "Core.hpp"
#include <vector>

class Piece;
class ForsythEdwards;

class IChessModes{
public:
    virtual ~IChessModes() {}

    virtual Core::Ref<ForsythEdwards> GetFenStrategy() const = 0;
};

class IChessVariants : public IChessModes{
    public:
    virtual ~IChessVariants() {}

    virtual Core::Ref<ForsythEdwards> GetFenStrategy() const = 0;
    private:
};

class RapidMode final : public IChessModes{
public:
    Core::Ref<ForsythEdwards> GetFenStrategy() const override;
private:
};

#endif //!__CHESS_MODES_HPP__