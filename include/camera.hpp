#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "common.h"

enum MoveDirection
{
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT,
    UP,
    DOWN
};

class Camera
{
public:
    // camera status
    vec3 position;
    vec3 front;
    vec3 top;
    vec3 right;
    // perspective status
    float fieldOfView;
    float aspect;
    float near;
    float far;
    // move parameter
    float moveSpeed;
    // trackball parameter
    float trackballSpeed;
    float theta;
    float phi;

    Camera()
    {
        position       = vec3(0.0f, 0.0f, 0.0f);
        front          = vec3(1.0f, 0.0f, 0.0f);
        top            = vec3(0.0f, 1.0f, 0.0f);
        right          = vec3(0.0f, 0.0f, 1.0f);
        fieldOfView    = 60.0f;
        aspect         = (float)INIT_VIEWPORT_WIDTH / (float)INIT_VIEWPORT_HEIGHT;
        near           = 0.1f;
        far            = 1000.0f;
        moveSpeed      = 10.0f;
        trackballSpeed = 0.1f;
        theta          = 0.0f;
        phi            = 0.0f;
    }

    Camera& withPosition(vec3 val)
    {
        position = val;
        return *this;
    }

    Camera& withFront(vec3 val)
    {
        front = normalize(val);
        return *this;
    }

    Camera& withTop(vec3 val)
    {
        top = normalize(val);
        return *this;
    }

    Camera& withRight(vec3 val)
    {
        right = normalize(val);
        return *this;
    }

    Camera& withFieldOfView(float val)
    {
        fieldOfView = val;
        return *this;
    }

    Camera& withAspect(float val)
    {
        aspect = val;
        return *this;
    }

    Camera& withNear(float val)
    {
        near = val;
        return *this;
    }

    Camera& withFar(float val)
    {
        far = val;
        return *this;
    }

    Camera& withMoveSpeed(float val)
    {
        moveSpeed = val;
        return *this;
    }

    Camera& withTrackballSpeed(float val)
    {
        trackballSpeed = val;
        return *this;
    }

    Camera& withTheta(float val)
    {
        theta = val;
        return *this;
    }
    Camera& withPhi(float val)
    {
        phi = val;
        return *this;
    }

    mat4 getPerspective()
    {
        return perspective(radians(fieldOfView), aspect, near, far);
    }

    mat4 getView()
    {
        // cout << "DEBUG::MAIN::C-CAMERA-F-GV: " << position.x << " " << position.y << " " << position.z << endl;
        // cout << "DEBUG::MAIN::C-CAMERA-F-GV: " << front.x << " " << front.y << " " << front.z << endl;
        return lookAt(position, position + front, top);
    }

    void processMove(MoveDirection moveDirction, float timeDifferent)
    {
        float shift = moveSpeed * timeDifferent;
        cout << "DEBUG::MAIN::C-CAMERA-F-PM-1: " << shift << endl;
        if (moveDirction == FORWARD)
            position += front * shift;
        if (moveDirction == BACKWARD)
            position -= front * shift;
        if (moveDirction == LEFT)
            position -= right * shift;
        if (moveDirction == RIGHT)
            position += right * shift;
        if (moveDirction == UP)
            position += top * shift;
        if (moveDirction == DOWN)
            position -= top * shift;
        cout << "DEBUG::MAIN::C-CAMERA-F-PM-2: " << position. x << " " << position.y << " " << position.z << endl;
    }

    void processTrackball(float thetaDifferent, float phiDifferent)
    {
        theta -= thetaDifferent * trackballSpeed;
        phi += phiDifferent * trackballSpeed;
        if (phi > 85)
            phi = 85.0f;
        else if (phi < -85)
            phi += -85.0f;
        updateCameraStatus();
    };

private:
    void updateCameraStatus()
    {
        front.x = cos(radians(theta)) * cos(radians(phi));
        front.y = sin(radians(phi));
        front.z = sin(radians(theta)) * cos(radians(phi));
        front = normalize(front);

        right = normalize(cross(front, vec3(0.0f, 1.0f, 0.0f)));
        top = normalize(cross(right, front));
    }
    
};


#endif
