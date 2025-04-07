#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class Camera {
public:
    Camera();
    ~Camera() {}

    // Update camera based on input flags and delta time.
    void update(bool up, bool down, bool left, bool right, float deltaTime);
    
    // Get the view matrix computed from the camera position and direction.
    glm::mat4 getViewMatrix() const;
    // Debug: Get a string with camera info (position, yaw, pitch, and view matrix).
    std::string getDebugInfo() const;
    
    void setMovementSpeed(float speed) { movementSpeed = speed; }
    void setRotationSpeed(float speed) { rotationSpeed = speed; }
    void setPosition(const glm::vec3 &pos);
    void setYaw(float y);
    glm::vec3 getPosition(); 
    
private:
    glm::vec3 position;
    float yaw;   // rotation around the y-axis in degrees
    float pitch; // not used in this simple example, but available for future extension

    float movementSpeed;
    float rotationSpeed;
};

#endif // CAMERA_H
