#include "resource.h"

#include "game/color_data.h"
#include "core/resource_manager.h"

Resources Resources::operator*(int scalar)
{
    return {
        this->Wood  * scalar,
        this->Rock  * scalar,
        this->Steel * scalar,
        this->Gold  * scalar
    };
}

Resources& Resources::operator+=(const Resources& other)
{
    Wood  += other.Wood;
    Rock  += other.Rock;
    Steel += other.Steel;
    Gold  += other.Gold;
    return *this;
}

Resources& Resources::operator-=(const Resources& other)
{
    Wood  -= other.Wood;
    Rock  -= other.Rock;
    Steel -= other.Steel;
    Gold  -= other.Gold;
    return *this;
}

bool Resources::operator>=(const Resources& other)
{
    return (
        Wood  >= other.Wood &&
        Rock  >= other.Rock &&
        Steel >= other.Steel &&
        Gold  >= other.Gold
    );
}

ResourceData Resources::GetResourceData()
{
    ResourceData data;

    static auto woodTexture = ResourceManager::GetTexture("wood");
    static auto rockTexture = ResourceManager::GetTexture("rock");
    static auto steelTexture = ResourceManager::GetTexture("steel");
    static auto goldTexture = ResourceManager::GetTexture("gold");

    data.ResourceNumberColors[0] = ColorData::Get().Resources.Wood;
    data.ResourceNumberColors[1] = ColorData::Get().Resources.Rock;
    data.ResourceNumberColors[2] = ColorData::Get().Resources.Steel;
    data.ResourceNumberColors[3] = ColorData::Get().Resources.Gold;

    data.ResourceTextures[0] = woodTexture;
    data.ResourceTextures[1] = rockTexture;
    data.ResourceTextures[2] = steelTexture;
    data.ResourceTextures[3] = goldTexture;

    data.ResourceTextureScales[0] = 1.0f;
    data.ResourceTextureScales[1] = 1.1f;
    data.ResourceTextureScales[2] = 0.8f;
    data.ResourceTextureScales[3] = 0.78f;

    return data;
}
