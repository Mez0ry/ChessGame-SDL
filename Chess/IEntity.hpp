#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__
#include "memory"
#include "Vector.hpp"
#include "Draggable.hpp"
#include "Texture.hpp"

namespace Core{
    template <typename T>
    using Ref = std::shared_ptr<T>;
}; //!Core

class EventHandler;
class Renderer;

class IEntity{
public:
    virtual ~IEntity() {}

    virtual void Update(float dt) = 0;
    virtual void Render(const Core::Ref<Renderer> renderer) = 0;
    
    virtual void SetSize(ObjectSize size) = 0;
    virtual ObjectSize GetSize() const= 0;

    virtual void SetPosition(const Vec2i& pos) = 0;
    virtual Vec2i GetPosition() const = 0;
    
    virtual void Kill() = 0;
    virtual bool IsKilled() const = 0;
    virtual void Revive() = 0;

    virtual Draggable<Texture>& GetTexture() = 0;
};

#endif //!__ENTITY_HPP__