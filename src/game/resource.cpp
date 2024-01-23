#include "resource.h"

#include "game/color_data.h"
#include "core/resource_manager.h"
#include "graphics/renderer.h"
#include "util/util.h"

Resources Resources::operator*(double scalar)
{
    return {
        (int)((double) this->Wood  * scalar),
        (int)((double) this->Rock  * scalar),
        (int)((double) this->Steel * scalar),
        (int)((double) this->Gold  * scalar)
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

void Resources::Draw2x2(const Resources& res, const glm::vec2& position)
{
    static auto resourceData = Resources::GetResourceData();

    static float hOffset = 0.09f;
    static float resSize = 0.07f;
    static float resHOffset = 0.02f;
    static float textScale = 0.2f;

    int costValues[resourceData.NumResources] = { res.Wood, res.Rock, res.Steel, res.Gold };

    for (int i = 0; i < resourceData.NumResources; i++)
    {
        Renderer2D::DrawQuad(
            glm::vec2(
                position.x - hOffset * glm::pow(-1.0, (double)(i % 2)) - hOffset / 2.0f,
                position.y - resHOffset - resSize * Util::Clamp<int>(i - 1, 0, 1)
            ),
            glm::vec2(resSize),
            resourceData.ResourceTextures[i]
        );

        Renderer2D::DrawTextStr(
            std::to_string(costValues[i]),
            glm::vec2(
                position.x - resSize / 2.4f - hOffset * glm::pow(-1.0, (double)(i % 2)) + hOffset / 3.2f,
                position.y - resHOffset - resSize * Util::Clamp<int>(i - 1, 0, 1)
            ),
            textScale,
            glm::vec3(1.0f),
            HTextAlign::LEFT, VTextAlign::MIDDLE,
            "rexlia"
        );
    }

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
