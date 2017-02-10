#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>

class $safeprojectname$App : public aie::Application {
public:

	$safeprojectname$App();
	virtual ~$safeprojectname$App();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	// camera transforms
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
};