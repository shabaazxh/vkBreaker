#pragma once
#include "Buffer.h"

#include <memory>

class AABB {
    public:
        bool AABB::intersects(glm::vec3 playerPosition, glm::vec3 playerSize, glm::vec3 obstaclePos, glm::vec3 obstacleSize) const {
            
            return playerPosition.x + playerSize.x > obstaclePos.x &&
            playerPosition.x < obstaclePos.x + obstacleSize.x &&
            playerPosition.y + playerSize.y > obstaclePos.y &&
            playerPosition.y < obstaclePos.y + obstacleSize.y;
        }
    private:
};