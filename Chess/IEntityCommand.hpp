#ifndef __ENTITY_COMMAND_HPP__
#define __ENTITY_COMMAND_HPP__
#include "IEntity.hpp"
#include <optional>
class IEntityCommand
{
public:
    virtual ~IEntityCommand() {}

    virtual bool Execute(std::optional<float> dt) = 0;
};

#endif //! __ENTITY_COMMAND_HPP__