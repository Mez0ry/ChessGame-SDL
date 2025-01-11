#ifndef __ENTITY_COMMAND_HPP__
#define __ENTITY_COMMAND_HPP__
#include "IEntity.hpp"

class IEntityCommand
{
public:
    virtual ~IEntityCommand() {}

    virtual void Execute() = 0;
};

#endif //! __ENTITY_COMMAND_HPP__