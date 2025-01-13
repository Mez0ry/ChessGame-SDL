#ifndef __ENTITY_COMMAND_HPP__
#define __ENTITY_COMMAND_HPP__
#include "IEntity.hpp"
#include <optional>

enum class CommandStatus{
    FAILED,SUCCESS,WAITING
};
class IEntityCommand
{
public:
    virtual ~IEntityCommand() {}

    virtual CommandStatus Execute(std::optional<float> dt) = 0;
};

#endif //! __ENTITY_COMMAND_HPP__